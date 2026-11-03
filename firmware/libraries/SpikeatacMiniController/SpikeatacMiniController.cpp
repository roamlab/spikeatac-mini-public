#include "SpikeatacMiniController.h"

SpikeatacMiniController::SpikeatacMiniController(){
    // do nothing
}

SpikeatacMiniController::~SpikeatacMiniController(){
    // Free all mallocs, delete all new operations
}

bool SpikeatacMiniController::start(SpikeatacMiniConfig* config){
    // Initialize sensors based on configuration, must be called after node init
    SPI1.setMISO(MISO1);
    SPI1.setMOSI(MOSI1);
    SPI1.setSCK(SCLK1);
    SPI1.begin();

    if (config->accel_on) {
        if (config->accel_SPI) {
            if (!mc3479.start(0, config->accel_chip_select, NULL)){
                return false;
            };
        } else
        if (!mc3479.start(1, config->accel_device_address, config->accel_wire)){
            return false;
        };
    }

    if (config->cdc_on) {
        if (config->cdc_SPI) {
            if (!ad7147.start(config->cdc_SPI, config->cdc_chip_select, NULL)){
                return false;
            };
        } else
        if (!ad7147.start(config->cdc_SPI, config->cdc_device_address, config->cdc_wire)){
            return false;
        };
    }


    if (config->adc_on) {
        pinMode(config->adc_chip_select, OUTPUT);
        adcChipSelect_ = config->adc_chip_select;

        SPI1.beginTransaction(SPISettings(AD7953_SPI_SPEED, MSBFIRST, SPI_MODE0));  // 4M
        digitalWriteFast(config->adc_chip_select, LOW); // Select ADC
        // Send command to set Auto-2 mode
        SPI1.transfer16(0x3C40); // 0011 11xx x100 xxxx - Sets the device into auto 2, with range 2 (0 to 2xVref) selected (bit 6=HIGH)
        // SPI1.transfer16(0x3C00); // 0011 11xx x000 xxxx - Sets the device into auto 2, with range 1 (0 to Vref) selected (bit 6=LOW)
        digitalWriteFast(config->adc_chip_select, HIGH); // Deselect ADC
        SPI1.endTransaction();
        
        SPI1.beginTransaction(SPISettings(AD7953_SPI_SPEED, MSBFIRST, SPI_MODE0));
        digitalWriteFast(config->adc_chip_select, LOW); // Select ADC
        // SPI1.transfer16(0x93C0); // 1001 xx11 11xx xxxx - Sets max register (1001) to 15 (1111). (see Table 6)
        SPI1.transfer16(0x90C0); // 1001 xx00 11xx xxxx - Sets max register (1001) to 3 (0011). (see Table 6)
        digitalWriteFast(config->adc_chip_select, HIGH); // Deselect ADC
        SPI1.endTransaction();
    }
    // error_loop(20);
    return true;
}


void SpikeatacMiniController::readCapacitive(uint16_t *data) {
    ad7147.ReadSensors();  // updates the AD7147Registers ADCRESULTS
    // SaT Mini has 4 CDC channels (stages 0-3)
    data[0] = ad7147.AD7147Registers[ADCRESULT_S0];
    data[1] = ad7147.AD7147Registers[ADCRESULT_S1];
    data[2] = ad7147.AD7147Registers[ADCRESULT_S2];
    data[3] = ad7147.AD7147Registers[ADCRESULT_S3];
}


void SpikeatacMiniController::readAccel(int32_t *data) { 
    MC34X9_acc_t rawAccel = mc3479.readRawAccel();
    std::vector<int32_t> acc_vec = {(int32_t) (rawAccel.XAxis_g * 1000.),
                                    (int32_t) (rawAccel.YAxis_g * 1000.),
                                    (int32_t) (rawAccel.ZAxis_g * 1000.)
                                    };
    data[0] = acc_vec[0];
    data[1] = acc_vec[1];
    data[2] = acc_vec[2];
}


void SpikeatacMiniController::readPvdf(int32_t *data) {

    // account for changing clock polarity if coming from a different SPI mode
    // https://forum.pjrc.com/index.php?threads/spi-clock-polarity-setup-not-long-enough.50442/
    // SPI1.beginTransaction(SPISettings(MC34X9_SPI_SPEED, MSBFIRST, SPI_MODE0)); -- I believe we don't need this here because we are going TO mode0. 
    // We definitely need if going from mode0 to mode3 (or 1 to 2), but I think not the other way around
    // SPI1.transfer(0x00);
    // SPI1.endTransaction();
  
    for (int i=0; i<4; i++) {
        SPI1.beginTransaction(SPISettings(AD7953_SPI_SPEED, MSBFIRST, SPI_MODE0));
        digitalWriteFast(adcChipSelect_, LOW);      // Select ADC
        uint16_t result = SPI1.transfer16(0x0000);
        digitalWriteFast(adcChipSelect_, HIGH);     // Deselect ADC
        SPI1.endTransaction();
        
        // we get the index by shifting the actual reading 12 bits to the right
        // SaT Mini has 4 PVDF channels (0-3)
        uint16_t idx = result >> 12;
        if (idx < 4) {
            data[idx] = result & 0x0FFF;  // first 4 bits are the channel number, next 12 are the 12 bit reading
        }
    }
}