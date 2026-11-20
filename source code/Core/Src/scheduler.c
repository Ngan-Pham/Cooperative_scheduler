/*
 * scheduler.c
 *
 *  Created on: Nov 9, 2026
 *      Author: Admin
 *  Updated: Optimized & Verified Cooperative Scheduler (O(1) Delta-Queue)
 */
#include "scheduler.h"
#include "main.h"

static sTask SCH_tasks_G[SCH_MAX_TASKS];
static uint32_t current_task_id = 1;
static uint32_t current_running_task_id = NO_TASK_ID;
static uint8_t task_cancelled = 0;
uint8_t Error_code_G = 0;

void SCH_Init(void) {
    __disable_irq();
    for (uint8_t i = 0; i < SCH_MAX_TASKS; i++) {
        SCH_tasks_G[i].pTask = 0x0000;
        SCH_tasks_G[i].Delay = 0;
        SCH_tasks_G[i].Period = 0;
        SCH_tasks_G[i].RunMe = 0;
        SCH_tasks_G[i].TaskID = NO_TASK_ID;
    }
    current_task_id = 1;
    current_running_task_id = NO_TASK_ID;
    task_cancelled = 0;
    Error_code_G = 0;
    __enable_irq();
}

static uint32_t SCH_Add_Task_Internal(void (*pFunction)(void), uint32_t DELAY, uint32_t PERIOD, uint32_t taskID) {
    if (pFunction == 0x0000) {
        return NO_TASK_ID;
    }

    __disable_irq();

    if (SCH_tasks_G[SCH_MAX_TASKS - 1].pTask != 0x0000) {
        Error_code_G = ERROR_SCH_TOO_MANY_TASKS;
        __enable_irq();
        return NO_TASK_ID;
    }

    uint8_t new_index = 0;
    uint32_t sum_delay = 0;

    while (new_index < SCH_MAX_TASKS && SCH_tasks_G[new_index].pTask != 0x0000) {
        if (sum_delay + SCH_tasks_G[new_index].Delay > DELAY) {
            break;
        }
        sum_delay += SCH_tasks_G[new_index].Delay;
        new_index++;
    }

    if (new_index == SCH_MAX_TASKS) {
        Error_code_G = ERROR_SCH_TOO_MANY_TASKS;
        __enable_irq();
        return NO_TASK_ID;
    }

    for (int8_t i = SCH_MAX_TASKS - 1; i > new_index; i--) {
        SCH_tasks_G[i] = SCH_tasks_G[i - 1];
    }

    uint32_t new_delay = DELAY - sum_delay;
    SCH_tasks_G[new_index].pTask = pFunction;
    SCH_tasks_G[new_index].Delay = new_delay;
    SCH_tasks_G[new_index].Period = PERIOD;
    SCH_tasks_G[new_index].RunMe = (sum_delay + new_delay == 0) ? 1 : 0;

    if (taskID == NO_TASK_ID) {
        SCH_tasks_G[new_index].TaskID = current_task_id++;
        if (current_task_id == 0) current_task_id = 1;
    } else {
        SCH_tasks_G[new_index].TaskID = taskID;
    }

    if (new_index + 1 < SCH_MAX_TASKS && SCH_tasks_G[new_index + 1].pTask != 0x0000) {
        SCH_tasks_G[new_index + 1].Delay -= new_delay;
    }

    uint32_t registered_id = SCH_tasks_G[new_index].TaskID;
    __enable_irq();

    return registered_id;
}

uint32_t SCH_Add_Task(void (*pFunction)(void), uint32_t DELAY, uint32_t PERIOD) {
    return SCH_Add_Task_Internal(pFunction, DELAY, PERIOD, NO_TASK_ID);
}

void SCH_Update(void) {
    if (SCH_tasks_G[0].pTask != 0x0000) {
        if (SCH_tasks_G[0].Delay > 0) {
            SCH_tasks_G[0].Delay--;
        }
        if (SCH_tasks_G[0].Delay == 0) {
            SCH_tasks_G[0].RunMe = 1;
        }
    }
}

void SCH_Dispatch_Tasks(void) {
    while (SCH_tasks_G[0].pTask != 0x0000 && (SCH_tasks_G[0].RunMe > 0 || SCH_tasks_G[0].Delay == 0)) {
        void (*pTempFunc)(void) = SCH_tasks_G[0].pTask;
        uint32_t temp_period = SCH_tasks_G[0].Period;
        uint32_t temp_id = SCH_tasks_G[0].TaskID;

        SCH_Delete_Task(temp_id);

        current_running_task_id = temp_id;
        task_cancelled = 0;

        (*pTempFunc)();

        current_running_task_id = NO_TASK_ID;

        if (temp_period > 0 && !task_cancelled) {
            SCH_Add_Task_Internal(pTempFunc, temp_period, temp_period, temp_id);
        }
    }

    SCH_Go_To_Sleep();
}

uint8_t SCH_Delete_Task(uint32_t taskID) {
    if (taskID == NO_TASK_ID) return RETURN_ERROR;

    __disable_irq();

    if (taskID == current_running_task_id) {
        task_cancelled = 1;
        __enable_irq();
        return RETURN_NORMAL;
    }

    uint8_t index = 0;
    while (index < SCH_MAX_TASKS && SCH_tasks_G[index].TaskID != taskID) {
        index++;
    }

    if (index == SCH_MAX_TASKS || SCH_tasks_G[index].pTask == 0x0000) {
        Error_code_G = ERROR_SCH_CANNOT_DELETE_TASK;
        __enable_irq();
        return RETURN_ERROR;
    }

    if (index + 1 < SCH_MAX_TASKS && SCH_tasks_G[index + 1].pTask != 0x0000) {
        SCH_tasks_G[index + 1].Delay += SCH_tasks_G[index].Delay;
    }

    for (uint8_t i = index; i < SCH_MAX_TASKS - 1; i++) {
        SCH_tasks_G[i] = SCH_tasks_G[i + 1];
    }

    SCH_tasks_G[SCH_MAX_TASKS - 1].pTask = 0x0000;
    SCH_tasks_G[SCH_MAX_TASKS - 1].Delay = 0;
    SCH_tasks_G[SCH_MAX_TASKS - 1].Period = 0;
    SCH_tasks_G[SCH_MAX_TASKS - 1].RunMe = 0;
    SCH_tasks_G[SCH_MAX_TASKS - 1].TaskID = NO_TASK_ID;

    __enable_irq();
    return RETURN_NORMAL;
}

void SCH_Go_To_Sleep(void) {
}
