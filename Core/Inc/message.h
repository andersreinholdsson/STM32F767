//////////////////////////////////////////////////////////////////////
//
//	Copyright © King Tiger Technology Inc., 2019
//	All Rights Reserved
//
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//	$Workfile: Message.h $
//	$Author: Archer $
//////////////////////////////////////////////////////////////////////
//
//	defines the messages passed around with in the KTI_ATC
//
//////////////////////////////////////////////////////////////////////

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MESSAGE_H
#define __MESSAGE_H
#include "main.h"
#include "cmsis_os2.h"

//
// message types
//

typedef enum {
	eOpen,				// open the chamber
	eClose,				// close the chamber
	eSetTemp,			// sets the temperature
	eGetTemp,			// gets the last set temp
	eReadTemp,			// read the actual temperature
} eMessage	;

//
// Message Structure
//

typedef struct {
	eMessage 	msgType;
	uint8_t 	buf[32];			// can be null (usually will be null)
} T_MSG;

#define MSG_COUNT 16
//
// message queue
//
extern osMessageQueueId_t MotorQHandle;
extern osMessageQueueId_t HeaterQHandle;
extern osMessageQueueId_t UsbQHandle;
extern osMessageQueueId_t GfxQHandle;
extern osMessageQueueId_t MainQHandle;



#endif 		// __MESSAGE_H
