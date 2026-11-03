/*****************************************************************************
 *  Copyright Statement:
 *  --------------------
 *  This software is protected by Copyright, and the information and source code
 *  contained herein is confidential. The software including the source code
 *  may not be copied and the information contained herein may not be used or
 *  disclosed except with the written permission of MEMSIC Inc. (C) 2020
 *****************************************************************************/

/**
 * @brief
 * This file implement magnetic sensor driver APIs. 
 * Modified history: 
 * V1.1: Add the interrupt function on 20190214
 * V1.2: Add the DEVICE_ID_2 checking on 20190215
 * V1.3: Bug fix when converting register data on 20190821
 * V1.4: Open the API of z-axis offset compensation for user on 20200212
 */
 

#include "MC34X9.h"
#include "stdbool.h"

// write by rock.fan 2021/0708/
// device address   0x98 or 0xD8 [8bit] ,Address pin connect gnd or vdd .

/*if you want to write registers besides[0x14,0x07],you need to switch standbymode first */


static MC34X9_COM_INTF g_com_intf = MC34X9_COM_I2C;

int acc_rate[16] = {2,4,8,10,20,25,30,50,100,125,200,250,400,500,1000,2000};
MC34X9_InterruptEvent int_evt_MC34X9 = {0};
int16_t rdata[30][3] = {0};

uint8_t fifo_enable = 0;

#define MC34X9_MED_FILTER_LEN 11

int16_t raw_z_array[MC34X9_MED_FILTER_LEN];
int16_t sort_z_array[MC34X9_MED_FILTER_LEN];
int filter_data_cnt = 0;
int filter_data_index = 0;


MC34X9_DATA MC34X9_AccData = {0,0,0};

int MC34X9_Read_Reg(unsigned char reg_add, unsigned char *data)
{
  int ret = -1;
  
  if (g_com_intf == MC34X9_COM_I2C)
  {
    ret = I2C_Read_Reg(MC34X9_7BITI2C_ADDRESS, reg_add, data);
  }
  else if (g_com_intf == MC34X9_COM_SPI)
  {
    uint8_t tx_buf[4] = {0};
    uint8_t rx_buf[4] = {0};
        
    tx_buf[0] = reg_add | 0x80;
    tx_buf[1] = reg_add & 0x00; 
    ret = SPI_Read_Reg(tx_buf, rx_buf, 3);
    if (ret == HAL_OK)
    {
      *data = rx_buf[2];
    }
  }

  return ret;
}

int MC34X9_Write_Reg(unsigned char reg_add, unsigned char data)
{
  int ret = -1;
 
  
  if (g_com_intf == MC34X9_COM_I2C)
  {
    ret = I2C_Write_Reg(MC34X9_7BITI2C_ADDRESS, reg_add, data);
  }
  else if (g_com_intf == MC34X9_COM_SPI)
  {
    uint8_t tx_buf[4] = {0};
    uint8_t rx_buf[4] = {0};
  
    tx_buf[0] = reg_add & 0x7F;
    tx_buf[1] = data;
    ret = SPI_Write_Reg((uint8_t*)tx_buf, (uint8_t *)rx_buf, 2);
    //delay_us(100);
  }

  if (ret != HAL_OK)
  {
    memsic_printf("Reg 0x%x(0x%x) write failed\r\n", reg_add, data);
  }

#ifdef MC34X9_VERIFY_REG_WRITE
  ret = MC34X9_Read_Reg(reg_add, &read_data);
  if ((reg_add != 0x10) && (read_data != data))
  {
    memsic_printf("Reg 0x%x(0x%x : 0x%x) write failed\r\n", reg_add, data, read_data);
  }
#endif  

  return ret;
}


int MC34X9_MultiRead_Reg(unsigned char reg_add, int num, unsigned char *data)
{
  int ret = -1;
  
  if (g_com_intf == MC34X9_COM_I2C)
  {
    ret = I2C_MultiRead_Reg(MC34X9_7BITI2C_ADDRESS, reg_add, num, data);
  }
  else if (g_com_intf == MC34X9_COM_SPI)
  {
    uint8_t tx_buf[SPI_BURST_READ_MAX_LEN + 2] = {0};
    uint8_t rx_buf[SPI_BURST_READ_MAX_LEN + 2] = {0};  

    if (num > SPI_BURST_READ_MAX_LEN)
    {
      num = SPI_BURST_READ_MAX_LEN;
    }
  
    tx_buf[0] = reg_add | 0x80;
    tx_buf[1] = reg_add & 0x00; 
 
    ret = SPI_MultiRead_Reg(tx_buf, rx_buf, num + 2);
    if (ret == HAL_OK)
    {
      memcpy(data, &rx_buf[2], num);
    }

  }

  return ret;
}







/*read back the interrupt state ,fifo,motion,etc*/
uint8_t MC34X9_Interrupt_State(void)
{
	uint8_t reg;  
	MC34X9_Read_Reg(MC34X9_REG_INTR_STAT, &reg);
	//memsic_printf("status 0x14: %x\r\n", reg);  
	int_evt_MC34X9.tilt = 	( (reg>>0)&0x01 ) ;
	int_evt_MC34X9.flip = 	( (reg>>1)&0x01 ) ;
	int_evt_MC34X9.anymotion= ( (reg>>2)&0x01 ) ;
	int_evt_MC34X9.shake= 	( (reg>>3)&0x01 ) ;
	int_evt_MC34X9.tilt35=	 ( (reg>>4)&0x01 ) ;
	int_evt_MC34X9.fifo = 	( (reg>>5)&0x01 ) ;
	int_evt_MC34X9.resv = 	( (reg>>6)&0x01 ) ;
	int_evt_MC34X9.acq = 	( (reg>>7)&0x01 ) ;
	
	return reg;
}
/*calculate the length of data in fifo pipeline*/
uint8_t  MC34X9_Read_FIFO_Length(void)
{
	uint8_t reg ,reg1,fifo_length; 
	MC34X9_Read_Reg(MC34X9_REG_FIFO_RD_P, &reg);
	MC34X9_Read_Reg(MC34X9_REG_FIFO_WR_P, &reg1);		
	if(reg>reg1)
		fifo_length=64+reg1-reg;
	else
		fifo_length=reg1-reg;
	memsic_printf("Fifo length ,%d,Point,%d %d\r\n",fifo_length,reg,reg1);
	return fifo_length;
}
/* standbymode ,wakemode  */
int MC34X9_Modechange(MC34X9_MODE mode)
{
	uint8_t reg;
	reg = mode;
	MC34X9_Write_Reg(MC34X9_REG_MODE, reg);
	if(mode!=MC34X9_WAKEMODE)
	{
	 /*reset fifo point*/
	// reg=0x10;
	// MC34X9_Write_Reg(0x2D, reg);
	}
	return 0;
}
/*read 0x18 product code */
uint8_t MC34X9_Validate()
{
	uint8_t reg; 
	MC34X9_Read_Reg(MC34X9_REG_CHIP_ID, &reg);
	memsic_printf("0x18====%x\r\n ",reg);
	if(reg!=0xA4)
		return 0;
	else
		return 1;
}

/*main oscillator frequency ,can ignore it*/
int mc34X9_MOSF(void)
{
	uint8_t reg; 
	MC34X9_Read_Reg(MC34X9_REG_SECURITY_OPT, &reg);
	memsic_printf("0x3B= 0x%x\r\n", reg); 
	reg=(reg&0x30)>>4;	
	switch(reg)
	{
		case MC34X9_25MZ:
			memsic_printf("2.5MZ\r\n ");
			break;
		case MC34X9_12MZ:
			memsic_printf("1.2MZ\r\n ");
			break;
		case MC34X9_6MZ:
			memsic_printf("0.6MZ\r\n ");
			break;
		case MC34X9_3MZ:
			memsic_printf("0.3MZ\r\n ");
			break;
			
	}
	return reg;
}
void MC34X9_Lpf_Range(  MC34X9_RANGE range,MC34X9_LPF_DIVISOR fc,bool lpf_enable)
{
	uint8_t reg;
	reg=(range<<4)|(lpf_enable<<3)|(fc);
	MC34X9_Write_Reg(MC34X9_REG_RANGE, reg);
}
/*anymotion interrupt enable ,pay attentioin to 0x06 bit6 ,set '1' ,you don't need to clear motion interrupt*/
int MC34X9_Anymotion_Enable(bool enable)
{
	uint8_t reg;
	if(enable)
		reg=0x44;// autoclear interrupt of motion interrupt
	else
		reg=0x40;
	
	MC34X9_Write_Reg(MC34X9_REG_INT_EN, reg);
	return 0;
}

int MC34X9_Shake_Enable(bool enable)
{
	uint8_t reg;
	if(enable)
		reg=0x48;// autoclear interrupt of motion interrupt
	else
		reg=0x40;
	
	MC34X9_Write_Reg(MC34X9_REG_INT_EN, reg);
	return 0;
}

int MC34X9_Tilt_Enable(bool enable)
{
	uint8_t reg;
	if(enable)
		reg=0x41;// autoclear interrupt of motion interrupt
	else
		reg=0x40;
	
	MC34X9_Write_Reg(MC34X9_REG_INT_EN, reg);
	return 0;
}

int MC34X9_Flip_Enable(bool enable)
{
	uint8_t reg;
	if(enable)
		reg=0x42;// autoclear interrupt of motion interrupt
	else
		reg=0x40;
	
	MC34X9_Write_Reg(MC34X9_REG_INT_EN, reg);
	return 0;
}



int MC34X9_Config_FIFO(uint8_t watermark,bool enable,MC34X9_COMBIN_INT comb,FIFO_INT fifo_int_type,FIFO_MODE fifo_mode)
{
	uint8_t reg;
	/*reset fifo point*/
	//reg=0x10;
	//MC34X9_Write_Reg(0x2D, reg);

	reg=watermark;
	if(reg>MAX_FIFO_LENGTH)
	{	
		reg=MAX_FIFO_LENGTH;
		memsic_printf("watermark is bigger than MAX_FIFO_LENGTH ,set watermark=%d\r\n ",MAX_FIFO_LENGTH);
	}
	MC34X9_Write_Reg(MC34X9_REG_FIFO_TH, reg);
	/* N samples one burst read */
	
	MC34X9_Write_Reg(MC34X9_REG_RD_CNT, reg);
		fifo_enable = enable;
	
	reg=(fifo_mode<<6)|(enable<<5)|(comb<<3)|(fifo_int_type);
	MC34X9_Write_Reg(MC34X9_REG_FIFO_CTRL, reg);

	reg=0xCC;
	MC34X9_Write_Reg(MC34X9_REG_GPIO_CTRL,reg);
	return 0;
		
}


/*
set main odr reg 0x08, frequency can be divded by setting reg 0x30
  surely you can change the odr you needed only by 0x08
*/
int MC34X9_Set_Basic_ODR(MC34X9_RATE rate)
{
	uint8_t reg;
	reg=rate;//0f 2khz ,4f 1khz
	MC34X9_Write_Reg(MC34X9_REG_SAMPLE_RATE, reg);
	return 0;

}
/*optional ,frequency can be divded by setting reg 0x30*/
void MC34X9_Set_ODR(int odr)
{
	uint8_t reg,i;
	/*0x08 is fixed while initializing ,change odr by  0x30*/
	
	for(i=0;i<=sizeof(acc_rate);i++)
		{
			if(odr<=acc_rate[i] || i==sizeof(acc_rate))
				break;
		}
	reg = (0xf-i);
	MC34X9_Write_Reg(MC34X9_REG_FIFO_CTRL2_SR2, reg);

}

void MC34X9_Set_Burstmode(bool enable)
{
	uint8_t reg;
	MC34X9_Read_Reg(MC34X9_REG_FIFO_CTRL2_SR2, &reg);
	
	if(enable)
	reg = reg|0x80;	
	MC34X9_Write_Reg(MC34X9_REG_FIFO_CTRL2_SR2, reg);
}

/* configure the anymotion registers , threshold and debounce */
int MC34X9_Config_Anymotion(uint16_t thr,uint8_t debounce)
{
	/*1365=1000mg*/
	uint8_t reg,temp;
	/*motion threshold*/
	temp=thr>>8;
	MC34X9_Write_Reg(MC34X9_REG_AM_THRESH_MSB, temp);
	temp=thr&0x00ff;
	MC34X9_Write_Reg(MC34X9_REG_AM_THRESH_LSB, temp);
	/*motion debounce*/
	MC34X9_Write_Reg(MC34X9_REG_AM_DB, debounce);
	reg=0x04;
	MC34X9_Write_Reg(MC34X9_REG_MOTION_CTRL, reg);
	return 0;
}

/* configure the shake registers , threshold and p2p */
int MC34X9_Config_Shake(uint16_t thr,uint16_t p2p)
{
	/*1365=1000mg*/
	uint8_t reg,temp,temp1;
	/*motion threshold*/
	temp=thr>>8;
	MC34X9_Write_Reg(MC34X9_REG_SHK_THRESH_MSB, temp);
	temp=thr&0x00ff;
	MC34X9_Write_Reg(MC34X9_REG_SHK_THRESH_LSB, temp);
	
	/*motion Peak-to-Peak Duration*/
	temp1=p2p>>8;
	MC34X9_Write_Reg(MC34X9_REG_PK_P2P_DUR_THRESH_MSB, temp1);
	temp1=p2p&0x00ff;
	MC34X9_Write_Reg(MC34X9_REG_PK_P2P_DUR_THRESH_LSB, temp1);
	
	reg=0x08;
	MC34X9_Write_Reg(MC34X9_REG_MOTION_CTRL, reg);
	return 0;
}

/* configure the tilt/flip registers , threshold and debounce */
int MC34X9_Config_TF(uint16_t thr,uint8_t debounce)
{
	/*1365=1000mg*/
	uint8_t reg,temp;
	/*motion threshold*/
	temp=thr>>8;
	MC34X9_Write_Reg(MC34X9_REG_TF_THRESH_MSB, temp);
	temp=thr&0x00ff;
	MC34X9_Write_Reg(MC34X9_REG_TF_THRESH_LSB, temp);
	/*motion debounce*/
	MC34X9_Write_Reg(MC34X9_REG_TF_DB, debounce);
	reg=0x01;
	MC34X9_Write_Reg(MC34X9_REG_MOTION_CTRL, reg);
	return 0;
}

/*clear the interrupt states ,fifo ,motion ,data without fifo*/
void MC34X9_Clearint()
{
	uint8_t reg;
	reg = 0x00;
	MC34X9_Write_Reg(MC34X9_REG_INTR_STAT,reg); //clear INT


}

static int partition(int16_t arr[], int low, int high)
{
    int16_t key = arr[low];
    
    while(low < high){
        while(low < high && arr[high] >= key )
            high--;
        if(low < high)
            arr[low++] = arr[high];
        while( low < high && arr[low] <= key )
            low++;
        if(low < high)
            arr[high--] = arr[low];
    }
    arr[low] = key;
    
    return low;
}

static void quickSort(int16_t arr[], int start, int end)
{
    int pos = 0;
    
    if (start<end){
        pos = partition(arr, start, end);
        quickSort(arr, start, pos - 1);
        quickSort(arr,pos + 1,end);
    }
}

static int16_t medFilter(int16_t arr[], int len)
{
  quickSort(arr, 0, len - 1);

  return arr[(len - 1) / 2];
}


/*read data from fifo or not by polling mode */
static void MC34X9_FIFO_Timer_Handle(void * p_context)
{
	uint8_t fifo_length,i; 
	unsigned char rawdata[192]; //6*32	 
	int16_t Xvalue,Yvalue,Zvalue;

	MC34X9_Read_Reg(MC34X9_REG_INTR_STAT, rawdata);
	if(1 == fifo_enable)
	{
		fifo_length = MC34X9_Read_FIFO_Length();
		/*must set 0x30 burstread bit7, if want to read N*samples one time */
		MC34X9_MultiRead_Reg(MC34X9_REG_XOUT_EX_L,6*fifo_length, rawdata);	
		for(i = 0; i < (fifo_length); i++)
		{	
			rdata[i][0] = (short)(((unsigned short)rawdata[1+i*6] << 8) + (unsigned short)rawdata[0+i*6]);
			rdata[i][1] = (short)(((unsigned short)rawdata[3+i*6] << 8) + (unsigned short)rawdata[2+i*6]);
			rdata[i][2] = (short)(((unsigned short)rawdata[5+i*6] << 8) + (unsigned short)rawdata[4+i*6]);
			
			if (filter_data_cnt < MC34X9_MED_FILTER_LEN)
			{
				raw_z_array[filter_data_index] = rdata[i][2];
				sort_z_array[filter_data_index] = rdata[i][2];
				filter_data_index++;
				filter_data_cnt++;		
			}
			else
			{
				int j = 0;
				uint16_t diff = 0;
		
				filter_data_index = filter_data_index % MC34X9_MED_FILTER_LEN;
		
				for (j = 0; j < MC34X9_MED_FILTER_LEN; j++)
				{
					if (sort_z_array[j] == raw_z_array[filter_data_index])
					{
						sort_z_array[j] = rdata[i][2];
						break;
					}
				}
		
				raw_z_array[filter_data_index] = rdata[i][2];
		
				filter_data_index++;
		
				
				rdata[i][2] = medFilter(sort_z_array, MC34X9_MED_FILTER_LEN);
				
			}
			
		}
	}
	else
	{
		MC34X9_MultiRead_Reg(MC34X9_REG_XOUT_EX_L,6,rawdata);
		Xvalue = (short)(((unsigned short)rawdata[1] << 8) + (unsigned short)rawdata[0]);
		Yvalue = (short)(((unsigned short)rawdata[3] << 8) + (unsigned short)rawdata[2]);
		Zvalue = (short)(((unsigned short)rawdata[5] << 8) + (unsigned short)rawdata[4]);
		memsic_printf("POLL Rawdata:  %d, %d, %d\r\n",Xvalue,Yvalue,Zvalue);

	}

}

/*handle the interrupt states ,fifo ,motion ,etc*/

void MC34X9_Handle_INT(void)
{
	
	uint8_t fifo_length,i; 
	unsigned char rawdata[192]; //6*32	 
	

	memsic_printf("Intr hpp \r\n");

	MC34X9_Interrupt_State();
	
	if(int_evt_MC34X9.fifo)
	{
		fifo_length = MC34X9_Read_FIFO_Length();
		/*must set 0x30 burstread bit7, if want to read N*samples one time */
		MC34X9_MultiRead_Reg(MC34X9_REG_XOUT_EX_L,6*fifo_length, rawdata);  
		
		for(i = 0; i < (fifo_length); i++)
		{	
			rdata[i][0] = (short)(((unsigned short)rawdata[1+i*6] << 8) + (unsigned short)rawdata[0+i*6]);
			rdata[i][1] = (short)(((unsigned short)rawdata[3+i*6] << 8) + (unsigned short)rawdata[2+i*6]);
			rdata[i][2] = (short)(((unsigned short)rawdata[5+i*6] << 8) + (unsigned short)rawdata[4+i*6]);

			if (filter_data_cnt < MC34X9_MED_FILTER_LEN)
			{
				raw_z_array[filter_data_index] = rdata[i][2];
				sort_z_array[filter_data_index] = rdata[i][2];
				filter_data_index++;
				filter_data_cnt++;		
			}
			else
			{
				int j = 0;
				uint16_t diff = 0;
		
				filter_data_index = filter_data_index % MC34X9_MED_FILTER_LEN;
		
				for (j = 0; j < MC34X9_MED_FILTER_LEN; j++)
				{
					if (sort_z_array[j] == raw_z_array[filter_data_index])
					{
						sort_z_array[j] = rdata[i][2];
						break;
					}
				}
		
				raw_z_array[filter_data_index] = rdata[i][2];
		
				filter_data_index++;
		
								
				rdata[i][2] = medFilter(sort_z_array, MC34X9_MED_FILTER_LEN);
				
			}

		}
		//BSP_Led_TurnOnOff(4, TRUE);
	
	}
	if(int_evt_MC34X9.anymotion)
	{	
		BSP_Led_TurnOnOff(0, TRUE);
		delay_ms(3);
	}
	
	if(int_evt_MC34X9.shake)
	{	
		BSP_Led_TurnOnOff(1, TRUE);
	}
		
	if(int_evt_MC34X9.tilt)
	{	
		BSP_Led_TurnOnOff(1, TRUE);
	}

	if(int_evt_MC34X9.flip)
	{	
		BSP_Led_TurnOnOff(4, TRUE);
	}

	MC34X9_Clearint();
	
}

int Get_MC34X9_ID(uint8_t *chip_id)
{
	I2C_Read_Reg(MC34X9_7BITI2C_ADDRESS,MC34X9_REG_CHIP_ID, chip_id);
	if(chip_id[0] != MC34X9_CHIP_ID_VAL)
		return -1;

	return 1;
}	

int MC34X9_Init(void)
{
	int ret;
	uint8_t reg;  
		
	if(MC34X9_Validate())
	{
		memsic_printf("MC34X9_Init\r\n");

		/*if you want to write registers besides[0x14,0x07],you need to switch standbymode first */
		MC34X9_Modechange(MC34X9_STANDBYMODE);
		//mc_delay_ms(1);
		/*here reload all the registers to default value*/
		reg = 0x40; 
		MC34X9_Write_Reg(MC34X9_REG_RESET,reg); //reload otp
		
		Delay_Ms(10);
		//mc34X9_MOSF();
		
		MC34X9_Modechange(MC34X9_STANDBYMODE);

		/*you can use only MC34X9_Set_Basic_ODR()  ,or use the two func below */
		MC34X9_Set_Basic_ODR(MC34X9_HIGH_2000HZ);
		MC34X9_Set_ODR(100);
		
		MC34X9_Set_Burstmode(1);

		/*set the range to 16G,turn on LPF*/
		MC34X9_Lpf_Range(MC34X9_16G,MC34X9_16fc, 1);///lpf on
		
		//configure anymotion 
		// dounce depends on your final sample rate (0x08,0x30)
		MC34X9_Config_Anymotion(0xF1,50);	
		MC34X9_Anymotion_Enable(DISABLE);//or ENABLE

		//configure shake 
		MC34X9_Config_Shake(0x10,0x10);	
		MC34X9_Shake_Enable(DISABLE);//or ENABLE

		//configure tilt
		MC34X9_Config_TF(0x01,0x01);	
		MC34X9_Tilt_Enable(DISABLE);//or ENABLE

		//configure flip
		MC34X9_Config_TF(0x10,0x10);	
		MC34X9_Flip_Enable(DISABLE);//or ENABLE
		
		//configure fifo
		MC34X9_Config_FIFO(30,ENABLE,FIFO_INT1,FIFO_THR,FIFO_WATERMARK_MODE);//ENABLE
		
		//   0x10 : int1,2 request routed to int 2,1 .
		//   0x00 : int1,2 request routed to int 1,2 .
		//reg = 0x00; 
		//MC34X9_Write_Reg(0x31,reg); 
		//reg=0x0;
		//MC34X9_Write_Reg(0x4A, reg);

		
		
		MC34X9_Clearint();
		MC34X9_Modechange(MC34X9_WAKEMODE);
		
		memsic_printf("MC34X9_Init end\r\n");
		ret = 1;
	}
	else
	{
		memsic_printf("not MC34X9 \r\n");
		ret = 0;
	}
	return ret;
}

int MC34X9_Check_ID(void)	
{
  uint8_t reg_val;

	return Get_MC34X9_ID(&reg_val);
}

int MC34X9_Disable(void)		
{	
 	I2C_Write_Reg(MC34X9_7BITI2C_ADDRESS,MC34X9_REG_MODE,0x00);

	return 1;
}

int MC34X9_Enable(void)	
{
	
	I2C_Write_Reg(MC34X9_7BITI2C_ADDRESS,MC34X9_REG_MODE,0x01);	//Wake up

	filter_data_cnt = 0;
    filter_data_index = 0;
	return 1;
}

MC34X9_DATA MC34X9_Get_Raw_Data(void)
{
	uint8_t buffer[6];
	
	
	I2C_MultiRead_Reg(MC34X9_7BITI2C_ADDRESS,MC34X9_REG_XOUT_EX_L,6,buffer);
	
	//load data	
	MC34X9_AccData.x = (uint16_t)(buffer[1] << 8 | buffer[0]);	
	MC34X9_AccData.y = (uint16_t)(buffer[3] << 8 | buffer[2]);
	MC34X9_AccData.z = (uint16_t)(buffer[5] << 8 | buffer[4]);	

		
	return MC34X9_AccData;
}











