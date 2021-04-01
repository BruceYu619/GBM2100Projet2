/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

/*
#include "touch_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


// 100 scans par seconde
#define TOUCH_SCAN_INTERVAL    (psMS_TO_TICKS(10u))
#define GESTURE_TIME_INCREMENT (5u)

QueueHandle_t touchQueue;


void Task_Touch (void *pvParameters)
{
   touch_data_t previousTouchData = NO_TOUCH;
   touch_data_t currentTouchData;
   uint32_t sliderGesture;
   uint32_t gestureTimestamp = 0;
    
   (void)pvParameters; //enlever l'avertissement de parametre non utilisé
    
   CapSense_Start();

   if (CapSense_IsBusy() == CapSense_NOT_BUSY)
   {
        currentTouchData= NO_TOUCH;
        CapSense_ProcessAllWidgets();
        
        if (CapSense_IsWidgetActive(CapSense_BUTTON0_WDGT_ID))
        {
            if(previousTouchData != BUTTON0_TOUCHED)
                {
                    currentTouchData = BUTTON0_TOUCHED;
                }
            }
            
            else if (CapSense_IsWidgetActive (CapSense_BUTTON1_WDGT_ID))
            {
                if(previousTouchData != BUTTON1_TOUCHED)
                {
                    currentTouchData = BUTTON1_TOUCHED;
                }
            }
            else
            {
                
                sliderGesture = CapSense_DecodeWidgetGestures
                                (CapSense_LINEARSLIDER0_WDGT_ID);
                                
                    
                switch(sliderGesture)
                {    
                    // à gauche
                    case CapSense_ONE_FINGER_FLICK_LEFT:
                        currentTouchData = SLIDER_FLICK_LEFT;
                        break;
                    // à droite
                    case CapSense_ONE_FINGER_FLICK_RIGHT:
                        currentTouchData = SLIDER_FLICK_RIGHT;
                        break;                        
                    // aucun mouvement
                    default:
                    break;   
                }
            }
            
            // Prochain scan
            CapSense_ScanAllWidgets();
            
            
            if(currentTouchData != NO_TOUCH)
            {
                previousTouchData = currentTouchData;
                // envoyer les données
                //xQueueOverwrite(touchQueue, &currentTouchData);
            }
        }
        
        
        gestureTimestamp += GESTURE_TIME_INCREMENT;
        CapSense_SetGestureTimestamp(gestureTimestamp);

        
        //vTaskDelay(TOUCH_SCAN_INTERVAL);
    
        
}

*/
/* [] END OF FILE */
