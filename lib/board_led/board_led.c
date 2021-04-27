/* board.c - Board-specific hooks */

/*
 * Copyright (c) 2017 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>

#include "driver/gpio.h"
#include "esp_log.h"
#include "board_led.h"

#define TAG "BOARD"

struct _led_state led_state[SIZE_LED_STATE_AR] = {{ LED_OFF, LED_OFF, LED_B, "blue" }};

void board_led_operation(uint8_t pin, uint8_t onoff){
    for (uint8_t i = 0; i < SIZE_LED_STATE_AR; i++) {
        if (led_state[i].pin != pin) {
            continue;
        }
        if (onoff == led_state[i].current) {
            ESP_LOGW(TAG, "led %s is already %s",
                     led_state[i].name, (onoff ? "on" : "off"));
            return;
        }
        gpio_set_level(pin, onoff);
        led_state[i].previous = led_state[i].current;
        led_state[i].current = onoff;
        return;
    }
    ESP_LOGE(TAG, "LED is not found!");
}

static void board_led_init(void){
    for(uint8_t i = 0; i < SIZE_LED_STATE_AR; i++){
        gpio_pad_select_gpio(led_state[i].pin);
        gpio_set_direction(led_state[i].pin, GPIO_MODE_OUTPUT);
        gpio_set_level(led_state[i].pin, LED_OFF);
        led_state[i].previous = LED_OFF;
        led_state[i].current = LED_OFF;
    }
}

void board_init(void)
{
    board_led_init();
}
