#include "Wdt.h"

#define LOOP_TIME_SLOWSCANMODE          (300u)
#define ILO_CLOCK_FACTOR                (40u)
#define WDT_TIMEOUT_SLOW_SCAN           (ILO_CLOCK_FACTOR * LOOP_TIME_SLOWSCANMODE)


static Wdt_t Wdt;

Wdt_t *WDT_Status(void)
{
    return &Wdt;
}

void Wdt_Feed_Dog(void)
{
    Wdt.clear = true;
}

CY_ISR(WDTIMER_ISR)
{
    if(Wdt.clear) //初始化第一次必须清
    {
        /* Clear the watchdog interrupt */
        CySysWdtClearInterrupt();
        isr_wdt_ClearPending();  
        /* WDT match value is updated in order to obtain periodic interrupts */
        CySysWdtWriteMatch(CySysWdtReadMatch() + WDT_TIMEOUT_SLOW_SCAN);
        Wdt.clear = false;
    }

    /* Set to variable that indicates that WDT interrupt had triggered*/
    Wdt.Isr = true;  
}

void WDT_Init(void)
{
    Wdt.clear = true;
    Wdt.Isr = false;
    
    CySysWdtDisable();	
    /* Map the WatchDog_ISR vector to the WDT_ISR */
    isr_wdt_StartEx(WDTIMER_ISR);  
	/* Update the match register for generating a periodic WDT ISR.
    Note: In order to do a periodic ISR using WDT, Match value needs to be
    updated every WDT ISR with the desired time value added on top of the
    existing match value */
    CySysWdtWriteMatch(CySysWdtReadMatch() + WDT_TIMEOUT_SLOW_SCAN);
    /* Enable the WDT interrupt in SRSS INTR mask register */
    CySysWdtUnmaskInterrupt();
    /* Start the WDT */
    CySysWdtEnable();
}


