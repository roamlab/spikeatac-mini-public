
// write by rock.fan 2021/0708/
// device address   0x98 or 0xD8 [8bit] ,Address pin connect gnd or vdd .

/*if you want to write registers besides[0x14,0x07],you need to switch standbymode first */


#define ENABLE 1
#define DISABLE 0
#define MAX_FIFO_LENGTH  30



extern mc_read_regs(unsigned char register_address, unsigned char * destination, unsigned char number_of_bytes);
extern mc_write_regs(unsigned char register_address, unsigned char *value, unsigned char number_of_bytes);



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
    MC34X9_4fc=2,
	MC34X9_6fc=3,
	MC34X9_12fc=4,
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


int acc_rate[16] = {2,4,8,10,20,25,30,50,100,125,200,250,400,500,1000,2000};
MC34X9_InterruptEvent int_evt_mc34x9 = {0};
int16_t rdata[30][3] = {0};

uint8_t fifo_enable = 0;

/*read back the interrupt state ,fifo,motion,etc*/
uint8_t mc_interrupt_state(void)
{
	uint8_t reg;  
	mc_read_regs(0x14, &reg, 1);
	//mc_printf("status 0x14: %x\r\n", reg);  
	int_evt_mc34x9.tilt = 	( (reg>>0)&0x01 ) ;
	int_evt_mc34x9.flip = 	( (reg>>1)&0x01 ) ;
	int_evt_mc34x9.anymotion= ( (reg>>2)&0x01 ) ;
	int_evt_mc34x9.shake= 	( (reg>>3)&0x01 ) ;
	int_evt_mc34x9.tilt35=	 ( (reg>>4)&0x01 ) ;
	int_evt_mc34x9.fifo = 	( (reg>>5)&0x01 ) ;
	int_evt_mc34x9.resv = 	( (reg>>6)&0x01 ) ;
	int_evt_mc34x9.acq = 	( (reg>>7)&0x01 ) ;
	
	return reg;
}
/*calculate the length of data in fifo pipeline*/
uint8_t  mc_read_fifo_length(void)
{
	uint8_t reg ,reg1,fifo_length; 
	mc_read_regs(0x0B, &reg,1);
	mc_read_regs(0x0C, &reg1,1);		
	if(reg>reg1)
		fifo_length=64+reg1-reg;
	else
		fifo_length=reg1-reg;
	mc_printf("Fifo length ,%d,Point,%d %d\r\n",fifo_length,reg,reg1);
	return fifo_length;
}
/* standbymode ,wakemode  */
int mc3419_modechange(MC34X9_MODE mode)
{
	uint8_t reg;
	reg = mode;
	mc_write_regs(0x07, &reg, 1);
	if(mode!=MC34X9_WAKEMODE)
	{
	 /*reset fifo point*/
	// reg=0x10;
	// mc_write_regs(0x2D, &reg, 1);
	}
}
/*read 0x18 product code */
uint8_t mc3419_validate()
{
	uint8_t reg; 
	mc_read_regs(0x18, &reg, 1);
	mc_printf("0x18====%x\r\n ",reg);
	if(reg!=0xA4)
		return 0;
	else
		return 1;
}

/*main oscillator frequency ,can ignore it*/
int mc3419_MOSF(void)
{
	uint8_t reg; 
	mc_read_regs(0x3B, &reg, 1);
	mc_printf("0x3B= 0x%x\r\n", reg); 
	reg=(reg&0x30)>>4;	
	switch(reg)
	{
		case MC34X9_25MZ:
			mc_printf("2.5MZ\r\n ");
			break;
		case MC34X9_12MZ:
			mc_printf("1.2MZ\r\n ");
			break;
		case MC34X9_6MZ:
			mc_printf("0.6MZ\r\n ");
			break;
		case MC34X9_3MZ:
			mc_printf("0.3MZ\r\n ");
			break;
			
	}
	return reg;
}
void mc3419_lpf_range(MC34X9_RANGE range,MC34X9_LPF_DIVISOR fc,bool lpf_enable)
{
	uint8_t reg;
	reg=(range<<4)|(lpf_enable<<3)|(fc);
	mc_write_regs(0x20, &reg, 1);
}
/*anymotion interrupt enable ,pay attentioin to 0x06 bit6 ,set '1' ,you don't need to clear motion interrupt*/
int mc3419_anymotion_enable(bool enable)
{
	uint8_t reg;
	if(enable)
		reg=0x44;// autoclear interrupt of motion interrupt
	else
		reg=0x40;
	
	 mc_write_regs(0x06, &reg, 1);
	
}

int mc3419_config_fifo(uint8_t watermark,bool enable,MC34X9_COMBIN_INT comb,FIFO_INT fifo_int_type,FIFO_MODE fifo_mode)
{
	uint8_t reg;
	/*reset fifo point*/
	//reg=0x10;
	//mc_write_regs(0x2D, &reg, 1);

	reg=watermark;
	if(reg>MAX_FIFO_LENGTH)
	{	
		reg=MAX_FIFO_LENGTH;
		mc_printf("watermark is bigger than MAX_FIFO_LENGTH ,set watermark=%d\r\n ",MAX_FIFO_LENGTH);
	}
	mc_write_regs(0x2E, &reg, 1);
	/* N samples one burst read */
	
	mc_write_regs(0x4B, &reg, 1);
		fifo_enable = enable;
	
	reg=(fifo_mode<<6)|(enable<<5)|(comb<<3)|(fifo_int_type);
	mc_write_regs(0x2D, &reg, 1);
		
}


/*
set main odr reg 0x08, frequency can be divded by setting reg 0x30
  surely you can change the odr you needed only by 0x08
*/
int mc3419_set_basic_odr(MC34X9_RATE rate)
{
	uint8_t reg;
	reg=rate;//0f 2khz ,4f 1khz
	mc_write_regs(0x08, &reg, 1);

}
/*optional ,frequency can be divded by setting reg 0x30*/
void mc3419_set_odr(int odr)
{
	uint8_t reg,i;
	/*0x08 is fixed while initializing ,change odr by  0x30*/
	
	for(i=0;i<=sizeof(acc_rate);i++)
		{
			if(odr<=acc_rate[i] || i==sizeof(acc_rate))
				break;
		}
	reg = (0xf-i);
	mc_write_regs(0x30, &reg, 1);

}

void mc3419_set_burstmode(bool enable)
{
	uint8_t reg;
	mc_read_regs(0x30, &reg, 1);
	
	if(enable)
	reg = reg|0x80;	
	mc_write_regs(0x30, &reg, 1);
}

/* configure the anymotion registers , threshold and debounce */
int mc3419_config_anymotion(uint16_t thr,uint8_t debounce)
{
	/*1365=1000mg*/
	uint8_t reg,temp;
	/*motion threshold*/
	temp=thr>>8;
	mc_write_regs(0x44, &temp, 1);
	temp=thr&0x00ff;
	mc_write_regs(0x43, &temp, 1);
	/*motion debounce*/
	mc_write_regs(0x45, &debounce, 1);
	reg=0x04;
	mc_write_regs(0x09, &reg, 1);
}
/*clear the interrupt states ,fifo ,motion ,data without fifo*/
void mc3419_clearint()
{
	uint8_t reg;
	reg = 0x00;
	mc_write_regs(0x14,&reg, 1); //clear INT

}
/*read data from fifo or not by polling mode */
static void mc_fifo_timer_handle(void * p_context)
{
	uint8_t fifo_length,i; 
	unsigned char rawdata[192]; //6*32	 
	int16_t Xvalue,Yvalue,Zvalue;

	mc_read_regs(0x14, rawdata,1);
	if(1 == fifo_enable)
	{
		fifo_length = mc_read_fifo_length();
		/*must set 0x30 burstread bit7, if want to read N*samples one time */
		mc_read_regs(0x0D, rawdata,6*fifo_length);	
		for(i = 0; i < (fifo_length); i++)
		{	
			rdata[i][0] = (short)(((unsigned short)rawdata[1+i*6] << 8) + (unsigned short)rawdata[0+i*6]);
			rdata[i][1] = (short)(((unsigned short)rawdata[3+i*6] << 8) + (unsigned short)rawdata[2+i*6]);
			rdata[i][2] = (short)(((unsigned short)rawdata[5+i*6] << 8) + (unsigned short)rawdata[4+i*6]);
		}
	}
	else
	{
		mc_read_regs(0x0d,rawdata,6);
		Xvalue = (short)(((unsigned short)rawdata[1] << 8) + (unsigned short)rawdata[0]);
		Yvalue = (short)(((unsigned short)rawdata[3] << 8) + (unsigned short)rawdata[2]);
		Zvalue = (short)(((unsigned short)rawdata[5] << 8) + (unsigned short)rawdata[4]);
		mc_printf("POLL Rawdata:  %d, %d, %d\r\n",Xvalue,Yvalue,Zvalue);

	}

}

/*handle the interrupt states ,fifo ,motion ,etc*/

void mc_interrupt_handler(void)
{
	
	unsigned char rawdata[192];	//6*32
	uint8_t fifo_length,i;  
	int16_t Xvalue,Yvalue,Zvalue;

	mc_printf("Intr hpp \r\n");

	mc_interrupt_state();
	
	if(int_evt_mc34x9.fifo)
	{
		fifo_length = mc_read_fifo_length();
		/*must set 0x30 burstread bit7, if want to read N*samples one time */
		mc_read_regs(0x0D, rawdata,6*fifo_length);  
		
		for(i = 0; i < (fifo_length); i++)
		{	
			rdata[i][0] = (short)(((unsigned short)rawdata[1+i*6] << 8) + (unsigned short)rawdata[0+i*6]);
			rdata[i][1] = (short)(((unsigned short)rawdata[3+i*6] << 8) + (unsigned short)rawdata[2+i*6]);
			rdata[i][2] = (short)(((unsigned short)rawdata[5+i*6] << 8) + (unsigned short)rawdata[4+i*6]);
		}
	
	}
	else
	{	
	}

	mc3419_clearint();
	
}


int mc3419_init(void)
{
	int ret;
	uint8_t reg;  
		
	if(mc3419_validate())
	{
		mc_printf("mc3419_init\r\n");

		/*if you want to write registers besides[0x14,0x07],you need to switch standbymode first */
		mc3419_modechange(MC34X9_STANDBYMODE);
		//mc_delay_ms(1);
		
		/*Reset Sensor*/
		reg = 0x40; 
		mc_write_regs(0x1c,&reg, 1); 
		
		mc_delay_ms(10);
		//mc3419_MOSF();
		
		mc3419_modechange(MC34X9_STANDBYMODE);

		/*you can use only mc3419_set_basic_odr()  ,or use the two func below */
		mc3419_set_basic_odr(MC34X9_HIGH_2000HZ);
		mc3419_set_odr(100);
		
		mc3419_set_burstmode(1);

		/*set the range to 16G,turn on LPF*/
		mc3419_lpf_range(MC34X9_16G,MC34X9_16fc, 1);///lpf on
		
		//configure anymotion 
		// dounce depends on your final sample rate (0x08,0x30)
		mc3419_config_anymotion(0x50,50);	
		mc3419_anymotion_enable(DISABLE);

		//configure fifo
		mc3419_config_fifo(30,ENABLE,FIFO_INT1,FIFO_THR,FIFO_WATERMARK_MODE);//ENABLE
		
		//   0x10 : int1,2 request routed to int 2,1 .
		//   0x00 : int1,2 request routed to int 1,2 .
		//reg = 0x00; 
		//mc_write_regs(0x31,&reg, 1); 
		//reg=0x0;
		//mc_write_regs(0x4A, &reg, 1);
		
		
		mc3419_modechange(MC34X9_WAKEMODE);
		mc3419_clearint();
		mc_printf("mc3419_init end\r\n");
		ret = 1;
	}
	else
	{
		mc_printf("not mc3419 \r\n");
		ret = 0;
	}
	return ret;
}

