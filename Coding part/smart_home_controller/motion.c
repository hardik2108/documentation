// FreeRTOS
#include <FreeRTOS.h>
#include <task.h>

// Input/output
#include <stdio.h>
#include <stdint.h>
#include <bl_uart.h>   

// GPIO library
#include <bl_gpio.h>

// Timer/Delay
#include "bl_timer.h"

#include "motion.h"

// GPIO pin connected to PIR sensor
#define PIR_PIN 4
int motion_detected = 0;

// Function to initialize the PIR sensor pin
void init_pir_sensor() {
    bl_gpio_enable_input(PIR_PIN, 0, 0); // Set PIR_PIN as input with no pull-up or pull-down
    printf("PIR sensor initialized on GPIO pin %d.\r\n", PIR_PIN);
}

// Function to read data from PIR motion sensor
int read_pir_data() {
    int state = bl_gpio_input_get_value(PIR_PIN);
    vTaskDelay(pdMS_TO_TICKS(50));
    return state == 1;
}

// PIR sensor monitoring task
void pir_monitor_task(void *pvParameters) {
    init_pir_sensor(); // Initialize PIR sensor

    while (1) {
        motion_detected = read_pir_data();  // Read PIR sensor data
        vTaskDelay(pdMS_TO_TICKS(5000));  // Delay for 2 seconds before next reading
    }
}
