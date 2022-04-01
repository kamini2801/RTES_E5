/* FreeRTOS 10 Tiva Demo
 *
 * main.c
 *
 * Andy Kobyljanec
 *
 * This is a simple demonstration project of FreeRTOS 8.2 on the Tiva Launchpad
 * EK-TM4C1294XL.  TivaWare driverlib sourcecode is included.
 */

#include <stdint.h>
#include <stdbool.h>
#include "main.h"
#include "drivers/pinout.h"
#include "utils/uartstdio.h"


// TivaWare includes
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "inc/tm4c1294ncpdt.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"

// FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"


// Demo Task declarations
void demoLEDTask(void *pvParameters);
void demoSerialTask(void *pvParameters);

// Services
void Task1(void *pvParameters);
void Task2(void *pvParameters);
void Task3(void *pvParameters);
void Task4(void *pvParameters);
void Task5(void *pvParameters);
void Task6(void *pvParameters);
void Task7(void *pvParameters);
void TaskSequencer(void *pvParameters);


uint32_t ui32SysClkFreq;
uint32_t ui32Period;
uint32_t idx = 0, jdx = 1;
uint32_t fib = 0, fib0 = 0, fib1 = 1;

#define FIB_TEST(seqCnt, iterCnt)      \
   for(idx=0; idx < iterCnt; idx++)    \
   {                                   \
      fib = fib0 + fib1;               \
      while(jdx < seqCnt)              \
      {                                \
         fib0 = fib1;                  \
         fib1 = fib;                   \
         fib = fib0 + fib1;            \
         jdx++;                        \
      }                                \
   }                                   \

unsigned int seqIterations = 47;
unsigned int reqIterations = 10000;

#define SEQ_CYCLES 300
#define TRUE 1
#define FALSE 0

int abortTest=FALSE;
int abortS1=FALSE, abortS2=FALSE, abortS3=FALSE, abortS4=FALSE, abortS5=FALSE, abortS6=FALSE, abortS7=FALSE;
SemaphoreHandle_t semS1, semS2, semS3, semS4, semS5, semS6, semS7;
SemaphoreHandle_t mutex;


/*
 *
 *
 */
int max (long int arr[], int len) {
    long int maximum = arr[0];
    int i = 0;
    for (i = 1; i < len; i++) {
        if(maximum < arr[i]) {
            maximum = arr[i];
        }
    }

    return maximum;
}

static volatile int seqCount = 0;

void Timer0IntHandler(void)
{
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    seqCount++;
    int flag = 0;



    if( (seqCount > SEQ_CYCLES) && (flag == 0) ) {

        xSemaphoreGive( semS1 );
        xSemaphoreGive( semS2 );
        xSemaphoreGive( semS3 );
        xSemaphoreGive( semS4 );
        xSemaphoreGive( semS5 );
        xSemaphoreGive( semS6 );
        xSemaphoreGive( semS7 );

        abortS1 = TRUE;
        abortS2 = TRUE;
        abortS3 = TRUE;
        abortS4 = TRUE;
        abortS5 = TRUE;
        abortS6 = TRUE;
        abortS7 = TRUE;
        flag = 1;

    }

    else {

        if( seqCount%10 == 0 )
             {
                 if(xSemaphoreGive( semS1 ) != pdTRUE) UARTprintf("Failed to release sem1\r\n");
              }
         if( seqCount%30 == 0 )
             {
                 if(xSemaphoreGive( semS2 ) != pdTRUE) UARTprintf("Failed to release sem2\r\n");
             }
         if( seqCount%60 == 0 )
             {
                 if(xSemaphoreGive( semS3 ) != pdTRUE) UARTprintf("Failed to release sem3\r\n");
             }
         if( seqCount%30 == 0 )
             {
                 if(xSemaphoreGive( semS4 ) != pdTRUE) UARTprintf("Failed to release sem4\r\n");
             }
         if( seqCount%60 == 0 )
             {
                 if(xSemaphoreGive( semS5 ) != pdTRUE) UARTprintf("Failed to release sem5\r\n");
             }
         if( seqCount%30 == 0 )
             {
                 if(xSemaphoreGive( semS6 ) != pdTRUE) UARTprintf("Failed to release sem6\r\n");
             }
         if( seqCount%300 == 0 )
             {
                 if(xSemaphoreGive( semS7 ) != pdTRUE) UARTprintf("Failed to release sem7\r\n");

             }
    }

}

// Main function
int main(void)
{
    // Initialize system clock to 120 MHz
    uint32_t output_clock_rate_hz;
    output_clock_rate_hz = ROM_SysCtlClockFreqSet(
                               (SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN |
                                SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480),
                               SYSTEM_CLOCK);
    ASSERT(output_clock_rate_hz == SYSTEM_CLOCK);

    // Initialize the GPIO pins for the Launchpad
    PinoutSet(false, false);

    UARTStdioConfig(0, 57600, SYSTEM_CLOCK);

    // Create demo tasks
//    xTaskCreate(demoLEDTask, (const portCHAR *)"LEDs",
//                configMINIMAL_STACK_SIZE, NULL, 1, NULL);
//
//    xTaskCreate(demoSerialTask, (const portCHAR *)"Serial",
//                configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    // init sem

    semS1 = xSemaphoreCreateBinary();
    if( semS1 == NULL ) UARTprintf("Failed to create semaphore\n");
    semS2 = xSemaphoreCreateBinary();
    if( semS2 == NULL ) UARTprintf("Failed to create semaphore\n");
    semS3= xSemaphoreCreateBinary();
    if( semS3 == NULL ) UARTprintf("Failed to create semaphore\n");
    semS4 = xSemaphoreCreateBinary();
    if( semS4 == NULL ) UARTprintf("Failed to create semaphore\n");
    semS5 = xSemaphoreCreateBinary();
    if( semS5 == NULL ) UARTprintf("Failed to create semaphore\n");
    semS6 = xSemaphoreCreateBinary();
    if( semS6 == NULL ) UARTprintf("Failed to create semaphore\n");
    semS7 = xSemaphoreCreateBinary();
    if( semS7 == NULL ) UARTprintf("Failed to create semaphore\n");

    mutex = xSemaphoreCreateMutex();

    // Create tasks

    xTaskCreate(Task1, (const portCHAR *)"Task1",
                    configMINIMAL_STACK_SIZE, NULL, 7, NULL);

    xTaskCreate(Task2, (const portCHAR *)"Task2",
                        configMINIMAL_STACK_SIZE, NULL, 6, NULL);

    xTaskCreate(Task3, (const portCHAR *)"Task3",
                        configMINIMAL_STACK_SIZE, NULL, 5, NULL);

    xTaskCreate(Task4, (const portCHAR *)"Task4",
                        configMINIMAL_STACK_SIZE, NULL, 6, NULL);

    xTaskCreate(Task5, (const portCHAR *)"Task5",
                        configMINIMAL_STACK_SIZE, NULL, 5, NULL);

    xTaskCreate(Task6, (const portCHAR *)"Task6",
                        configMINIMAL_STACK_SIZE, NULL, 6, NULL);

    xTaskCreate(Task7, (const portCHAR *)"Task7",
                        configMINIMAL_STACK_SIZE, NULL, 1, NULL);



    ui32SysClkFreq = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

     SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
     SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

     GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);
     TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

     ui32Period = (ui32SysClkFreq / 30);
     TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period);
     IntEnable(INT_TIMER0A);
     TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
     IntMasterEnable();
     TimerEnable(TIMER0_BASE, TIMER_A);

    UARTprintf("starting Scheduler\n");
    vTaskStartScheduler();

    // Code should never reach this point
    return 0;
}



/*
*
*   Services
*
*/
void Task1(void *pvParameters)
{

    unsigned long long task_count = 0;  // iterator

    long int arr[90] = {0};             //timestamp array

    TickType_t Task_start = 0;

    UARTprintf("Task 1 start\n");

    while(!abortS1)
    {
        if( xSemaphoreTake( semS1, portMAX_DELAY ) != pdTRUE )
            UARTprintf("Task 1 failed to acquire semaphore");

        Task_start = xTaskGetTickCount();                           //start time

        task_count++;

        FIB_TEST(seqIterations, reqIterations);

        arr[task_count - 1] = xTaskGetTickCount() - Task_start;     //end time
    }

    int wcet = 0;

    wcet = max(arr, task_count);

    if( xSemaphoreTake( mutex, portMAX_DELAY ) != pdTRUE )
                UARTprintf("Task 1 failed to acquire mutex");

    UARTprintf("Task 1 WC execution time: %d\n", wcet);

    if( xSemaphoreGive( mutex ) != pdTRUE )
                                    UARTprintf("Task 1 failed to release mutex");


    vTaskDelete(NULL);

}


void Task2(void *pvParameters)
{
    unsigned long long task_count = 0;  // iterator

        long int arr[30] = {0};             //timestamp array

        TickType_t Task_start = 0;

        UARTprintf("Task 2 start\n");

        while(!abortS2)
        {
            if( xSemaphoreTake( semS2, portMAX_DELAY ) != pdTRUE )
                UARTprintf("Task 2 failed to acquire semaphore");

            Task_start = xTaskGetTickCount();                           //start time

            task_count++;

            FIB_TEST(seqIterations, reqIterations);

            arr[task_count - 1] = xTaskGetTickCount() - Task_start;     //end time
        }

        int wcet = 0;

        wcet = max(arr, task_count);

        if( xSemaphoreTake( mutex, portMAX_DELAY ) != pdTRUE )
                        UARTprintf("Task 1 failed to acquire mutex");

        UARTprintf("Task 2 WC execution time: %d\n", wcet);

        if( xSemaphoreGive( mutex ) != pdTRUE )
                                        UARTprintf("Task 1 failed to release mutex");

        vTaskDelete(NULL);
}

void Task3(void *pvParameters)
{
    unsigned long long task_count = 0;  // iterator

        long int arr[15] = {0};             //timestamp array

        TickType_t Task_start = 0;

        UARTprintf("Task 3 start\n");

        while(!abortS3)
        {
            if( xSemaphoreTake( semS3, portMAX_DELAY ) != pdTRUE )
                UARTprintf("Task 3  failed to acquire semaphore");

            Task_start = xTaskGetTickCount();                           //start time

            task_count++;

            FIB_TEST(seqIterations, reqIterations);

            arr[task_count - 1] = xTaskGetTickCount() - Task_start;     //end time
        }

        int wcet = 0;

        wcet = max(arr, task_count);

        if( xSemaphoreTake( mutex, portMAX_DELAY ) != pdTRUE )
                        UARTprintf("Task 1 failed to acquire mutex");

        UARTprintf("Task 3 WC execution time: %d\n", wcet);

        if( xSemaphoreGive( mutex ) != pdTRUE )
                                        UARTprintf("Task 1 failed to release mutex");


        vTaskDelete(NULL);
}


void Task4(void *pvParameters)
{
    unsigned long long task_count = 0;  // iterator

        long int arr[30] = {0};             //timestamp array

        TickType_t Task_start = 0;

        UARTprintf("Task 4 start\n");

        while(!abortS4)
        {
            if( xSemaphoreTake( semS4, portMAX_DELAY ) != pdTRUE )
                UARTprintf("Task 4 failed to acquire semaphore");

            Task_start = xTaskGetTickCount();                           //start time

            task_count++;

            FIB_TEST(seqIterations, reqIterations);

            arr[task_count - 1] = xTaskGetTickCount() - Task_start;     //end time
        }

        int wcet = 0;

        wcet = max(arr, task_count);

        if( xSemaphoreTake( mutex, portMAX_DELAY ) != pdTRUE )
                        UARTprintf("Task 1 failed to acquire mutex");


        UARTprintf("Task 4 WC execution time: %d\n", wcet);

        if( xSemaphoreGive( mutex ) != pdTRUE )
                            UARTprintf("Task 1 failed to release mutex");


        vTaskDelete(NULL);
}

void Task5(void *pvParameters)
{
    unsigned long long task_count = 0;  // iterator

        long int arr[15] = {0};             //timestamp array

        TickType_t Task_start = 0;

        UARTprintf("Task 5 start\n");

        while(!abortS5)
        {
            if( xSemaphoreTake( semS5, portMAX_DELAY ) != pdTRUE )
                UARTprintf("Task 5 failed to acquire semaphore");

            Task_start = xTaskGetTickCount();                           //start time

            task_count++;

            FIB_TEST(seqIterations, reqIterations);

            arr[task_count - 1] = xTaskGetTickCount() - Task_start;     //end time
        }

        int wcet = 0;

        wcet = max(arr, task_count);

        if( xSemaphoreTake( mutex, portMAX_DELAY ) != pdTRUE )
                        UARTprintf("Task 1 failed to acquire mutex");

        UARTprintf("Task 5 WC execution time: %d\n", wcet);

        if( xSemaphoreGive( mutex ) != pdTRUE )
                         UARTprintf("Task 1 failed to release mutex");


        vTaskDelete(NULL);
}


void Task6(void *pvParameters)
{
    unsigned long long task_count = 0;  // iterator

        long int arr[30] = {0};             //timestamp array

        TickType_t Task_start = 0;

        UARTprintf("Task 6 start\n");

        while(!abortS6)
        {
            if( xSemaphoreTake( semS6, portMAX_DELAY ) != pdTRUE )
                UARTprintf("Task 6 failed to acquire semaphore");

            Task_start = xTaskGetTickCount();                           //start time

            task_count++;

            FIB_TEST(seqIterations, reqIterations);

            arr[task_count - 1] = xTaskGetTickCount() - Task_start;     //end time
        }

        int wcet = 0;

        wcet = max(arr, task_count);

        if( xSemaphoreTake( mutex, portMAX_DELAY ) != pdTRUE )
                        UARTprintf("Task 1 failed to acquire mutex");


        UARTprintf("Task 6 WC execution time: %d\n", wcet);

        if( xSemaphoreGive( mutex ) != pdTRUE )
                                        UARTprintf("Task 1 failed to release mutex");

        vTaskDelete(NULL);
}


void Task7(void *pvParameters)
{
    unsigned long long task_count = 0;  // iterator

        long int arr[3] = {0};             //timestamp array

        TickType_t Task_start = 0;



        UARTprintf("Task 7 start\n");

        while(!abortS7)
        {
            if( xSemaphoreTake( semS7, portMAX_DELAY ) != pdTRUE )
                UARTprintf("Task 7 failed to acquire semaphore");

            Task_start = xTaskGetTickCount();                           //start time

            task_count++;

            FIB_TEST(seqIterations, reqIterations);

            arr[task_count - 1] = xTaskGetTickCount() - Task_start;     //end time
        }

        int wcet = 0;

        wcet = max(arr, task_count);

        if( xSemaphoreTake( mutex, portMAX_DELAY ) != pdTRUE )
                        UARTprintf("Task 1 failed to acquire mutex");

        UARTprintf("Task 7 WC execution time: %d\n", wcet);

        if( xSemaphoreGive( mutex ) != pdTRUE )
                                UARTprintf("Task 1 failed to release mutex");


        vTaskDelete(NULL);
}


/*  ASSERT() Error function
 *
 *  failed ASSERTS() from driverlib/debug.h are executed in this function
 */
void __error__(char *pcFilename, uint32_t ui32Line)
{
    // Place a breakpoint here to capture errors until logging routine is finished
    while (1)
    {
    }
}
