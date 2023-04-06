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

/***************************************************************************//**
 * Initialize application.
 ******************************************************************************/
//***********************************************************************************
// Include files
//***********************************************************************************
#include "app.h"
//***********************************************************************************
// global variables
//***********************************************************************************
#define  APP_DEFAULT_TASK_STK_SIZE       4096u  /*   Stack size in CPU_STK.         */
#define  APP_DEFAULT_TASK_PRIORITY       22u
#define  APP_IDLE_TASK_PRIORITY         24u
#define  tauSlider                      2u
#define  tauDisplay                     100u
#define Max_Safe_Speed                   25
#define discharge_cost                  100
#define max_shield_and_start            300
#define railgun_charge_rate             5
#define railgun_max_charge             50

//Global structs only accessed upon pending mutex for related struct, then posting.
volatile PlayerStatistics PlayerStats;
volatile PlatformDirection PlatformDirectionInst;
GLIB_Rectangle_t Platform;
GLIB_Rectangle_t RightCanyon;
volatile GlibCliff WallObjects;
volatile GlibCastle CastleObjects;

//Global button queue, cap array states, and timer for updating time spent holding a direction
BtnQueue button0;
BtnQueue button1;
volatile bool cap_array[4] = {false,false,false,false};
volatile uint32_t currTimeTicks = 0;

//***********************************************************************************
// OS Stack variables
//***********************************************************************************
OS_TCB   App_PlayerActionTaskTCB;                            /*   Task Control Block.   */
CPU_STK  App_PlayerActionTaskStk[APP_DEFAULT_TASK_STK_SIZE]; /*   Stack.                */

OS_TCB   App_PlatformCtrlTaskTCB;                            /*   Task Control Block.   */
CPU_STK  App_PlatformCtrlTaskStk[APP_DEFAULT_TASK_STK_SIZE]; /*   Stack.                */

OS_TCB   App_PhysicsTaskTCB;                            /*   Task Control Block.   */
CPU_STK  App_PhysicsTaskStk[APP_DEFAULT_TASK_STK_SIZE]; /*   Stack.                */

OS_TCB   App_LEDoutputTaskTCB;                            /*   Task Control Block.   */
CPU_STK  App_LEDoutputTaskStk[APP_DEFAULT_TASK_STK_SIZE]; /*   Stack.                */

OS_TCB   App_LCDdisplayTaskTCB;                            /*   Task Control Block.   */
CPU_STK  App_LCDdisplayTaskStk[APP_DEFAULT_TASK_STK_SIZE]; /*   Stack.                */

/* Example Task Data:      */
OS_TCB   App_IdleTaskTCB;                            /*   Task Control Block.   */
CPU_STK  App_IdleTaskStk[APP_DEFAULT_TASK_STK_SIZE]; /*   Stack.                */

//***********************************************************************************
// Intertask communication variables - semaphores, event flags, mutex, timers, LCD Glib Context
//***********************************************************************************
static OS_SEM App_PlayerAction_Semaphore;
static OS_SEM App_Platform_Semaphore;

static OS_FLAG_GRP App_Physics_Event_Flag_Group;
static OS_FLAG_GRP App_LEDoutput_Event_Flag_Group;

static OS_MUTEX App_PlatformAction_Mutex;
static OS_MUTEX App_VehiclePlayerAction_Mutex;

static OS_TMR App_Platform_Timer;

static GLIB_Context_t glibContext;
//***********************************************************************************
// Task synchronization creation functions (mutex,semaphore, event flag groups)
// IRQhandler, TMR callback creation as well.
//***********************************************************************************
/***************************************************************************//**
 * @brief
 *   Interrupt handler to service pressing of buttons - specifically button0
 ******************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  GPIO_IntClear(1 << BUTTON0_pin); //clear interrupt for even pin
  push(&button0, update_button0());
//  uint8_t flag = update_button0();
  RTOS_ERR  err;
  OSSemPost(&App_PlayerAction_Semaphore,
            OS_OPT_POST_ALL,  /* No special option.                     */
            &err);
}
/***************************************************************************//**
 * @brief
 *   Interrupt handler to service pressing of buttons - specifically button1
 ******************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
  GPIO_IntClear(1 << BUTTON1_pin); //clear interrupt for odd pin

  push(&button1, update_button1());
//  uint8_t flag = update_button1();
  RTOS_ERR  err;
  OSSemPost(&App_PlayerAction_Semaphore,
            OS_OPT_POST_ALL,  /* No special option.                     */
            &err);
}
/***************************************************************************//**
*   Updates button 0 state upon even IRQ
*******************************************************************************/
uint8_t update_button0(void){
  if(!GPIO_PinInGet(BUTTON0_port, BUTTON0_pin) && GPIO_PinInGet(BUTTON1_port, BUTTON1_pin)) {
      return (button0high);
  }
  else if(!GPIO_PinInGet(BUTTON0_port, BUTTON0_pin) && !GPIO_PinInGet(BUTTON1_port, BUTTON1_pin)) {
      return (button0low);
  }
  else {
      return (button0low);
  }
}
/***************************************************************************//**
*   Updates button 1 state upon odd IRQ
*******************************************************************************/
uint8_t update_button1(void){
  if(!GPIO_PinInGet(BUTTON1_port, BUTTON1_pin) && GPIO_PinInGet(BUTTON0_port, BUTTON0_pin)) {
      return (button1high);
  }
  else if(!GPIO_PinInGet(BUTTON1_port, BUTTON1_pin) && !GPIO_PinInGet(BUTTON0_port, BUTTON0_pin)) {
      return (button1low);
  }
  else {
      return (button1low);
  }
}
/***************************************************************************//**
*   VehiclePlayerAction data mutex creation
*******************************************************************************/
void App_VehiclePlayerAction_MutexCreation(void){
  RTOS_ERR     err;

  OSMutexCreate (&App_VehiclePlayerAction_Mutex,
                 "App_VehiclePlayerAction_Mutex ",
                 &err);
  if (err.Code != RTOS_ERR_NONE) {
      /* Handle error on task creation. */
      printf("Error handling VehiclePlayerAction mutex creation");
  }
}
/***************************************************************************//**
*   PlatformAction data mutex creation
*******************************************************************************/
void App_PlatformAction_MutexCreation(void){
  RTOS_ERR     err;

  OSMutexCreate (&App_PlatformAction_Mutex,
                 "App_PlatformAction_Mutex ",
                 &err);
  if (err.Code != RTOS_ERR_NONE) {
      /* Handle error on task creation. */
      printf("Error handling App_PlatformAction_Mutex creation");
  }
}
/***************************************************************************//**
*   Timer callback to communicate with Platform action task using a semaphore,
*    to indicate when state of capsense should be updated
*******************************************************************************/
void App_TimerCallback (void *p_tmr, void *p_arg)
{
  RTOS_ERR    err;
  (void)&p_arg;
  (void)&p_tmr;
  OSSemPost(&App_Platform_Semaphore,
            OS_OPT_POST_ALL,  /* No special option.                     */
            &err);
  //Logic here
  currTimeTicks = currTimeTicks + 1; //One fifth of a second has passed

  if (err.Code != RTOS_ERR_NONE) {
      /* Handle error on task semaphore post. */
      printf("Error handling timer callback with posting semaphore");
  }

}
/***************************************************************************//**
*   Timer Creation for timer to periodically update capsense states.
*******************************************************************************/
void  App_OS_TimerCreation (void)
{
    RTOS_ERR     err;
                                       /* Create a periodic timer.               */
    OSTmrCreate(&App_Platform_Timer,            /*   Pointer to user-allocated timer.     */
                "App Platform Timer",           /*   Name used for debugging.             */
                   0,                  /*     10 initial delay.                   */
                 tauSlider,                  /*   10 Timer Ticks period.              */
                 OS_OPT_TMR_PERIODIC,  /*   Timer is periodic.                   */
                &App_TimerCallback,    /*   Called when timer expires.           */
                 DEF_NULL,             /*   No arguments to callback.            */
                &err);
    if (err.Code != RTOS_ERR_NONE) {
        /* Handle error on timer create. */
      printf("Error while Handling OS Timer Creation");
    }
}
/***************************************************************************//**
*   Semaphore Creation for Vehicle Direction Task
*******************************************************************************/
void  App_OS_PlatformCtrl_SemaphoreCreation (void)
{
    RTOS_ERR     err;       /* Create the semaphore. */
    OSSemCreate(&App_Platform_Semaphore,    /*   Pointer to user-allocated semaphore.          */
                "App_Platform Semaphore",   /*   Name used for debugging.                      */
                 0,                /*   Initial count: available in this case.        */
                &err);
    if (err.Code != RTOS_ERR_NONE) {
        /* Handle error on semaphore create. */
      printf("Error while Handling OS Semaphore creation");
    }
}
/***************************************************************************//**
*   Semaphore Creation for Vehicle PlayerAction Task
*******************************************************************************/
void  App_OS_PlayerAction_SemaphoreCreation (void)
{
    RTOS_ERR     err;       /* Create the semaphore. */
    OSSemCreate(&App_PlayerAction_Semaphore,    /*   Pointer to user-allocated semaphore.          */
                "App_PlayerAction Semaphore",   /*   Name used for debugging.                      */
                 0,                /*   Initial count: available in this case.        */
                &err);
    if (err.Code != RTOS_ERR_NONE) {
        /* Handle error on semaphore create. */
      printf("Error while Handling OS Semaphore creation");
    }
}
/***************************************************************************//**
*   Event Flags Creation for Vehicle monitor task.
*******************************************************************************/
void  App_OS_Physics_EventFlagGroupCreation (void)
{
    RTOS_ERR     err;                               /* Create the event flag. */
    OSFlagCreate(&App_Physics_Event_Flag_Group,              /*   Pointer to user-allocated event flag.         */
                 "App Physics Event Flag Group",             /*   Name used for debugging.                      */
                  0,                      /*   Initial flags, all cleared.                   */
                 &err);
    if (err.Code != RTOS_ERR_NONE) {
        /* Handle error on event flag create. */
        printf("Error while Handling OS Physics Event Flag Groups creation");
    }
}
/***************************************************************************//**
*   Event Flags Creation for LED output task, to indicate current vioations
*******************************************************************************/
void  App_OS_LEDoutput_EventFlagGroupCreation (void)
{
    RTOS_ERR     err;                               /* Create the event flag. */
    OSFlagCreate(&App_LEDoutput_Event_Flag_Group,              /*   Pointer to user-allocated event flag.         */
                 "App LEDoutput Event Flag Group",             /*   Name used for debugging.                      */
                  0,                      /*   Initial flags, all cleared.                   */
                 &err);
    if (err.Code != RTOS_ERR_NONE) {
        /* Handle error on event flag create. */
        printf("Error while Handling OS LEDoutput Event Flag Groups creation");
    }
}
//***********************************************************************************
// task creation functions
//***********************************************************************************
/***************************************************************************//**
 * @brief
 *  PlayerAction task creation
 *
 ******************************************************************************/
void  App_PlayerAction_Creation (void)
{
    RTOS_ERR     err;

    OSTaskCreate(&App_PlayerActionTaskTCB,                /* Pointer to the task's TCB.  */
                 "PlayerAction Task.",                    /* Name to help debugging.     */
                 &App_PlayerAction_Task,                   /* Pointer to the task's code. */
                  DEF_NULL,                          /* Pointer to task's argument. */
                  APP_DEFAULT_TASK_PRIORITY,             /* Task's priority.            */
                 &App_PlayerActionTaskStk[0],             /* Pointer to base of stack.   */
                 (APP_DEFAULT_TASK_STK_SIZE / 10u),  /* Stack limit, from base.     */
                  APP_DEFAULT_TASK_STK_SIZE,         /* Stack size, in CPU_STK.     */
                  10u,                               /* Messages in task queue.     */
                  0u,                                /* Round-Robin time quanta.    */
                  DEF_NULL,                          /* External TCB data.          */
                  OS_OPT_TASK_STK_CHK,               /* Task options.               */
                 &err);
    if (err.Code != RTOS_ERR_NONE) {
        /* Handle error on task creation. */
        printf("Error while Handling Speed Setpoint Task Creation");
    }
}
/***************************************************************************//**
 * @brief
 *  Vehicle direction task creation
 *
 ******************************************************************************/
void  App_PlatformCtrl_creation (void)
{
    RTOS_ERR     err;

    OSTaskCreate(&App_PlatformCtrlTaskTCB,                /* Pointer to the task's TCB.  */
                 "PlatformCtrl Task.",                    /* Name to help debugging.     */
                 &App_PlatformCtrl_Task,                   /* Pointer to the task's code. */
                  DEF_NULL,                          /* Pointer to task's argument. */
                  APP_DEFAULT_TASK_PRIORITY,             /* Task's priority.            */
                 &App_PlatformCtrlTaskStk[0],             /* Pointer to base of stack.   */
                 (APP_DEFAULT_TASK_STK_SIZE / 10u),  /* Stack limit, from base.     */
                  APP_DEFAULT_TASK_STK_SIZE,         /* Stack size, in CPU_STK.     */
                  10u,                               /* Messages in task queue.     */
                  0u,                                /* Round-Robin time quanta.    */
                  DEF_NULL,                          /* External TCB data.          */
                  OS_OPT_TASK_STK_CHK,               /* Task options.               */
                 &err);
    if (err.Code != RTOS_ERR_NONE) {
        /* Handle error on task creation. */
        printf("Error while Handling PlatformCtrl Task Creation");
    }
}
/***************************************************************************//**
 * @brief
 *  Vehicle monitor task creation
 *
 ******************************************************************************/
void  App_Physics_Creation (void)
{
    RTOS_ERR     err;

    OSTaskCreate(&App_PhysicsTaskTCB,                /* Pointer to the task's TCB.  */
                 "Physics Task.",                    /* Name to help debugging.     */
                 &App_Physics_Task,                   /* Pointer to the task's code. */
                  DEF_NULL,                          /* Pointer to task's argument. */
                  APP_DEFAULT_TASK_PRIORITY,             /* Task's priority.            */
                 &App_PhysicsTaskStk[0],             /* Pointer to base of stack.   */
                 (APP_DEFAULT_TASK_STK_SIZE / 10u),  /* Stack limit, from base.     */
                  APP_DEFAULT_TASK_STK_SIZE,         /* Stack size, in CPU_STK.     */
                  10u,                               /* Messages in task queue.     */
                  0u,                                /* Round-Robin time quanta.    */
                  DEF_NULL,                          /* External TCB data.          */
                  OS_OPT_TASK_STK_CHK,               /* Task options.               */
                 &err);
    if (err.Code != RTOS_ERR_NONE) {
        /* Handle error on task creation. */
        printf("Error while Handling Vehicle monitor Task Creation");
    }
}
/***************************************************************************//**
 * @brief
 *  LED Output task creation
 *
 ******************************************************************************/
void  App_LEDoutput_Creation (void)
{
    RTOS_ERR     err;

    OSTaskCreate(&App_LEDoutputTaskTCB,                /* Pointer to the task's TCB.  */
                 "LEDoutput Task.",                    /* Name to help debugging.     */
                 &App_LEDoutput_Task,                   /* Pointer to the task's code. */
                  DEF_NULL,                          /* Pointer to task's argument. */
                  APP_DEFAULT_TASK_PRIORITY,             /* Task's priority.            */
                 &App_LEDoutputTaskStk[0],             /* Pointer to base of stack.   */
                 (APP_DEFAULT_TASK_STK_SIZE / 10u),  /* Stack limit, from base.     */
                  APP_DEFAULT_TASK_STK_SIZE,         /* Stack size, in CPU_STK.     */
                  10u,                               /* Messages in task queue.     */
                  0u,                                /* Round-Robin time quanta.    */
                  DEF_NULL,                          /* External TCB data.          */
                  OS_OPT_TASK_STK_CHK,               /* Task options.               */
                 &err);
    if (err.Code != RTOS_ERR_NONE) {
        /* Handle error on task creation. */
        printf("Error while Handling LEDoutput Task Creation");
    }
}
/***************************************************************************//**
 * @brief
 *  Vehicle LCD display task creation
 *
 ******************************************************************************/
void  App_LCDdisplay_Creation (void)
{
    RTOS_ERR     err;

    OSTaskCreate(&App_LCDdisplayTaskTCB,                /* Pointer to the task's TCB.  */
                 "LCDdisplay Task.",                    /* Name to help debugging.     */
                 &App_LCDdisplay_Task,                   /* Pointer to the task's code. */
                  DEF_NULL,                          /* Pointer to task's argument. */
                  APP_DEFAULT_TASK_PRIORITY,             /* Task's priority.            */
                 &App_LCDdisplayTaskStk[0],             /* Pointer to base of stack.   */
                 (APP_DEFAULT_TASK_STK_SIZE / 10u),  /* Stack limit, from base.     */
                  APP_DEFAULT_TASK_STK_SIZE,         /* Stack size, in CPU_STK.     */
                  10u,                               /* Messages in task queue.     */
                  0u,                                /* Round-Robin time quanta.    */
                  DEF_NULL,                          /* External TCB data.          */
                  OS_OPT_TASK_STK_CHK,               /* Task options.               */
                 &err);
    if (err.Code != RTOS_ERR_NONE) {
        /* Handle error on task creation. */
        printf("Error while Handling LCD display Task Creation");
    }
}
//***********************************************************************************
// task creation functions
//***********************************************************************************
/***************************************************************************//**
*   Requests update from the global button queue with exclusive access
*   Pends mutex to gain access to PlayerStats struct and update speed information
*   and total increments, decrements of speed. Posts mutex upon finishing.
*******************************************************************************/
void  App_PlayerAction_Task(void  *p_arg){
 (void)&p_arg;
 RTOS_ERR  err;
 PlayerStats.currSpeed = 0;
 PlayerStats.totalDecrement = 0;
 PlayerStats.totalIncrement = 0;
 volatile uint8_t prevSpeed;
 volatile uint8_t prevRailgun = button0high;
// volatile uint8_t endSpeed;

 while (DEF_TRUE) {
     OSSemPend(&App_PlayerAction_Semaphore,
                0,
                OS_OPT_PEND_BLOCKING,
                NULL,
                &err);
     //Ensure exclusive access during push and pop operation
     CORE_DECLARE_IRQ_STATE;
     CORE_ENTER_ATOMIC();
     uint8_t rail_gun = pop(&button0);
     uint8_t shield = pop(&button1);
     CORE_EXIT_ATOMIC();

           /* Acquire resource protected by mutex.       */
      OSMutexPend(&App_VehiclePlayerAction_Mutex,             /*   Pointer to user-allocated mutex.         */
      0,                  /*   Wait for a maximum of 0 OS Ticks.     */
      OS_OPT_PEND_BLOCKING,  /*   Task will block.                         */
      DEF_NULL,              /*   Timestamp is not used.                   */
      &err);
      prevSpeed = PlayerStats.currSpeed;
//     if(rail_gun == button0high && prevRailgun == button0low) {
      if(rail_gun == button0high) {
         PlayerStats.railgun_charge = 0;
         PlayerStats.railgun_charging = true;
         PlayerStats.railgun_fire = false;
     }
//     else if(rail_gun == button0high && prevRailgun == button0high) {
//         PlayerStats.railgun_charging = true;
//         PlayerStats.railgun_fire = false;
//     }
     else if(rail_gun == button0low && prevRailgun == button0high) {
         PlayerStats.railgun_fire = true;
         PlayerStats.railgun_charging = false;
//         PlayerStats.railgun_charge = 0;
     }
     else {
         PlayerStats.railgun_fire = false;
         PlayerStats.railgun_charging = false;
         //PlayerStats.railgun_charge = 0;
     }


     if(shield == button1high) {
//         PlayerStats.shield_remaining -= discharge_cost;
         PlayerStats.shield_active = true;
     }
     else {
         PlayerStats.shield_active = false;
     }
     PlayerStats.currSpeed = (5 * PlayerStats.totalIncrement) - (5 * PlayerStats.totalDecrement);
     if(PlayerStats.currSpeed != prevSpeed) {
         OSFlagPost(&App_Physics_Event_Flag_Group,
                     speed,
                     OS_OPT_POST_FLAG_SET,
                     &err);
     }
     prevRailgun = rail_gun;

           /* Release resource protected by mutex.       */
      OSMutexPost(&App_VehiclePlayerAction_Mutex,         /*   Pointer to user-allocated mutex.         */
      OS_OPT_POST_1,     /*   Only wake up highest-priority task.      */
      &err);

     if (err.Code != RTOS_ERR_NONE) {
         printf("Error while Handling PlayerAction task");
     }
   }
 }
/***************************************************************************//**
*   Requests update to capacitive touch sensor periodically, and determines platform control movement.
*  Pends mutex to gain access to update PlatformDirectionInst then posts mutex upon finishing.
*******************************************************************************/
void  App_PlatformCtrl_Task(void  *p_arg){
 (void)&p_arg;
 RTOS_ERR  err;
 OSTmrStart (&App_Platform_Timer,
             &err);
// volatile uint8_t prevDirection = none;

 while (DEF_TRUE) {
     OSSemPend(&App_Platform_Semaphore,
                0,
                OS_OPT_PEND_BLOCKING,
                NULL,
                &err);

          /* Acquire resource protected by mutex.       */
     OSMutexPend(&App_PlatformAction_Mutex,             /*   Pointer to user-allocated mutex.         */
     0,                  /*   Wait for a maximum of 0 OS Ticks.     */
     OS_OPT_PEND_BLOCKING,  /*   Task will block.                         */
     DEF_NULL,              /*   Timestamp is not used.                   */
     &err);
     //Logic to Update capsense
    update_capsense();
      //Logic to update Platform during mutex
     if(cap_array[0]) {
         PlatformDirectionInst.currDirection = hardLeft;
     }
     else if(cap_array[1]) {
         PlatformDirectionInst.currDirection = gradualLeft;
     }
     else if(cap_array[2]) {
         PlatformDirectionInst.currDirection = gradualRight;
     }
     else if(cap_array[3]) {
         PlatformDirectionInst.currDirection = hardRight;
     }
     else {
         PlatformDirectionInst.currDirection = none;
     }
//     prevDirection = PlatformDirectionInst.currDirection;
     OSFlagPost(&App_Physics_Event_Flag_Group,
                 direction,
                 OS_OPT_POST_FLAG_SET,
                 &err);

           /* Release resource protected by mutex.       */
      OSMutexPost(&App_PlatformAction_Mutex,         /*   Pointer to user-allocated mutex.         */
      OS_OPT_POST_1,     /*   Only wake up highest-priority task.      */
      &err);
     if (err.Code != RTOS_ERR_NONE) {
         printf("Error while Handling App_PlatformCtrl_Task task");
     }
   }
 }
/***************************************************************************//**
 * @brief
 *  Reads Capacitive sensor input and updates global bool array for each section.
 *  Note: pressing two sections on the same side will register as pressing the inner section (Gradual)
 ******************************************************************************/
void update_capsense(void){
    CAPSENSE_Sense();
    for(uint8_t i = 0; i < 4; i++) {
        cap_array[i] = CAPSENSE_getPressed(i);
    }
    //being touched on opposite sides
      if((cap_array[0] && cap_array[2]) || (cap_array[0] && cap_array[3])) {
          for(uint8_t i = 0; i < 4; i++) {//being touched on opposite sides
              cap_array[i] = false;
          }
      }
      //being touched on opposite sides
      else if((cap_array[1] && cap_array[2]) || (cap_array[1] && cap_array[3])) {
          for(uint8_t i = 0; i < 4; i++) {
              cap_array[i] = false;
          }
      } //Removed logic where two on same side will turn capsense array off - not desired here.
//      else if((cap_array[0] && cap_array[1]) || (cap_array[2] && cap_array[3])) {
//          for(uint8_t i = 0; i < 4; i++) {
//              cap_array[i] = false;
//          }
//      }
      else if(cap_array[0] && cap_array[1]) {
          for(uint8_t i = 0; i < 4; i++) {
              cap_array[i] = false;
          }
          cap_array[1] = true;
      }
      else if(cap_array[2] && cap_array[3]) {
          for(uint8_t i = 0; i < 4; i++) {
              cap_array[i] = false;
          }
          cap_array[2] = true;
      }
    return;
}


/***************************************************************************//**
* Pends upon Physics event flag group when either a speed update or direction update occurs
* Handles mutexes for each struct to update data, and then checks for violations.
* Sends flag to LED output event group to update visual violations.
*******************************************************************************/
void  App_Physics_Task(void  *p_arg){
 (void)&p_arg;
 RTOS_ERR  err;
 OS_FLAGS flag;
 volatile uint8_t prevDirection = 0;
 int8_t currentAccel = 0;
// volatile uint8_t prevSpeed = 0;
// volatile uint8_t prevTime = 0;
// OSTmrStart (&App_currVehicledirection_Timer,
//             &err);

 while (DEF_TRUE) {
     OSFlagPend(&App_Physics_Event_Flag_Group,                /*   Pointer to user-allocated event flag. */
               (speed | direction),             /*   Flag bitmask to be matched.                */
               0,                      /*   Wait for 0 OS Ticks maximum.        */
               OS_OPT_PEND_FLAG_SET_ANY |/*   Wait until all flags are set and      */
               OS_OPT_PEND_BLOCKING     |/*    task will block and                  */
               OS_OPT_PEND_FLAG_CONSUME, /*    function will clear the flags.       */
               DEF_NULL,                 /*   Timestamp is not used.                */
              &err);
     flag = OSFlagPendGetFlagsRdy(&err);
           /* Acquire resource protected by mutex.       */
     OSMutexPend(&App_VehiclePlayerAction_Mutex,             /*   Pointer to user-allocated mutex.         */
     0,                  /*   Wait for a maximum of 1000 OS Ticks.     */
     OS_OPT_PEND_BLOCKING,  /*   Task will block.                         */
     DEF_NULL,              /*   Timestamp is not used.                   */
     &err);
          /* Acquire resource protected by mutex.       */
     OSMutexPend(&App_PlatformAction_Mutex,             /*   Pointer to user-allocated mutex.         */
     0,                  /*   Wait for a maximum of 1000 OS Ticks.     */
     OS_OPT_PEND_BLOCKING,  /*   Task will block.                         */
     DEF_NULL,              /*   Timestamp is not used.                   */
     &err);

     if(flag == speed || direction) {
         //Don't need to do anything specific to speed - violations are checked after
         //over 75 - VIOLATION
         if(PlayerStats.currSpeed > 75) {
             OSFlagPost(&App_LEDoutput_Event_Flag_Group,
                         speed_violation,
                         OS_OPT_POST_FLAG_SET,
                         &err);
         }
             OSFlagPost(&App_LEDoutput_Event_Flag_Group,
                        speed_violation,
                         OS_OPT_POST_FLAG_SET,
                         &err);
//         //Turning and over 45 - VIOLATION
//         else if(PlayerStats.currSpeed > 45 && PlatformDirectionInst.currDirection < none) {
//             OSFlagPost(&App_LEDoutput_Event_Flag_Group,
//                        speed_violation,
//                         OS_OPT_POST_FLAG_SET,
//                         &err);
//         }
//         else {
//             OSFlagPost(&App_LEDoutput_Event_Flag_Group,
//                        no_speed_violation,
//                         OS_OPT_POST_FLAG_SET,
//                         &err);
//         }
         PlatformDirectionInst.currTime = (currTimeTicks/5); //1 tick = 1/5th of a second


         //Update railgun charge, fire status, etc
         if(PlayerStats.railgun_fire == true) {
             //         PlayerStats.railgun_charge = 0;
         }
         if(PlayerStats.railgun_charging == true && PlayerStats.railgun_charge < railgun_max_charge) {
             PlayerStats.railgun_charge += railgun_charge_rate;
         }


         //Update shield charge, discharge values, and indicate whether protection is active.
         if(PlayerStats.shield_active == true && (PlayerStats.shield_remaining >= discharge_cost/5)) {
             PlayerStats.shield_remaining -= discharge_cost/5;
             PlayerStats.shield_protection = true;
         }
         else {
             if(PlayerStats.shield_active == true) {
                 PlayerStats.shield_protection = false;
             }
             else {
                 if(PlayerStats.shield_remaining <= (max_shield_and_start -  discharge_cost/20) )
                 PlayerStats.shield_remaining += discharge_cost/20;
                 PlayerStats.shield_protection = false;
             }
         }
        //Logic
         //Same direction for at or above 5 seconds limit - VIOLATION
         if(PlatformDirectionInst.currTime >= 5 && PlatformDirectionInst.currDirection != none) {
             prevDirection = PlatformDirectionInst.currDirection;
             OSFlagPost(&App_LEDoutput_Event_Flag_Group,
                        direction_violation,
                         OS_OPT_POST_FLAG_SET,
                         &err);
         }
         else {
             OSFlagPost(&App_LEDoutput_Event_Flag_Group,
                        no_direction_violation,
                         OS_OPT_POST_FLAG_SET,
                         &err);
         }
     prevDirection = PlatformDirectionInst.currDirection;


     //Update platform accel, current velocity.
     if(PlatformDirectionInst.currDirection == hardLeft) {
         currentAccel = -2;
     }
     else if(PlatformDirectionInst.currDirection == gradualLeft) {
         currentAccel = -1;
     }
     else if(PlatformDirectionInst.currDirection == hardRight) {
         currentAccel = 2;
     }
     else if(PlatformDirectionInst.currDirection == gradualRight) {
         currentAccel = 1;
     }
     else {
         currentAccel = 0;
     }
     PlatformDirectionInst.velocity += currentAccel;
     Platform.xMin += PlatformDirectionInst.velocity;
     Platform.xMax += PlatformDirectionInst.velocity;


     //Right wall bounce
     if(Platform.xMax >= RightCanyon.xMin) {
         if(PlatformDirectionInst.velocity > Max_Safe_Speed) {
             //Destroy platform
             PlatformDirectionInst.velocity = -PlatformDirectionInst.velocity;
         }
         else {
             PlatformDirectionInst.velocity = -PlatformDirectionInst.velocity;
             //Bounce harmlessly off right wall.
         }
     }

     //Left wall bounce (only need to check one thickness, but check the four above it.
     for(int i = 0; i < 3; i++) {
         if((Platform.xMin <= WallObjects.P_Rectangle_T[0][i].xMax) && (WallObjects.HP_Rectangle_t[0][i] > 0)) { //Hit the wall and it still exists
             if(-PlatformDirectionInst.velocity > Max_Safe_Speed) { //Flip sign since you are travelling left
                 //Destroy platform
                 PlatformDirectionInst.velocity = -PlatformDirectionInst.velocity;
             }
             else {
                 PlatformDirectionInst.velocity = -PlatformDirectionInst.velocity;
                 //Bounce harmlessly off left wall.
             }
         }
     }

         /* Release resource protected by mutex.       */
     OSMutexPost(&App_VehiclePlayerAction_Mutex,         /*   Pointer to user-allocated mutex.         */
     OS_OPT_POST_1,     /*   Only wake up highest-priority task.      */
     &err);
          /* Release resource protected by mutex.       */
     OSMutexPost(&App_PlatformAction_Mutex,         /*   Pointer to user-allocated mutex.         */
     OS_OPT_POST_1,     /*   Only wake up highest-priority task.      */
     &err);
     }


     if (err.Code != RTOS_ERR_NONE) {
         printf("Error while Handling App_Physics_Task task");
     }
   }
 }
/***************************************************************************//**
*  Outputs to LED0 and LED1 based on any violations that occur, or if no violation occurs, turns off said LEDs.
*******************************************************************************/
void  App_LEDoutput_Task(void  *p_arg){
 (void)&p_arg;
 RTOS_ERR  err;
 OS_FLAGS flag;

 while (DEF_TRUE) {
     OSFlagPend(&App_LEDoutput_Event_Flag_Group,                /*   Pointer to user-allocated event flag. */
               (speed_violation | no_speed_violation | direction_violation | no_direction_violation),             /*   Flag bitmask to be matched.                */
               0,                      /*   Wait for 0 OS Ticks maximum.        */
               OS_OPT_PEND_FLAG_SET_ANY |/*   Wait until all flags are set and      */
               OS_OPT_PEND_BLOCKING     |/*    task will block and                  */
               OS_OPT_PEND_FLAG_CONSUME, /*    function will clear the flags.       */
               DEF_NULL,                 /*   Timestamp is not used.                */
              &err);
     flag = OSFlagPendGetFlagsRdy(&err);
     if(flag == speed_violation) {
         GPIO_PinOutSet(LED0_port, LED0_pin);
     }
     else if(flag == no_speed_violation ) {
         GPIO_PinOutClear(LED0_port, LED0_pin);
     }
     if(flag == direction_violation) {
         GPIO_PinOutSet(LED1_port, LED1_pin);
     }
     else if(flag == no_direction_violation) {
         GPIO_PinOutClear(LED1_port, LED1_pin);
     }

     if (err.Code != RTOS_ERR_NONE) {
         printf("Error while Handling App_LEDoutput_Task task");
     }
   }
 }
/***************************************************************************//**
*  Updates LCD display with Wolfenstein graphics
*******************************************************************************/
void  App_LCDdisplay_Task(void  *p_arg){
 (void)&p_arg;
 RTOS_ERR  err;
 uint8_t dispSpeed;
 uint8_t dispLeft;
 uint8_t dispRight;
 uint8_t dispTime;
// volatile uint8_t dispTEST;
 char *dispDir = "none"; //none initially
 GLIB_Rectangle_t ShieldCharge;
 GLIB_Rectangle_t RailgunCharge;
 ShieldCharge.xMax = 105;
 ShieldCharge.xMin = 110;
 RailgunCharge.xMax = 95;
 RailgunCharge.xMin = 100;
 uint32_t status;
 uint32_t MAX_STR_LEN = 100;

 while (DEF_TRUE) {
     OSMutexPend(&App_VehiclePlayerAction_Mutex,             /*   Pointer to user-allocated mutex.         */
     0,                  /*   Wait for a maximum of 1000 OS Ticks.     */
     OS_OPT_PEND_BLOCKING,  /*   Task will block.                         */
     DEF_NULL,              /*   Timestamp is not used.                   */
     &err);
     /* Initialize the glib context */
     status = GLIB_contextInit(&glibContext);
     EFM_ASSERT(status == GLIB_OK);

     glibContext.backgroundColor = White;
     glibContext.foregroundColor = Black;

     /* Fill lcd with background color */
     GLIB_clear(&glibContext);

     //Draw static canyon
     GLIB_drawRectFilled(&glibContext, &RightCanyon);


     dispSpeed = PlayerStats.currSpeed;
     char str[100];
     snprintf(str, MAX_STR_LEN, "SPD:%d MPH",dispSpeed);
     //GLIB_drawStringOnLine(&glibContext, str, 0, GLIB_ALIGN_LEFT,0,5,true);

         /* Release resource protected by mutex.       */
     OSMutexPost(&App_VehiclePlayerAction_Mutex,         /*   Pointer to user-allocated mutex.         */
     OS_OPT_POST_1,     /*   Only wake up highest-priority task.      */
     &err);


          /* Acquire resource protected by mutex.       */
     OSMutexPend(&App_PlatformAction_Mutex,             /*   Pointer to user-allocated mutex.         */
     0,                  /*   Wait for a maximum of 1000 OS Ticks.     */
     OS_OPT_PEND_BLOCKING,  /*   Task will block.                         */
     DEF_NULL,              /*   Timestamp is not used.                   */
     &err);

     //dispDir = PlatformDirectionInst.currDirection;
     dispLeft = PlatformDirectionInst.totalLeft;
     dispRight = PlatformDirectionInst.totalRight;
     dispTime = PlatformDirectionInst.currTime;
//     dispTEST = PlatformDirectionInst.currDirection;


     //Update Castle cliff wall
     for(int i = 0; i < 16; i++) {
         for(int j = 0; j < 3; j++) {
         GLIB_drawRectFilled(&glibContext, &WallObjects.P_Rectangle_T[j][i]);
         }
     }
     //Update castle
     for(int i = 0; i < 4; i++) {
         for(int j = 0; j < 7; j++) {
             GLIB_drawRectFilled(&glibContext, &CastleObjects.P_Rectangle_T[i][j]);
         }
     }
     //Draw updated platform
     GLIB_drawRectFilled(&glibContext, &Platform);

     //Draw shield bar
     ShieldCharge.yMin = 130 -(PlayerStats.shield_remaining/10);
     ShieldCharge.yMax = 130;
     GLIB_drawRect(&glibContext, &ShieldCharge);

     //Draw current railgun charging and indicate if railgun has been fired by filling rect in.
     RailgunCharge.yMin = 130 -(PlayerStats.railgun_charge);
     RailgunCharge.yMax = 130;
     if(PlayerStats.railgun_fire == true) {
         GLIB_drawRectFilled(&glibContext, &RailgunCharge);
         PlayerStats.railgun_charge = 0;
     }
     else {
         if(PlayerStats.railgun_charge == railgun_max_charge) {
             GLIB_drawRectFilled(&glibContext, &RailgunCharge);
         }
         else {
             GLIB_drawRect(&glibContext, &RailgunCharge);
         }
     }

     //Draw Shield (if active)
     if(PlayerStats.shield_protection == true) {
         //GLIB_drawPartialCircle(&glibContext, (Platform.xMax - Platform.xMin)/2, (Platform.yMax - Platform.yMin)/2, 5,0b100);
         //GLIB_drawCircleFilled(&glibContext, (200), (20), 5);
         snprintf(str, MAX_STR_LEN, "( )\n");
         GLIB_drawStringOnLine(&glibContext, str, 10, GLIB_ALIGN_LEFT,(Platform.xMax + Platform.xMin)/2 - 12,15,true);
     }
     else {
         snprintf(str, MAX_STR_LEN, "\n");
         GLIB_drawStringOnLine(&glibContext, str, 10, GLIB_ALIGN_LEFT,(Platform.xMax + Platform.xMin)/2 - 12,15,true);
     }

     if(PlatformDirectionInst.currDirection == hardLeft) {
        dispDir = "HardLeft";;
    }
    else if(PlatformDirectionInst.currDirection == gradualLeft) {
        dispDir = "GradualLeft";
    }
    else if(PlatformDirectionInst.currDirection == gradualRight) {
        dispDir = "GradualRight";
    }
    else if(PlatformDirectionInst.currDirection == hardRight) {
        dispDir = "HardRight";
    }
    else if(PlatformDirectionInst.currDirection == none) {
        dispDir = "none";
    }

     //snprintf(str, MAX_STR_LEN, "LFT:%d",dispLeft);
     //GLIB_drawStringOnLine(&glibContext, str, 1, GLIB_ALIGN_LEFT,0,5,true);

     snprintf(str, MAX_STR_LEN, "SH:%d",PlayerStats.shield_remaining);
     GLIB_drawStringOnLine(&glibContext, str, 8, GLIB_ALIGN_LEFT,0,5,true);

     snprintf(str, MAX_STR_LEN, "DIR:%s\n",dispDir);
     GLIB_drawStringOnLine(&glibContext, str, 9, GLIB_ALIGN_LEFT,0,5,true);

     snprintf(str, MAX_STR_LEN, "TIME:%d",dispTime);
     GLIB_drawStringOnLine(&glibContext, str, 4, GLIB_ALIGN_LEFT,0,5,true);


          /* Release resource protected by mutex.       */
     OSMutexPost(&App_PlatformAction_Mutex,         /*   Pointer to user-allocated mutex.         */
     OS_OPT_POST_1,     /*   Only wake up highest-priority task.      */
     &err);

     /* Post updates to display */
     DMD_updateDisplay();
     OSTimeDly(tauDisplay,              /*   Delay the task for 100 OS Ticks.         */
                OS_OPT_TIME_DLY,  /*   Delay is relative to current time.       */
               &err);
     if (err.Code != RTOS_ERR_NONE) {
         printf("Error while Handling App_LCDdisplay_Task task");
     }
   }
 }



static void LCD_init()
{
  uint32_t status;
  /* Enable the memory lcd */
  status = sl_board_enable_display();
  EFM_ASSERT(status == SL_STATUS_OK);

  /* Initialize the DMD support for memory lcd display */
  status = DMD_init(0);
  EFM_ASSERT(status == DMD_OK);

  /* Initialize the glib context */
  status = GLIB_contextInit(&glibContext);
  EFM_ASSERT(status == GLIB_OK);

  glibContext.backgroundColor = White;
  glibContext.foregroundColor = Black;

  /* Fill lcd with background color */
  GLIB_clear(&glibContext);

  /* Use Normal font */
  GLIB_setFont(&glibContext, (GLIB_Font_t *) &GLIB_FontNormal8x8);

  /* Draw text on the memory lcd display*/
  GLIB_drawStringOnLine(&glibContext,
                        "Welcome to...\n**Lab 7**!",
                        0,
                        GLIB_ALIGN_LEFT,
                        5,
                        5,
                        true);

  /* Draw text on the memory lcd display*/
  GLIB_drawStringOnLine(&glibContext,
                        "Review the lab\ninstructions!",
                        2,
                        GLIB_ALIGN_LEFT,
                        5,
                        5,
                        true);
  /* Post updates to display */
  DMD_updateDisplay();
}
/***************************************************************************//**
 * @brief
 *  Idle Task for PART III of lab
 *
 ******************************************************************************/
void  App_IdleTaskCreation (void)
{
    RTOS_ERR     err;

    OSTaskCreate(&App_IdleTaskTCB,                /* Pointer to the task's TCB.  */
                 "Idle Task.",                    /* Name to help debugging.     */
                 &App_IdleTask,                   /* Pointer to the task's code. */
                  DEF_NULL,                          /* Pointer to task's argument. */
                  APP_IDLE_TASK_PRIORITY,             /* Task's priority.            */
                 &App_IdleTaskStk[0],             /* Pointer to base of stack.   */
                 (APP_DEFAULT_TASK_STK_SIZE / 10u),  /* Stack limit, from base.     */
                  APP_DEFAULT_TASK_STK_SIZE,         /* Stack size, in CPU_STK.     */
                  10u,                               /* Messages in task queue.     */
                  0u,                                /* Round-Robin time quanta.    */
                  DEF_NULL,                          /* External TCB data.          */
                  OS_OPT_TASK_STK_CHK,               /* Task options.               */
                 &err);
    if (err.Code != RTOS_ERR_NONE) {
        /* Handle error on task creation. */
        printf("Error while Idle Task Creation");
    }
}
/***************************************************************************//**
*  Idle Low Energy Mode task
*******************************************************************************/
void  App_IdleTask (void  *p_arg)
{
    /* Use argument. */
   (void)&p_arg;
    while (DEF_TRUE) {
        EMU_EnterEM1();
    }
}

/***************************************************************************//**

 * @brief

 *   Setup castle drawing for display

 ******************************************************************************/

void castle_open(void)
{
  //Platform initial position
  Platform.xMin =   45;
  Platform.yMin =   125;
  Platform.xMax = 65;
  Platform.yMax = 130;
  //Right canyon wall - unbreakable
  RightCanyon.xMax = 125;
  RightCanyon.yMax = 150;
  RightCanyon.xMin = 120;
  RightCanyon.yMin = 0;
  //Map Setup
  for(int i = 0; i < 16; i++) {
      //First cliff set
      WallObjects.P_Rectangle_T[0][i].xMin = 5;
      WallObjects.P_Rectangle_T[0][i].xMax = 10;
      WallObjects.P_Rectangle_T[0][i].yMin = 28 + (7*i);
      WallObjects.P_Rectangle_T[0][i].yMax = 33 + (7*i);
      WallObjects.HP_Rectangle_t[0][i] = 200;
  }
  for(int i = 0; i < 12; i++) {
      //Second cliff set
      WallObjects.P_Rectangle_T[1][i].xMin = 12;
      WallObjects.P_Rectangle_T[1][i].xMax = 17;
      WallObjects.P_Rectangle_T[1][i].yMin = 28 + (7*i);
      WallObjects.P_Rectangle_T[1][i].yMax = 33 + (7*i);
      WallObjects.HP_Rectangle_t[1][i] = 200;
  }
  for(int i = 0; i < 8; i++) {
      //Third cliff set
      WallObjects.P_Rectangle_T[2][i].xMin = 19;
      WallObjects.P_Rectangle_T[2][i].xMax = 24;
      WallObjects.P_Rectangle_T[2][i].yMin = 28 + (7*i);
      WallObjects.P_Rectangle_T[2][i].yMax = 33 + (7*i);
      WallObjects.HP_Rectangle_t[0][i] = 200;
  }
  //Inner walls of castle
  for(int i = 0; i < 4; i++) {
      for(int j = 0; j < 7; j++) {
          if((j == 1 && i != 3) || (j == 3 && i != 3) || (j == 5 && i != 3)) {
              continue; //Make windows not rectangles!
          }
          CastleObjects.P_Rectangle_T[i][j].xMin = 5 + (7*j);
          CastleObjects.P_Rectangle_T[i][j].xMax = 10 + (7*j);
          CastleObjects.P_Rectangle_T[i][j].yMin = 0 + (7*i);
          CastleObjects.P_Rectangle_T[i][j].yMax = 5 + (7*i);
          CastleObjects.HP_Rectangle_t[0][i] = 100;
      }
  }
}
/***************************************************************************//**

 * @brief

 *   Setup initial player stat values

 ******************************************************************************/
void player_setup(volatile PlayerStatistics *Stats) {
  Stats->shield_remaining = max_shield_and_start;
}

void app_init(void)
{
  // Initialize GPIO
  gpio_open();

  // Initialize our capactive touch sensor driver!
  CAPSENSE_Init();

  // Initialize our LCD system
  LCD_init();
  player_setup(&PlayerStats);
  castle_open();
  button0_struct_init(&button0);
  button1_struct_init(&button1);
  App_PlayerAction_Creation();
  App_OS_PlayerAction_SemaphoreCreation();
  App_OS_PlatformCtrl_SemaphoreCreation();
  App_VehiclePlayerAction_MutexCreation();
  App_PlatformAction_MutexCreation();
  App_OS_LEDoutput_EventFlagGroupCreation();
  App_OS_Physics_EventFlagGroupCreation();
//  App_OS_currDirection_TimerCreation ();
  App_OS_TimerCreation ();
  App_PlatformCtrl_creation();
  App_Physics_Creation();
  App_LEDoutput_Creation();
  App_LCDdisplay_Creation();
//  App_IdleTaskCreation();
}
