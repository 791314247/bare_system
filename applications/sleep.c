#include "sleep.h"

// Sleep
void Sleep(void)
{
    LL_PMU_SleepInitTypeDef LPM_InitStruct;
    LL_RCC_SetSleepModeRCLPWorkMode(LL_RCC_RCLP_UNDER_SLEEP_CLOSE);//�����¹ر�RCLP
    LL_RMU_EnablePowerDownReset(RMU);//��PDR
    LL_RMU_DisableBORPowerDownReset(RMU);//�ر�BOR 2uA

    LL_ADC_Disable(ADC);//�ر�ADC
    LL_VREF_DisableVREF(VREF);//�ر�VREF1p2
    WRITE_REG(VREF->BUFCR, 0);//�ر�ȫ��VREFbuf

    LPM_InitStruct.DeepSleep = LL_PMU_SLEEP_MODE_NOMAL;
    LPM_InitStruct.PowerMode = LL_PMU_POWER_MODE_SLEEP_AND_DEEPSLEEP;
    LPM_InitStruct.WakeupFrequency = LL_PMU_SLEEP_WAKEUP_FREQ_RCHF_8MHZ;
    LPM_InitStruct.WakeupDelay = LL_PMU_WAKEUP_DELAY_TIME_2US;
    LPM_InitStruct.CoreVoltageScaling = DISABLE;
    LL_PMU_Sleep_Init(PMU, &LPM_InitStruct);
}

//DeepSleep
void DeepSleep(void)
{
    LL_PMU_SleepInitTypeDef LPM_InitStruct;
    LL_RCC_SetSleepModeRCLPWorkMode(LL_RCC_RCLP_UNDER_SLEEP_CLOSE);//�����¹ر�RCLP
    LL_RMU_EnablePowerDownReset(RMU);//��PDR
    LL_RMU_DisableBORPowerDownReset(RMU);//�ر�BOR 2uA

    LL_ADC_Disable(ADC);//�ر�ADC
    LL_VREF_DisableVREF(VREF);//�ر�VREF1p2
    WRITE_REG(VREF->BUFCR, 0);//�ر�ȫ��VREFbuf

    LPM_InitStruct.DeepSleep = LL_PMU_SLEEP_MODE_DEEP;
    LPM_InitStruct.PowerMode = LL_PMU_POWER_MODE_SLEEP_AND_DEEPSLEEP;
    LPM_InitStruct.WakeupFrequency = LL_PMU_SLEEP_WAKEUP_FREQ_RCHF_8MHZ;
    LPM_InitStruct.WakeupDelay = LL_PMU_WAKEUP_DELAY_TIME_2US;
    LPM_InitStruct.CoreVoltageScaling = DISABLE;
    LL_PMU_Sleep_Init(PMU, &LPM_InitStruct);
}

