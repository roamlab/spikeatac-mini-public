// interface to read from singletact and connect to Teensy4.1 over I2C
/******************************************************************************/

#include "AD7147RegMap_v3_satmini.h"
#include "AD7147_v3_satmini.h"
#include "Utils.h"


AD7147::AD7147(){}

bool AD7147::start(bool use_SPI, int address_chip_select, TwoWire *wire)
{
	use_SPI_ = use_SPI;
	wire_ = wire;
	bool success;
	if (use_SPI) {
		// Serial.println("calling ConfigSPI");
		chip_select_ = address_chip_select;
		success = ConfigSPI();
	}
	else {
		address_ = address_chip_select;
		success = ConfigWire();
	}

	if (success) ConfigAD7147();
	return success;
}

bool AD7147::ConfigWire(void)
{
	// I2C
	wire_->begin();
	wire_->setClock(400000);  // 400 Khz. AD7147 datasheet says max i2c clock is 400khz...
	// wire_->setClock(1000000);  // 1 Mhz, but I was still able to read data when upclocking to 1MHz
	// getDeviceID();
	return true;
}

bool AD7147::ConfigSPI(void)
{
	// SPI
	pinMode(chip_select_, OUTPUT);
	digitalWrite(chip_select_, HIGH);

	// Serial.println("getting device ID");
	if (getDeviceID() == 0) {
		return false;
	}

	return true;
}


uint16_t AD7147::getDeviceID(){
	AD7147Registers[DEVID] = 0x00;
	this->ReadFromAD7147(DEVID, 1, AD7147Registers, DEVID);
	return AD7147Registers[DEVID];
}


void AD7147::ConfigAD7147 (void)
{
	//==========================================
	//============ Bank 2 Registers ============
	//==========================================

	// WORD StageBuffer[8];
	uint16_t StageBuffer[8];  // temporary
	
	//======================
	//= Stage 0 - CIN0 (+), offsets ENABLED =
	//======================
	StageBuffer[0] = 0x3FFE;  //Register 0x80 - CIN0 positive input, see table 34
	StageBuffer[1] = 0x1FFF;  //Register 0x81 - SE=01, both offsets enabled
	StageBuffer[2] = 0x0000;  //Register 0x82 - no offsets
	// StageBuffer[2] = 0x3FBF;  //Register 0x82 - 1 LSB=0.32pF, see table 36
	// StageBuffer[2] = 0x5200;  //Register 0x82 - 1 LSB=0.32pF, see table 36
	StageBuffer[3] = 0x2626;  //Register 0x83 - sensitivity, see table 37
	StageBuffer[4]=600;
	StageBuffer[5]=500;
	StageBuffer[6]=600;
	StageBuffer[7]=600;
	this->WriteToAD7147(STAGE0_CONNECTION, 8, StageBuffer, 0);

	//======================
	//= Stage 1 - CIN1 (+), offsets ENABLED =
	//======================
	StageBuffer[0] = 0x3FFB;
	StageBuffer[1] = 0x1FFF;
	StageBuffer[2] = 0x0000;  //Register 0x82 - no offsets
	// StageBuffer[2] = 0x3FBF;
	StageBuffer[3] = 0x2626;
	StageBuffer[4]=950;
	StageBuffer[5]=800;
	StageBuffer[6]=950;
	StageBuffer[7]=950;
	this->WriteToAD7147(STAGE1_CONNECTION, 8, StageBuffer, 0);

	//======================
	//= Stage 2 - CIN2 (+), offsets ENABLED =
	//======================
	StageBuffer[0] = 0x3FEF;
	StageBuffer[1] = 0x1FFF;
	StageBuffer[2] = 0x0000;  //Register 0x82 - no offsets
	// StageBuffer[2] = 0x3FBF;
	StageBuffer[3] = 0x2626;
	StageBuffer[4]=1400;
	StageBuffer[5]=1200;
	StageBuffer[6]=1400;
	StageBuffer[7]=1400;
	this->WriteToAD7147(STAGE2_CONNECTION, 8, StageBuffer, 0);

	//======================
	//= Stage 3 - CIN3 (+), offsets ENABLED =
	//======================
	StageBuffer[0] = 0x3FBF;
	StageBuffer[1] = 0x1FFF;
	StageBuffer[2] = 0x0000;  //Register 0x82 - no offsets
	// StageBuffer[2] = 0x3FBF;  //Register 0x82 - POS_AFE_OFFSET=10pF applied to POS input, 1 LSB=0.32pF, see table 36
	StageBuffer[3] = 0x2626;
	StageBuffer[4]=2000;
	StageBuffer[5]=1800;
	StageBuffer[6]=2000;
	StageBuffer[7]=2000;
	this->WriteToAD7147(STAGE3_CONNECTION, 8, StageBuffer, 0);




	//===========================
	//= Stages 4-11 - Not Connected, offsets DISABLED =
	//===========================
	StageBuffer[0]=0xFFFF;   //Register 0xA0/0xA8/.../0xD8 - all CIN = BIAS
	StageBuffer[1]=0xCFFF;   //SE=00 (do not use), NEG_DISABLE=1, POS_DISABLE=1
	StageBuffer[2]=0x0000;
	StageBuffer[3]=0x2626;
	StageBuffer[4]=650;  StageBuffer[5]=500;  StageBuffer[6]=650;  StageBuffer[7]=650;
	this->WriteToAD7147(STAGE4_CONNECTION, 8, StageBuffer, 0);

	StageBuffer[4]=3350; StageBuffer[5]=3000; StageBuffer[6]=3350; StageBuffer[7]=3350;
	this->WriteToAD7147(STAGE5_CONNECTION, 8, StageBuffer, 0);

	StageBuffer[4]=1150; StageBuffer[5]=1000; StageBuffer[6]=1150; StageBuffer[7]=1150;
	this->WriteToAD7147(STAGE6_CONNECTION, 8, StageBuffer, 0);

	StageBuffer[4]=1800; StageBuffer[5]=1600; StageBuffer[6]=1800; StageBuffer[7]=1800;
	this->WriteToAD7147(STAGE7_CONNECTION, 8, StageBuffer, 0);

	StageBuffer[4]=2400; StageBuffer[5]=2200; StageBuffer[6]=2400; StageBuffer[7]=2400;
	this->WriteToAD7147(STAGE8_CONNECTION, 8, StageBuffer, 0);

	StageBuffer[4]=3400; StageBuffer[5]=3200; StageBuffer[6]=3400; StageBuffer[7]=3400;
	this->WriteToAD7147(STAGE9_CONNECTION, 8, StageBuffer, 0);

	StageBuffer[4]=4400; StageBuffer[5]=4200; StageBuffer[6]=4400; StageBuffer[7]=4400;
	this->WriteToAD7147(STAGE10_CONNECTION, 8, StageBuffer, 0);

	StageBuffer[4]=5000; StageBuffer[5]=4800; StageBuffer[6]=5000; StageBuffer[7]=5000;
	this->WriteToAD7147(STAGE11_CONNECTION, 8, StageBuffer, 0);


//==========================================
//============ Bank 1 Registers ============
//==========================================
	// 0000 0000 0110 0000 = 0x0060 -- full power mode, 7 stages in seq, 256 decimation 
	// 0000 0011 0110 0000 = 0x0360 -- full power mode, 7 stages in seq, 64 decimation
	// 0000 0011 0011 0000 = 0x0330 -- full power mode, 4 stages in seq, 64 decimation
	// 0001 0011 0011 0000 = 0x1330 -- disable excitation source
	// 0000 0011 0110 0011 = 0x0363 -- shutdown


	AD7147Registers[PWR_CONTROL]=0x0330; //Register 0x00 -- full power mode, bits [7:4] changes # stages in seq, see table 20
	// AD7147Registers[PWR_CONTROL]=0x0363;

	this->WriteToAD7147(PWR_CONTROL, 1, AD7147Registers, PWR_CONTROL);

	AD7147Registers[AMB_COMP_CTRL0]=0x3230; //Register 0x02  -- keep for now, depends on # stages
	AD7147Registers[AMB_COMP_CTRL1]=0x0419; //Register 0x03  -- from datasheet
	AD7147Registers[AMB_COMP_CTRL2]=0x0832; //Register 0x04  -- from datasheet

	// change these to desired int behavior
	AD7147Registers[STAGE_LOW_INT_EN]=POWER_UP_INTERRUPT; //0x05
	AD7147Registers[STAGE_HIGH_INT_EN]=0x000F; //Register 0x06
	AD7147Registers[STAGE_COMPLETE_INT_EN]=0x0000; //Register 0x07
	this->WriteToAD7147(AMB_COMP_CTRL0, 6, AD7147Registers,
	AMB_COMP_CTRL0);

	AD7147Registers[STAGE_CAL_EN]=0x000F;					//0x007F = enable stages 0-6, skip 3 samples
	this->WriteToAD7147(STAGE_CAL_EN, 1, AD7147Registers, STAGE_CAL_EN);
	//Read High and Low Limit Status registers to clear INT pin
	this->ReadFromAD7147(STAGE_LOW_LIMIT_INT, 2, AD7147Registers, STAGE_LOW_LIMIT_INT); //Registers 0x08 & 0x09

}


void AD7147::ReadSensors(void)
{
	// Read stage complete registers  -- what's this do
	ReadFromAD7147(STAGE_COMPLETE_LIMIT_INT, 1, AD7147Registers, STAGE_COMPLETE_LIMIT_INT);
	
	// This is how we access the ADC Values
	// AD7147Registers[ADCRESULT_S0] corresponds to ADC Value of Stage 0

	//Read all ADC Values
	this->ReadFromAD7147(ADCRESULT_S0, 7, this->AD7147Registers, ADCRESULT_S0);
}



// void AD7147::ReadFromAD7147(const WORD RegisterStartAddress, const BYTE NumberOfRegisters, WORD *DataBuffer, const WORD OffsetInBuffer)
void AD7147::ReadFromAD7147(const uint16_t RegisterStartAddress, const byte NumberOfRegisters, uint16_t *DataBuffer, const uint16_t OffsetInBuffer)
{
	if (use_SPI_){
		uint16_t ControlValue;
		byte RegisterIndex;

		//Create the 16-bool header
		ControlValue = 0xE400 | (RegisterStartAddress & 0x03FF);
		
		SPI1.beginTransaction(SPISettings(AD7147_SPI_SPEED, MSBFIRST, SPI_MODE0));  // AD7147 supports 5MHz
		// Serial.println("chip select: ");
		// Serial.println(chip_select_);
		digitalWriteFast(chip_select_, LOW);
		
		// Serial.println(ControlValue, BIN);
		SPI1.transfer16(ControlValue);
		// SPI1.transfer16(0x0000);  // dummy write to clock out data
		
		for (RegisterIndex=0; RegisterIndex<NumberOfRegisters; RegisterIndex++)
		{
			// *(DataBuffer+OffsetInBuffer+RegisterIndex) = SPI1.transfer16(0x00);
			DataBuffer[OffsetInBuffer+RegisterIndex] = SPI1.transfer16(0x0000);
		}
		
		digitalWriteFast(chip_select_, HIGH);
		SPI1.endTransaction();
	}
	else {  // i2c
		BYTE LowByteData, HighByteData;

		// set register address
		wire_->beginTransmission(address_);
		wire_->write(highByte(RegisterStartAddress));
		wire_->write(lowByte(RegisterStartAddress));
		uint8_t wire_return = wire_->endTransmission();
		(void) wire_return;


		wire_->requestFrom(address_, NumberOfRegisters*2);
		
		BYTE r = 0;
		while(wire_->available()) {
			HighByteData = wire_->read();
			LowByteData = wire_->read();

			DataBuffer[OffsetInBuffer+r] = word(HighByteData, LowByteData);

			r++;
		}
	}
}


// void AD7147::WriteToAD7147(const WORD RegisterStartAddress, const BYTE NumberOfRegisters, WORD *DataBuffer, const WORD OffsetInBuffer)
void AD7147::WriteToAD7147(const uint16_t RegisterStartAddress, const byte NumberOfRegisters, uint16_t *DataBuffer, const uint16_t OffsetInBuffer)
{
	if (use_SPI_) {
		uint16_t ControlValue;
		uint16_t ValueToWrite;
		byte RegisterIndex;

		//Create the 16-bool header
		ControlValue = 0xE000 | (RegisterStartAddress & 0x03FF);
		
		SPI1.beginTransaction(SPISettings(AD7147_SPI_SPEED, MSBFIRST, SPI_MODE0));  // AD7147 supports 5MHz
		digitalWriteFast(chip_select_, LOW);
		SPI1.transfer16(ControlValue);
		
		for (RegisterIndex=0; RegisterIndex<NumberOfRegisters; RegisterIndex++)
		{
			// *(DataBuffer+OffsetInBuffer+RegisterIndex) = SPI1.transfer16(0x00);
			ValueToWrite = DataBuffer[OffsetInBuffer+RegisterIndex];
			SPI1.transfer16(ValueToWrite);
		}
		
		digitalWriteFast(chip_select_, HIGH);
		SPI1.endTransaction();
	}
	else { // i2c
		wire_->beginTransmission(address_);  

		wire_->write(highByte(RegisterStartAddress));
		wire_->write(lowByte(RegisterStartAddress));
		BYTE r = 0;
		for (r = 0; r < NumberOfRegisters; r++) {
			wire_->write(highByte(DataBuffer[OffsetInBuffer+r]));
			wire_->write(lowByte(DataBuffer[OffsetInBuffer+r]));
		}
		
		uint8_t wire_return = wire_->endTransmission();
		(void) wire_return;

		// It would be nice to use the Wire.write(pointer, bytes) function
		// Not sure how to use this when the pointer is a WORD. 
		// Wire.write() must take a uint8_t* but casting to uint8_t* is not correct
		// Wire.write((uint16_t*) (DataBuffer + OffsetInBuffer), NumberOfRegistersToWrite*2);
	}
}
