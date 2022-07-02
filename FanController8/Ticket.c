/*
 * CFile1.c
 *
 * Created: 1/9/2021 2:11:04 PM
 *  Author: Jack2bs
 */ 

#include "Ticket.h"

void rpmToString(int rpm, uint8_t string[4])
{
	for (int i = 0; i < 4; i++)
	{
		string[3-i] = rpm % 10 + '0';
		rpm /= 10;
	}
}

void tempToString(int temp_l, uint8_t string[])
{
		for (int i = 0; i < 3; i++)
		{
			string[2-i] = temp_l % 10 + '0';
			temp_l /= 10;
		}
}

int sendNumberOfFansTicket(int nOfFans)
{
	Endpoint_SelectEndpoint(CDC_DEVICE_TO_HOST_EPADDR);
	
	uint8_t buffer[5] = {"%%%& "};
	buffer[4] = (nOfFans + '0');
	
	Endpoint_Write_Stream_LE(buffer, 5, NULL);
	
	buffer[0] = '7';
	buffer[1] = '5';
	buffer[2] = '3';
	buffer[3] = '\r';
	buffer[4] = '\n';
	
	Endpoint_Write_Stream_LE(buffer, 5, NULL);
	
	Endpoint_ClearIN();
	
	return 1;
	
}

/*

int sendMaxTicket(int nOfFans)
{
	Endpoint_SelectEndpoint(CDC_DEVICE_TO_HOST_EPADDR);
	
	uint8_t buffer[6] = {"%%%&& "};
	buffer[5] = (nOfFans + '0');
	
	Endpoint_Write_Stream_LE(buffer, 6, NULL);
	
	uint8_t string[4] = {"0000"};
	for (int i = 0; i < 6; i++)
	{
		if (fanMaxSpeeds[i])
		{
			rpmToString(fanMaxSpeeds[i], string);
			buffer[0] = '#';
			buffer[1] = (i + '0');
			for (int j = 0; j < 4; j++)
			{
				buffer[j + 2] = string[j];
			}
			Endpoint_Write_Stream_LE(buffer, 6, NULL);
		}
	}
	
	buffer[0] = '7';
	buffer[1] = '5';
	buffer[2] = '0';
	buffer[3] = '3';
	buffer[4] = '\r';
	buffer[5] = '\n';
	
	Endpoint_Write_Stream_LE(buffer, 6, NULL);
	Endpoint_ClearIN();
	
	return 1;
}

*/

int sendCommonTicket(int nOfFans)
{
	Endpoint_SelectEndpoint(CDC_DEVICE_TO_HOST_EPADDR);
	
	uint8_t buffer[6] = {"%%%&& "};
	buffer[5] = (nOfFans + '0');
	
	Endpoint_Write_Stream_LE(buffer, 6, NULL);
	
	uint8_t string[5] = {"00000"};
	for (int i = 0; i < 6; i++)
	{
		if (g_fanTypes[i])
		{
			rpmToString(g_fanSpeeds[i], string);
		}
		else
		{
			string[0] = '9';
			string[1] = '9';
			string[2] = '9';
			string[3] = '9';
		}
		buffer[0] = '#';
		buffer[1] = (i + '0');
		for (int j = 0; j < 4; j++)
		{
			buffer[j + 2] = string[j];
		}
		Endpoint_Write_Stream_LE(buffer, 6, NULL);
	}
	
	putTempInString(g_temp1, string, 0);
	Endpoint_Write_Stream_LE(string, sizeof(string), NULL);
	putTempInString(g_temp2, string, 1);
	Endpoint_Write_Stream_LE(string, sizeof(string), NULL);
	
	rpmToString(OCR1A, string);
	Endpoint_Write_Stream_LE(string, 4, NULL);
	rpmToString(OCR1C, string);
	Endpoint_Write_Stream_LE(string, 4, NULL);
	
	buffer[0] = '7';
	buffer[1] = '5';
	buffer[2] = '0';
	buffer[3] = '3';
	buffer[4] = '\r';
	buffer[5] = '\n';
	
	Endpoint_Write_Stream_LE(buffer, 6, NULL);
	
	//Endpoint_Write_Stream_LE(fanTypes, sizeof(fanTypes), NULL);
	
	//Endpoint_Write_Stream_LE(fanPercentages, sizeof(fanPercentages), NULL);
	
	Endpoint_ClearIN();
	
	//temp1 = 0;
	//temp2 = 0;
	
	return 1;
}