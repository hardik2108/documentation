#ifndef MOTION_H
#define MOTION_H

extern int motion_detected;

void init_pir_sensor();
void pir_monitor_task();
int read_pir_data();

#endif