/********************************************************************************************************/
// SpikeatacMiniController - ROS-agnostic sensor controller for SpikeATac Mini
//
// This library handles initialization and data reading for:
//   - AD7147 capacitive touch sensor (CDC)
//   - MC3479 accelerometer
//   - AD7953 ADC for PVDF sensors
//
// This library is completely separated from ROS nodes, publishers, etc.
// For ROS integration, use SaTMiniControlBoardv1 which wraps this controller
// and handles all micro-ROS communication.
/********************************************************************************************************/
#ifndef SPIKEATAC_MINI_CONTROLLER_H
#define SPIKEATAC_MINI_CONTROLLER_H

#include <Wire.h>
#include <AD7147_v3_satmini.h>
#include <AD7147RegMap_v3_satmini.h>
#include <MC34X9_v2.h>
#include <vector>
#include <Utils.h>


#define SPEED_20M 20000000
#define SPEED_18M 18000000
#define SPEED_10M 10000000
#define SPEED_9M 9000000
#define SPEED_8M 8000000
#define SPEED_5M 5000000
#define SPEED_1M 1000000
// AD7953 can run up to 20MHz according to datasheet. But with 4 fingers connected, data is corrupted beyond 9MHz.
#define AD7953_SPI_SPEED SPEED_9M

/**
  The SpikeatacMiniConfig struct is meant to store all the relevant information
  to read data from a spikeatac sensor
*/
struct SpikeatacMiniConfig {
    String label;
    bool accel_on; // 
    bool accel_SPI;  // true if SPI, false if I2C
    uint8_t accel_chip_select; // SPI chip select pin
    uint8_t accel_device_address;  // i2c device address
    TwoWire* accel_wire;  // pointer to Wire, Wire1, Wire2
    uint8_t num_accel_signals;
    bool cdc_on;
    bool cdc_SPI;  // true if SPI, false if I2C
    uint8_t cdc_chip_select; // SPI chip select pin
    uint8_t cdc_device_address;
    TwoWire* cdc_wire;
    uint8_t num_cdc_signals;
    bool adc_on;  // adc samples PVDF, always uses SPI
    uint8_t adc_chip_select;
    uint8_t num_adc_signals;
};

/**
  The SpikeatacMiniController class is meant to handle the spikeatac initialization and sampling process.
  It is completely separated from ROS nodes, publishers, etc.
*/
class SpikeatacMiniController {
public:
    SpikeatacMiniController();
    ~SpikeatacMiniController();

    /**
    Initialize accelerometer and cdc. Uses I2C or SPI depending on what is specified in the config. 
    
    @param config the config struct
    @return boolean to indicate if initialization with ICs was a success or not.
    */
    bool start(SpikeatacMiniConfig *config);

    /**
    Reads the capacitive data from the AD7147 and stores it in the data array.
    It assumes there are 7 singletacts connected to the CDC at consistent ports (specified in the AD7147 library)
    @param data pointer to the array where the data will be stored
     */
    void readCapacitive(uint16_t *data);

    /**
    Reads the accelerometer data (x, y, and z) from the MC3479 and stores it in the data array.
    @param data pointer to the array where the data will be stored
     */
    void readAccel(int32_t *data);

    /**
    Reads the pvdf data (x, y, and z) from the ADC and stores it in the data array.
    @param data pointer to the array where the data will be stored
     */
    void readPvdf(int32_t *data);


private:
    MC34X9 mc3479 = MC34X9();
    AD7147 ad7147 = AD7147();
    uint8_t adcChipSelect_;
};

#endif // SPIKEATAC_MINI_CONTROLLER_H