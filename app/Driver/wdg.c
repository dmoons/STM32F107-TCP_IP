#include "wdg.h"
#include "products.h"
#include "stm32f10x.h"

#if WDT_ENABLE
void watchdog_init(void)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_256);
	IWDG_SetReload(1000);
	IWDG_ReloadCounter();
	IWDG_Enable();
}

#endif


