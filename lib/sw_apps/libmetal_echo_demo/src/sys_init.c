/******************************************************************************
*
* Copyright (C) 2010 - 2015 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

#include "xparameters.h"
#include "xil_cache.h"
#include "xil_exception.h"
#include "xstatus.h"
#include "xscugic.h"

#include "platform_config.h"
#include "metal/io.h"
#include "metal/device.h"
#include "metal/sys.h"

#ifdef STDOUT_IS_16550
 #include "xuartns550_l.h"

 #define UART_BAUD 9600
#endif

#define INTC_DEVICE_ID		XPAR_SCUGIC_0_DEVICE_ID

#define IPI_IRQ_VECT_ID         65

XScuGic InterruptController;

const metal_phys_addr_t metal_phys[] = {
	0xFF310000, /**< base IPI address */
	0x3ED00000, /**< shared memory description base address */
	0x3ED10000, /**< shared memory base address */
};

struct metal_device metal_dev_table[] = {
	{
		/* IPI device */
		"ff310000.ipi",
		NULL,
		1,
		{
			{
				(void *)0xFF310000,
				&metal_phys[0],
				0x1000,
				(sizeof(metal_phys_addr_t) << 3),
				(unsigned long)(-1),
				METAL_UNCACHED,
				{NULL},
			}
		},
		{NULL},
		1,
		(void *)IPI_IRQ_VECT_ID,

	},
	{
		/* Shared memory management device */
		"3ed00000.shm_desc",
		NULL,
		1,
		{
			{
				(void *)0x3ED00000,
				&metal_phys[1],
				0x1000,
				(sizeof(metal_phys_addr_t) << 3),
				(unsigned long)(-1),
				METAL_UNCACHED | METAL_SHARED_MEM,
				{NULL},
			}
		},
		{NULL},
		0,
		NULL,

	},
	{
		/* Shared memory management device */
		"3ed10000.shm",
		NULL,
		1,
		{
			{
				(void *)0x3ED10000,
				&metal_phys[2],
				0x100000,
				(sizeof(metal_phys_addr_t) << 3),
				(unsigned long)(-1),
				METAL_UNCACHED | METAL_SHARED_MEM,
				{NULL},
			}
		},
		{NULL},
		0,
		NULL,

	},
};

extern void metal_irq_bm_isr(int irq);

void enable_caches()
{
#ifdef __PPC__
	Xil_ICacheEnableRegion(CACHEABLE_REGION_MASK);
	Xil_DCacheEnableRegion(CACHEABLE_REGION_MASK);
#elif __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_ICACHE
	Xil_ICacheEnable();
#endif
#ifdef XPAR_MICROBLAZE_USE_DCACHE
	Xil_DCacheEnable();
#endif
#endif
}

void disable_caches()
{
	Xil_DCacheDisable();
	Xil_ICacheDisable();
}

void init_uart()
{
#ifdef STDOUT_IS_16550
	XUartNs550_SetBaud(STDOUT_BASEADDR, XPAR_XUARTNS550_CLOCK_HZ,
			   UART_BAUD);
	XUartNs550_SetLineControlReg(STDOUT_BASEADDR, XUN_LCR_8_DATA_BITS);
#endif
	/* Bootrom/BSP configures PS7/PSU UART to 115200 bps */
}

int init_irq()
{
	int ret = 0;
	Xil_ExceptionDisable();

	XScuGic_Config *IntcConfig;	/* The configuration parameters of
					 * the interrupt controller */

	/*
	 * Initialize the interrupt controller driver
	 */
	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
	if (NULL == IntcConfig) {
		return (int)XST_FAILURE;
	}

	ret = XScuGic_CfgInitialize(&InterruptController, IntcConfig,
				       IntcConfig->CpuBaseAddress);
	if (ret != XST_SUCCESS) {
		return (int)XST_FAILURE;
	}

	/*
	 * Register the interrupt handler to the hardware interrupt handling
	 * logic in the ARM processor.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,
			(Xil_ExceptionHandler)XScuGic_InterruptHandler,
			&InterruptController);

	Xil_ExceptionEnable();
	/* Connect Interrupt ID with ISR */
	XScuGic_Connect(&InterruptController, IPI_IRQ_VECT_ID,
			   (Xil_ExceptionHandler)metal_irq_bm_isr,
			   (void *)IPI_IRQ_VECT_ID);

	XScuGic_Enable(&InterruptController, IPI_IRQ_VECT_ID);

	return 0;
}

/**
 * This funciton is to install baremeta/RTOS libmetal devices.
 */
int platform_register_metal_device (void)
{
	int i;
	int ret;
	struct metal_device *dev;
	metal_bus_register(&metal_generic_bus);
	for (i = 0; i < sizeof(metal_dev_table)/sizeof(struct metal_device);
	     i++) {
		dev = &metal_dev_table[i];
		xil_printf("registering: %d, name=%s\n", i, dev->name);
		ret = metal_register_generic_device(dev);
		if (ret)
			return ret;
	}
	return 0;
}

int sys_init()
{
	struct metal_init_params metal_param = METAL_INIT_DEFAULTS;
	int ret;
	enable_caches();
	init_uart();
	if (init_irq()) {
		xil_printf("Failed to intialize interrupt\n");
	}
	/** Register the device */
	metal_init(&metal_param);
	ret = platform_register_metal_device();
	if (ret) {
		xil_printf(
			"%s: failed to register device: %d\n", __func__, ret);
		return ret;
	}

	return 0;
}

void sys_cleanup()
{
	metal_finish();
	disable_caches();
}

typedef void *(*task_to_run)(void *arg);
int run_comm_task(task_to_run task, void *arg)
{
	task(arg);
	return 0;
}