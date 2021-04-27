#ifndef _RSSI_LEDC_H_
#define _RSSI_LEDC_H_

#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"

#define LEDC_HS_TIMER          LEDC_TIMER_0
#define LEDC_HS_MODE           LEDC_HIGH_SPEED_MODE
#define LEDC_HS_CH0_GPIO       (16)
#define LEDC_HS_CH0_CHANNEL    LEDC_CHANNEL_0
#define LEDC_TEST_CH_NUM       (1)
#define LEDC_TIMER_FREQ        (5000)
#define LEDC_PWM_LEVEL_DEF     (4000)
#define LEDC_PWM_DELAY_FADE    (1000)

void rssi_fade_pwm_modes(short int channel_num, int *blink_pwm);
//void TaskRssiFadePwm(void *pvPatameters);
void rssi_ledc_pwm_fade_init(void);

#endif