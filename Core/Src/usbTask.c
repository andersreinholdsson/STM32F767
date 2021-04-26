#include <main.h>
#include <dfu.h>
#include <usbTask.h>

/////////////////////////////////////////////////////////////////
//
// StartTaskUSB()
//		polls the USB interface
//		if a packet is found, check it via a CRC32
//		put the passed pack on the USB input queue
//		if no packet received then check the USB USB output queue
//
//
/////////////////////////////////////////////////////////////////
uint16_t CalcCheckSum(uint8_t* ptr, uint16_t size)
{
	uint16_t checkSum = 0;

	for (int i = 0; i < size; i++)
		checkSum = checkSum ^ *(ptr + i);

	return checkSum;
}
void SendNak()
{
	uint8_t buf[16];

	buf[0] = 0x68;
	buf[1] = 0;
	buf[2] = 0;
	buf[3] = 0x68;
	buf[4] = usbNAK;
	buf[5] = 0;
	buf[6] = 0x16;
	VCP_Write(buf, 7);
}

uint32_t ConvertBytes(uint8_t size, uint8_t *ptr, uint32_t value)
{
	if(size == 4)
	{
		*ptr     = (value >> 0)  & 0x00ff;
		*(ptr+1) = (value >> 8)  & 0x00ff;
		*(ptr+2) = (value >> 16) & 0x00ff;
		*(ptr+3) = (value >> 24) & 0x00ff;
	}
	else if(size == 2)
	{
		*ptr 		= (value >> 0) & 0x00ff;
		*(ptr+1)	= (value >> 8) & 0x00ff;
	}
	else if (size == 1)
	{
		*ptr = value & 0x00ff;
	}
	return size;
}

uint8_t GetPacket(uint8_t *buf, unt16_t *msgSize, uint16_t *pktSize)
{
	uint32_t tick = HAL_GetTick();
	int cnt = 0;
	uint8_t cmd = 0;
	uint8_t *ptr;
	// int bufSize = *pktSize;		// SAVE OFF SIZE OF THE INITIAL Buffer

	int tmp = VCP_Read((buffer + cnt), (HEADER_SIZE - cnt));
	// check for timeout
	if ((tmp == 0) && ((HAL_GetTick() - tick) > 5000) && (cnt > 0))
	{
		return 0;
	}

	if (tmp > 0)
		cnt += tmp;

	if (cnt >= HEADER_SIZE)
	{
		if ((buffer[0] != 0x68) || (buffer[3] != 0x68))
		{
			return 0;
		}

		cmd = buffer[4];
		*msgSize = (buffer[2] << 8) | buffer[1];

		*pktSize = (*msgSize) + USB_OVERHEAD;

		memcpy(ptr, buffer, HEADER_SIZE);	// copy in the header

		while (cnt < *pktSize)				// read the rest of the message plus the EOF and the CRC16
		{
			cnt += VCP_Read((buf + cnt), *pktSize - cnt);
			if((cnt < *pktSize) && ((HAL_GetTick() - tick) > 5000))	// 5 sec timeout
			{
				return 0;
			}
		}

		// check the message format
		uint8_t checkSum = CalcCheckSum(buf + HEADER_SIZE, *msgSize);

		// check the checksum and the EOF marker
		uint16_t size = *pktSize;
		if ((checkSum != *(buf + size - 2)) || 	(*(buf + size - 1) != 0x16))
		{
			return 0;
		}
	}
	return cmd;
}
///////////////////////////////////////////////////////////////////////////////////////////////
//
//	bool IsEchoOnly(uint8_t cmd)
//
//		True if the command only requires an echo for NAK
//		FALSE if the command need data sent back as well
//
///////////////////////////////////////////////////////////////////////////////////////////////
BOOL IsEchoOnly(uint8_t cmd)
{
	BOOL retVal = TRUE;
	if(cmd == usbStatusQuery)
		retVal = FALSE;
	return retVal;
}

__NO_RETURN void StartTaskUSB(void *argument)
{
	int cnt = 0;
	T_MSG msg;
	uint8_t buffer[64];
	uint8_t checkSum = 0;
	uint16_t msgSize = 0;		// size of the payload
	uint16_t pktSize = 0;		// total size of the msg

	//int tick = HAL_GetTick();
	while (1)
	{

		uint8_t cmd = GetPacket(&buffer, &msgSize, &pktSize);

		// packet is Valid so process it
		if(cmd == 0)
			SendNak();
		else
		{
			memset((void *)&msg, 0, sizeof(T_MSG));
			if(IsEchoOnly(cmd))
				VCP_Write(buffer, pktSize);						// echo back the ack.
			switch (cmd)
			{
				// startup messages work even if locked
				case usbHeaterPresent:			// host to tester reply if the tester is there (ping)
					break;
				case usbStatusQuery:// query the embedded controller for status.  valid states are LWAIT, INSERT, SWAIT, TESTING, PASS, FAIL.
				              // each state is a bit in the returned data, so multiple conditions can be true.  PASS and FAIL indicate the latest result.
				              // format 32 bit, state, FW version, UID[2:0]
					memset(buffer, 0, sizeof(buffer));
					// paySize = 21;
					buffer[0] = 0x68;
					buffer[1] = 0;		// (paySize >> 0) & 0x00ff;
					buffer[2] = 0;		// (paySize >> 8) & 0x00ff;
					buffer[3] = 0x68;
					buffer[4] = usbStatusQuery;
					int firmware = FIRMWARE;		// byte 3 is 0, byte21 major, byte 1 minor, byte 0 revision
					cnt = 0;
					cnt += ConvertBytes(4, buffer + HEADER_SIZE + cnt, firmware);
					cnt += ConvertBytes(4, buffer + HEADER_SIZE + cnt, status);

					uint32_t uidData = *(uint32_t*) (0x1FFF7A48);	// msb [95:64]
					cnt += ConvertBytes(4, buffer + HEADER_SIZE + cnt, uidData);
					uidData = *(uint32_t*) (0x1FFF7A44);		// msb [95:64]
					cnt += ConvertBytes(4, buffer + HEADER_SIZE + cnt, uidData);
					uidData = *(uint32_t*) (0x1FFF7A40);		// lsb [31::0]
					cnt += ConvertBytes(4, buffer + HEADER_SIZE + cnt, uidData);

					uint8_t checkSum = CalcCheckSum(buffer + HEADER_SIZE, cnt);

					buffer[cnt] = checkSum;
					buffer[cnt+1] = 0x16;
					buffer[1] = (cnt >> 0) & 0x00ff;
					buffer[2] = (cnt >> 8) & 0x00ff;
					VCP_Write(buffer, cnt + USB_OVERHEAD);
					break;
				case usbDFU:
					if(!lockFlag && !jobFlag)
						DfuRunBootLoader();
					break;

				case usbStartJob:
					if(!lockFlag || !jobFlag)
						break;

					msg.msgType = eStartJob;
					msg.count = pktSize;
					for(int i = 0; (i < pktSize) && (i < sizeof(buffer)); i++)
						buffer[i] = usbBuf[i];
					msg.address = buffer;
					// starts a job
					osMessageQueuePut(GfxQHandle, &msg, 0U, 0U);
					osMessageQueuePut(SocketQHandle, &msg, 0U, 0U);
					status &= ~statJobWait;
					status |= statStartWait;
					jobFlag = TRUE;
					status |= statJobFlag;

					break;
				case usbStopJob:				// stop current job
					if(!lockFlag && jobFlag)
						break;

					jobFlag = FALSE;
					status &= ~statJobFlag;
					msg.msgType = eStopJob;
					msg.count = pktSize;
					for(int i = 0; (i < pktSize) && (i < sizeof(buffer)); i++)
						buffer[i] = usbBuf[i];
					msg.address = buffer;
					// starts a job
					osMessageQueuePut(GfxQHandle, &msg, 0U, 0U);
					osMessageQueuePut(SocketQHandle, &msg, 0U, 0U);
					status |= statJobWait;
					break;

				// Testing commands				Testing commands are ignored (NAK) if not unlocked, and job started
				case usbInsertDetect:		 	// DIMM is inserted and insert debouce time satisfied
					// this msg type is only sent to the host PC
					break;
				case usbPowerOn:				// turn on power for supplies specified, at the specified voltage
					if(!lockFlag)
						break;
					msg.msgType = ePowerOn;
					msg.count = pktSize;
					for(int i = 0; (i < pktSize) && (i < sizeof(buffer)); i++)
						buffer[i] = usbBuf[i];
					msg.address = buffer;

					osMessageQueuePut(GfxQHandle, &msg, 0U, 0U);
					osMessageQueuePut(SocketQHandle, &msg, 0U, 0U);
					msg.msgType = 0;
					// wait for the acknowledgment
					while(msg.msgType != ePowerOn)
						osMessageQueueGet(SocketQHandle, &msg, 0U, 0U);
					break;
				case usbReadSPD:				// read spd and send back requested bytes  		? Encrypted ?
					if(!lockFlag)
						break;
					msg.msgType = eReadSPD;
					msg.count = pktSize;
					for(int i = 0; (i < pktSize) && (i < sizeof(buffer)); i++)
						buffer[i] = usbBuf[i];
					msg.address = buffer;
					osMessageQueuePut(GfxQHandle, &msg, 0U, 0U);
					osMessageQueuePut(SocketQHandle, &msg, 0U, 0U);

					// wait for the acknowledgment
					msg.msgType = 0;
					while(msg.msgType != eReadSPD)
						osMessageQueueGet(SocketQHandle, &msg, 0U, 0U);

					// msg from Socket task, has malloc'd buffer, and size of buffer
					buffer[0] 	= 0x68;
					buffer[1]	= (msg.count & 0x00ff);
					buffer[2]	= (msg.count >> 8) & 0xff;
					buffer[3]	= 0x68;
					buffer[4] 	= usbReadSPD;
					checkSum 	= CalcCheckSum(msg.address, msg.count);
					buffer[5]	= checkSum;
					buffer[6]	= 0x16;
					VCP_Write(buffer, HEADER_SIZE);
					VCP_Write(msg.address, msg.count);
					VCP_Write(buffer + HEADER_SIZE, USB_OVERHEAD - HEADER_SIZE);
					// free the SPD buffer
					free(msg.address);
					msg.address = NULL;
					msg.count = 0;
					break;
				case usbExecuteTest:			// erase, program, verify, SPD DAT in payload	? Encrypted ?
					if(lockFlag || !jobFlag)
						break;

					status &= ~statStartWait;
					status |= statTesting;
					msg.msgType = eTesting;
					msg.count = pktSize;
					msg.address = usbBuf;
					osMessageQueuePut(SocketQHandle, &msg, 0U, 0U);

					msg.msgType = 0;
					// wait for the acknowledgment
					while(msg.msgType != eTesting)
						osMessageQueueGet(SocketQHandle, &msg, 0U, 0U);

					// switching to pass or fail screen is driven from host
					buffer[0] 	= 0x68;
					buffer[1]	= 0;
					buffer[2]	= 0;
					buffer[3]	= 0x68;
					buffer[4] 	= usbExecuteTest;
					//buffer[5]	= msg.count;
					checkSum 	= 0;	//	= CalcCheckSum(buffer + 5, 1);
					buffer[5]	= checkSum;
					buffer[6]	= 0x16;
					VCP_Write(buffer, USB_OVERHEAD);
					status |= statStartWait;
					status &= ~statTesting;
					break;
				case usbPassScreen:
					msg.msgType = ePassScreen;
					msg.count = 0;
					osMessageQueuePut(GfxQHandle, &msg, 0U, 0U);
					break;
				case usbFailScreen:
					msg.msgType = eFailScreen;
					msg.count = 0;
					osMessageQueuePut(GfxQHandle, &msg, 0U, 0U);
					break;
				case usbPowerDown:				// all power turn off
					msg.msgType = ePowerOff;
					msg.count = 0;
					osMessageQueuePut(SocketQHandle, &msg, 0U, 0U);
					break;
				case usbRemoveDetect:			// DIMM is removed and remove debouce time satisfied
					// this message is only sent to the HOST
					break;
				case usbInsertQuery:			// returns if module is inserted or not payload[0] = 0 inserted, 1 if removed
					msg.msgType = eInsertQuery;
					msg.count = 0;
					osMessageQueuePut(MainQHandle, &msg, 0U, 0U);

					// wait for the acknowledgment
					msg.msgType = 0;
					while(msg.msgType != eInsertQuery)
						osMessageQueueGet(MainQHandle, &msg, 0U, 0U);

					buffer[0] = 0x68;
					buffer[1] = 1;
					buffer[2] = 0;
					buffer[3] = 0x68;
					buffer[4] = usbInsertQuery;
					buffer[5] = msg.buf[0];
					checkSum = CalcCheckSum(buffer + HEADER_SIZE, 1);
					buffer[6] = checkSum;
					buffer[7] = 0x16;
					VCP_Write(buffer, USB_OVERHEAD + 1);
					break;
				// Other				Diags can only be run if DIMM removed, not in a JOB, and unlocked (NAK if these conditions not true)
				case usbDiags:			// not defined
					break;
				//default:
//					break;
			}
			if(usbBuf != NULL)
			{
				free(usbBuf);
				usbBuf = NULL;
			}
		}
		// check Usb TX Queue -- from embedded to controller
		T_MSG msg;
		osStatus_t status = osMessageQueueGet(USB_TXQ_Handle, &msg, NULL, 50U);   // wait for message 50 mS
		if (status == osOK)
		{
			switch (msg.msgType)
			{
				case eInsert:	// insert detected
					if(!lockFlag && jobFlag)
					{
						status |= statInsert;
						status |= statStartWait;
						uint8_t tmp[] = {0x68, 0x0, 0x0, 0x68, usbInsertDetect, 0x0, 0x16};
						VCP_Write(tmp, 7);
					}
					break;
				case eRemove:
					if(!lockFlag && jobFlag)	// not locked and a job has started
					{
						status &= ~statInsert;
						status &= ~statStartWait;
						uint8_t tmp[] = {0x68, 0x0, 0x0, 0x68, usbRemoveDetect, 0x0, 0x16};
						VCP_Write(tmp, 7);
					}
					break;
				case eSpdString:
					{
						uint8_t tmp[] = {0x68, msg.count & 0x00ff, (msg.count >> 8) & 0x00ff, 0x68, usbString, 0x0, 0x16};
						checkSum = CalcCheckSum(msg.address, msg.count);
						tmp[5] = checkSum;
						VCP_Write(tmp, HEADER_SIZE);
						VCP_Write(msg.address, msg.count);
						VCP_Write(tmp + HEADER_SIZE, USB_OVERHEAD - HEADER_SIZE);
						free(msg.address);
					}
					break;
				default:
					break;
			}

		}
		cnt = 0;
		// tick = HAL_GetTick();

		osDelay(100);
	}

}
