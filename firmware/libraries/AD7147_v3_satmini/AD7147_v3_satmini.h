/********************************************************************************************************/
// I2C inteface for Teensy4.1
// Code comes partially from the AD7147 library by Analog Devices and some SPI code is from https://github.com/deanm1278/AD7147/tree/master
/********************************************************************************************************/
#ifndef AD7147_h
#define AD7147_h

#include <Wire.h>
#include <SPI.h>


#define AD7147_ADD 0x2C  // =0101 100
// #define AD7147_ADD 0x2D  // =0101 101
// #define AD7147_ADD 0x2E  // =0101 110
// #define AD7147_ADD 0x2F  // =0101 111
#define POWER_UP_INTERRUPT 0x000F

#define	BYTE	unsigned char
#define WORD	unsigned int
#define DWORD	unsigned long int

#define AD7147_SPI_SPEED_5M 5000000
#define AD7147_SPI_SPEED_1M 1000000
#define AD7147_SPI_SPEED_500K 500000
#define AD7147_SPI_SPEED_100K 100000
#define AD7147_SPI_SPEED_50K 50000
#define AD7147_SPI_SPEED_10K 10000
#define AD7147_SPI_SPEED_1K 1000

#define AD7147_SPI_SPEED AD7147_SPI_SPEED_5M  // AD7147 supports up to 5MHz SPI speed


class AD7147 {
  public:
    AD7147();
    // WORD AD7147Registers[24];	//[0...23 inc]=1st set of registers [0...23 inc]
								//Element[23] = 0x17 = ID register @ 0x17
    uint16_t AD7147Registers[24];  // temporary
    bool start(bool use_SPI, int address_chip_select, TwoWire *wire);
    void ReadSensors(void);


  private:
    bool use_SPI_;
    TwoWire *wire_;
    int address_;
    int chip_select_;
    void ConfigAD7147(void);
    bool ConfigWire(void);
    bool ConfigSPI(void);
    // void ReadFromAD7147(const WORD RegisterStartAddress, const BYTE NumberOfRegisters, WORD *DataBuffer, const WORD OffsetInBuffer);
    void ReadFromAD7147(const uint16_t RegisterStartAddress, const byte NumberOfRegisters, uint16_t *DataBuffer, const uint16_t OffsetInBuffer);
    // ^ temporary

    // void WriteToAD7147(const WORD RegisterStartAddress, const BYTE NumberOfRegisters, WORD *DataBuffer, const WORD OffsetInBuffer);
    void WriteToAD7147(const uint16_t RegisterStartAddress, const byte NumberOfRegisters, uint16_t *DataBuffer, const uint16_t OffsetInBuffer);
    // ^ temporary

    uint16_t getDeviceID();


};

#endif