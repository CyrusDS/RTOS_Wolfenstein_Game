/***************************************************************************//**
 * @file
 * @brief Top level application functions
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef APP_H
#define APP_H
//***********************************************************************************
// Include files
//***********************************************************************************
#include <gpio.h>
#include <capsense.h>
#include <lib_def.h>
#include <os.h>
#include <stdio.h>
#include <em_emu.h>
#include <os_trace.h>
#include "sl_board_control.h"
#include "em_assert.h"
#include "glib.h"
#include "dmd.h"
#include "os_cfg.h"
#include "btnqueue.h"
/***************************************************************************//**
 * Initialize application.
 ******************************************************************************/
//***********************************************************************************

// global variables

//***********************************************************************************

typedef struct{
  GLIB_Rectangle_t P_Rectangle_T[3][16];
  uint8_t HP_Rectangle_t[3][16];
}GlibCliff;
typedef struct{
  GLIB_Rectangle_t P_Rectangle_T[5][7];
  uint8_t HP_Rectangle_t[5][7];
}GlibCastle;

typedef struct{
  uint8_t currSpeed;
  uint8_t totalIncrement;
  uint8_t totalDecrement;
  uint8_t railgun_charge;
  uint16_t shield_remaining;
  bool shield_active;
  bool shield_protection;
  bool railgun_fire;
  bool railgun_charging;
}PlayerStatistics;

typedef struct{
  uint8_t currDirection;
  float currTime;
  uint8_t totalLeft;
  uint8_t totalRight;
  int8_t velocity;
}PlatformDirection;




enum VehicleFlags{
  speed = 0b1 << 0,
  direction = 0b1 << 1,
  shieldup = 0b1 << 0,
  railgun = 0b1 << 1,
};
enum LedOutputFlags{
  speed_violation = 0b1 << 0,
  no_speed_violation = 0b1 << 1,
  direction_violation = 0b1 << 2,
  no_direction_violation = 0b1 << 3,
};
enum PlatformDir{
   hardLeft = 0b1 << 0,
   gradualLeft = 0b1 << 1,
   hardRight = 0b1 << 2,
   gradualRight = 0b1 << 3,
   none = 0b1 << 4,
};

enum ButtonEventFlag{
   button0high = 0b1 << 0,
   button0low = 0b1 << 1,
   button1low = 0b1 << 2,
   button1high = 0b1 << 3,
};

//***********************************************************************************
// init / setup function prototypes
//***********************************************************************************
void GPIO_EVEN_IRQHandler(void);
void GPIO_ODD_IRQHandler(void);
uint8_t update_button0(void);
uint8_t update_button1(void);

void App_VehiclePlayerAction_MutexCreation(void);
void App_OS_Display_SemaphoreCreation(void);
void  App_OS_PlatformCtrl_SemaphoreCreation(void);
void App_PlatformAction_MutexCreation(void);
void App_TimerCallback (void *p_tmr, void *p_arg);
void  App_OS_TimerCreation (void);
void App_PlayerAction_Creation(void);
void App_PlatformCtrl_creation(void);
void App_Physics_Creation(void);
void App_LEDoutput_Creation(void);
void App_LCDdisplay_Creation(void);

void App_PlayerAction_Task(void  *p_arg);
void App_PlatformCtrl_Task(void  *p_arg);
void update_capsense(void);
void App_Physics_Task(void  *p_arg);
void App_LEDoutput_Task(void  *p_arg);
void App_LCDdisplay_Task(void  *p_arg);
void App_IdleTaskCreation(void);
void App_IdleTask (void  *p_arg);
void app_init(void);
void castle_open(void);
void player_setup(volatile PlayerStatistics *Stats);
#endif  // APP_H
