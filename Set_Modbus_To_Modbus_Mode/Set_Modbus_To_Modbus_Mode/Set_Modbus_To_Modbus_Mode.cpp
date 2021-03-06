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

using namespace std;

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
		string ComNumString;
		cout << "\nEnter COM port number: ";
		cin >> ComNumString;
		string ComPrefix = "\\\\.\\COM";

		string comID = ComPrefix + ComNumString;

		//covert string to LPCWSTR
		wstring stemp = s2ws(comID);
		LPCWSTR ComPort = stemp.c_str();

		SerialHandle = CreateFile(ComPort,
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			0,
			NULL);

		if (SerialHandle == INVALID_HANDLE_VALUE)
			cout << "Error opening port: " << ComPort << endl;
		else
			cout << "Port open: " << ComPort << endl;

	} while (SerialHandle == INVALID_HANDLE_VALUE);

	//--------------------Setting up connection variables--------------------//

	DCB dcbSerialParams = { 0 };
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

	BOOL Status = GetCommState(SerialHandle, &dcbSerialParams);

	if (Status == FALSE)
		cout << "Error in function GetCommState" << endl;

	dcbSerialParams.BaudRate = CBR_57600;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;

	Status = SetCommState(SerialHandle, &dcbSerialParams);

	if (Status == FALSE)
	{
		cout << "Error in function SetCommState" << endl;
	}
	else
	{
		cout << "Baudrate = " << dcbSerialParams.BaudRate << endl;
		cout << "ByteSize = " << dcbSerialParams.ByteSize << endl;
		cout << "StopBits = " << dcbSerialParams.StopBits << endl;
		cout << "Parity = " << dcbSerialParams.Parity << endl;
	}

	//--------------------Setting up time-out--------------------//

	COMMTIMEOUTS timeouts = { 0 };

	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 50;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;

	if (SetCommTimeouts(SerialHandle, &timeouts) == FALSE)
		cout << "Error! in Setting Time Outs" << endl ;
	else
		cout << "Setting Serial Port Timeouts Successful" << endl;


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
		cout << " WRITING: " <<  RequestDataString << endl;
	else
		cout << "Error  when writing to data: " <<  GetLastError();



}



