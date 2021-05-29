#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_gap_ble_api.h"

#include "bt.h"
#include "board_led.h"
#include "ble_mesh.h"
#include "rssi_ledc.h"
#include "rssi_calc.h"
#include "adapt_bandpass_filter.h"
#include "kalman_filter.h"

// parameters for adapt_bandpass_filter():
#define WIND_SIZE 5
#define A 0.015
#define B 0.013

// parameters for kalman_filter():
#define Q 0.2624915     // process_noise
#define R 68.749999     // sensor_noise
#define P 20.08333375   // estimated_error
#define INIT_RSSI -71

uint8_t ble_addr[16] = {127,42,198,70,155,141,73,148,158,162,131,135,46,55,63,158};
int rssi_filtered;

static esp_ble_scan_params_t scan_params = {
    .scan_type              = BLE_SCAN_TYPE_PASSIVE,
	.own_addr_type          = BLE_ADDR_TYPE_PUBLIC,
	.scan_filter_policy     = BLE_SCAN_FILTER_ALLOW_ALL,
	.scan_interval          = 0x50,
	.scan_window            = 0x30
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                            FUNC DECLARATION                                           //
///////////////////////////////////////////////////////////////////////////////////////////////////////////

static void rssi_check_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
void TaskRssiFadePwm(void *pvPatameters);

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                  MAIN                                                 //
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void app_main(void)
{
    esp_err_t err;

    ESP_LOGI(TAG, "Initializing...");

    board_init();

    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    err = bluetooth_init();
    if (err) {
        ESP_LOGE(TAG, "esp32_bluetooth_init failed (err %d)", err);
        return;
    }

    /* Initialize the BLE scan */
    esp_ble_gap_register_callback(rssi_check_cb);

    err = esp_ble_gap_set_scan_params(&scan_params);
    if(err != ESP_OK){
        ESP_LOGE("TEST", "esp_ble_gap_set_scan_params: rc=%d", err);
    }

    err = esp_ble_gap_start_scanning(0);
    if(err != ESP_OK){
        ESP_LOGE("TEST", "esp_ble_gap_start_scanning: rc=%d", err);
    }

    ble_mesh_get_dev_uuid(dev_uuid);
    ESP_LOGI("TEST","BT_MAC: %x:%x:%x:%x:%x:%x", dev_uuid[2],dev_uuid[3],dev_uuid[4],dev_uuid[5],dev_uuid[6],dev_uuid[7]);
    
    /* Initialize the Bluetooth Mesh Subsystem */
    err = ble_mesh_init();
    if (err) {
        ESP_LOGE(TAG, "Bluetooth mesh init failed (err %d)", err);
    }

    rssi_ledc_pwm_fade_init();
    xTaskCreate(TaskRssiFadePwm, "PWM_fade", 2048, NULL, 2, NULL);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                            FUNC DEFINITION                                            //
///////////////////////////////////////////////////////////////////////////////////////////////////////////


static void rssi_check_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param){
    short int uuid_compare_numb = 0;

    //static int buffer_arr[WIND_SIZE];   // for adapt_bandpass_filter
    //short int static i = 0;             //
    
    for(short int i = 0; i < 16; i++){
        if(param->scan_rst.ble_adv[i+6] == ble_addr[i]){
            uuid_compare_numb++;
        }
    }
    if(param -> scan_rst.search_evt == ESP_GAP_SEARCH_INQ_RES_EVT){
        if(uuid_compare_numb == 16){
            ESP_LOGI("TEST","UUID: %02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x, MAC: %02x:%02x:%02x:%02x:%02x:%02x, RSSI %d",
            param->scan_rst.ble_adv[6],
            param->scan_rst.ble_adv[7],
            param->scan_rst.ble_adv[8],
            param->scan_rst.ble_adv[9],
            param->scan_rst.ble_adv[10],
            param->scan_rst.ble_adv[11],
            param->scan_rst.ble_adv[12],
            param->scan_rst.ble_adv[13],
            param->scan_rst.ble_adv[14],
            param->scan_rst.ble_adv[15],
            param->scan_rst.ble_adv[16],
            param->scan_rst.ble_adv[17],
            param->scan_rst.ble_adv[18],
            param->scan_rst.ble_adv[19],
            param->scan_rst.ble_adv[20],
            param->scan_rst.ble_adv[21],
			param->scan_rst.bda[0],
			param->scan_rst.bda[1],
			param->scan_rst.bda[2],
			param->scan_rst.bda[3],
			param->scan_rst.bda[4],
			param->scan_rst.bda[5],
			param->scan_rst.rssi
            );

            //ESP_LOGI("TEST", "DISTANCE: %lf", rssi_distance_calculate(param->scan_rst.rssi, -64.5, 2));

            // Adaptive Band-Pass filter:
            /*if(i < 5){
                buffer_arr[i] = param->scan_rst.rssi;
                i++;
            }else if(i == 5){
                init_buffer(buffer_arr, buffer_arr, WIND_SIZE);
                i++;
            }else{
                ESP_LOGI("TEST", "%d", adapt_bandpass_filter(param->scan_rst.rssi, buffer_arr, WIND_SIZE, A, B));
            }*/

            // Kalman filter:
            ESP_LOGI("TEST", "%d", (int)round(Kalman_filter(Q, R, P, INIT_RSSI, param->scan_rst.rssi)));
        }
    }else{
        ESP_LOGE("TEST","RSSI not read!");
    }
}

void TaskRssiFadePwm(void *pvPatameters){
    while(1){
        rssi_fade_pwm_modes(0, &rssi_filtered);
        //rssi_fade_pwm_onoff(0, &rssi_filtered);
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
}