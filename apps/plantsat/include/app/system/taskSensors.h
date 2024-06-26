/**
 * @file  taskSensors.h
 * @author Carlos Gonzalez C - carlgonz@uchile.cl
 * @author Camilo Rojas - camrojas@uchile.cl
 * @date 2020
 * @copyright GNU GPL v3
 *
 * This task implements a listener, that sends commands at periodical times.
 */
#ifndef _TASKSENSORS_H
#define _TASKSENSORS_H

#include "config.h"
#include "suchai/globals.h"

#include "suchai/osQueue.h"
#include "suchai/osDelay.h"

#include "suchai/repoCommand.h"


void taskSensors(void *param);
#endif //_TASKSENSORS_H
