#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_

#include "main.h"
#include <stdint.h>

#define SCH_MAX_TASKS   40
#define NO_TASK_ID      0

#define RETURN_NORMAL   0
#define RETURN_ERROR    1

#define ERROR_SCH_TOO_MANY_TASKS        1
#define ERROR_SCH_CANNOT_DELETE_TASK    2

extern uint8_t Error_code_G;

typedef struct {
    void (*pTask)(void);
    uint32_t Delay;     // Remaining ticks until execution
    uint32_t Period;    // Period in ticks
    uint8_t  RunMe;     // Ready flag
    uint32_t TaskID;    // Unique Task Identifier
} sTask;

/* Driver Interface */
void     SCH_Init(void);
void     SCH_Update(void);
void     SCH_Dispatch_Tasks(void);
uint32_t SCH_Add_Task(void (*pFunction)(void), uint32_t DELAY, uint32_t PERIOD);
uint8_t  SCH_Delete_Task(uint32_t taskID);
void     SCH_Go_To_Sleep(void);

#endif /* INC_SCHEDULER_H_ */
