// FreeRTOS
#include <FreeRTOS.h>
#include <task.h>

// Input/output
#include <stdio.h>

#include <stdbool.h>

// GPIO library
#include "bl_gpio.h"

// Timer/Delay
#include "bl_timer.h"

//Motion Sensor
#include "motion.h"

//DHT22 Sensor
#include "dht22.h"

#include "relay.h"

void relay_control_task() {
    bl_gpio_enable_output(RELAY_LIGHT_PIN, 0, 0);
    bl_gpio_enable_output(RELAY_HEATER_PIN, 0, 0);
    bl_gpio_enable_output(RELAY_HUMIDIFIER_PIN, 0, 0);

    while (1) {
        int dht22_data[2];
        get_dht22_data(dht22_data);
        int raw_temperature = dht22_data[0];
        int raw_humidity = dht22_data[1];
        int temperature = (raw_temperature + 5)  / 10;  // Rounding temperature
        int humidity = (raw_humidity + 5)  / 10;        // Rounding humidity
        printf("Temperature: %d.%d°C, Humidity: %d.%d%%\r\n", 
            raw_temperature / 10, raw_temperature % 10, 
            raw_humidity / 10, raw_humidity % 10);
        
        bool high_temperature = temperature > 28; 
        bool high_humidity = humidity > 60;
        bool low_temperature = temperature < 22;
        bool low_humidity = humidity < 36;

        if (high_temperature || high_humidity) {
            if (high_temperature){
                printf("Temperature too high!\r\n");
                bl_gpio_output_set(RELAY_HEATER_PIN, 1); //turn off heater
            }
            if (high_humidity){
                printf("Humidity too high!\r\n");
                bl_gpio_output_set(RELAY_HUMIDIFIER_PIN, 1); //turn off huumidifier
            } 
        } else if (low_temperature || low_humidity) {
            if (low_temperature){
                printf("Temperature too low!\r\n");
                bl_gpio_output_set(RELAY_HEATER_PIN, 0); //turn on heater
            }
            if (low_humidity){
                printf("Humidity too low!\r\n");
                bl_gpio_output_set(RELAY_HUMIDIFIER_PIN, 0); //turn on huumidifier
            }            
        }else {
            printf("Optimal temperature : Heater OFF.\r\n");
            bl_gpio_output_set(RELAY_HEATER_PIN, 1);
            bl_gpio_output_set(RELAY_HUMIDIFIER_PIN, 1);
        }

        if (read_pir_data()) {
            printf("Motion detected!\r\nLights ON\r\n");
            bl_gpio_output_set(RELAY_LIGHT_PIN, 0); // Turn light ON
        } else {
            printf("No motion detected!\r\nLights OFF\r\n");
            bl_gpio_output_set(RELAY_LIGHT_PIN, 1); // Turn light OFF
        }

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}