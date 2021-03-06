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
#include "project.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "bmi160.h"

void LED_TASK(void)
{
    struct bmi160_sensor_data acc;
        for(;;)
        {
            Cy_GPIO_Write(RED_PORT,RED_NUM,0);
            vTaskDelay(500);
            Cy_GPIO_Write(RED_PORT,RED_NUM,1);
            vTaskDelay(500);
            if(acc.x<3000)
            {
                motionTask();
            }
        }

}

static struct bmi160_dev bmi160Dev;
static int8_t BMI160BurstWrite(uint8_t dev_addr, uint8_t reg_addr,uint8_t *data, uint16_t len)
{
    
    Cy_SCB_I2C_MasterSendStart(I2C_1_HW,dev_addr,CY_SCB_I2C_WRITE_XFER,0,&I2C_1_context);
    Cy_SCB_I2C_MasterWriteByte(I2C_1_HW,reg_addr,0,&I2C_1_context);
    for(int i = 0;i<len; i++)
    { 
        Cy_SCB_I2C_MasterWriteByte(I2C_1_HW,data[i],0,&I2C_1_context);
    }
    
    Cy_SCB_I2C_MasterSendStop(I2C_1_HW,0,&I2C_1_context);
    
    return 0;
}

static int8_t BMI160BurstRead(uint8_t dev_addr, uint8_t reg_addr,uint8_t *data, uint16_t len)
{
    
    Cy_SCB_I2C_MasterSendStart(I2C_1_HW,dev_addr,CY_SCB_I2C_WRITE_XFER,0,&I2C_1_context);
    Cy_SCB_I2C_MasterWriteByte(I2C_1_HW,reg_addr,0,&I2C_1_context);
    Cy_SCB_I2C_MasterSendReStart(I2C_1_HW,dev_addr,CY_SCB_I2C_READ_XFER,0,&I2C_1_context);
    for(int i = 0;i<len-1; i++)
    {
        Cy_SCB_I2C_MasterReadByte(I2C_1_HW,CY_SCB_I2C_ACK,&data[i],0,&I2C_1_context);
    }
    Cy_SCB_I2C_MasterReadByte(I2C_1_HW,CY_SCB_I2C_NAK,&data[len-1],0,&I2C_1_context);
    
    Cy_SCB_I2C_MasterSendStop(I2C_1_HW,0,&I2C_1_context);
    
    
    return 0;
}


static void sensorsDeviceInit(void)
{

  int8_t rslt;
  vTaskDelay(500); // guess

  /* BMI160 */
  bmi160Dev.read = (bmi160_read_fptr_t)BMI160BurstRead;
  bmi160Dev.write = (bmi160_write_fptr_t)BMI160BurstWrite;
  bmi160Dev.delay_ms = (bmi160_delay_fptr_t)vTaskDelay;
  
  bmi160Dev.id = BMI160_I2C_ADDR;  // I2C device address

  rslt = bmi160_init(&bmi160Dev); // initialize the device
  if (rslt == 0)
    {
      printf("BMI160 I2C connection [OK].\n");
      bmi160Dev.gyro_cfg.odr = BMI160_GYRO_ODR_800HZ;
      bmi160Dev.gyro_cfg.range = BMI160_GYRO_RANGE_125_DPS;
      bmi160Dev.gyro_cfg.bw = BMI160_GYRO_BW_OSR4_MODE;

      /* Select the power mode of Gyroscope sensor */
      bmi160Dev.gyro_cfg.power = BMI160_GYRO_NORMAL_MODE;

      bmi160Dev.accel_cfg.odr = BMI160_ACCEL_ODR_1600HZ;
      bmi160Dev.accel_cfg.range = BMI160_ACCEL_RANGE_4G;
      bmi160Dev.accel_cfg.bw = BMI160_ACCEL_BW_OSR4_AVG1;
      bmi160Dev.accel_cfg.power = BMI160_ACCEL_NORMAL_MODE;

      /* Set the sensor configuration */
      bmi160_set_sens_conf(&bmi160Dev);
      bmi160Dev.delay_ms(50);
    }
  else
    {
      printf("BMI160 I2C connection [FAIL].\n");
    }
}

#define MAXACCEL 8000
void motionTask(void *arg)
{
    (void)arg;
    I2C_1_Start();
    sensorsDeviceInit();
    struct bmi160_sensor_data acc;

    while(1)
    {
        bmi160_get_sensor_data(BMI160_ACCEL_ONLY, &acc, NULL, &bmi160Dev);
        printf("x=%4d y=%4d z=%4d\r\n",acc.x,acc.y,acc.z);
        vTaskDelay(200);
        if(abs(acc.x)>3000)
        {
            LED_TASK();
        }
        
    }
}

int main(void)
{
    __enable_irq(); /* Enable global interrupts. */
    
    Cy_GPIO_Write(RED_PORT,RED_NUM,1);

    UART_1_Start();
    xTaskCreate( motionTask, "Motion Task",400,0,1,0);
    vTaskStartScheduler();
    while(1);
}
/* [] END OF FILE */