/*
 * TWI.c
 *
 * Created: 1/26/2021 7:31:05 PM
 *  Author: Jack2bs
 */ 


#include "TWI.h"

//Tempheader true = t1, else t2
volatile int tempHeader = 1;
volatile int16_t temp1 = 10;
volatile int16_t temp2 = 20;
volatile int initialized1 = 0;
volatile int initialized2 = 0;
volatile int16_t temptemp = 0;

volatile int16_t g_countingUp = 0;

int tempToFloat(int16_t temp)
{
	temp &= 0b0000111111111111;
	int tempf = (temp >> 4);
	return tempf;
}

void sendTemp(int16_t temp)
{
	int tempf = tempToFloat(temp);
	uint8_t string[5];
	for (int i = 0; i < 3; i++)
	{
			string[2-i] = (tempf % 10) + '0';
			tempf /= 10;
	}
	string[3] = 'T';
	string[4] = '\n';
	Endpoint_SelectEndpoint(CDC1_RX_EPADDR);
	Endpoint_Write_Stream_LE(string, sizeof(string), NULL);
	Endpoint_ClearIN();
}

void putTempInString(int16_t temp, uint8_t string[5], int header)
{
	for (int i = 0; i < 3; i++)
	{
		string[4-i] = (temp % 10) + '0';
		temp /= 10;
	}
	string[0] = 'T';
	string[1] = header + '0';
}

ISR(TWI_vect)
{
	USB_USBTask();
	//CDC_Device_USBTask(&VirtualSerial1_CDC_Interface);
	
	switch(TWSR)
	{
	case 0x10:
	case 0x08:
		if(tempHeader)
		{
			if(initialized1)
			{
				TWDR = TEMP_SENSOR_ONE_ADDRESS_PLUS_READ;
			}
			else
			{
				TWDR = TEMP_SENSOR_ONE_ADDRESS_PLUS_WRITE;
			}
			TWCR = (1<<TWINT)|(1<<TWEN)| (1<<TWIE);
			
			/*
			Endpoint_SelectEndpoint(CDC1_RX_EPADDR);
			uint8_t string0[] = {"Sent Address For T1\n"};
			Endpoint_Write_Stream_LE(string0, sizeof(string0), NULL);
			Endpoint_ClearIN();
			*/
		}
		else
		{
			if(initialized2)
			{
				TWDR = TEMP_SENSOR_TWO_ADDRESS_PLUS_READ;
			}
			else
			{
				TWDR = TEMP_SENSOR_TWO_ADDRESS_PLUS_WRITE;
			}
			TWCR = (1<<TWINT)|(1<<TWEN)| (1<<TWIE);
			/*
			Endpoint_SelectEndpoint(CDC1_RX_EPADDR);
			uint8_t string0[] = {"Sent Address and Write\n"};
			Endpoint_Write_Stream_LE(string0, sizeof(string0), NULL);
			Endpoint_ClearIN();
			*/
		}
		

		break;
		
	case 0x18:
		TWDR = 0b00000101;
		TWCR = (1<<TWINT)|(1<<TWEN)| (1<<TWIE);

		/*
		Endpoint_SelectEndpoint(CDC1_RX_EPADDR);
		uint8_t string10[] = {"Sent pointer to temp register\n"};
		Endpoint_Write_Stream_LE(string10, sizeof(string10), NULL);
		Endpoint_ClearIN();
		*/
		break;
		
	case 0x20:
		TWCR = (1<<TWSTO) | (1<<TWINT)|(1<<TWEN)| (1<<TWIE);
		
		if(tempHeader)
		{
			tempHeader = 0;
		}
		else
		{
			tempHeader = 1;
		}
		
		/*
		Endpoint_SelectEndpoint(CDC1_RX_EPADDR);
		uint8_t string6[] = {"Stopped because nak was received\n"};
		Endpoint_Write_Stream_LE(string6, sizeof(string6), NULL);
		Endpoint_ClearIN();
		*/
		break;
		
	case 0x28:
		
		//Send stop bit
		TWCR = (1<<TWINT) | (1<<TWSTO)|(1<<TWEN)| (1<<TWIE);
		if (tempHeader)
		{
			initialized1 = 1;
			tempHeader = 0;
			/*
			Endpoint_SelectEndpoint(CDC1_RX_EPADDR);
			uint8_t string8[] = {"Header 1 Complete, switching to header 2\n"};
			Endpoint_Write_Stream_LE(string8, sizeof(string8), NULL);
			Endpoint_ClearIN();
			*/
		}
		else
		{
			initialized2 = 1;
			tempHeader = 1;
			/*
			Endpoint_SelectEndpoint(CDC1_RX_EPADDR);
			uint8_t string7[] = {"Initialization Complete\n"};
			Endpoint_Write_Stream_LE(string7, sizeof(string7), NULL);
			Endpoint_ClearIN();
			*/
		}
		
		break;
	
	case 0x30:
		TWCR = (1<<TWINT) | (1<<TWSTO)|(1<<TWEN)| (1<<TWIE);
		/*
		Endpoint_SelectEndpoint(CDC1_RX_EPADDR);
		uint8_t string9[] = {"Stopped on nak HERE\n"};
		Endpoint_Write_Stream_LE(string9, sizeof(string9), NULL);
		Endpoint_ClearIN();
		*/
		break;
		
	case 0x38:
		TWCR = (1<<TWINT)|(1<<TWEN)| (1<<TWIE);
		/*
		Endpoint_SelectEndpoint(CDC1_RX_EPADDR);
		uint8_t string1[] = {"Lost Arbitration?\n"};
		Endpoint_Write_Stream_LE(string1, sizeof(string1), NULL);
		Endpoint_ClearIN();
		*/
		break;
		
	case 0x40:
		TWCR = (1<<TWINT)|(1<<TWEN)| (1<<TWIE) | (1<<TWEA);
	
		/*
		Endpoint_SelectEndpoint(CDC1_RX_EPADDR);
		uint8_t string2[] = {"SLA+R Transmitted and ACK Received?\n"};
		Endpoint_Write_Stream_LE(string2, sizeof(string2), NULL);
		Endpoint_ClearIN();
		*/
		break;
		
	case 0x48:
		TWCR = (1<<TWINT) | (1<<TWSTO);
		//T sensor becomes uninitialized
		if(tempHeader)
		{
			initialized1 = 0;
			tempHeader = 0;
			temp1 = 0;
		}
		else
		{
			initialized2 = 0;
			tempHeader = 1;
			temp2 = 0;
		}
		/*
		Endpoint_SelectEndpoint(CDC1_RX_EPADDR);
		uint8_t string3[] = {"HERE SLA+R Transmitted and NOT ACK NOT NOT Received?\n"};
		Endpoint_Write_Stream_LE(string3, sizeof(string3), NULL);
		Endpoint_ClearIN();
		*/
		break;
		
	case 0x50:
		
		temptemp = (TWDR);
		TWCR = (1<<TWINT)|(1<<TWEN)| (1<<TWIE);
		
		//Have it send a nak after the first byte
		//TWCR &= ~(1<<TWEA);
		
		//Load the data into the first half of the temp variable
		if(tempHeader)
		{
			temp1 = (temptemp << 8);
		}
		else
		{
			temp2 = (temptemp << 8);
		}
		/*
			Endpoint_SelectEndpoint(CDC1_RX_EPADDR);
			uint8_t string4[] = {"DATA received and ACK transmitted?\n"};
			Endpoint_Write_Stream_LE(string4, sizeof(string4), NULL);
			Endpoint_ClearIN();
		*/
		
		break;
		
	case 0x58:
		temptemp = TWDR;
		TWCR = (1<<TWINT) |(1<<TWEN)| (1<<TWIE) | (1<<TWSTO);
			
		//Load the data into the second half of the temp variable
		if(tempHeader)
		{
			temp1 |= (temptemp);
			g_temp1 = tempToFloat(temp1);
			tempHeader = 0;
			//sendTemp(temp1);
			temp1 = 0;
			temptemp = 0;
			/*
			Endpoint_SelectEndpoint(CDC1_RX_EPADDR);
			uint8_t string5[] = {"1 and NOT ACK transmitted?\n"};
			Endpoint_Write_Stream_LE(string5, sizeof(string5), NULL);
			Endpoint_Write_Stream_LE(g_countingUp, 2, NULL);
			Endpoint_ClearIN();
			*/
		}
		else
		{
			temp2 |= (temptemp);
			g_temp2 = tempToFloat(temp2);
			tempHeader = 1;
			//sendTemp(temp2);
			temp2 = 0;
			temptemp = 0;
			/*
			Endpoint_SelectEndpoint(CDC1_RX_EPADDR);
			uint8_t string5[] = {"2 and NOT ACK transmitted?\n"};
			Endpoint_Write_Stream_LE(string5, sizeof(string5), NULL);
			Endpoint_Write_Stream_LE(g_countingUp, 2, NULL);
			Endpoint_ClearIN();
			*/
		}

		break;
	default:
		break;
	}
}

void TWITimerInit()
{
	TCCR1A |= (1<<WGM10) | (1<<WGM11);
	TCCR1B |= (1<<WGM12) | (1<<CS10);
	TIMSK1 |= (1<<TOIE1);
}

void TWIInit()
{
	
	//TWITimerInit();
	
	TWBR = 240;
	//DDRD |= (1<<DDD0) | (1<<DDD1);
	//PORTD |= (1<<PORTD0) | (1<<PORTD1);
	TWCR |= (1<<TWEA) | (1<<TWEN) | (1<<TWIE);
	TWAR |= (1<<TWA4) | (1<<TWA1) | (1<<TWGCE);
	
	
	TWCR |= (1<<TWSTA) | (1<<TWINT) | (1<<TWEN);
}