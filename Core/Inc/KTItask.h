/*
 * tasks.h
 *
 *  Created on: April 22, 2021
 *      Author: archer
 */

#ifndef SRC_KTI_TASKS_H_
#define SRC_KTI_TASKS_H_
#include "cmsis_os2.h"

//
// external handles for the tasks
//
extern osThreadId_t MainTskHandle;
extern osThreadId_t MotorTskHandle;
extern osThreadId_t HeaterTskHandle;
extern osThreadId_t UsbTskHandle;
extern osThreadId_t GfxTskHandle;

//
// timer
//

//
// Task prototypes
//

void StartThreads();
__NO_RETURN void StartTaskMain(void *argument);
__NO_RETURN void StartTaskMotor(void *argument);
__NO_RETURN void TouchGFX_Task(void *argument);
__NO_RETURN void StartDefaultTask(void *argument);
__NO_RETURN void StartTaskHeater(void *argument);
__NO_RETURN void StartTaskUSB(void *argument);

#endif /* SRC_KTI_TASKS_H_ */
