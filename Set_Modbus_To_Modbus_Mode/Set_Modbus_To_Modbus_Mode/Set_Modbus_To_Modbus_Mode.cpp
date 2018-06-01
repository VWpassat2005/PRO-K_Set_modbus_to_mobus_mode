// serial communication PC <--> Modbus
// T.D. Landzaat (14073595)
// PRO-K Group E
// The Hague university of applied sciences
// Electrical Engineering


// !!This script assumes that the modbus is NOT set to modbus mode!!

#include "stdafx.h"
#include <cstdio>
#include <windows.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <atlbase.h>
#include <atlconv.h>

std::wstring s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

void main(void)
{
	//--------------------Connecting to COM-port--------------------//
	HANDLE SerialHandle;
	do {
		std::string ComNumString;
		std::cout << "\nEnter COM port number: ";
		std::cin >> ComNumString;
		std::string ComPrefix = "\\\\.\\COM";

		std::string comID = ComPrefix + ComNumString;

		//covert string to LPCWSTR
		std::wstring stemp = s2ws(comID);
		LPCWSTR ComPort = stemp.c_str();

		SerialHandle = CreateFile(ComPort,
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			0,
			NULL);

		if (SerialHandle == INVALID_HANDLE_VALUE)
			printf("Error opening port %s\n", ComPort);
		else
			printf("Port open\n\n", ComPort);

	} while (SerialHandle == INVALID_HANDLE_VALUE);

	//--------------------Setting up connection variables--------------------//

	DCB dcbSerialParams = { 0 };
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

	BOOL Status = GetCommState(SerialHandle, &dcbSerialParams);

	if (Status == FALSE)
		printf("An error occured\n");

	dcbSerialParams.BaudRate = CBR_57600;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;

	Status = SetCommState(SerialHandle, &dcbSerialParams);

	if (Status == FALSE)
	{
		printf("Error! in Setting DCB Structure\n\n");
	}
	else
	{
		printf("Baudrate = %d\n", dcbSerialParams.BaudRate);
		printf("ByteSize = %d\n", dcbSerialParams.ByteSize);
		printf("StopBits = %d\n", dcbSerialParams.StopBits);
		printf("Parity   = %d\n\n", dcbSerialParams.Parity);
	}

	//--------------------Setting up time-out--------------------//

	COMMTIMEOUTS timeouts = { 0 };

	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 50;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;

	if (SetCommTimeouts(SerialHandle, &timeouts) == FALSE)
		printf("Error! in Setting Time Outs\n");
	else
		printf("Setting Serial Port Timeouts Successful\n");


	//--------------------Writing to COM-port--------------------//
	char   RequestDataString[] = ":AC03FF00FF";
	DWORD  dNoOFBytestoWrite;
	DWORD  dNoOfBytesWritten = 0;

	dNoOFBytestoWrite = sizeof(RequestDataString);

	Status = WriteFile(SerialHandle,
		RequestDataString,
		dNoOFBytestoWrite,
		&dNoOfBytesWritten,
		NULL);

	if (Status == TRUE)
		printf("\n WRITING: %s  \n\n", RequestDataString);
	else
		printf("\nError %d when writing to data\n", GetLastError());



}



