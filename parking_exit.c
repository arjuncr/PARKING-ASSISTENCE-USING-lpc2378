#include<lpc23xx.h>

void lcd_init(void);
void lcd_cmd(unsigned int);
void lcd_print(unsigned char word[]);
void lcd_clear(void);
void delay(void);

void motor_init(void);
void motor_left(void);
void motor_right(void);
void motor_delay(void);


void key_pad_init(void);
char key_pad(void);
void get_data(void);
void delay1(int);

void lcd_disp(char);
void second_row(int);
void disp_clr(void);

void rtc_init(void);
void timer_read(void);

void spi_slave_init(void);
char spi_slave_read(void);

unsigned char a[5];
unsigned char int0;
char data[20];
char c[20];

__irq void int0_ISR (void)
{
int0 = 1;	 				/* Set flag */
EXTINT=1; 					/* Clear EINT0 */
VICVectAddr = 0; 
}

int main()
{
	int k,j,flag=0,l,t;
	char pass[10];
	char time0[10];
	spi_slave_init();
	lcd_init();
	rtc_init();
	motor_init();

	VICVectAddr14 = (unsigned long)int0_ISR;  /* interruppt configuration for INT0  */
	VICVectPriority14 = 11; 
	VICIntEnable = (1 << 14);
	PINSEL4=0x100000;	 /* Enabling INT1 interrupt */


	lcd_print("Parking Exit"); // Display Welcome to ARM Parking
	
	for(k=0;k<10;k++)
	{
	  data[k]=spi_slave_read();
	if(data[k]=='\0')
			break;	
	}		

for(j=0;j<4;j++)
   pass[j]=data[j];

	pass[j]='\0';
	
	
	t=0;
	for(j=4;data[j]!=0;j++)
	{
	    time0[t]=data[j];
		  t++;
	}
	time0[t]='\0';
	
	
	
	
	lcd_clear();
	for(l=0;pass[l]!=0;l++)
	   lcd_disp(pass[l]);
	  
	
	
	
	
	
	while(1)
	{
	if(int0)
	{
	lcd_clear();
	delay();	
start:	
	lcd_print("Enter PIN");
	PINSEL4=0x000000;  // Disabling INT0 interrupt 
	delay();
  
	get_data();

	for(j=0;a[j]!=0;j++)
	{
		if(a[j]==pass[j])
		{
			flag=1;
			continue;
		}	
		else
		{
			flag=0;
			break;
		}	
	}
	if(flag)
	{
	lcd_print("ACCESS GRANTED");
		timer_read();
	}
  	else
	{	
		lcd_print("ACCESS DENIED");
		goto start;
	}
	motor_delay();
	
	motor_left();
	motor_delay();
	motor_right();
	lcd_clear();
	lcd_print(" Parking Exit "); // Display Welcome to ARM Parking
	
	PINSEL4=0x100000;	 // Enabling INT0 interrupt 
	int0=0;
	}
	} 

}

void lcd_init()  /* 4-Bit LCD Initialization */
{
IODIR1=0xFF000000;
delay();
IOPIN1=0X83000000;
IOPIN1=0X03000000;   //3
delay();
IOPIN1=0X83000000;
IOPIN1=0X03000000;   //3
delay();
IOPIN1=0X83000000;
IOPIN1=0X03000000;   //3
delay();
IOPIN1=0X82000000;
IOPIN1=0X02000000;   //2
delay();
IOPIN1=0X82000000;
IOPIN1=0X02000000;   //2
delay();
IOPIN1=0X88000000;
IOPIN1=0X08000000;   //8
delay();
IOPIN1=0X80000000;
IOPIN1=0X00000000;   //0
delay();
IOPIN1=0X8F000000;
IOPIN1=0X0F000000;   //F
delay();
IOPIN1=0X80000000;
IOPIN1=0X00000000;   //0
delay();
IOPIN1=0X81000000;
IOPIN1=0X01000000;   //1
delay();
}

void lcd_cmd(unsigned int g) /* 4-Bit LCD Command Function */
{
unsigned int msb,lsb;
if(g==0X90)
g=0X0C;
msb=(g&0XF0)<<20;
lsb=(g&0X0F)<<24;
delay();
IOPIN1=(0X80000000)|msb;
IOPIN1=msb;
delay();
IOPIN1=(0X80000000)|lsb;
IOPIN1=lsb;
delay();
}

void lcd_print(unsigned char word[])  /* Display a Charactor from Char array */
{
unsigned int a,msb,lsb,i;
for(i=0;word[i]!='\0';i++)
{
a=word[i];
msb=(a&0XF0)<<20;
lsb=(a&0X0F)<<24;
delay();
IOPIN1=(0X90000000)|msb;
IOPIN1=msb;
delay();
IOPIN1=(0X90000000)|lsb;
IOPIN1=lsb;
delay();
if(i==15)
{
IOPIN1=0X8C000000;
IOPIN1=0X0C000000;
delay();
IOPIN1=0X80000000;
IOPIN1=0X00000000;
}
}
}

void lcd_clear()  /* Display clear */
{
delay();
IOPIN1=0X80000000;
IOPIN1=0X00000000;   //0
delay();
IOPIN1=0X81000000;
IOPIN1=0X01000000;   //1
delay();
}

/* ******************************************************************************* */

/* ************************** Delay Function ***************************** */
void delay()
{
unsigned int i,j;
for(i=6;i!=0;i--)
{
for(j=10000;j!=0;j--);
}
}
/* ********************************************************************** */
/* ************************** Motor Function **************************** */

void motor_init()  /* Motor Initialization */
{
PINSEL10=0;
FIO2DIR=0x00000003;
FIO2PIN=0X00000000;
}

void motor_left()
{
FIO2PIN=0X01;
motor_delay();
FIO2PIN=0X00;
}

void motor_right()
{
FIO2PIN=0X02;
motor_delay();
FIO2PIN=0X00;
}

void motor_delay() 	/* big delay for motor */
{
unsigned int i,j;
for(i=600;i!=0;i--)
{
for(j=100000;j!=0;j--);
}
}



/* ************************* KeyPad Function ****************************  */

void key_pad_init(void) /* Keypad Initialization P3.0 TO P3.3 --OUTPUT -row
												 P3.4 TO P3.7 --INPUT -column*/
{
    PINSEL6=0X00;                    
	FIO3DIR=0X0F;
}

char key_pad(void) /* Key Pad Reading */
{
int a;
			
FIO3PIN=0X0;
while((FIO3PIN&0XF0)==0XF0);
            
FIO3PIN=0XE;
a=FIO3PIN&0XFF;
switch(a)
	{
			case 0XEE:  while((FIO3PIN&0XFF)==0XEE);
			           
		                return '1';
					
			case 0XDE:  while((FIO3PIN&0XFF)==0XDE);
			            return '2';
						
			case 0XBE:  while((FIO3PIN&0XFF)==0XBE);
			           
			            return '3';
			           
			case 0X7E:  while((FIO3PIN&0XFF)==0X7E);
			           
			            return 'A';
						
			default: 	break;
	}		
						
FIO3PIN=0XD;
a=FIO3PIN&0XFF;
			
switch(a)
	{
			case 0XED: while((FIO3PIN&0XFF)==0XED);
			            return '4';
					
			case 0XDD:  while((FIO3PIN&0XFF)==0XDD);
			            return '5';
						
			case 0XBD:  while((FIO3PIN&0XFF)==0XBD);
			            return '6';
			            
			case 0X7D:  while((FIO3PIN&0XFF)==0X7D);
			           return 'B';
					
			default:   break;
			
	}

FIO3PIN=0X0B;
a=FIO3PIN&0XFF;
			
switch(a)
	{
			case 0XEB: while((FIO3PIN&0XFF)==0XEB);
			           return '7';
						
			case 0XDB:  while((FIO3PIN&0XFF)==0XDB);
			           return '8';
					
			case 0XBB:  while((FIO3PIN&0XFF)==0XBB);
			            return '9';
			           
			case 0X7B:  while((FIO3PIN&0XFF)==0X7B);
			           return 'C';
					
			default:   break;
	}

FIO3PIN=0X07;
a=FIO3PIN&0XFF;
			
switch(a)
	{
			case 0XE7: while((FIO3PIN&0XFF)==0XE7);
			           return '*';
					
			case 0XD7:  while((FIO3PIN&0XFF)==0XD7);
			           return '0';
					
			case 0XB7:  while((FIO3PIN&0XFF)==0XB7);
			            return '#';
			            
			case 0X77:  while((FIO3PIN&0XFF)==0X77);
			           return 'D';
					
			default:   break;
	}
return '\0';

}

void get_data(void)

{
int i=0;
key_pad_init(); 
    do{ 
        a[i]=key_pad();
        if(a[i]=='D')
          break;
    
            
        if(a[i]=='C')
             { 
            disp_clr();
             i=0;
              continue;
              }
        lcd_disp(a[i]);
        i++;
        }
        while(1);
        a[i]='\0';


}

/* ******************************************************************************** */
void lcd_disp(char s)
{ 

	int c=0,f=0,g,h;
	
		
			           
			           g=s&0xf0;
                       h=s&0x0f;				
             
                       c=g<<20;
			
			           f=h<<24;
				
			           delay1(5);
			           IOPIN1=0x90000000|c;
			           IOPIN1=c;
			           delay1(5);
				       IOPIN1=0x90000000|f;
			           IOPIN1=f;
	                   delay1(5);
                  
}

void second_row(int q)
{
					
	if(q==15)
		{
			  delay1(5);
			  IOPIN1=0x8c000000;
			  IOPIN1=0x0c000000;
			  delay1(5);
			  IOPIN1=0x80000000;
			  IOPIN1=0x00000000;
			  delay1(5);
					
		}
}

void delay1(int a)
{        
   int i,j,b;
		  	      b=a*47;
			
				  for(j=0;j<b;j++)
					for(i=0;i<0xff;i++);
}

	void disp_clr(void)
{
			
		    IOPIN1=0x80000000;
			IOPIN1=0x00000000;
		    delay1(5);
		    IOPIN1=0x81000000;
		    IOPIN1=0x01000000;
}

/* ********************************************************************** */
/* ************************** SPI FUNCTION **************************** */

void spi_slave_init(void)
{
PCONP |= (1 << 8);
PINSEL0 |= 0xC0000000;
PINSEL1 |= 0x0000003F;
S0SPCR = 0x0804;
S0SPCR |=1<<6;
//S0SPCCR=0X08;	
//S0SPCR=0X0000;	
}
char spi_slave_read(void)
{
char j;
while(!(S0SPSR&0X80));
j=S0SPDR;	
return j;
}

/* ********************************************************************** */
/* ************************** RTC FUNCTION **************************** */

void rtc_init(void)
{
PCONP|=1<<9;
PCLKSEL0|=0X40000;
RTC_CCR=0<<4;
RTC_PREINT=0x16D;
RTC_PREFRAC=0x1B00;

RTC_SEC=10;
RTC_MIN=30;
RTC_HOUR=01;

RTC_CCR|=0x1;
}

void timer_read(void)
{
	int i=0,sec=0,min=0,hr=0,d=0;
	
	sec=RTC_CTIME0&0x3F;
	
	min=(RTC_CTIME0&0x3F00)>>8;
	
	hr=(RTC_CTIME0&0x1F0000)>>16;
	
	for(i=0;i<2;i++)
	   {
			 d=sec%10;
			 c[i]=d+'0';
			 sec=sec/10;
		 }
		 for(i=2;i<4;i++)
		 {
			 d=min %10;
			 c[i]=d+'0';
			 min=min/10;
		 }
		 for(i=4;i<6;i++)
		 {
			 d=hr%10;
			 c[i]=d+'0';
			 hr=hr/10;
		 }
		 c[i]='\0';
		 }
