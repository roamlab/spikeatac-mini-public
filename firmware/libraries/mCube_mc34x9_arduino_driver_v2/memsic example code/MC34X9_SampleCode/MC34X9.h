/*****************************************************************************
 *  Copyright Statement:
 *  --------------------
 *  This software is protected by Copyright and the information and source code
 *  contained herein is confidential. The software including the source code
 *  may not be copied and the information contained herein may not be used or
 *  disclosed except with the written permission of MEMSIC Inc. (C) 2019
 *****************************************************************************/

 /**
 * @brief
 * This file implement magnetic sensor driver APIs.
 * Modified history: 
 * V1.5: Update to 1 step factory selftest on 20190619
 */
#ifndef _MC34X9_H
#define _MC34X9_H 

#define SPI_BURST_READ_MAX_LEN  (24)


typedef enum 
{
  MC34X9_COM_I2C = 1,
  MC34X9_COM_SPI = 2,
} MC34X9_COM_INTF;
  

typedef   signed char  int8_t; 		// signed 8-bit number    (-128 to +127)
typedef unsigned char  uint8_t; 	// unsigned 8-bit number  (+0 to +255)
typedef   signed short int16_t; 	// signed 16-bt number    (-32768 to +32767)
typedef unsigned short uint16_t; 	// unsigned 16-bit number (+0 to +65535)
typedef   signed int   int32_t; 	// signed 32-bt number    (-2,147,483,648 to +2,147,483,647)
typedef unsigned int   uint32_t; 	// unsigned 32-bit number (+0 to +4,294,967,295)

#define MC34X9_7BITI2C_ADDRESS		0x4C

#define MC34X9_CHIP_ID_VAL		    0xA4

#define ENABLE 						1
#define DISABLE						0
#define MAX_FIFO_LENGTH  			30


#define MC34X9_REG_XOUT_8BIT 		0x00
#define MC34X9_REG_YOUT_8BIT		0x01
#define MC34X9_REG_ZOUT_8BIT		0x02

#define MC34X9_REG_STAT1			0x03
#define MC34X9_REG_INTR_STAT1		0x04
#define MC34X9_REG_DEV_STAT			0x05

#define MC34X9_REG_INT_EN			0x06
#define MC34X9_REG_MODE				0x07
#define MC34X9_REG_SAMPLE_RATE		0x08
#define MC34X9_REG_MOTION_CTRL		0x09

#define MC34X9_REG_FIFO_STAT		0x0A
#define MC34X9_REG_FIFO_RD_P		0x0B
#define MC34X9_REG_FIFO_WR_P		0x0C

#define MC34X9_REG_XOUT_EX_L		0x0D
#define MC34X9_REG_XOUT_EX_H		0x0E
#define MC34X9_REG_YOUT_EX_L		0x0F
#define MC34X9_REG_YOUT_EX_H		0x10
#define MC34X9_REG_ZOUT_EX_L		0x11
#define MC34X9_REG_ZOUT_EX_H		0x12

#define MC34X9_REG_STAT  			0x13
#define MC34X9_REG_INTR_STAT  		0x14

#define MC34X9_REG_CHIP_ID			0x18
#define MC34X9_REG_RESET			0x1C

#define MC34X9_REG_RANGE  			0x20

#define MC34X9_REG_XOFFL			0x21
#define MC34X9_REG_XOFFH			0x22
#define MC34X9_REG_YOFFL			0x23
#define MC34X9_REG_YOFFH			0x24
#define MC34X9_REG_ZOFFL			0x25
#define MC34X9_REG_ZOFFH			0x26

#define MC34X9_REG_XGAIN			0x27
#define MC34X9_REG_YGAIN			0x28
#define MC34X9_REG_ZGAIN			0x29

#define MC34X9_REG_FIFO_CTRL		0x2D
#define MC34X9_REG_FIFO_TH			0x2E
#define MC34X9_REG_FIFO_INTR		0x2F
#define MC34X9_REG_FIFO_CTRL2_SR2	0x30

#define MC34X9_REG_COMM_CTRL		0x31
#define MC34X9_REG_GPIO_CTRL		0x33
#define MC34X9_REG_SECURITY_OPT		0x3B

#define MC34X9_REG_TF_THRESH_LSB	0x40
#define MC34X9_REG_TF_THRESH_MSB	0x41
#define MC34X9_REG_TF_DB			0x42

#define MC34X9_REG_AM_THRESH_LSB	0x43
#define MC34X9_REG_AM_THRESH_MSB	0x44
#define MC34X9_REG_AM_DB			0x45

#define MC34X9_REG_SHK_THRESH_LSB	0x46
#define MC34X9_REG_SHK_THRESH_MSB	0x47
#define MC34X9_REG_SHK_DB			0x48

#define MC34X9_REG_PK_P2P_DUR_THRESH_LSB	0x49
#define MC34X9_REG_PK_P2P_DUR_THRESH_MSB	0x4A

#define MC34X9_REG_RD_CNT			0x4B

// 16-bit mode, null field output (32768)
#define	MC34X9_16BIT_OFFSET			32768


#define MC34X9_2G_SENSITIVITY		16384
#define MC34X9_4G_SENSITIVITY		8192
#define MC34X9_8G_SENSITIVITY		4096
#define MC34X9_12G_SENSITIVITY		2730
#define MC34X9_16G_SENSITIVITY		2048



typedef struct
{
    unsigned char    tilt;
    unsigned char    flip;
    unsigned char    anymotion;
    unsigned char    shake;
    unsigned char    tilt35;
    unsigned char    fifo;
    unsigned char 	 resv;
    unsigned char    acq;
}   MC34X9_InterruptEvent;

typedef enum
{
    MC34X9_SLEEP,
	MC34X9_WAKEMODE,
	MC34X9_RESERVED,
	MC34X9_STANDBYMODE,
}   MC34X9_MODE;



typedef enum
{
    MC34X9_25MZ,
	MC34X9_12MZ,
	MC34X9_6MZ,
	MC34X9_3MZ,
}   MC34X9_MOSF;



typedef enum
{
    MC34X9_2G,
	MC34X9_4G,
	MC34X9_8G,
	MC34X9_16G,
	MC34X9_12G,
}   MC34X9_RANGE;

typedef enum
{
    MC34X9_4fc=1,
	MC34X9_6fc=2,
	MC34X9_12fc=3,
	MC34X9_16fc=5,
}   MC34X9_LPF_DIVISOR;



typedef enum
{
 	FIFO_INT2,
 	FIFO_INT1,
}   MC34X9_COMBIN_INT;


typedef enum
{
	FIFO_NULL=0,
	FIFO_EMPTY=1,
 	FIFO_FULL=2,
 	FIFO_THR=4,
 	
}   FIFO_INT;
	

typedef enum
{
	FIFO_NORMAL_MODE,
 	FIFO_WATERMARK_MODE,
}   FIFO_MODE;
	

typedef enum
{
	MC34X9_HIGH_50HZ=0x08,
	MC34X9_HIGH_100HZ,
	MC34X9_HIGH_125HZ,
	MC34X9_HIGH_200HZ,
	MC34X9_HIGH_250HZ,
	MC34X9_HIGH_500HZ,
	MC34X9_HIGH_1000HZ,
	MC34X9_HIGH_2000HZ, // 0x0f
	
}   MC34X9_RATE;

/**
 * @brief Delay function.
 */
void Delay_Ms(int cnt);

/**
 * @brief I2C write register.
 */
int I2C_Write_Reg(unsigned char i2c_add, unsigned char reg_add, unsigned char cmd);

/**
 * @brief I2C read register.
 */
int I2C_Read_Reg(unsigned char i2c_add, unsigned char reg_add, unsigned char *data);

/**
 * @brief I2C multi read.
 */
int I2C_MultiRead_Reg(unsigned char i2c_add, unsigned char reg_add, int num, unsigned char *data);

/**
 * @brief SPI write register.
 */
int SPI_Write_Reg(unsigned char *tx_buf, unsigned char *rx_buf, unsigned short size);

/**
 * @brief SPI read register.
 */
int SPI_Read_Reg(unsigned char *tx_buf, unsigned char *rx_buf, unsigned short size);

/**
 * @brief SPI multi read.
 */
int SPI_MultiRead_Reg(unsigned char *tx_buf, unsigned char *rx_buf, unsigned short size);

	
typedef struct {
	unsigned short x,y,z;	
} MC34X9_DATA;


#endif



