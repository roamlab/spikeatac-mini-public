/********************************************************************************
    Controller sketch for SaTMiniControlBoardv1 and rigid on-finger PCB stack v2 and v3
********************************************************************************/

/********************************************************************************
**************************  INCLUDES AND DEFINES ********************************
********************************************************************************/

#include <SaTMiniControlBoardv1.h>

/********************************************************************************
*****************************  GLOBAL VARIABLES  ********************************
********************************************************************************/

const uint8_t NUM_OF_SPIKEATACS = 1;

SpikeatacMiniConfig* get_spikeatac_config(){
  bool accel_on = true;
  bool cdc_on = true;
  bool adc_on = true;

  static SpikeatacMiniConfig* spikeatac_config = NULL;
  if (!spikeatac_config) {
    spikeatac_config = new SpikeatacMiniConfig[NUM_OF_SPIKEATACS];

    // sensor connected to J3
    spikeatac_config[0].label = "SpikeATacX";
    spikeatac_config[0].accel_on = accel_on;
    spikeatac_config[0].accel_SPI = true;
    spikeatac_config[0].accel_chip_select = 32;  // J3=32, J4=37, J5=20, J6=5
    spikeatac_config[0].accel_device_address = 0x4C;  // ignored for SPI
    spikeatac_config[0].accel_wire = &Wire1;  // ignored for SPI
    spikeatac_config[0].num_accel_signals = 3;
    spikeatac_config[0].cdc_on = cdc_on;
    spikeatac_config[0].cdc_SPI = true;
    spikeatac_config[0].cdc_chip_select = 31;  // J3=31, J4=36, J5=19, J6=4
    spikeatac_config[0].cdc_device_address = 0x2C;  // ignored for SPI
    spikeatac_config[0].cdc_wire = &Wire;  // ignored for SPI
    spikeatac_config[0].num_cdc_signals = 4;  // SaT Mini has 4 CDC channels (0-3)
    spikeatac_config[0].adc_on = adc_on;
    spikeatac_config[0].adc_chip_select = 30;  // adc can only use SPI, J3=30, J4=33, J5=18, J6=3
    spikeatac_config[0].num_adc_signals = 4;  // SaT Mini has 4 PVDF channels (0-3)

    // sensor connected to J4
    // spikeatac_config[1].label = "SpikeATacX";
    // spikeatac_config[1].accel_on = accel_on;
    // spikeatac_config[1].accel_SPI = true;
    // spikeatac_config[1].accel_chip_select = 37;  // J3=32, J4=37, J5=20, J6=5
    // spikeatac_config[1].accel_device_address = 0x4C;  // ignored for SPI
    // spikeatac_config[1].accel_wire = &Wire1;  // ignored for SPI
    // spikeatac_config[1].num_accel_signals = 3;
    // spikeatac_config[1].cdc_on = cdc_on;
    // spikeatac_config[1].cdc_SPI = true;
    // spikeatac_config[1].cdc_chip_select = 36;  // J3=31, J4=36, J5=19, J6=4
    // spikeatac_config[1].cdc_device_address = 0x2C;  // ignored for SPI
    // spikeatac_config[1].cdc_wire = &Wire;  // ignored for SPI
    // spikeatac_config[1].num_cdc_signals = 4;
    // spikeatac_config[1].adc_on = adc_on;
    // spikeatac_config[1].adc_chip_select = 33; // adc can only use SPI, J3=30, J4=33, J5=18, J6=3
    // spikeatac_config[1].num_adc_signals = 4;

    // sensor connected to J5
    // spikeatac_config[2].label = "SpikeATacX";
    // spikeatac_config[2].accel_on = accel_on;
    // spikeatac_config[2].accel_SPI = true;
    // spikeatac_config[2].accel_chip_select = 20;  // J3=32, J4=37, J5=20, J6=5
    // spikeatac_config[2].accel_device_address = 0x4C;  // ignored for SPI
    // spikeatac_config[2].accel_wire = &Wire1;  // ignored for SPI
    // spikeatac_config[2].num_accel_signals = 3;
    // spikeatac_config[2].cdc_on = cdc_on;
    // spikeatac_config[2].cdc_SPI = true;
    // spikeatac_config[2].cdc_chip_select = 19;  // J3=31, J4=36, J5=19, J6=4
    // spikeatac_config[2].cdc_device_address = 0x2C;  // ignored for SPI
    // spikeatac_config[2].cdc_wire = &Wire;  // ignored for SPI
    // spikeatac_config[2].num_cdc_signals = 4;
    // spikeatac_config[2].adc_on = adc_on;
    // spikeatac_config[2].adc_chip_select = 18; // adc can only use SPI, J3=30, J4=33, J5=18, J6=3
    // spikeatac_config[2].num_adc_signals = 4;

    // sensor connected to J6
    // spikeatac_config[3].label = "SpikeATacX";
    // spikeatac_config[3].accel_on = accel_on;
    // spikeatac_config[3].accel_SPI = true;
    // spikeatac_config[3].accel_chip_select = 5;  // J3=32, J4=37, J5=20, J6=5
    // spikeatac_config[3].accel_device_address = 0x4C;  // ignored for SPI
    // spikeatac_config[3].accel_wire = &Wire1;  // ignored for SPI
    // spikeatac_config[3].num_accel_signals = 3;
    // spikeatac_config[3].cdc_on = cdc_on;
    // spikeatac_config[3].cdc_SPI = true;
    // spikeatac_config[3].cdc_chip_select = 4;  // J3=31, J4=36, J5=19, J6=4
    // spikeatac_config[3].cdc_device_address = 0x2C;  // ignored for SPI.
    // spikeatac_config[3].cdc_wire = &Wire;  // ignored for SPI
    // spikeatac_config[3].num_cdc_signals = 4;
    // spikeatac_config[3].adc_on = adc_on;
    // spikeatac_config[3].adc_chip_select = 3; // adc can only use SPI, J3=30, J4=33, J5=18, J6=3
    // spikeatac_config[3].num_adc_signals = 4;
  }

  return spikeatac_config;
}

SaTMiniControlBoardv1* get_controller() {
  static SaTMiniControlBoardv1* controller = NULL;
  if (!controller) {
    controller = new SaTMiniControlBoardv1(
      4000,                   // spikeatac_cap_tick_us -- 4000 us = 250 Hz
      500,                    // spikeatac_accel_pvdf_tick_us -- 500 us = 2000 Hz
      NUM_OF_SPIKEATACS,      // Number of spikeatac sensors connected
      get_spikeatac_config()  // spikeatac sensor information
    );
  }
  return controller;
}

/********************************************************************************
******************************  PROGRAM CODE  ***********************************
********************************************************************************/

void setup() {
    if(!get_controller()->initialize()){
        while(1);
    }
}

void loop() {
  get_controller()->spinStateMachine();
}
