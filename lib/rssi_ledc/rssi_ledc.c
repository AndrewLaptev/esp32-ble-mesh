#include "rssi_ledc.h"

ledc_timer_config_t ledc_timer = {
    .duty_resolution = LEDC_TIMER_13_BIT, // resolution of PWM duty
    .freq_hz = LEDC_TIMER_FREQ,           // frequency of PWM signal
    .speed_mode = LEDC_HS_MODE,           // timer mode
    .timer_num = LEDC_HS_TIMER,            // timer index
    .clk_cfg = LEDC_AUTO_CLK,              // Auto select the source clock
};

ledc_channel_config_t ledc_channel[LEDC_TEST_CH_NUM] = {
    {
        .channel    = LEDC_HS_CH0_CHANNEL,
        .duty       = 0,
        .gpio_num   = LEDC_HS_CH0_GPIO,
        .speed_mode = LEDC_HS_MODE,
        .hpoint     = 0,
        .timer_sel  = LEDC_HS_TIMER
    }
};

void rssi_ledc_pwm_fade_init(void){
    ledc_timer_config(&ledc_timer);
    for (short int channel_num = 0; channel_num < LEDC_TEST_CH_NUM; channel_num++) {
        ledc_channel_config(&ledc_channel[channel_num]);
    }
    ledc_fade_func_install(0);
}

void rssi_fade_pwm_modes(short int channel_num, int *rssi_mean){
    static short int mode_cur = 0;
    if((*rssi_mean > -91) && (*rssi_mean < -87) && mode_cur != 1){
        ledc_set_duty(ledc_channel[channel_num].speed_mode, ledc_channel[channel_num].channel, 0);
        ledc_update_duty(ledc_channel[channel_num].speed_mode, ledc_channel[channel_num].channel);
        ESP_LOGW("TEST", "mode OFF: %d", *rssi_mean);
        mode_cur = 1;
    }else if((*rssi_mean > -85) && (*rssi_mean < -75) && mode_cur != 2){
        ledc_set_fade_with_time(ledc_channel[channel_num].speed_mode,
                ledc_channel[channel_num].channel, 500, LEDC_PWM_DELAY_FADE);
        ledc_fade_start(ledc_channel[channel_num].speed_mode,
                ledc_channel[channel_num].channel, LEDC_FADE_NO_WAIT);
        ESP_LOGW("TEST", "mode LOW: %d", *rssi_mean);            
        mode_cur = 2;
    }else if((*rssi_mean > -70) && (*rssi_mean < -60) && mode_cur != 3){
        ledc_set_fade_with_time(ledc_channel[channel_num].speed_mode,
                ledc_channel[channel_num].channel, 2000, LEDC_PWM_DELAY_FADE);
        ledc_fade_start(ledc_channel[channel_num].speed_mode,
                ledc_channel[channel_num].channel, LEDC_FADE_NO_WAIT);
        ESP_LOGW("TEST", "mode LOW-MEDIUM: %d", *rssi_mean);
        mode_cur = 3;
    }else if((*rssi_mean > -55) && (*rssi_mean < -45) && mode_cur != 4){
        ledc_set_fade_with_time(ledc_channel[channel_num].speed_mode,
                ledc_channel[channel_num].channel, 4000, LEDC_PWM_DELAY_FADE);
        ledc_fade_start(ledc_channel[channel_num].speed_mode,
                ledc_channel[channel_num].channel, LEDC_FADE_NO_WAIT);
        ESP_LOGW("TEST", "mode MEDIUM: %d", *rssi_mean);
        mode_cur = 4;
    }else if((*rssi_mean > -40) && (*rssi_mean < -30) && mode_cur != 5){
        ledc_set_fade_with_time(ledc_channel[channel_num].speed_mode,
                ledc_channel[channel_num].channel, 8000, LEDC_PWM_DELAY_FADE);
        ledc_fade_start(ledc_channel[channel_num].speed_mode,
                ledc_channel[channel_num].channel, LEDC_FADE_NO_WAIT);
        ESP_LOGW("TEST", "mode HIGH: %d", *rssi_mean);
        mode_cur = 5;
    }
}

void rssi_fade_pwm_onoff(short int channel_num, int *rssi_mean){
    static short int onoff = 0;
    if(*rssi_mean >= -50 && onoff == 0){
        ledc_set_fade_with_time(ledc_channel[channel_num].speed_mode,
                ledc_channel[channel_num].channel, 8191, LEDC_PWM_DELAY_FADE);
        ledc_fade_start(ledc_channel[channel_num].speed_mode,
                ledc_channel[channel_num].channel, LEDC_FADE_NO_WAIT);
        ESP_LOGW("TEST", "LIGHT-ON: %d", *rssi_mean);
        onoff = 1; 
    }else if(*rssi_mean < -60 && onoff == 1){
        ledc_set_fade_with_time(ledc_channel[channel_num].speed_mode,
                ledc_channel[channel_num].channel, 0, LEDC_PWM_DELAY_FADE);
        ledc_fade_start(ledc_channel[channel_num].speed_mode,
                ledc_channel[channel_num].channel, LEDC_FADE_NO_WAIT);
        ESP_LOGW("TEST", "LIGHT-OFF: %d", *rssi_mean);
        onoff = 0;
    }
}