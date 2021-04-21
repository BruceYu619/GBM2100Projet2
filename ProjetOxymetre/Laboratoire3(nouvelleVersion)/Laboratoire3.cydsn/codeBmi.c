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

static struct bmi160_dev bmi160Dev; //structure du capteur

static int8_t BMI160BurstWrite(uint8_t dev_addr, uint8_t reg_addr,uint8_t *data, uint16_t len) // 4 arguments: addresse de I2C, le registre ou l'on veut écrire, l'information qu'on veut écrire et le nombre bytes à écrire
{
    
    Cy_SCB_I2C_MasterSendStart(I2C_1_HW,dev_addr,CY_SCB_I2C_WRITE_XFER,0,&I2C_1_context); // on démarre la fonction
    
    Cy_SCB_I2C_MasterWriteByte(I2C_1_HW,reg_addr,0,&I2C_1_context); // on envoie le registre où l'on veut écrire
    
    for(int i = 0;i<len; i++) // loop pour tous les bytes
    { 
        
        Cy_SCB_I2C_MasterWriteByte(I2C_1_HW,data[i],0,&I2C_1_context); //on écrit tous les bytes avec cette fonction
        
    }
    
    Cy_SCB_I2C_MasterSendStop(I2C_1_HW,0,&I2C_1_context);// on arrête la fonction
    
    return 0;
}

static int8_t BMI160BurstRead(uint8_t dev_addr, uint8_t reg_addr,uint8_t *data, uint16_t len) //fonction pour lire
{
    
    Cy_SCB_I2C_MasterSendStart(I2C_1_HW,dev_addr,CY_SCB_I2C_WRITE_XFER,0,&I2C_1_context); //I2C START
    
    Cy_SCB_I2C_MasterWriteByte(I2C_1_HW,reg_addr,0,&I2C_1_context); //écrit l'addresse que l'on veut lire
    
    Cy_SCB_I2C_MasterSendReStart(I2C_1_HW,dev_addr,CY_SCB_I2C_READ_XFER,0,&I2C_1_context); //I2C RESTART
    
    for(int i = 0;i<len-1; i++)
    {
        
        Cy_SCB_I2C_MasterReadByte(I2C_1_HW,CY_SCB_I2C_ACK,&data[i],0,&I2C_1_context); //fonction avec ACK jusqu'à la fin de la lecture
        
    }
    
    Cy_SCB_I2C_MasterReadByte(I2C_1_HW,CY_SCB_I2C_NAK,&data[len-1],0,&I2C_1_context); //fonction pour dire que la lecture est finie
    
    Cy_SCB_I2C_MasterSendStop(I2C_1_HW,0,&I2C_1_context); //on arrête la fonction
    
    
    return 0;
}


static void sensorsDeviceInit(void)
{

  int8_t rslt;
    
  vTaskDelay(500); 

  bmi160Dev.read = (bmi160_read_fptr_t)BMI160BurstRead;

  bmi160Dev.write = (bmi160_write_fptr_t)BMI160BurstWrite;

  bmi160Dev.delay_ms = (bmi160_delay_fptr_t)vTaskDelay;
  
  bmi160Dev.id = BMI160_I2C_ADDR;  

  rslt = bmi160_init(&bmi160Dev); 

  if (rslt == 0)
    {
        
      printf("Determination des valeurs du mouvement\n");
      bmi160Dev.gyro_cfg.odr = BMI160_GYRO_ODR_800HZ;
      bmi160Dev.gyro_cfg.range = BMI160_GYRO_RANGE_125_DPS;
      bmi160Dev.gyro_cfg.bw = BMI160_GYRO_BW_OSR4_MODE;

      bmi160Dev.gyro_cfg.power = BMI160_GYRO_NORMAL_MODE;

      bmi160Dev.accel_cfg.odr = BMI160_ACCEL_ODR_1600HZ;
      bmi160Dev.accel_cfg.range = BMI160_ACCEL_RANGE_4G;
      bmi160Dev.accel_cfg.bw = BMI160_ACCEL_BW_OSR4_AVG1;
      bmi160Dev.accel_cfg.power = BMI160_ACCEL_NORMAL_MODE;

      bmi160_set_sens_conf(&bmi160Dev);
      bmi160Dev.delay_ms(50);
    
    }
    
  else
    
    {
        
      printf("ALERTE: erreur\n");
    }
}

#define MAXACCEL 8000;


/* [] END OF FILE */
