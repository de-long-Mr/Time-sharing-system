#include "task.h"


static TaskList_t xTaskList;

static int32_t prvTaskFind(uint32_t ulTaskID)
{
    uint32_t ulLoopTimes;
    int32_t slRet = -1;

    for ( ulLoopTimes = 0U; ulLoopTimes < xTaskList.ulTaskNum; ++ulLoopTimes ) {
        if ( xTaskList.xTask[ulLoopTimes].ulId == ulTaskID ) {
            slRet = (int32_t) ulLoopTimes;
            break;
        }
    }
    return slRet;
}

void vTaskInit(void)
{
    uint32_t ulLoopTimes;
    uint8_t * pucPointer = (uint8_t *)&xTaskList;

    for ( ulLoopTimes = 0U; ulLoopTimes < sizeof(TaskList_t); ++ulLoopTimes ) {
        pucPointer[ulLoopTimes] = 0U;
    }
}

int32_t slTaskCreate(TASK pTask, _pVOID pUserdata, uint32_t ulRunBeat)
{
    int32_t slRet = -1;

    if ( pTask != NULL ) {
        taskGLOBAL_LOCK_FUN();

        if ( xTaskList.ulTaskNum < taskMAX_TASK_NUM ) {
            xTaskList.xTask[xTaskList.ulTaskNum].ulId = xTaskList.ulTaskNum;
            xTaskList.xTask[xTaskList.ulTaskNum].ulRunTime = 0U;
            xTaskList.xTask[xTaskList.ulTaskNum].pTaskFun = pTask;
            xTaskList.xTask[xTaskList.ulTaskNum].pUserParam = pUserdata;
            xTaskList.xTask[xTaskList.ulTaskNum].ulTaskBeat = ulRunBeat;
            xTaskList.xTask[xTaskList.ulTaskNum].ulTaskLastBeat = 0U;
            xTaskList.xTask[xTaskList.ulTaskNum].ucTaskPauseFlag = eRunning;
            slRet = (int32_t)xTaskList.ulTaskNum;
            ++xTaskList.ulTaskNum;
        }

        taskGLOBAL_UNLOCK_FUN();
    }
    return slRet;
}

int32_t slTaskDelete(uint32_t ulTaskID)
{
    int32_t slRet = -1;
    int32_t slBeenDeletedIndex;

    taskGLOBAL_LOCK_FUN();

    if ( xTaskList.ulTaskNum > 0U ) {
        slBeenDeletedIndex = prvTaskFind(ulTaskID);
        if ( slBeenDeletedIndex >= 0 ) {
            --xTaskList.ulTaskNum;
            if ( slBeenDeletedIndex == (int32_t)xTaskList.ulCurrentRunTaskIndex ) {
                if ( xTaskList.ucStartSchedulerFlag == 1U ) {
                    xTaskList.ucCurrentTaskDeletedFlag = 1U;
                }
            }

            if ( slBeenDeletedIndex != (int32_t)xTaskList.ulTaskNum ) {
                xTaskList.xTask[slBeenDeletedIndex].ulId = xTaskList.xTask[xTaskList.ulTaskNum].ulId;
                xTaskList.xTask[slBeenDeletedIndex].ulRunTime = xTaskList.xTask[xTaskList.ulTaskNum].ulRunTime;
                xTaskList.xTask[slBeenDeletedIndex].pTaskFun = xTaskList.xTask[xTaskList.ulTaskNum].pTaskFun;
                xTaskList.xTask[slBeenDeletedIndex].pUserParam = xTaskList.xTask[xTaskList.ulTaskNum].pUserParam;
                xTaskList.xTask[slBeenDeletedIndex].ulTaskBeat = xTaskList.xTask[xTaskList.ulTaskNum].ulTaskBeat;
                xTaskList.xTask[slBeenDeletedIndex].ulTaskLastBeat = xTaskList.xTask[xTaskList.ulTaskNum].ulTaskLastBeat;
                xTaskList.xTask[slBeenDeletedIndex].ucTaskPauseFlag = xTaskList.xTask[xTaskList.ulTaskNum].ucTaskPauseFlag;
                slBeenDeletedIndex = (int32_t)xTaskList.ulTaskNum;
            }

            xTaskList.xTask[slBeenDeletedIndex].ulId = 0U;
            xTaskList.xTask[slBeenDeletedIndex].ulRunTime = 0U;
            xTaskList.xTask[slBeenDeletedIndex].pTaskFun = 0U;
            xTaskList.xTask[slBeenDeletedIndex].pUserParam = 0U;
            xTaskList.xTask[slBeenDeletedIndex].ulTaskBeat = 0U;
            xTaskList.xTask[slBeenDeletedIndex].ulTaskLastBeat = 0U;
            xTaskList.xTask[slBeenDeletedIndex].ucTaskPauseFlag = 0U;

            slRet = (int32_t)ulTaskID;
        }
    }

    taskGLOBAL_UNLOCK_FUN();
    return slRet;
}

void vTaskSchedulerStart(void)
{
    uint32_t ulLoopTimes;

    xTaskList.ucStartSchedulerFlag = true;
    while ( true ) {
        for ( ulLoopTimes = 0; ulLoopTimes < xTaskList.ulTaskNum; ++ulLoopTimes ) {

            if ( xTaskList.xTask[ulLoopTimes].ucTaskPauseFlag == 0U ) {

                if ( ( taskGET_SYSTEM_BEAT() - xTaskList.xTask[ulLoopTimes].ulTaskLastBeat ) >= xTaskList.xTask[ulLoopTimes].ulTaskBeat ) {

                    if ( xTaskList.xTask[ulLoopTimes].pTaskFun != NULL ) {

                        xTaskList.ulCurrentRunTaskIndex = ulLoopTimes;
                        xTaskList.xTask[ulLoopTimes].ulTaskLastBeat = taskGET_SYSTEM_BEAT();
                        xTaskList.xTask[ulLoopTimes].pTaskFun( xTaskList.xTask[ulLoopTimes].pUserParam );

                        if ( xTaskList.ucCurrentTaskDeletedFlag == 0U ) {
                            xTaskList.xTask[ulLoopTimes].ulRunTime = taskGET_SYSTEM_BEAT() - xTaskList.xTask[ulLoopTimes].ulTaskLastBeat;
                        }
                        else {
                            xTaskList.ucCurrentTaskDeletedFlag = 0U;
                        }
                    }
                }
            }
        }
    }
}

int32_t slTaskSuspend(uint32_t ulTaskID)
{
    int32_t slRet = -1;
    int32_t slTaskIndex;

    taskGLOBAL_LOCK_FUN();

    slTaskIndex = prvTaskFind( ulTaskID );
    if ( slTaskIndex >= 0 ) {
        xTaskList.xTask[slTaskIndex].ucTaskPauseFlag = true;
        slRet = (int32_t)ulTaskID;
    }

    taskGLOBAL_UNLOCK_FUN();
    return slRet;
}

int32_t slTaskWakeUp(uint32_t ulTaskID)
{
    int32_t slRet = -1;
    int32_t slTaskIndex;

    taskGLOBAL_LOCK_FUN();

    slTaskIndex = prvTaskFind( ulTaskID );
    if ( slTaskIndex >= 0 ) {
        xTaskList.xTask[slTaskIndex].ucTaskPauseFlag = false;
        slRet = (int32_t)ulTaskID;
    }

    taskGLOBAL_UNLOCK_FUN();
    return slRet;
}

