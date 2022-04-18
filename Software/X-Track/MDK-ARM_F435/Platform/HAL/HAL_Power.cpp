#include "HAL/HAL.h"

#if CONFIG_LIPO_FUEL_GAUGE_ENABLE

#include "BQ27220/BQ27220.h"
static BQ27220 fuel_gauge;
#endif

#define BATT_ADC                    ADC1
#define BATT_MIN_VOLTAGE            3300
#define BATT_MAX_VOLTAGE            4200
#define BATT_FULL_CHARGE_VOLTAGE    4100

#define BATT_CHG_DET_PULLUP         1
#if BATT_CHG_DET_PULLUP
#  define BATT_CHG_DET_PIN_MODE     INPUT_PULLUP
#  define BATT_CHG_DET_STATUS       (!digitalRead(CONFIG_BAT_CHG_DET_PIN))
#else
#  define BATT_CHG_DET_PIN_MODE     INPUT_PULLDOWN
#  define BATT_CHG_DET_STATUS       ((usage == 100) ? false : digitalRead(CONFIG_BAT_CHG_DET_PIN))
#endif

struct
{
    uint32_t LastHandleTime;
    uint16_t AutoLowPowerTimeout;
    bool AutoLowPowerEnable;
    bool ShutdownReq;
    uint16_t ADCValue;
    HAL::Power_CallbackFunction_t EventCallback;
} Power;

static void Power_ADC_Init(adc_type* ADCx)
{
#if CONFIG_LIPO_FUEL_GAUGE_ENABLE
    fuel_gauge.init();
    fuel_gauge.refreshData();
#else
    adc_common_config_type adc_common_struct;
    adc_base_config_type adc_base_struct;

    crm_periph_clock_enable(CRM_ADC1_PERIPH_CLOCK, TRUE);

    adc_common_default_para_init(&adc_common_struct);
    adc_common_struct.combine_mode = ADC_INDEPENDENT_MODE;
    adc_common_struct.div = ADC_HCLK_DIV_16;
    adc_common_struct.common_dma_mode = ADC_COMMON_DMAMODE_DISABLE;
    adc_common_struct.common_dma_request_repeat_state = FALSE;
    adc_common_struct.sampling_interval = ADC_SAMPLING_INTERVAL_10CYCLES;
    adc_common_struct.tempervintrv_state = FALSE;
    adc_common_struct.vbat_state = FALSE;
    adc_common_config(&adc_common_struct);

    adc_base_default_para_init(&adc_base_struct);
    adc_base_struct.sequence_mode = FALSE;
    adc_base_struct.repeat_mode = FALSE;
    adc_base_struct.data_align = ADC_RIGHT_ALIGNMENT;
    adc_base_struct.ordinary_channel_length = 1;
    adc_base_config(ADCx, &adc_base_struct);
    adc_resolution_set(ADCx, ADC_RESOLUTION_12B);

    adc_ordinary_conversion_trigger_set(ADCx, ADC_ORDINARY_TRIG_TMR1CH1, ADC_ORDINARY_TRIG_EDGE_NONE);

    adc_dma_mode_enable(ADCx, TRUE);
    adc_dma_request_repeat_enable(ADCx, FALSE);
    adc_interrupt_enable(ADCx, ADC_OCCO_INT, FALSE);

    adc_enable(ADCx, TRUE);
    while(adc_flag_get(ADCx, ADC_RDY_FLAG) == RESET);

    /* adc calibration */
    adc_calibration_init(ADCx);
    while(adc_calibration_init_status_get(ADCx));
    adc_calibration_start(ADCx);
    while(adc_calibration_status_get(ADCx));
#endif
}

static uint16_t Power_ADC_GetValue()
{
    uint16_t retval = 0;
    if(adc_flag_get(BATT_ADC, ADC_OCCE_FLAG))
    {
        retval = adc_ordinary_conversion_data_get(BATT_ADC);
    }
    return retval;
}

static void Power_ADC_Update()
{

#if CONFIG_LIPO_FUEL_GAUGE_ENABLE
    fuel_gauge.refreshData();
#else
    static bool isStartConv = false;

    if(!isStartConv)
    {
        adc_ordinary_channel_set(
            BATT_ADC,
            (adc_channel_select_type)PIN_MAP[CONFIG_BAT_DET_PIN].ADC_Channel,
            1,
            ADC_SAMPLETIME_47_5
        );

        adc_ordinary_software_trigger_enable(BATT_ADC, TRUE);
        isStartConv = true;
    }
    else
    {
        Power.ADCValue = Power_ADC_GetValue();
        isStartConv = false;
    }
#endif
}

void HAL::Power_Init()
{
    memset(&Power, 0, sizeof(Power));
    Power.AutoLowPowerTimeout = 60;

    Serial.printf("Power: Waiting[%dms]...\r\n", CONFIG_POWER_WAIT_TIME);
    pinMode(CONFIG_POWER_EN_PIN, OUTPUT);
    digitalWrite(CONFIG_POWER_EN_PIN, LOW);
    delay(CONFIG_POWER_WAIT_TIME);
    digitalWrite(CONFIG_POWER_EN_PIN, HIGH);
    Serial.println("Power: ON");

    Power_ADC_Init(BATT_ADC);
    pinMode(CONFIG_BAT_DET_PIN, INPUT_ANALOG);

#if !CONFIG_LIPO_FUEL_GAUGE_ENABLE
    pinMode(CONFIG_BAT_CHG_DET_PIN, BATT_CHG_DET_PIN_MODE);

//    Power_SetAutoLowPowerTimeout(5 * 60);
//    Power_HandleTimeUpdate();
    Power_SetAutoLowPowerEnable(false);
#else
    Power_SetAutoLowPowerTimeout(60);
    Power_HandleTimeUpdate();
    Power_SetAutoLowPowerEnable(true);
#endif
}

void HAL::Power_HandleTimeUpdate()
{
    Power.LastHandleTime = millis();
}

void HAL::Power_SetAutoLowPowerTimeout(uint16_t sec)
{
    Power.AutoLowPowerTimeout = sec;
}

uint16_t HAL::Power_GetAutoLowPowerTimeout()
{
    return Power.AutoLowPowerTimeout;
}

void HAL::Power_SetAutoLowPowerEnable(bool en)
{
    Power.AutoLowPowerEnable = en;
    Power_HandleTimeUpdate();
}

void HAL::Power_Shutdown()
{
    CM_EXECUTE_ONCE(Power.ShutdownReq = true);
}

void HAL::Power_Update()
{
    CM_EXECUTE_INTERVAL(Power_ADC_Update(), 1000);

    if(!Power.AutoLowPowerEnable)
        return;

#if CONFIG_LIPO_FUEL_GAUGE_ENABLE
    if((!fuel_gauge.should_power_off) && fuel_gauge.voltage > BATT_MIN_VOLTAGE)
    {
        Power_HandleTimeUpdate();
    }
#endif

    if(Power.AutoLowPowerTimeout == 0)
        return;

    if(millis() - Power.LastHandleTime >= (Power.AutoLowPowerTimeout * 1000))
    {
        Power_Shutdown();
    }
}

void HAL::Power_EventMonitor()
{
    if(Power.ShutdownReq)
    {
        if(Power.EventCallback)
        {
            Power.EventCallback();
        }
        Backlight_SetGradual(0, 500);
        digitalWrite(CONFIG_POWER_EN_PIN, LOW);
        Serial.println("Power: OFF");
        Power.ShutdownReq = false;
    }
}

void HAL::Power_GetInfo(Power_Info_t* info)
{
#if !CONFIG_LIPO_FUEL_GAUGE_ENABLE
    int voltage = map(
                      Power.ADCValue,
                      0, 4095,
                      0, 3300
                  );

    voltage *= 2;

    CM_VALUE_LIMIT(voltage, BATT_MIN_VOLTAGE, BATT_MAX_VOLTAGE);

    int usage = map(
                    voltage,
                    BATT_MIN_VOLTAGE, BATT_FULL_CHARGE_VOLTAGE,
                    0, 100
                );

    CM_VALUE_LIMIT(usage, 0, 100);

    info->usage = usage;
    info->isCharging = BATT_CHG_DET_STATUS;
    info->voltage = voltage;
#else
    info->voltage = fuel_gauge.voltage;
    info->usage = uint8_t(fuel_gauge.remaining_capacity * 100.0 / fuel_gauge.fullcharge_capacity);
    info->isCharging = (!digitalRead(CONFIG_BAT_CHG_DET_PIN)) && (fuel_gauge.battery_status == BQ27220::CHARGING || fuel_gauge.battery_status == BQ27220::FULL);
    if (info->isCharging)
    {
        info->time_to = fuel_gauge.time_to_full;
    }
    else
    {
        info->time_to = fuel_gauge.time_to_empty;
    }
    info->fullcharge_capacity = fuel_gauge.fullcharge_capacity;
    info->design_capacity = fuel_gauge.design_capacity;
    info->remaining_capacity = fuel_gauge.remaining_capacity;
    info->current = fuel_gauge.current;
    info->average_power = fuel_gauge.average_power;
#endif
}

void HAL::Power_RevertCapacity(uint16_t designCapacity, uint16_t fullChargeCapacity)
{
#if CONFIG_LIPO_FUEL_GAUGE_ENABLE
    fuel_gauge.setCapacity(designCapacity, fullChargeCapacity);
#endif
}


void HAL::Power_SetEventCallback(Power_CallbackFunction_t callback)
{
    Power.EventCallback = callback;
}
