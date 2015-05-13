/*
 * Copyright (C) 2014 - 2015 Xilinx, Inc.  All rights reserved.
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * XILINX CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Except as contained in this notice, the name of the Xilinx shall not be used
 * in advertising or otherwise to promote the sale, use or other dealings in
 * this Software without prior written authorization from Xilinx.
 */

/*********************************************************************
 * Master related data and function definitions:
 * 1. Data structures for masters and array of their requirements for
 *    slaves capabilities
 * 2. Functions for managing requirements and accessing master data
 *********************************************************************/

#include "pm_master.h"
#include "pm_proc.h"
#include "pm_defs.h"
#include "pm_sram.h"
#include "pm_usb.h"
#include "pm_periph.h"
#include "ipi_buffer.h"

/* Requirement of APU master */
PmRequirement pmApuReq_g[PM_MASTER_APU_SLAVE_MAX] = {
	[PM_MASTER_APU_SLAVE_OCM0] = {
		.slave = &pmSlaveOcm0_g.slv,
		.requestor = &pmMasterApu_g,
		.info = PM_MASTER_USING_SLAVE_MASK,
		.currReq = PM_CAP_ACCESS | PM_CAP_CONTEXT,
		.nextReq = PM_CAP_ACCESS | PM_CAP_CONTEXT,
	},
	[PM_MASTER_APU_SLAVE_OCM1] = {
		.slave = &pmSlaveOcm1_g.slv,
		.requestor = &pmMasterApu_g,
		.info = PM_MASTER_USING_SLAVE_MASK,
		.currReq = PM_CAP_ACCESS | PM_CAP_CONTEXT,
		.nextReq = PM_CAP_ACCESS | PM_CAP_CONTEXT,
	},
	[PM_MASTER_APU_SLAVE_OCM2] = {
		.slave = &pmSlaveOcm2_g.slv,
		.requestor = &pmMasterApu_g,
		.info = PM_MASTER_USING_SLAVE_MASK,
		.currReq = PM_CAP_ACCESS | PM_CAP_CONTEXT,
		.nextReq = PM_CAP_ACCESS | PM_CAP_CONTEXT,
	},
	[PM_MASTER_APU_SLAVE_OCM3] = {
		.slave = &pmSlaveOcm3_g.slv,
		.requestor = &pmMasterApu_g,
		.info = PM_MASTER_USING_SLAVE_MASK,
		.currReq = PM_CAP_ACCESS | PM_CAP_CONTEXT,
		.nextReq = PM_CAP_ACCESS | PM_CAP_CONTEXT,
	},
	[PM_MASTER_APU_SLAVE_L2] = {
		.slave = &pmSlaveL2_g.slv,
		.requestor = &pmMasterApu_g,
		.info = PM_MASTER_USING_SLAVE_MASK,
		.currReq = PM_CAP_ACCESS | PM_CAP_CONTEXT,
		.nextReq = PM_CAP_ACCESS | PM_CAP_CONTEXT,
	},
	[PM_MASTER_APU_SLAVE_USB0] = {
		.slave = &pmSlaveUsb0_g.slv,
		.requestor = &pmMasterApu_g,
		.info = 0U,
		.currReq = 0U,
		.nextReq = 0U,
	},
	[PM_MASTER_APU_SLAVE_USB1] = {
		.slave = &pmSlaveUsb1_g.slv,
		.requestor = &pmMasterApu_g,
		.info = 0U,
		.currReq = 0U,
		.nextReq = 0U,
	},
	[PM_MASTER_APU_SLAVE_TTC0] = {
		.slave = &pmSlaveTtc0_g.slv,
		.requestor = &pmMasterApu_g,
		.info = 0U,
		.currReq = 0U,
		.nextReq = 0U,
	},
	[PM_MASTER_APU_SLAVE_SATA] = {
		.slave = &pmSlaveSata_g.slv,
		.requestor = &pmMasterApu_g,
		.info = 0U,
		.currReq = 0U,
		.nextReq = 0U,
	},
};

/* Requirement of RPU_0 master */
PmRequirement pmRpu0Req_g[PM_MASTER_RPU_0_SLAVE_MAX] = {
	[PM_MASTER_RPU_0_SLAVE_OCM0] = {
		.slave = &pmSlaveOcm0_g.slv,
		.requestor = &pmMasterRpu0_g,
		.info = 0U,
		.currReq = 0U,
		.nextReq = 0U,
	},
	[PM_MASTER_RPU_0_SLAVE_OCM1] = {
		.slave = &pmSlaveOcm1_g.slv,
		.requestor = &pmMasterRpu0_g,
		.info = 0U,
		.currReq = 0U,
		.nextReq = 0U,
	},
	[PM_MASTER_RPU_0_SLAVE_OCM2] = {
		.slave = &pmSlaveOcm2_g.slv,
		.requestor = &pmMasterRpu0_g,
		.info = 0U,
		.currReq = 0U,
		.nextReq = 0U,
	},
	[PM_MASTER_RPU_0_SLAVE_OCM3] = {
		.slave = &pmSlaveOcm3_g.slv,
		.requestor = &pmMasterRpu0_g,
		.info = PM_MASTER_USING_SLAVE_MASK,
		.currReq = PM_CAP_ACCESS | PM_CAP_CONTEXT,
		.nextReq = PM_CAP_ACCESS | PM_CAP_CONTEXT,
	},
	[PM_MASTER_RPU_0_SLAVE_TCM0A] = {
		.slave = &pmSlaveTcm0A_g.slv,
		.requestor = &pmMasterRpu0_g,
		.info = PM_MASTER_USING_SLAVE_MASK,
		.currReq = PM_CAP_ACCESS | PM_CAP_CONTEXT,
		.nextReq = PM_CAP_ACCESS | PM_CAP_CONTEXT,
	},
	[PM_MASTER_RPU_0_SLAVE_TCM0B] = {
		.slave = &pmSlaveTcm0B_g.slv,
		.requestor = &pmMasterRpu0_g,
		.info = PM_MASTER_USING_SLAVE_MASK,
		.currReq = PM_CAP_ACCESS | PM_CAP_CONTEXT,
		.nextReq = PM_CAP_ACCESS | PM_CAP_CONTEXT,
	},
	[PM_MASTER_RPU_0_SLAVE_TCM1A] = {
		.slave = &pmSlaveTcm1A_g.slv,
		.requestor = &pmMasterRpu0_g,
		.info = PM_MASTER_USING_SLAVE_MASK,
		.currReq = PM_CAP_ACCESS | PM_CAP_CONTEXT,
		.nextReq = PM_CAP_ACCESS | PM_CAP_CONTEXT,
	},
	[PM_MASTER_RPU_0_SLAVE_TCM1B] = {
		.slave = &pmSlaveTcm1B_g.slv,
		.requestor = &pmMasterRpu0_g,
		.info = PM_MASTER_USING_SLAVE_MASK,
		.currReq = PM_CAP_ACCESS | PM_CAP_CONTEXT,
		.nextReq = PM_CAP_ACCESS | PM_CAP_CONTEXT,
	},
	[PM_MASTER_RPU_0_SLAVE_USB0] = {
		.slave = &pmSlaveUsb0_g.slv,
		.requestor = &pmMasterRpu0_g,
		.info = 0U,
		.currReq = 0U,
		.nextReq = 0U,
	},
	[PM_MASTER_RPU_0_SLAVE_USB1] = {
		.slave = &pmSlaveUsb1_g.slv,
		.requestor = &pmMasterRpu0_g,
		.info = 0U,
		.currReq = 0U,
		.nextReq = 0U,
	},
	[PM_MASTER_RPU_0_SLAVE_TTC0] = {
		.slave = &pmSlaveTtc0_g.slv,
		.requestor = &pmMasterRpu0_g,
		.info = 0U,
		.currReq = 0U,
		.nextReq = 0U,
	},
	[PM_MASTER_RPU_0_SLAVE_SATA] = {
		.slave = &pmSlaveSata_g.slv,
		.requestor = &pmMasterRpu0_g,
		.info = 0U,
		.currReq = 0U,
		.nextReq = 0U,
	},
};

PmMaster pmMasterApu_g = {
	.procs = pmApuProcs_g,
	.procsCnt = PM_PROC_APU_MAX,
	.ipiMask = IPI_PMU_0_IER_APU_MASK,
	.ipiTrigMask = IPI_PMU_0_TRIG_APU_MASK,
	.pmuBuffer = IPI_BUFFER_PMU_BASE + IPI_BUFFER_TARGET_APU_OFFSET,
	.buffer = IPI_BUFFER_APU_BASE + IPI_BUFFER_TARGET_PMU_OFFSET,
	.reqs = pmApuReq_g,
	.reqsCnt = ARRAY_SIZE(pmApuReq_g),
};

PmMaster pmMasterRpu0_g = {
	.procs = &pmRpuProcs_g[PM_PROC_RPU_0],
	.procsCnt = 1,
	.ipiMask = IPI_PMU_0_IER_RPU_0_MASK,
	.ipiTrigMask = IPI_PMU_0_TRIG_RPU_0_MASK,
	.pmuBuffer = IPI_BUFFER_PMU_BASE + IPI_BUFFER_TARGET_RPU_0_OFFSET,
	.buffer = IPI_BUFFER_RPU_0_BASE + IPI_BUFFER_TARGET_PMU_OFFSET,
	.reqs = pmRpu0Req_g,
	.reqsCnt = ARRAY_SIZE(pmRpu0Req_g),
};

PmMaster pmMasterRpu1_g = {
	.procs = &pmRpuProcs_g[PM_PROC_RPU_1],
	.procsCnt = 1,
	.ipiMask = IPI_PMU_0_IER_RPU_1_MASK,
	.ipiTrigMask = IPI_PMU_0_TRIG_RPU_1_MASK,
	.pmuBuffer = IPI_BUFFER_PMU_BASE + IPI_BUFFER_TARGET_RPU_1_OFFSET,
	.buffer = IPI_BUFFER_RPU_1_BASE + IPI_BUFFER_TARGET_PMU_OFFSET,
	.reqs = NULL,   /* lockstep mode is assumed for now */
	.reqsCnt = 0,
};

static const PmMaster *const pmAllMasters[PM_MASTER_MAX] = {
	[PM_MASTER_APU] = &pmMasterApu_g,
	[PM_MASTER_RPU_0] = &pmMasterRpu0_g,
	[PM_MASTER_RPU_1] = &pmMasterRpu1_g,
};

/**
 * PmRequirementSchedule() - Schedule requirements of the master for slave.
 *                           Slave state will be updated according to the
 *                           requirement once primary processor goes to sleep.
 * @masterReq   Pointer to master requirement structure (for a slave)
 * @caps        Required capabilities of slave to be set once primary core
 *              goes to sleep.
 *
 * @return      Status of the operation
 */
u32 PmRequirementSchedule(PmRequirement* const masterReq, const u32 caps)
{
	u32 status;

	/* Check if slave has a state with requested capabilities */
	status = PmCheckCapabilities(masterReq->slave, caps);
	if (PM_RET_SUCCESS != status) {
		status = PM_RET_ERROR_NOTSUPPORTED;
		goto done;
	}

	/* Schedule setting of the requirement for later */
	masterReq->nextReq = caps;

done:
	return status;
}

/**
 * PmRequirementUpdate() - Set slaves capabilities according to the master's
 * requirements
 * @masterReq   Pointer to structure keeping informations about the
 *              master's requirement
 * @caps        Capabilities of a slave requested by the master
 *
 * @return      Status of the operation
 */
u32 PmRequirementUpdate(PmRequirement* const masterReq, const u32 caps)
{
	u32 status;
	u32 tmpCaps;

	PmDbg("%s\n", __func__);
	/* Check if slave has a state with requested capabilities */
	status = PmCheckCapabilities(masterReq->slave, caps);

	PmDbg("PmCheckCapabilities status=%d\n", status);
	if (PM_RET_SUCCESS != status) {
		goto done;
	}

	/* Configure requested capabilities */
	tmpCaps = masterReq->currReq;
	masterReq->currReq = caps;
	status = PmUpdateSlave(masterReq->slave);

	PmDbg("PmUpdateSlave status=%d\n", status);

	if (PM_RET_SUCCESS == status) {
		/* All capabilities requested in active state are constant */
		masterReq->nextReq = masterReq->currReq;
	} else {
		/* Remember the last setting, will report an error */
		masterReq->currReq = tmpCaps;
	}

done:
	return status;
}

/**
 * PmRequirementUpdateScheduled() - Triggers the setting for scheduled requirements
 * @master  Master which changed the state and whose scheduled requirements are
 *          triggered
 * @swap    Flag stating should current requirement be saved as next
 *
 * a) swap=false
 * Set scheduled requirements of a master without swapping current and
 * next requirements - means the current requirements will be dropped.
 * Upon every self suspend, master has to explicitly re-request
 * slave requirements.
 * b) swap=true
 * Set scheduled requirements of a master with swapping current and
 * next requirements (swapping means the current requirements will be
 * saved as next, and will be configured once master wakes-up)
 */
void PmRequirementUpdateScheduled(const PmMaster* const master, const bool swap)
{
	u32 status;
	PmRequirementId i;

	PmDbg("%s master %s\n", __func__, PmStrNode(master->procs[0].node.nodeId));

	for (i = 0; i < master->reqsCnt; i++) {
		if (master->reqs[i].currReq != master->reqs[i].nextReq) {
			u32 tmpReq = master->reqs[i].nextReq;

			if (true == swap) {
				/* Swap current and next requirements */
				master->reqs[i].nextReq = master->reqs[i].currReq;
			}

			master->reqs[i].currReq = tmpReq;

			/* Update slave setting */
			status = PmUpdateSlave(master->reqs[i].slave);
			/* if rom works correctly, status should be always ok */
			if (PM_RET_SUCCESS != status) {
				PmDbg("%s ERROR setting slave node %s\n",
				      __func__,
				      PmStrNode(master->reqs[i].slave->node.nodeId));
			}
		}
	}
}

/**
 * PmRequirementCancelScheduled() - Called when master aborts suspend, to cancel
 * scheduled requirements (slave capabilities requests)
 * @master  Master whose scheduled requests should be cancelled
 */
void PmRequirementCancelScheduled(const PmMaster* const master)
{
	PmRequirementId i;

	for (i = 0; i < master->reqsCnt; i++) {
		if (master->reqs[i].currReq != master->reqs[i].nextReq) {
			/* Drop the scheduled request by making it constant */
			PmDbg("%s %s\n", __func__,
			      PmStrNode(master->reqs[i].slave->node.nodeId));
			master->reqs[i].nextReq = master->reqs[i].currReq;
		}
	}
}

/**
 * PmRequirementReleaseAll() - Called when master primary processor is forced to
 *                             power down, so all requirements of the processor
 *                             are automatically released.
 * @master  Master whose primary processor was forced to power down
 */
void PmRequirementReleaseAll(const PmMaster* const master)
{
	u32 status;
	PmRequirementId i;

	for (i = 0; i < master->reqsCnt; i++) {
		master->reqs[i].currReq = 0U;
		master->reqs[i].nextReq = 0U;
		/* Update slave setting */
		status = PmUpdateSlave(master->reqs[i].slave);
		/* if pmu rom works correctly, status should be always ok */
		if (PM_RET_SUCCESS != status) {
			PmDbg("%s ERROR setting slave node %s\n", __func__,
			      PmStrNode(master->reqs[i].slave->node.nodeId));
		}
	}
}

/**
 * PmGetRequirementForSlave() - Get pointer to the master's request structure for
 *          a given slave
 * @master  Master whose request structure should be found
 * @nodeId  Slave nodeId
 *
 * @return  Pointer to the master's request structure dedicated to a slave with
 *          given node. If such structure is not found, it means the master is
 *          not allowed to use the slave.
 */
PmRequirement* PmGetRequirementForSlave(const PmMaster* const master,
					const PmNodeId nodeId)
{
	u32 i;
	PmRequirement *req = NULL;

	for (i = 0; i < master->reqsCnt; i++) {
		if (master->reqs[i].slave->node.nodeId == nodeId) {
			req = &master->reqs[i];
			break;
		}
	}

	return req;
}

/**
 * PmEnableAllMasterIpis() - Iterate through all masters and enable their IPI
 *                           interrupt
 */
void PmEnableAllMasterIpis(void)
{
	u8 i;

	for (i = 0U; i < ARRAY_SIZE(pmAllMasters); i++) {
		XPfw_RMW32(IPI_PMU_0_IER,
			   pmAllMasters[i]->ipiMask,
			   pmAllMasters[i]->ipiMask);
	}
}

/**
 * PmGetMasterByIpiMask() - Use to get pointer to master structure by ipi mask
 * @mask    IPI Mask of a master (requestor) in IPI registers
 *
 * @return  Pointer to a PmMaster structure or NULL if master is not found
 */
const PmMaster* PmGetMasterByIpiMask(const u32 mask)
{
	u32 i;
	const PmMaster *mst = NULL;

	for (i = 0U; i < ARRAY_SIZE(pmAllMasters); i++) {
		if (mask == pmAllMasters[i]->ipiMask) {
			mst = pmAllMasters[i];
			break;
		}
	}

	return mst;
}

/**
 * PmGetProcOfThisMaster() - Get processor pointer with given node id, if
 *          such processor exist within the master
 * @master  Master within which the search is performed
 * @nodeId  Node of the processor to be found
 *
 * @return  Pointer to processor with the given node id (which is within the
 *          master), or NULL if such processor is not found.
 */
PmProc* PmGetProcOfThisMaster(const PmMaster* const master,
			      const PmNodeId nodeId)
{
	u32 i;
	PmProc *proc = NULL;

	for (i = 0U; i < master->procsCnt; i++) {
		if (nodeId == master->procs[i].node.nodeId) {
			proc = &master->procs[i];
		}
	}

	return proc;
}

/**
 * PmGetProcOfOtherMaster() - Get pointer to the processor with given node id,
 *          by excluding given master from the search
 * @master  Master to be excluded from search
 * @nodeId  Node id of the processor to be found
 *
 * @return  Pointer to processor that is not within the master and which has
 *          given node id, or NULL if such processor is not found
 */
PmProc* PmGetProcOfOtherMaster(const PmMaster* const master,
			       const PmNodeId nodeId)
{
	u32 i;
	PmProc *proc = NULL;

	for (i = 0U; i < ARRAY_SIZE(pmAllMasters); i++) {
		u32 p;

		if (master == pmAllMasters[i]) {
			continue;
		}

		for (p = 0; p < pmAllMasters[i]->procsCnt; p++) {
			if (nodeId == pmAllMasters[i]->procs[p].node.nodeId) {
				proc = &pmAllMasters[i]->procs[p];
				goto done;
			}
		}
	}

done:
	return proc;
}

/**
 * PmGetProcByNodeId() - Get a pointer to processor structure by the node id
 * @nodeId  Node of the processor to be found
 *
 * @return  Pointer to a processor structure whose node is provided, or
 *          NULL if processor is not found
 */
PmProc* PmGetProcByNodeId(const PmNodeId nodeId)
{
	u32 i;
	PmProc *proc = NULL;

	for (i = 0U; i < ARRAY_SIZE(pmAllMasters); i++) {
		u32 p;

		for (p = 0U; p < pmAllMasters[i]->procsCnt; p++) {
			if (nodeId == pmAllMasters[i]->procs[p].node.nodeId) {
				proc = &pmAllMasters[i]->procs[p];
				goto done;
			}
		}
	}

done:
	return proc;
}

/**
 * PmGetProcByWfiStatus() - Get processor struct by wfi interrupt status
 * @mask    WFI interrupt mask read from GPI2 register
 *
 * @return  Pointer to a processor structure whose wfi mask is provided, or
 *          NULL if processor is not found
 */
PmProc* PmGetProcByWfiStatus(const u32 mask)
{
	u32 i;
	PmProc *proc = NULL;

	for (i = 0U; i < ARRAY_SIZE(pmAllMasters); i++) {
		u32 p;

		for (p = 0U; p < pmAllMasters[i]->procsCnt; p++) {
			if (mask & pmAllMasters[i]->procs[p].wfiStatusMask) {
				proc = &pmAllMasters[i]->procs[p];
				goto done;
			}
		}
	}

done:
	return proc;
}

/**
 * PmGetProcByWakeStatus() - Get proc struct by wake interrupt status
 * @mask    GIC wake mask read from GPI1 register
 *
 * @return  Pointer to a processor structure whose wake mask is provided
 */
PmProc* PmGetProcByWakeStatus(const u32 mask)
{
	u32 i;
	PmProc *proc = NULL;

	for (i = 0U; i < ARRAY_SIZE(pmAllMasters); i++) {
		u32 p;

		for (p = 0U; p < pmAllMasters[i]->procsCnt; p++) {
			if (mask & pmAllMasters[i]->procs[p].wakeStatusMask) {
				proc = &pmAllMasters[i]->procs[p];
				goto done;
			}
		}
	}

done:
	return proc;
}

/**
 * PmMasterGetAwakeProcCnt() - count how many processors within master are not
 *          in sleep state
 *          processor
 * @master  Pointer to the master whose awake processors are to be counted
 *
 * @return  Number of awake processors within a master
 */
u32 PmMasterGetAwakeProcCnt(const PmMaster* const master)
{
	u32 i;
	u32 activeCnt = 0U;

	for (i = 0U; i < master->procsCnt; i++) {
		if (PM_PROC_STATE_ACTIVE == master->procs[i].node.currState) {
			activeCnt++;
		}
	}

	return activeCnt;
}

/**
 * PmEnableProxyWake() - Enable scheduled wake-up sources in GIC Proxy
 * @master  Pointer to master whose scheduled wake-up sources should be enabled
 *
 * When FPD is powered down, wake-up sources are enabled in GIC Proxy
 */
void PmEnableProxyWake(PmMaster* const master)
{
	u32 i;

	PmDbg("%s %s\n", __func__, PmStrNode(master->procs->node.nodeId));

	for (i = 0; i < master->reqsCnt; i++) {
		if (master->reqs[i].info & PM_MASTER_WAKEUP_REQ_MASK) {
			PmSlaveWakeEnable(master->reqs[i].slave);
		}
	}
}

/**
 * PmWakeUpCancelScheduled() - Cancel scheduled wake-up sources of the master
 * @master  Pointer to a master whose scheduled wake-up sources should be
 *          cancelled
 */
static void PmWakeUpCancelScheduled(PmMaster* const master)
{
	u32 i;

	PmDbg("%s %s\n", __func__, PmStrNode(master->procs->node.nodeId));

	for (i = 0; i < master->reqsCnt; i++) {
		master->reqs[i].info &= ~PM_MASTER_WAKEUP_REQ_MASK;
	}
}

/**
 * PmWakeUpDisableAll() - Disable all wake-up sources of this master
 * @master  Pointer to a master whose wake-up sources are to be disabled
 */
static void PmWakeUpDisableAll(PmMaster* const master)
{
	u32 i;

	PmDbg("%s for %s\n", __func__, PmStrNode(master->procs->node.nodeId));

	for (i = 0; i < master->reqsCnt; i++) {
		PmMasterId r;
		bool hasOtherReq = false;

		if (0U == (master->reqs[i].info & PM_MASTER_WAKEUP_REQ_MASK)) {
			continue;
		}

		master->reqs[i].info &= ~PM_MASTER_WAKEUP_REQ_MASK;
		/* Check if there are other masters waiting for slave's wake-up */
		for (r = 0U; r < master->reqs[i].slave->reqsCnt; r++) {
			if (0U != (master->reqs[i].slave->reqs[r]->info &
				   PM_MASTER_WAKEUP_REQ_MASK)) {
				hasOtherReq = true;
				break;
			}
		}

		if (false == hasOtherReq) {
			/* No other masters waiting for wake, disable wake event */
			PmSlaveWakeDisable(master->reqs[i].slave);
		}
	}
}

/**
 * PmMasterNotify() - Notify master channel of a state change in its primary processor
 * @master      Pointer to master object which needs to be notified
 * @event       Processor Event to notify the master about
 *
 * Primary processor has changed its state, notify master to update its requirements
 * accordingly.
 */
void PmMasterNotify(PmMaster* const master, const PmProcEvent event)
{
	switch (event) {
	case PM_PROC_EVENT_SLEEP:
		PmRequirementUpdateScheduled(master, true);
		break;
	case PM_PROC_EVENT_ABORT_SUSPEND:
		PmRequirementCancelScheduled(master);
		PmWakeUpCancelScheduled(master);
		break;
	case PM_PROC_EVENT_WAKE:
		PmRequirementUpdateScheduled(master, false);
		PmWakeUpDisableAll(master);
		break;
	case PM_PROC_EVENT_FORCE_PWRDN:
		PmRequirementReleaseAll(master);
		PmWakeUpCancelScheduled(master);
		break;
	default:
		PmDbg("%s undefined event #%d\n", __func__, event);
		break;
	}
}