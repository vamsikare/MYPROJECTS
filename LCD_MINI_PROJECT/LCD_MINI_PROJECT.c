#define RCC_AHB1ENR *((int *) 0x40023830)
#define GPIOB_MODER *((int *) 0x40020400)
#define GPIOB_ODR *((int *)0x40020414)

#define STK_CTRL *((int *)0xE000E010)
#define STK_LOAD *((int *)0xE000E014)
#define STK_VAL *((int *)0xE000E018)
	
void initstk()
{
	STK_CTRL |= 0x05;
	STK_VAL = 0x111;
}

void delayms(int x)
{
	STK_LOAD = x*16000;            // x ms delay
	while((STK_CTRL & (0X1<<16)) == 0)
	{
		;
	}	
}
	
void init(void)
{
        RCC_AHB1ENR |= 0X2;		// enable port B clock 
	while(!(RCC_AHB1ENR & 0X2))	// checking port B enabled or not
	{
		;
	}
}

void config(void)
{
	GPIOB_MODER &=0xfffcf000;       // clear pb [0:5, 8]
	GPIOB_MODER |=0x00010555;       //load 01 pb[0:5, 8]
	
}

char a;

void write_high_nibble( unsigned char data )
{
	GPIOB_ODR &= 0XFFFFFFF0;        //CLEAR FIRST 4 BITS OF GPIOB_ODR
	data = data>>4;
	GPIOB_ODR |=data;
	
	GPIOB_ODR |=0X100;              //set enable E=1
	delayms(10);                    //delay 10ms
	GPIOB_ODR &=(~(0X100));         //clear enable E=0
}

void write_low_nibble( unsigned char data )
{
	GPIOB_ODR &= 0XFFFFFFF0;        //CLEAR FIRST 4 BITS OF GPIOB_ODR
	data &= 0x0f;
	GPIOB_ODR |=data;
	
	GPIOB_ODR |=0X100;              //set enable pin E=1
	delayms(10);                    //delay 10ms
	GPIOB_ODR &=(~(0X100));         //clear enable pin E=0
}


void LCD_Write_Cmd(unsigned char temp)
{
	  GPIOB_ODR &=~(0X01<<4);       //RS=0
	  write_high_nibble(temp);
	  write_low_nibble(temp);
}

void LCD_Write_Data( unsigned char temp)
{
	GPIOB_ODR |=0x01<<4;            //RS=1
	write_high_nibble(temp);
	write_low_nibble(temp);
}

void LCD_Write_Str(char *s)
{
	int i;
	for(i=0;s[i];i++)
	{
		  LCD_Write_Data(s[i]);
	}
	
}

void LCD_Init(void)
{
	delayms(20);                    //20 ms delay
	LCD_Write_Cmd (0x33);           //set to work only on 4 data pins
	delayms(1);
	LCD_Write_Cmd (0x32);
	LCD_Write_Cmd (0x0C);           //Write 0x0C (Display On, Cursor OFF) Command
	LCD_Write_Cmd (0x01);           //Write 0x01 (Display Clear) Command
}


int main()
{
	char a[20]="Hello World";
	init();                         //initialize port b
	config();                       //lcd configuration
	initstk();                      //system tic timer  initialization
	delayms(10);                    //10 ms delay
	LCD_Init();                     //lcd initialization
	LCD_Write_Str(a);               //write data to lcd
	
}

