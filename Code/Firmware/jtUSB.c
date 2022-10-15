/*
             LUFA Library
     Copyright (C) Dean Camera, 2017.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2017  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *
 *  Main source file for the USB device application. This file contains the
 *  main tasks of the application and is responsible for the initial
 *  application hardware configuration.
 */

#include "LUFAConfig.h"
#include "jtUSB.h"


/** Main program entry point. This routine contains the overall program flow, including initial
 *  setup of all components and the main program loop.
 */

USB_ClassInfo_CDC_Device_t VirtualSerial1_CDC_Interface =
{
	.Config =
	{
		.ControlInterfaceNumber   = INTERFACE_ID_CDC1_CCI,
		.DataINEndpoint           =
		{
			.Address          = CDC_DEVICE_TO_HOST_EPADDR,
			.Size             = CDC_DATA_EPSIZE,
			.Banks            = 1,
		},
		.DataOUTEndpoint =
		{
			.Address          = CDC_HOST_TO_DEVICE_EPADDR,
			.Size             = CDC_DATA_EPSIZE,
			.Banks            = 1,
		},
		.NotificationEndpoint =
		{
			.Address          = CDC_CONTROL_EPADDR,
			.Size             = CDC_CONTROL_EPSIZE,
			.Banks            = 1,
		},
	},
};


static CDC_LineEncoding_t LineEncoding1 = { .BaudRateBPS = 0,
	.CharFormat  = CDC_LINEENCODING_OneStopBit,
	.ParityType  = CDC_PARITY_None,
.DataBits    = 8                            };

void USB_Init2(void)
{
	Endpoint_SelectEndpoint(CDC_HOST_TO_DEVICE_EPADDR);
	UEIENX |= (1<<RXOUTE);
}


/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
	#if (ARCH == ARCH_AVR8)
		/* Disable watchdog if enabled by bootloader/fuses */
		MCUSR &= ~(1 << WDRF);
		wdt_disable();

		/* Disable clock division */
		clock_prescale_set(clock_div_1);

		/* Hardware Initialization */
		USB_Init();
	#elif (ARCH == ARCH_XMEGA)
		/* Start the PLL to multiply the 2MHz RC oscillator to 32MHz and switch the CPU core to run from it */
		XMEGACLK_StartPLL(CLOCK_SRC_INT_RC2MHZ, 2000000, F_CPU);
		XMEGACLK_SetCPUClockSource(CLOCK_SRC_PLL);

		/* Start the 32MHz internal RC oscillator and start the DFLL to increase it to 48MHz using the USB SOF as a reference */
		XMEGACLK_StartInternalOscillator(CLOCK_SRC_INT_RC32MHZ);
		XMEGACLK_StartDFLL(CLOCK_SRC_INT_RC32MHZ, DFLL_REF_INT_USBSOF, F_USB);

		PMIC.CTRL = PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;

		/* Hardware Initialization */
		USB_Init(USB_OPT_RC32MCLKSRC | USB_OPT_BUSEVENT_PRIHIGH);
	#endif
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	/* Setup first CDC Interface's Endpoints */
	ConfigSuccess &= Endpoint_ConfigureEndpoint(CDC_OUT_EPADDR, EP_TYPE_BULK, CDC_DATA_EPSIZE, 1);
	ConfigSuccess &= Endpoint_ConfigureEndpoint(CDC_IN_EPADDR, EP_TYPE_BULK, CDC_DATA_EPSIZE, 1);
	ConfigSuccess &= Endpoint_ConfigureEndpoint(CDC_CONTROL_EPADDR, EP_TYPE_INTERRUPT, CDC_CONTROL_EPSIZE, 1);

	/* Reset line encoding baud rates so that the host knows to send new values */
	LineEncoding1.BaudRateBPS = 0;
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	/* Determine which interface's Line Coding data is being set from the wIndex parameter */
	void* LineEncodingData = &LineEncoding1;

	/* Process CDC specific control requests */
	switch (USB_ControlRequest.bRequest)
	{
		case CDC_REQ_GetLineEncoding:
		if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
		{
			Endpoint_ClearSETUP();

			/* Write the line coding data to the control endpoint */
			Endpoint_Write_Control_Stream_LE(LineEncodingData, sizeof(CDC_LineEncoding_t));
			Endpoint_ClearOUT();
		}

		break;
		case CDC_REQ_SetLineEncoding:
		if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
		{
			Endpoint_ClearSETUP();

			/* Read the line coding data in from the host into the global struct */
			Endpoint_Read_Control_Stream_LE(LineEncodingData, sizeof(CDC_LineEncoding_t));
			Endpoint_ClearIN();
		}

		break;
		case CDC_REQ_SetControlLineState:
		if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
		{
			Endpoint_ClearSETUP();
			Endpoint_ClearStatusStage();
		}

		break;
	}
}

