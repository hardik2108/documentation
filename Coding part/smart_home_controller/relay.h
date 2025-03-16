#ifndef RELAY_H
#define RELAY_H

extern int temperature_limit_low;
extern int temperature_limit_high;
extern int humidity_limit_high;

#define RELAY_LIGHT_PIN 2
#define RELAY_HEATER_PIN 11
#define RELAY_HUMIDIFIER_PIN 1

void get_dht22_data(int *sensor_data);
void dht22_monitor_task();
void init_pir_sensor();

#endif