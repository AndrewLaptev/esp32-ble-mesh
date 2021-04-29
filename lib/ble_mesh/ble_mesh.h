#ifndef _BLE_MESH_H_
#define _BLE_MESH_H_

#include "esp_log.h"
#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_common_api.h"
#include "esp_ble_mesh_networking_api.h"
#include "esp_ble_mesh_provisioning_api.h"
#include "esp_ble_mesh_config_model_api.h"
#include "esp_ble_mesh_generic_model_api.h"
#include "esp_ble_mesh_local_data_operation_api.h"

#define CID_ESP 0x02E5
#define TAG "BLE-MESH"

static uint8_t dev_uuid[16];

void prov_complete(uint16_t net_idx, uint16_t addr, uint8_t flags, uint32_t iv_index);
void example_change_led_state(esp_ble_mesh_model_t *model, esp_ble_mesh_msg_ctx_t *ctx, uint8_t onoff);
void example_handle_gen_onoff_msg(esp_ble_mesh_model_t *model, esp_ble_mesh_msg_ctx_t *ctx, esp_ble_mesh_server_recv_gen_onoff_set_t *set);
void example_ble_mesh_provisioning_cb(esp_ble_mesh_prov_cb_event_t event, esp_ble_mesh_prov_cb_param_t *param);
void example_ble_mesh_generic_server_cb(esp_ble_mesh_generic_server_cb_event_t event, esp_ble_mesh_generic_server_cb_param_t *param);
void example_ble_mesh_config_server_cb(esp_ble_mesh_cfg_server_cb_event_t event, esp_ble_mesh_cfg_server_cb_param_t *param);
esp_err_t ble_mesh_init(void);

#endif