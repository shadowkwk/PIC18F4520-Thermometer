#include <p18F4520.h>
#include <delays.h>

#pragma config OSC = HS, WDT = OFF, LVP = OFF
						//0		1	2	  3		4	5	  6		7	8	9
unsigned char number[10]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};

unsigned char number1[16]={0x3F,0x3F,0x06,0x06,0x5B,0x4F,0x4F,0x66,0x6D,0x6D,0x7D,0x7D,0x07,0x7F,0x7F,0x6F};
unsigned char number2[16]={0x3F,0x7D,0x4F,0x6F,0x6D,0x06,0x7F,0x4F,0x3F,0x7D,0x4F,0x6F,0x6D,0x06,0x7F,0x66};
unsigned char temp1, temp2;

unsigned char reset(void)
{
	unsigned char k;
	TRISAbits.TRISA4=0;
	PORTAbits.RA4 = 0;
	Delay10TCYx(50); //870us
	TRISAbits.TRISA4=1;
	Delay10TCYx(7);		//80us

	if(PORTAbits.RA4 == 1)
		k=0;
	else 
		k=1;
	Delay10TCYx(43);		//400us
	return k;	
}

void writebyte(unsigned char data)
{
	unsigned char i,k;
	for(i=0;i<8;i++)
	{
	k=data&0x01;
	TRISAbits.TRISA4=0;
	PORTAbits.RA4 = 0;
	Nop();
	Nop();
	Nop();
	Nop();
	Nop();
	if(k==1)
	{
		TRISAbits.TRISA4=1;
	}
	Delay10TCYx(7);
	TRISAbits.TRISA4=1;
	Nop();
	Nop();
	data=data>>1;
	}	
}

unsigned char readbyte(void)
{
	unsigned char buf=0,j;
	for(j=0;j<8;j++){
		buf=buf>>1;
		TRISAbits.TRISA4 = 0;
		PORTAbits.RA4 = 0;
		Nop();
		Nop();
		Nop();
		Nop();
		Nop();
		Nop();
		TRISAbits.TRISA4 = 1;
		Nop();
		Nop();
		Nop();
		Nop();
		if(PORTAbits.RA4 == 1)
			buf|=0x80;
		Delay10TCYx(6);
	}
	return buf;
}

unsigned char convert(void)
{	
	if(reset()==1){
		writebyte(0xcc);
		writebyte(0x44);
		return 1;
	}
	else 
	{
		return 0;
	}
}


unsigned char readflash(void)
{
	unsigned char lsb,msb;
	if(reset()==1)
	{
		writebyte(0xcc);
		writebyte(0xbe);
		lsb=readbyte();
		msb=readbyte();
		temp2=lsb&0x0F;
		temp1=(lsb>>4)|(msb<<4);
		return 1;
	}
	return 0;
}

void main(void)
{
	CMCON=0x07;
	ADCON1=0x07;
	TRISA=0xFF;
	TRISB=0;
	TRISD=0;
	PORTB=0;
	PORTD=0;
	while(1)
	{
			convert();
			//Delay100TCYx(900);
			readflash();	
			
			PORTBbits.RB0=0;
			PORTBbits.RB1=0;
			PORTBbits.RB2=0;
			PORTD=number[temp1/10];
			Delay100TCYx(20);
	
			PORTBbits.RB0=1;
			PORTBbits.RB1=0;
			PORTBbits.RB2=0;
			PORTD=number[temp1%10];
			PORTDbits.RD7=1;
			Delay100TCYx(20);

			PORTBbits.RB0=0;
			PORTBbits.RB1=1;
			PORTBbits.RB2=0;
			PORTD=number1[temp2];
			Delay100TCYx(20);
			
			PORTBbits.RB0=1;
			PORTBbits.RB1=1;
			PORTBbits.RB2=0;
			PORTD=number2[temp2];
			Delay100TCYx(20);
	}
}

