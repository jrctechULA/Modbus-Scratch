//____________________________________________________________________________________________________
// Include section:
//____________________________________________________________________________________________________
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"

#include "JRC_WiFi.h"

#include "mbcontroller.h"

#include "esp_log.h"
#include "esp_err.h"

//____________________________________________________________________________________________________
// Macro definitions:
//____________________________________________________________________________________________________

#define SLAVE_IP_ADDRESS "172.16.0.4"
#define ledGreen 36

#define STR(fieldname) ((const char*)( fieldname ))
#define OPTS(min_val, max_val, step_val) { .opt1 = min_val, .opt2 = max_val, .opt3 = step_val }

//____________________________________________________________________________________________________
// Global declarations:
//____________________________________________________________________________________________________
static const char *TAG = "MASTER_TEST";

esp_netif_t* esp_netif_ptr = NULL;

// Enumeration of modbus slave addresses accessed by master device
enum {
    MB_DEVICE_ADDR1 = 1,
    MB_SLAVE_COUNT
};

// Enumeration of all supported CIDs for device
enum {
    CID_HOLDING_1 = 0,
    CID_HOLDING_2,
    CID_COIL_1,
    CID_DISCRETE_1
};

// Modbus Dictionary
const mb_parameter_descriptor_t device_parameters[] = {
    // CID, Name, Units, Modbus addr, register type, Modbus Reg Start Addr, Modbus Reg read length,
    // Instance offset (NA), Instance type, Instance length (bytes), Options (NA), Permissions
    { CID_HOLDING_1,                    // CID
      STR("Holding_1"),                 // Param Name
      STR("--"),                        // Units
      MB_DEVICE_ADDR1,                  // Modbus Slave Addr
      MB_PARAM_HOLDING,                 // Modbus Reg Type
      0,                                // Reg Start
      1,                                // Reg Size
      0,                                // Instance Offset
      PARAM_TYPE_U16,                   // Data Type
      2,                                // Data Size
      OPTS( 0,0,0 ),                    // Parameter options MIN-MAX-STEP
      PAR_PERMS_READ_WRITE_TRIGGER      // Access Mode
    },

    { CID_HOLDING_2,                    // CID
      STR("Holding_2"),                 // Param Name
      STR("--"),                        // Units
      MB_DEVICE_ADDR1,                  // Modbus Slave Addr
      MB_PARAM_HOLDING,                 // Modbus Reg Type
      1,                                // Reg Start
      1,                                // Reg Size
      0,                                // Instance Offset
      PARAM_TYPE_U16,                   // Data Type
      2,                                // Data Size
      OPTS( 0,0,0 ),                    // Parameter options MIN-MAX-STEP
      PAR_PERMS_READ_WRITE_TRIGGER      // Access Mode
    },

    { CID_COIL_1,                       // CID
      STR("Coil_1"),                    // Param Name
      STR("on/off"),                    // Units
      MB_DEVICE_ADDR1,                  // Modbus Slave Addr
      MB_PARAM_COIL,                    // Modbus Reg Type
      0,                                // Reg Start
      1,                                // Reg Size
      0,                                // Instance Offset
      PARAM_TYPE_U8,                    // Data Type
      1,                                // Data Size
      OPTS( 0,0,0 ),                    // Parameter options MIN-MAX-STEP
      PAR_PERMS_READ_WRITE_TRIGGER      // Access Mode
    },

    { CID_DISCRETE_1,                    // CID
      STR("Discrete_1"),                 // Param Name
      STR("on/off"),                     // Units
      MB_DEVICE_ADDR1,                   // Modbus Slave Addr
      MB_PARAM_DISCRETE,                 // Modbus Reg Type
      0,                                 // Reg Start
      1,                                 // Reg Size
      0,                                 // Instance Offset
      PARAM_TYPE_U8,                     // Data Type
      1,                                 // Data Size
      OPTS( 0,0,0 ),                     // Parameter options MIN-MAX-STEP
      PAR_PERMS_READ_WRITE_TRIGGER       // Access Mode
    },
};

// Calculate number of parameters in the table
uint16_t num_device_parameters = (sizeof(device_parameters) / sizeof(device_parameters[0]));

//____________________________________________________________________________________________________
// Function prototypes:
//____________________________________________________________________________________________________
esp_err_t modbus_master_init(void);


//____________________________________________________________________________________________________
// Main program:
//____________________________________________________________________________________________________
void app_main(void)
{
    // GPIO Initialization
    gpio_reset_pin(ledGreen);
    gpio_set_direction(ledGreen, GPIO_MODE_OUTPUT);
    gpio_set_level(ledGreen, 0);

    // Connect to Router
    JRC_WiFi_Begin();

    // Modbus Stack Init
    ESP_ERROR_CHECK(modbus_master_init());

    uint16_t temp_data[1] = {0};    // temporary buffer to hold maximum CID size
    uint16_t temp_data2[1] = {150}; // temporary buffer to hold maximum CID size
    uint8_t temp_data3[1] = {0};    // temporary buffer to hold maximum CID size
    uint8_t temp_data4[1] = {0};    // temporary buffer to hold maximum CID size
    uint8_t type = 0;               //Type of parameter
    int cid;
    esp_err_t err;

    const mb_parameter_descriptor_t* param_descriptor = NULL;
    
    while(true){    
        // Get the information for characteristic cid from data dictionary
        cid = 1;
        err = mbc_master_get_cid_info(cid, &param_descriptor);
        if ((err != ESP_ERR_NOT_FOUND) && (param_descriptor != NULL)) {
            err = mbc_master_get_parameter(param_descriptor->cid, (char*)param_descriptor->param_key, (uint8_t*)temp_data, &type);
            if (err == ESP_OK) {
                ESP_LOGI(TAG, "Characteristic #%d %s (%s) value = (%u) read successful.",
                                param_descriptor->cid,
                                (char*)param_descriptor->param_key,
                                (char*)param_descriptor->param_units,
                                *(uint16_t*)temp_data);
            } else {
                ESP_LOGE(TAG, "Characteristic #%d (%s) read fail, err = 0x%x (%s).",
                                param_descriptor->cid,
                                (char*)param_descriptor->param_key,
                                (int)err,
                                (char*)esp_err_to_name(err));
            }
        } else {
            ESP_LOGE(TAG, "Could not get information for characteristic %d.", cid);
        }

        if (temp_data[0] < 1000)
            temp_data3[0] = 0;
        else
            temp_data3[0] = 1;
     

        // Get the information for characteristic cid from data dictionary
        cid = 3;
        err = mbc_master_get_cid_info(cid, &param_descriptor);
        if ((err != ESP_ERR_NOT_FOUND) && (param_descriptor != NULL)) {
            err = mbc_master_get_parameter(param_descriptor->cid, (char*)param_descriptor->param_key, (uint8_t*)temp_data4, &type);
            if (err == ESP_OK) {
                ESP_LOGI(TAG, "Characteristic #%d %s (%s) value = (%u) read successful.",
                                param_descriptor->cid,
                                (char*)param_descriptor->param_key,
                                (char*)param_descriptor->param_units,
                                *(uint8_t*)temp_data4);
            } else {
                ESP_LOGE(TAG, "Characteristic #%d (%s) read fail, err = 0x%x (%s).",
                                param_descriptor->cid,
                                (char*)param_descriptor->param_key,
                                (int)err,
                                (char*)esp_err_to_name(err));
            }
        } else {
            ESP_LOGE(TAG, "Could not get information for characteristic %d.", cid);
        }

        gpio_set_level(ledGreen, temp_data4[0]);


        err = mbc_master_set_parameter(CID_COIL_1, "Coil_1", (uint8_t*)temp_data3, &type);
        if (err == ESP_OK) {
            ESP_LOGI(TAG, "Set parameter data successfully.");
        } else {
            ESP_LOGE(TAG, "Set data fail, err = 0x%x (%s).", (int)err, (char*)esp_err_to_name(err));
        }

        err = mbc_master_set_parameter(CID_HOLDING_1, "Holding_1", (uint8_t*)temp_data2, &type);
        if (err == ESP_OK) {
            ESP_LOGI(TAG, "Set parameter data successfully.");
        } else {
            ESP_LOGE(TAG, "Set data fail, err = 0x%x (%s).", (int)err, (char*)esp_err_to_name(err));
        }
        temp_data2[0]++;

        printf("\n\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


//____________________________________________________________________________________________________
// Function implementations:
//____________________________________________________________________________________________________

esp_err_t modbus_master_init(void){
    void* master_handler = NULL; // Pointer to allocate interface structure
    // Initialization of Modbus master for serial port
    esp_err_t err = mbc_master_init_tcp(&master_handler);
    if (master_handler == NULL || err != ESP_OK) {
        ESP_LOGE(TAG, "mb controller initialization fail.");
    }

    const char* slave_ip_address_table[3] = {
        SLAVE_IP_ADDRESS,     // Address corresponds to UID1 and set to predefined value by user
        NULL               // end of table
    };

    const mb_communication_info_t comm_info = {
        .ip_port = 502,                    // Modbus TCP port number (default = 502)
        .ip_addr_type = MB_IPV4,                   // version of IP protocol
        .ip_mode = MB_MODE_TCP,                    // Port communication mode
        .ip_addr = (void*)slave_ip_address_table,  // assign table of IP addresses
        .ip_netif_ptr = esp_netif_ptr              // esp_netif_ptr pointer to the corresponding network interface
    };

    ESP_ERROR_CHECK(mbc_master_setup((void*)&comm_info));
    
    ESP_ERROR_CHECK(mbc_master_set_descriptor(&device_parameters[0], num_device_parameters));

    err = mbc_master_start();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "mb controller start fail, err=%x.", err);
    }
    return err;
}