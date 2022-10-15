/*
 * Fan Controller 6.c
 *
 * Created: 1/29/2021 10:29:55 AM
 * Author : Jack2bs
 */ 

#include "Main.h"

//Holds the types of fans
//1 equals pwm, 0 equals 3 pin
volatile int g_fanTypes[6] = {1,1,1,1,1,1};
//Holds the speeds of the fans
volatile uint16_t g_fanSpeeds[6] = {};

//Holds the percentages of the fans
volatile int8_t g_fanPercentages[6] = {20, 30, 40, 40, 30, 20};

//Holds the temperature
int g_temp1 = 100;
int g_temp2 = 100;
int g_cpuTemp = 100;

//Holds the masks to use when turning on/off the fans on port b/d on overflow of timer
uint8_t g_timer3FanMask = 0b00011100;
uint8_t g_timer1FanMask = 0b11100000;

//Holds the data points used to calculate fan percentage
volatile uint8_t g_currentFanProfile[6][21] = {};

//Used for debugging where reset is coming from.	
//volatile uint8_t g_resetFlags = 0b11000000;

//Holds the number of seconds between calls to getFanPercent()
static volatile int g_seconds = 0;

//If an Interrupt that I haven't written occurs, just return
//This shouldn't ever happen
ISR(BADISR_vect)
{
	return;
}

//Updates the output compare register based on the values in g_fanpercentages
void alterFanSpeed(int fanNum)
{
	//which fan header
	switch(fanNum)
	{
		//header 0 - connected to Output compare register 3A
		case 0:
		
		//If it should spin (its not off at this temperature)
		if(g_fanPercentages[fanNum])
		{
			//Set the corresponding bit in the mask (PORTD2 evaluates to 2)
			g_timer3FanMask |= (1<<PORTD2);
			
			//Set the corresponding D and B pins (likely unnecessary)
			PORTD |= (1<<PORTD2);
			PORTB |= (1<<PORTB2);
			
			//If it should be at 100 percent
			if(g_fanPercentages[fanNum] == 100)
			{
				//Disable the interrupt for this fan
				TIMSK3 &= ~(1<<OCIE3A);
			}
			else
			{
				//Enable the interrupt
				TIMSK3 |= (1<<OCIE3A);
				
				//Set the Output Compare register to the proper value
				OCR3A = ((g_fanPercentages[fanNum] * (uint32_t)1022 )/ 100) + 1; //This calculation is based on the TOP of the timer being 1022, then just multiplying by percent/100
			}
		}
		//If the fan should not spin (percentage is 0 at this temp)
		else
		{
			//Clear the corresponding bit in the mask
			g_timer3FanMask &= ~(1<<PORTD2);
			
			//Disable the corresponding interrupt
			TIMSK3 &= ~(1<<OCIE3A);
			
			//Clear the corresponding B and D PORTS
			PORTD &= ~(1<<PORTD2);
			PORTB &= ~(1<<PORTB2);
		}
		break;
		
		//See annotations above
		
		//header 1 - connected to Output compare register 3B
		case 1:
				if(g_fanPercentages[fanNum])
				{
					g_timer3FanMask |= (1<<PORTD3);
					PORTD |= (1<<PORTD3);
					PORTB |= (1<<PORTB3);
					if(g_fanPercentages[fanNum] == 100)
					{
						TIMSK3 &= ~(1<<OCIE3B);
						
					}
					else
					{
						TIMSK3 |= (1<<OCIE3B);
						OCR3B = ((g_fanPercentages[fanNum] * (uint32_t)1022 )/ 100) + 1;
					}
				}
				else
				{
					g_timer3FanMask &= ~(1<<PORTD3);
					TIMSK3 &= ~(1<<OCIE3B);
					PORTD &= ~(1<<PORTD3);
					PORTB &= ~(1<<PORTB3);
				}
		break;
		//header 2 - connected to Output compare register 3C
		case 2:
				if(g_fanPercentages[fanNum])
				{
					g_timer3FanMask |= (1<<PORTD4);
					PORTD |= (1<<PORTD4);
					PORTB |= (1<<PORTB4);
					if(g_fanPercentages[fanNum] == 100)
					{
						TIMSK3 &= ~(1<<OCIE3C);
						
					}
					else
					{
						TIMSK3 |= (1<<OCIE3C);
						OCR3C = ((g_fanPercentages[fanNum] * (uint32_t)1022 )/ 100) + 1;
					}
				}
				else
				{
					g_timer3FanMask &= ~(1<<PORTD4);
					TIMSK3 &= ~(1<<OCIE3C);
					PORTD &= ~(1<<PORTD4);
					PORTB &= ~(1<<PORTB4);
				}
		break;
		//header 3 - connected to Output compare register 1A
		case 3:
		if(g_fanPercentages[fanNum])
		{
			g_timer1FanMask |= (1<<PORTD5);
			PORTD |= (1<<PORTD5);
			PORTB |= (1<<PORTB5);
			if(g_fanPercentages[fanNum] == 100)
			{
				TIMSK1 &= ~(1<<OCIE1A);
				
			}
			else
			{
				TIMSK1 |= (1<<OCIE1A);
				OCR1A = ((g_fanPercentages[fanNum] * (uint32_t)1022 )/ 100) + 1;
			}
		}
		else
		{
			g_timer1FanMask &= ~(1<<PORTD5);
			TIMSK1 &= ~(1<<OCIE1A);
			PORTD &= ~(1<<PORTD5);
			PORTB &= ~(1<<PORTB5);
		}
		break;
		//header 4 - connected to Output compare register 1B
		case 4:
		if(g_fanPercentages[fanNum])
		{
			g_timer1FanMask |= (1<<PORTD6);
			PORTD |= (1<<PORTD6);
			PORTB |= (1<<PORTB6);
			if(g_fanPercentages[fanNum] == 100)
			{
				TIMSK1 &= ~(1<<OCIE1B);
			}
			else
			{
				TIMSK1 |= (1<<OCIE1B);
				OCR1B = ((g_fanPercentages[fanNum] * (uint32_t)1022 )/ 100) + 1;
			}
		}
		else
		{
			g_timer1FanMask &= ~(1<<PORTD6);
			TIMSK1 &= ~(1<<OCIE1B);
			PORTD &= ~(1<<PORTD6);
			PORTB &= ~(1<<PORTB6);
		}
		break;
		//header 5 - connected to Output compare register 1C
		case 5:
		if(g_fanPercentages[fanNum])
		{
			g_timer1FanMask |= (1<<PORTD7);
			PORTD |= (1<<PORTD7);
			PORTB |= (1<<PORTB7);
			if(g_fanPercentages[fanNum] == 100)
			{
				TIMSK1 &= ~(1<<OCIE1C);
			}
			else
			{
				TIMSK1 |= (1<<OCIE1C);
				OCR1C = ((g_fanPercentages[fanNum] * (uint32_t)1022 )/ 100) + 1;
			}
		}
		else
		{
			g_timer1FanMask &= ~(1<<PORTD7);
			TIMSK1 &= ~(1<<OCIE1C);
			PORTD &= ~(1<<PORTD7);
			PORTB &= ~(1<<PORTB7);
		}
		break;
	}
	return;
}

//Used to get the fanpercentages based on g_currentFanProfile
void getFanPercent()
{
	//Loops through every fan
	for(int i = 0; i < 6; i++)
	{
		//A variable to store the temp we should use
		int temp;
		//If temp we should use is CPU Temp
		if (g_currentFanProfile[i][20] == 0)
		{
			temp = g_cpuTemp;
		}
		//else if the temp we should use is the first temp header
		else if (g_currentFanProfile[i][20] == 1)
		{
			temp = g_temp1;
		}
		//else use the second temp header
		else
		{
			temp = g_temp2;
		}
		
		//Loop through all ten data points (10 sets of a temp and fan speed make 20) {Basically I flattened an int[2][10] int an int[20]}
		for(int j = 0; j < 20; j+=2)
		{
			//If we reach the last data point
			if (j == 18)
			{
				//Just use the fan speed corresponding to the highest temperature value which in most cases should be 100 degrees C, and 0 fs
				g_fanPercentages[i] = g_currentFanProfile[i][19];
				
				//See annotations above
				alterFanSpeed(i);
				break;
			}
			
			//Fun math ahead
			
			//Find the first point whose temperature is greater than the current temp
			if(g_currentFanProfile[i][j] > temp)
			{
				//If this is the first point
				if (j == 0)
				{
					//Just use the fan speed corresponding to the lowest temp value which in most cases will be 0 degrees C, and 0 fs
					g_fanPercentages[i] = g_currentFanProfile[i][j + 1];
					
					//See annotations above
					alterFanSpeed(i);
					break;
				}
				else
				{
					double j1 = (double)g_currentFanProfile[i][j];		//Holds the first x value (the first temp value)
					double k1 = (double)g_currentFanProfile[i][j+1];	//Holds the first y value (the first fan speed value)
					
					double j2 = (double)g_currentFanProfile[i][j-2];	//Holds the second x value
					double k2 = (double)g_currentFanProfile[i][j-1];	//Holds the second y value
					
					//If the x values are equal (the slope is zero)
					if (j2 == j1)
					{
						//Then the y value is constant and we can use an endpoint
						g_fanPercentages[i] = k1;
					}
					//else The slope is nonzero
					else
					{
						//Find the slope ((k2-k1)/(j2-j1))
						//Multiply the slope by the change in x/temp (in the negative direction)
						//Add the upper fan speed endpoint value (because the change in x was negative)
						g_fanPercentages[i] = ((temp - j1) * ((k2-k1)/(j2-j1))) + k1;
					}
					
					//See above annotations
					alterFanSpeed(i);
					break;
				}
			}
		}
		
	}
}

//Called when receving a cpu temp value from the pc
void updateCpuTemp(uint8_t string[])
{
	int cputempTemp = 0;	//Temp Variable we can edit
	int index = 2;			//The index to use in the string
	int tens = 100;			//Used to unformat the cpu temp from string form
	//unformat the string bit by bit
	for (; index < 5; index++)
	{
		cputempTemp += (string[index] - '0') * tens;
		tens /= 10;
	}
	//Load the temp value into the actual variable
	g_cpuTemp = cputempTemp;
	
	//See above annotations
	getFanPercent();
}

//Called when receiving a new profile from the pc
void useProfileFromPC(uint8_t string[])
{
	//Starting index to use for the string
	int index = 2;
	
	//Get the fan num from the string
	int fanNum = string[index] - '0';
	index++;
	
	for (int j = 0; j < 20; j+=2)
	{
		g_currentFanProfile[fanNum][j] = 0;
		g_currentFanProfile[fanNum][j+1] = 100;
	}
	
	g_currentFanProfile[fanNum][20] = 2;
	
	uint8_t numOfDataPoints = ((string[index] - '0') + 2);
	index++;
	
	for(int j = 0; j < (2*numOfDataPoints); j+=2)
	{
		uint8_t temp = 0;
		temp += ((string[index] - '0') * 100);
		index++;
		temp += ((string[index] - '0') * 10);
		index++;
		temp += ((string[index] - '0'));
		index++;
		
		uint8_t fs = 0;
		fs += ((string[index] - '0') * 100);
		index++;
		fs += ((string[index] - '0') * 10);
		index++;
		fs += ((string[index] - '0'));
		index++;
		
		g_currentFanProfile[fanNum][j] = temp;
		g_currentFanProfile[fanNum][j + 1] = fs;
	}
	
	g_currentFanProfile[fanNum][20] = string[index] - '0';
	index++;
	
	getFanPercent();
	return;
}


int handleRequests(uint8_t string[])
{
	switch(string[1])
	{
		case '0':
		break;
		case '1':
		sendNumberOfFansTicket(6);
		break;
		case '2':
		useProfileFromPC(string);
		break;
		case '3':
		saveProfile();
		break;
		case '4':
		useProfileFromEEprom();
		break;
		case '5':
		updateCpuTemp(string);
		break;
			
	}
	return 1;
}

int incrementAdmux()
{
	if (ADMUX == 1)
	{
		ADMUX = 4;
	}
	else if (ADMUX == 7)
	{
		ADMUX = 0;
	}
	else
	{
		ADMUX++;
	}
	return ADMUX;
}

void watchdogInit()
{
	WDTCSR |= (1<<WDE);
}

int main(void)
{
	cli();
	
	CLKPR = (1<<CLKPCE);
	CLKPR = 0;
	
	MCUCR |= (1<<JTD);
	MCUCR |= (1<<JTD);
	
	DDRD = 0xff;
	DDRB = 0xff;
	PORTB = 0xff;
	DDRF = 0;
	//watchdogInit();
	TWIInit();
	analogCompInit();
	pwmInit();
	SetupHardware();
	
	useProfileFromEEprom();
		
	sei();
	
    while (1) 
    {	
		uint8_t buffer[200] = {};
		uint16_t bytesRead = 0;
		
		USB_USBTask();
		CDC_Device_USBTask(&VirtualSerial1_CDC_Interface);
		
		Endpoint_SelectEndpoint(CDC_HOST_TO_DEVICE_EPADDR);
		if(g_oneSecond)
		{
			g_seconds++;
			TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN) | (1<<TWEA)| (1<<TWIE);
			
			prepareForTach();
			g_oneSecond = 0;
			sendCommonTicket(6);
		}
		if(g_thirdTime)
		{
			handleTach();
			
		}
		else if(Endpoint_IsOUTReceived())
		{
			Endpoint_WaitUntilReady();
			Endpoint_Read_Stream_LE(buffer, 200, &bytesRead);
			Endpoint_ClearOUT();
			
			//If it is a request then handle it
			if(buffer[0] == '!')
			{
				handleRequests(buffer);
			}
		}
		if(g_seconds > 10)
		{
			g_seconds = 0;
			getFanPercent();
		}
    }
}

