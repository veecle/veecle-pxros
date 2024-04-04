/***************************************************************
 *   PXROS-HR Network Stack
 *   Copyright HighTec EDV-Systeme GmbH 2020
 ***************************************************************/

#include "PxEthDriverCreate.h"

#include "EthPeripherals.h"
#include "PxEthDriver.h"
#include "PxNetConfig.h"
#include "pxdef.h"

#include <string.h>


extern PxUInt_t PX_ETH_DATA_BASE;
extern PxUInt_t PX_ETH_DATA_END;

static const PxProtectRegion_T PxEthProtectRegion[] =
{
    { (unsigned long) ETH_MODULE_BASE,  (unsigned long) ETH_MODULE_END,  WRProtection },
    { (unsigned long) GPIO_MODULE_BASE, (unsigned long) GPIO_MODULE_END, WRProtection },
    { (unsigned long) SRC_MODULE_BASE,  (unsigned long) SRC_MODULE_END,  WRProtection },
    { (unsigned long) SCU_MODULE_BASE,  (unsigned long) SCU_MODULE_END,  WRProtection },
	{ (unsigned long) TMR_MODULE_BASE,  (unsigned long) TMR_MODULE_END,  WRProtection },
    { 0, 0, NoAccessProtection }
};

/* access rights of the task */
#define THISTASK_PXACCESS   PXACCESS_HANDLERS           \
                          | PXACCESS_INSTALL_HANDLERS   \
                          | PXACCESS_REGISTERS          \
                          | PXACCESS_RESOURCES

PxTask_t PxEthTaskCreate(PxPrio_t prio, PxEvents_t events, PxMc_t memClass, PxOpool_t objPool)
{
    PxTaskSpec_T    ts;
    PxTaskContext_T context;
    PxTask_t        task;

    memset(&ts, 0, sizeof(ts));

    /* readonly area will be inherited */
    context.protection[0].lowerBound = 0;
    context.protection[0].upperBound = 0;
    context.protection[0].prot = NoAccessProtection;
    context.protection[1].lowerBound = (unsigned int) &PX_ETH_DATA_BASE;
    context.protection[1].upperBound = (unsigned int) &PX_ETH_DATA_END;
    context.protection[1].prot = WRProtection;

    ts.ts_name = (const PxChar_t *)"GETH_Driver";
    ts.ts_fun = PxEthDriverFunction;
    ts.ts_mc = memClass;
    ts.ts_opool = objPool;
    ts.ts_taskstack.stk_type = PXStackAlloc;
    ts.ts_taskstack.stk_size = (PXETH_DRIVER_TASK_STACK_SIZE / sizeof(int));
    ts.ts_taskstack.stk_src.mc = memClass;

    /* Task interrupt stack is unused */
    ts.ts_inttaskstack.stk_type = PXStackFall;
    ts.ts_inttaskstack.stk_size = PXStackDontCheck;
    ts.ts_inttaskstack.stk_src.stk = (PxStackAligned_t *)0;

    ts.ts_prio = prio;
    ts.ts_context = &context;
    ts.ts_privileges = PXUser1Privilege;
    ts.ts_accessrights = THISTASK_PXACCESS;
    ts.ts_protect_region = PxEthProtectRegion;

    task = PxTaskCreate(objPool, &ts, prio, events);

    return task;
}
