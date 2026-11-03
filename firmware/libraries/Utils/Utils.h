#ifndef Utils_h
#define Utils_h

#include <Arduino.h>
// #include <Logger.h>

// void error_loop();
// void error_loop(uint16_t freq=10);

void error_loop(uint16_t freq = 10);
#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop(5);}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}

void debug_loop(int time_1, int time_2);

#define SPIKEATAC_ERROR 25
#define OPTIC_ERROR 15

#define MOSI1 26
#define MISO1 39
#define SCLK1 27

#endif
