#ifndef TASK_H_
#define TASK_H_

#include "stdint.h"

#define taskMAX_TASK_NUM (32U)

#ifndef NULL
    #define NULL ((void *)0)
#endif

#ifndef TRUE
    #define TRUE (1U)
#endif
#ifndef true
    #define true (1U)
#endif
#ifndef FALSE
    #define FALSE (0U)
#endif
#ifndef false
    #define false (0U)
#endif

#if (0)
    extern void taskGLOBAL_LOCK_FUN(void);
    extern void taskGLOBAL_UNLOCK_FUN(void);
#else 
    #define taskGLOBAL_LOCK_FUN()
    #define taskGLOBAL_UNLOCK_FUN()
#endif

#if (0)
    extern uint32_t taskGET_SYSTEM_BEAT(void);
#else
    #define taskGET_SYSTEM_BEAT() (0U)
#endif


typedef  void(*TASK) (void *p_arg);
typedef  void* _pVOID;

typedef enum {
    eRunning = 0,
    eSuspended,
}eTaskRunStatus_t;

typedef struct
{
    uint32_t ulId;                 /* Task ID */
    uint32_t ulRunTime;           
    TASK pTaskFun;         
    _pVOID pUserParam;           
    uint32_t ulTaskBeat;         
    uint32_t ulTaskLastBeat;   
    uint8_t ucTaskPauseFlag;      
}TaskStruct_t;

typedef struct
{
    TaskStruct_t xTask[taskMAX_TASK_NUM];
    uint32_t ulTaskNum;
    uint32_t ulCurrentRunTaskIndex;
    uint8_t  ucCurrentTaskDeletedFlag;
    uint8_t  ucStartSchedulerFlag;
}TaskList_t;


void vTaskInit(void);
int32_t slTaskCreate(TASK pTask, _pVOID pUserdata, uint32_t ulRunBeat);
int32_t slTaskDelete(uint32_t ulTaskID);
void vTaskSchedulerStart(void);
int32_t slTaskSuspend(uint32_t ulTaskID);
int32_t slTaskWakeUp(uint32_t ulTaskID);


#endif