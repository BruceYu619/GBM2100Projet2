/******************************************************************************
* File Name: main_cm4.c
*
* Code final - Oxymètre de pouls 6688
* Par :
* Lauriane Grondin-Reiher (2011285)
* Gabriel Lopez-Garcia (2017012)
* Joëlle Morissette (2017462)
* Yi Yang Yu (2017484)
* 
*
********************************************************************************/

// LA VERSION FINALEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE !!!!!!!! 

// inclure tous les fichiers pertinents
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
#include "queue.h"
#include <arm_math.h> 
#include <core_cm4.h>
#include "bmi160.h"
#include "codeBmi.c"
#include "math.h"


// Définitions - Acquisition
#define SLAVEADRESS 0x57
#define REG_FIFO_DATA 0x07

// Définitions - Interface
#define TASK_DISPLAY_PRIORITY    (1u)
#define TASK_DISPLAY_STACK_SIZE  (2048u)
#define TASK_TOUCH_STACK_SIZE    (configMINIMAL_STACK_SIZE)
#define TOUCH_QUEUE_LENGHT       (1u)

// Définitions - Traitement de signal
#define TEST_LENGTH_SAMPLES  1000 
#define BLOCK_SIZE 1000 
#define FILTER_TAP_NUM 861 // 
#define FILTER_TAP_HAUT 51
#define FILTER_TAP_BAS 51
static float32_t firStateF32[BLOCK_SIZE + FILTER_TAP_NUM -1];

volatile bool Index = false;
extern bool flagAlarme;
extern volatile int courantDEL;

// PARTIE ACQUISITION


// Trouver la position du capteur afin de déterminer s'il y a un mouvement important 
void motionTask(void *arg)
{
    (void)arg;
    I2C_1_Start();
    sensorsDeviceInit();
    struct bmi160_sensor_data acc;
    int xx = 0;

    while(1)
    {
        bmi160_get_sensor_data(BMI160_ACCEL_ONLY, &acc, NULL, &bmi160Dev);
        //printf("x=%4d y=%4d z=%4d\r\n",acc.x,acc.y,acc.z); // Décommenter pour voir les coordonnées du capteur sur TeraTerm

        vTaskDelay(2);
        
        // Si mouvement trop important, on allume une DEL rose
        if (acc.x > xx + 200 && flagAlarme == true  /*|| acc.y > yy + 200*/ )
        {   
            UART_1_PutString("\r\n ATTENTION: Veuillez ne pas bouger \r\n");
            Cy_GPIO_Write(RED_PORT,RED_NUM,0);
        }
        
        else if(acc.x<600){
            Cy_GPIO_Write(RED_PORT,RED_NUM,1);
            xx = acc.x;
            interrupt_MAX30102();
        }    
        
        xx = acc.x;
    }
}

// Définition des données qui seront utiles au transfer de données dans le traitement de signal
float DataRouge[1000];
float DataIR[1000];


uint32_t bufferROUGE[1000];
uint32_t bufferINFRAROUGE[1000];
uint16_t indice;

uint32_t* pointeurIr =&bufferINFRAROUGE[0];
uint32_t*pointeurRed =&bufferROUGE[0];

// Lecture des registres 
uint8_t read_MAX30102_Register(uint8_t address)
{
    I2C_MasterSendStart( SLAVEADRESS,CY_SCB_I2C_WRITE_XFER,100);
    I2C_MasterWriteByte(address,100);  
    I2C_MasterSendReStart(SLAVEADRESS,CY_SCB_I2C_READ_XFER,100);
    uint8_t regValue=0;
    I2C_MasterReadByte(CY_SCB_I2C_MASTER_DATA_NAK,(uint8_t*)&regValue,100);
    I2C_MasterSendStop(100);  
    return regValue;
}

// // 4 arguments: addresse de I2C, le registre ou l'on veut écrire, l'information qu'on veut écrire et le nombre bytes à écrire
void write_MAX30102_Register(uint8_t address, uint8_t data)
{ 
    I2C_MasterSendStart(SLAVEADRESS,CY_SCB_I2C_WRITE_XFER,100);
    I2C_MasterWriteByte(address,100);        
    I2C_MasterWriteByte(data,100);         
    I2C_MasterSendStop(100);   
    
}

void burstRead_MAX30102(uint8_t baseAddress, uint8_t *buffer, uint8_t length)
{
        I2C_MasterSendStart( SLAVEADRESS,CY_SCB_I2C_WRITE_XFER,100); 
        I2C_MasterWriteByte(baseAddress,100);       
        I2C_MasterSendReStart( SLAVEADRESS,CY_SCB_I2C_READ_XFER,100);
        uint8_t idx = 0;
        for(idx=0 ; idx < length-1 ; idx++){ 
            I2C_MasterReadByte(CY_SCB_I2C_ACK,(uint8_t*)&buffer[idx],100);  
        }
        I2C_MasterReadByte(CY_SCB_I2C_NAK,(uint8_t*)&buffer[idx],100);
        I2C_MasterSendStop(100); 
}

// Fonction pour écrire les données du capteur dans les buffers
void read_FIFO_DATA(uint32_t *bufferR, uint32_t* bufferIR){
        
    uint8_t buffer[6];
    
    burstRead_MAX30102(REG_FIFO_DATA, buffer, 6); 
    
    UART_1_PutString("Burst Read ");
    
    *bufferR = ((buffer[0]&0b00000011)<<16) + (buffer[1]<<8) + buffer[2];
    
    *bufferIR = ((buffer[3]&0b00000011)<<16) + (buffer[4]<<8) + buffer[5];
}

/*--- ACQUISITION DES DONNÉES DU MAX30102---*/

// Fonction pour envoyer les buffers remplis au traitement de signal
void interrupt_MAX30102(void){      
    
    UART_1_PutString("interrupt ");
    
    read_FIFO_DATA(&bufferROUGE[indice], &bufferINFRAROUGE[indice]);
    UART_1_PutString("Reading done ");
    char OutPutStream[150];
    
    sprintf(OutPutStream, "%lu :: %lu \r\n",(unsigned long)bufferROUGE[indice],(unsigned long) bufferINFRAROUGE[indice]);
    
    UART_1_PutString(OutPutStream);
    
    indice++;
    
    if(indice==1000){ 
       
        saturation(pointeurIr, pointeurRed);
        indice=0;}
    
    
    void motionTask();  
}

/*---AJUSTEMENT DES REGISTRES DU MAX30102---*/
bool maxim_max30102_init()
{
    Cy_GPIO_Write(BOUTON1_0_PORT,BOUTON1_0_NUM,1);
    UART_1_PutString("Initializing\r\n");
    write_MAX30102_Register(0x09, 0x40);
    CyDelay(1000);
    
    write_MAX30102_Register(0x02, 0xc0);
    write_MAX30102_Register(0x03, 0x00);
    
    write_MAX30102_Register(0x06, 0x00);
    write_MAX30102_Register(0x08, 0x0f);
    write_MAX30102_Register(0x09, 0x03);
    write_MAX30102_Register(0x0A, 0b01000111);
    
    write_MAX30102_Register(0x0C, courantDEL); //changement intensité
    write_MAX30102_Register(0x0D, courantDEL);// ^
    write_MAX30102_Register(0x10, 0x7f);
    
    
    UART_1_PutString("Initializing done\r\n");
    return true;
}

// FIN AQUISITION


// TRAITEMENT DE SIGNAL

// Définition des coefficients des filtres
const float32_t PasseHaut [51]= {-0.0005 , -0.0005, -0.0006 , -0.0007 , -0.0009 , -0.0011 , -0.0013 , -0.0016 , -0.0019, -0.0022 , -0.0026 , -0.0030 , -0.0034, -0.0037 , -0.0041, -0.0045 , -0.0049 , -0.0052, -0.0055 , -0.0058 , -0.0061 ,-0.0063, -0.0064 , -0.0066 ,-0.0066 , 0.9928, -0.0066 , -0.0066 , -0.0064, -0.0063 ,-0.0061 , -0.0058 , -0.0055, -0.0052 , -0.0049 , -0.0045 , -0.0041 , -0.0037 , -0.0034,-0.0030 ,-0.0026 ,-0.0022 , -0.0019 , -0.0016 , -0.0013 ,-0.0011 , -0.0009 ,-0.0007 , -0.0006 ,-0.0005 ,-0.0005};
const float32_t PasseBas[51]= { 0.0020, 0.0022 , 0.0026 , 0.0031 , 0.0040 , 0.0050 , 0.0063 , 0.0078 , 0.0095 , 0.0114 ,0.0135 , 0.0157 , 0.0180, 0.0204 , 0.0228 , 0.0252, 0.0275 , 0.0297 , 0.0318 , 0.0337, 0.0354 , 0.0368 , 0.0379 ,0.0388, 0.0393 , 0.0394, 0.0393 , 0.0388, 0.0379 ,0.0368 , 0.0354 , 0.0337 , 0.0318 , 0.0297 ,0.0275 ,0.0252 ,0.0228, 0.0204 , 0.0180 ,0.0157 , 0.0135 , 0.0114 , 0.0095 , 0.0078 , 0.0063 , 0.0050 , 0.0040 , 0.0031 , 0.0026 ,0.0022 , 0.0020};

// Définitions de variables importantes 
uint32_t blockSize = BLOCK_SIZE;
arm_fir_instance_f32 S; 
volatile float SpO2 =0;
volatile int bpm =0;
volatile int NbreMax =0; //combien de maximums dans le signal traité pour etre capable de trouver bpm
volatile int NbreSec =0; //permet de suivre l'évolution du signal dans le temps

// Déclaration des bornes supérieure et inférieure pour le rythme cardiaque (peut être modifié par l'utilisateur)
volatile int borneSup = 130; 
volatile int borneInf = 40;


// Déclaration des vecteurs utilisés pour stocker et filtrer les données
float32 IrFiltre[1000]; //tableau avec données filtrées
float32 RedFiltre[1000];
float IrFiltre1[1000]; 
float RedFiltre1[1000];
float dataIR[1000];
float dataRed[1000];


/* ----CALCUL DE LA SATURATION ET DE LA FRÉQUENCE CARDIAQUE---- */

void saturation(uint32_t*DataIR, uint32_t *DataRed){ 
   
    //Éteindre la DEL (elle s'allumera si l'alarme est déclenchée)
    Cy_GPIO_Write(Pin_1_0_PORT, Pin_1_0_NUM,1);
    
    // Conversion des types de données pour passer de l'acquisition au traitement de signal
    for(int i =0; i < 1000; i++){
        dataIR[i] = (float) DataIR[i];
        dataRed[i] = (float) DataRed[i];
    }
       
// Trouver les composantes DC sur le signal non-traité
	float DCIR =0;
	float DCRED =0;
	arm_mean_f32(dataIR, BLOCK_SIZE,&DCIR);
	arm_mean_f32(dataRed, BLOCK_SIZE, &DCRED);

// Filtrage des signaux AC et DC 
    
// Filtre passe-haut    
    arm_fir_init_f32(&S, FILTER_TAP_HAUT, (float32_t*)&PasseHaut[0], &firStateF32[0], blockSize);    // Verifier comment s'
    arm_fir_f32(&S, dataIR, IrFiltre1, BLOCK_SIZE);
    arm_fir_f32(&S, dataRed, RedFiltre1,BLOCK_SIZE);
// Filtre passe-bas
    arm_fir_init_f32(&S, FILTER_TAP_BAS, (float32_t*)&PasseBas[0], &firStateF32[0], blockSize);
    arm_fir_f32(&S, IrFiltre1, IrFiltre, BLOCK_SIZE);
    arm_fir_f32(&S, RedFiltre1, RedFiltre,BLOCK_SIZE);
    
    
// Trouver les composantes AC à partir du signal filtré
	float maxIR =0;             
	float maxRed =0;
	float minIR =0;
	float minRed =0;	
	uint32_t position;
   
	arm_max_f32(IrFiltre, BLOCK_SIZE, &maxIR, &position);
	arm_max_f32(RedFiltre, BLOCK_SIZE, &maxRed, &position);
	arm_min_f32(IrFiltre, BLOCK_SIZE, &minIR, &position);
	arm_min_f32(RedFiltre, BLOCK_SIZE, &minRed, &position);

	float ACIr = maxIR - minIR;
	float ACRed = maxRed - minRed;
    
// Trouver la saturation dans l'intervale:
	float R = (ACRed/DCRED)/(ACIr/DCIR);
	//SpO2 = -16.67*(R*R) + 8.33*R + 96;  // autre formule - fonctionne seulement à 40 mA dans les DELs
    
    SpO2 = 1.5958422*(R*R) -34.6596622*R + 105.6898759;
   
    
// Trouver les maximums dans la période en cours
	NbreSec = NbreSec+1;
    for(int i=1; i<(BLOCK_SIZE-1); i++){
		if((IrFiltre[i] > IrFiltre[i-1]) && (IrFiltre[i] > IrFiltre[i+1]) && (IrFiltre[i] >0.4*ACIr)){
			NbreMax =NbreMax +1;
		}        
	}
   
// Réinitialiser les vecteurs dont nous n'avons plus besoin   
    for(int i =0; i < 1000; i++){
        dataIR[i] = 0;
        IrFiltre[i] = 0;
        IrFiltre1[i] = 0;
        dataRed[i] = 0;
        RedFiltre1[i] =0;
        RedFiltre[i] =0;
    }
    
    
// Calcul du bpm
    if(NbreSec ==1){
        bpm = 3*NbreMax; // Dans le traitement Matlab, on faisait *6. Ici, *3 après tests.
    }
    if(NbreSec == 2){
        bpm = (NbreMax*3)/2; // Sur Matlab, *3. En C, *3/2 donne de meilleurs résultats.
        NbreMax =0;
        NbreSec =0;
    }
    
    /* ---DÉCLENCHEMENT DE L'ALARME SI LE RYTHME CARDIAQUE À L'EXTÉRIEUR DES BORNES---*/
    if((bpm > borneSup)|| (bpm < borneInf)){
            Cy_GPIO_Write(Pin_1_0_PORT, Pin_1_0_NUM,0);
    }
    
    if (maxIR < 900){
        SpO2 = 666; // pour avertir l'utilisateur que le doigt n'a pas encore été placé.
        bpm = 666;
    }
    
    
}




int main(void)
{
  
    __enable_irq(); /* Enable global interrupts. */
    Cy_GPIO_Write(RED_PORT,RED_NUM,1); // initalise la led liée à l'accéléromètre (éteinte)
    
    // Initialisation - Interface
    CapSense_Start(); 
    CapSense_ScanAllWidgets(); 
    
    // Initialisation - Acquisition
    I2C_Start();
    UART_1_Start();
    maxim_max30102_init();
    
    // Initialisation - Traitement
    SystemInit(); 
    
    UART_1_Start(); // Seulement pour tester l'acquisition des données
    
    // Tâche d'acquisition
    xTaskCreate(motionTask, "Motion Task",400,0,1,0);

    // Tâches de l'interface
    xTaskCreate(capSense_Task,"capSense_task",400,NULL,1,0);
    xTaskCreate(Task_Display, "Display task", 400,NULL, 1, NULL); 
    
    vTaskStartScheduler();
    while(CapSense_IsBusy());
    
for(;;)
    {
      
    }
}

/* [] END OF FILE */
