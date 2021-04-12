/******************************************************************************
* File Name: main_cm4.c
*
********************************************************************************/

#include "project.h"
#include "GUI.h"
#include "pervasive_eink_hardware_driver.h"
#include "cy_eink_library.h"
#include "LCDConf.h"
#include <stdlib.h>
#include "LCDConf.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "display_task.h"
#include "task.h"
//#include "touch_task.h"
#include "queue.h"

#define TASK_TOUCH_PRIORITY      (10u)
#define TASK_DISPLAY_PRIORITY    (1u)
#define TASK_DISPLAY_STACK_SIZE  (2048u)
#define TASK_TOUCH_STACK_SIZE    (configMINIMAL_STACK_SIZE)
#define TOUCH_QUEUE_LENGHT       (1u)

//void isr_bouton(void);




int main(void)
{
    
    __enable_irq(); /* Enable global interrupts. */
    
    CapSense_Start();
    CapSense_ScanAllWidgets();
    
    xTaskCreate(capSense_Task,"capSense_task",400,NULL,1,0);
    xTaskCreate(Task_Display, "Display task", 400,NULL, 1, NULL);
  
    
    vTaskStartScheduler();
    while(CapSense_IsBusy());
    
    for(;;)
    {
        
    }
}

/* [] END OF FILE */
