/* ========================================
 *
 *Interface
 *
 * ========================================
*/
#include "cy_eink_library.h"
#include "display_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "project.h"
#include "GUI.h"
#include "pervasive_eink_hardware_driver.h"
#include "cy_eink_library.h"
#include "LCDConf.h"
#include <stdlib.h>


#define NUMBER_OF_FRAME_BUFFERS     (uint8_t) (2u)

#define PAGE_MENU 1
#define PAGE_LED 2
#define PAGE_ALARME_1 3
#define PAGE_ALARME_2 4
#define PAGE_AFFICHAGE 5

#define RED_SIGNAL 1
#define IR_SIGNAL 2

uint8_t currentPage = PAGE_LED; 
uint8_t previousPage;

uint8_t currentGraph = RED_SIGNAL; 
uint8_t previousGraph;
bool flagSignalSwitch; // lever le flag dans une interruption reliée à l'appui du bouton SW2

volatile int courantDEL = 5;

volatile int borneSup = 130;
volatile int borneInf = 40;
volatile bool flag = false;

int32_t vecRED[] = {205973,207305,206168,208001,206188,208050,206371,208225,206544,208577,206719,208501,206990,208473,207183,208417,207367,208379,207724,208151,207986,207993,208303,207802,208682,207622,208730,207587,209365,207455,209209,207541,209337,207549,209635,207806,209501,207957,209461,207996,209317,208103,209140,208491,209000,208672,208748,209006,208491,209357,208350,209326,208242,209947,208089,209826,208216,210028,208306,210395,208429,210203,208596,210292,208686,210169,208954,210055,209354,209987,209644,209821,210027,209660,210440,209483,210556,209554,211304,209484,211253,209696,211585,209817,211970,210011,211842,210178,211772,209972,211207,209606,210230,208954,209085,208043,207710,207263,206266,206439,205001,205392,203820,204970,202756,203955,202073,203512,201459,203377,201194,202759,200993,202713,201002,202459,201198,202302,201513,202188,201835,202108,202327,202081,202824,201965,203003,202176,203932,202267,203918,202500,204436,202777,204921,203032,204880,203312,205157,203520,205057,203810,205034,204221,204997,204527,204912,204994,204917,205416,204837,205671,204732,206338,204657,206251,204614,206460,204591,206662,204581,206367,204586,206402,204548,205967,204548,205683,204576,205312,204550,204995,204844,204647,205035,204328,205121,204143,205709,203943,205474,203995,205910,204074,206112,204207,205943,204379,206337,204543,206193,204879,206211,205289,206165,205581,206150,206104,206141,206566,206010,206910,206113,207618,206076,207602,206252,208240,206424,208563,206665,208406,206886,208919,207081,208835,207367,208762,207638,208729,207896,208526,208348,208387,208732,208203,209019,208070,209391,207916,209311,207900,209782,207913,209978,208038,209863,208165,210137,208232,209963,208477,209827,208599,209678,208829,209472,209224,209399,209521,209079,209731,208932,210161,208772,210177,208810,210728,208819,210815,208995,210858,209138,211288,209319,211079,209575,211060,209773,210901,209995,210738,210439,210645,210706,210529,211089,210404,211618,210365,211667,210429,212263,210423,212435,210609,212498,210811,212917,210990,212716,211242,212775};
int32_t vecIR[] = {};
int32_t *ptrVecRED = &vecRED[0]; // pointeur vers le vecteur contenant les données du signal rouge
const int longueur = 293;
int32_t vecRED_Affichage[]={};
const int freq_ech = 250;
const float tempsTotal = 293/250;

float vecTime[]={};

int32_t *pointeurAffichage;



void isr_bouton(void)
{
    Cy_GPIO_ClearInterrupt(Bouton_0_PORT, Bouton_0_NUM);
    NVIC_ClearPendingIRQ(Bouton_isr_cfg.intrSrc);
    if (currentGraph==IR_SIGNAL)
    {
        currentGraph=RED_SIGNAL;
        previousGraph = IR_SIGNAL;
        pointeurAffichage = &vecRED[0];
    }
    else if (currentGraph==RED_SIGNAL)
    {
        currentGraph=IR_SIGNAL;
        previousGraph = RED_SIGNAL;
        pointeurAffichage = &vecIR[0];
    }
}





/* Image buffer cache */
uint8 imageBufferCache[CY_EINK_FRAME_SIZE] = {0};


void UpdateDisplay(cy_eink_update_t updateMethod, bool powerCycle)
{
    cy_eink_frame_t* pEmwinBuffer;

    /* Get the pointer to Emwin's display buffer */
    pEmwinBuffer = (cy_eink_frame_t*)LCD_GetDisplayBuffer();

    /* Update the EInk display */
    Cy_EINK_ShowFrame(imageBufferCache, pEmwinBuffer, updateMethod, powerCycle);

    /* Copy the EmWin display buffer to the imageBuffer cache*/
    memcpy(imageBufferCache, pEmwinBuffer, CY_EINK_FRAME_SIZE);
}

void ClearScreen(void)
{
    GUI_SetColor(GUI_BLACK);
    GUI_SetBkColor(GUI_WHITE);
    GUI_Clear();
    UpdateDisplay(CY_EINK_FULL_4STAGE, true);
}



void drawSignal(int32_t* pointeur)
{
    ClearScreen();
    for (int i =0; i<longueur; i++)
    {
        vecTime[i]= i*tempsTotal/longueur;
    }
    
    for (int i=0; i<=longueur; i+=4) // afficher une valeur sur 4 pour ne pas surcharger l'affichage
    {
        int x0 = vecTime[i]*170;    // Les valeurs de temps varient entre 0 et 
        int x1 = vecTime[i+1]*170;
        int y0 = (int) (pointeur[i]/1700); // conversion des valeurs pour obtenir la position des pixels
        int y1 = (int) (pointeur[i+1]/1700);
        GUI_DrawLine (x0, y0, x1, y1);
    }
    UpdateDisplay(CY_EINK_FULL_4STAGE, true);
    
}

void updateParameters(int param1, int param2)  // on peut utiliser cette fonction pour afficher la saturation et la fréquence
{
    char buffer[50];
    char buffer2[50];
    GUI_SetFont(GUI_FONT_8_1);
    GUI_SetTextAlign(GUI_TA_LEFT);
    GUI_DispStringAt("Frequence cardiaque (Hz): ", 10, 160); 
    sprintf(&buffer[0], "%d", param1);
    GUI_DispStringAt(buffer,80,160);
    GUI_DispStringAt("Saturation en oxygene (%): ", 100,160);
    sprintf(&buffer2[0], "%d", param2);
    GUI_DispStringAt(buffer2, 166, 160);
    UpdateDisplay(CY_EINK_FULL_4STAGE, true);
    
}



// écrire une fonction void handlerQueue()


void Task_Display(void *pvParameters) // à transformer en fonction et appeler dans une autre tâche?
{
    
    (void) pvParameters;
    
    
  if (currentPage==PAGE_MENU)
    {
        GUI_DrawRect(1,1,263,175);
        GUI_SetPenSize(4);
        GUI_DrawLine(20,15,243,15);
    
        GUI_SetColor(GUI_BLACK);
        GUI_SetBkColor(GUI_WHITE);
        GUI_SetFont(GUI_FONT_24B_1);
        GUI_SetTextAlign(GUI_TA_CENTER);
        GUI_DispStringAt("MENU", 132, 5);
        GUI_SetFont(GUI_FONT_16_1);
        GUI_SetTextAlign(GUI_TA_LEFT);
    // Cercle qui indique on est où dans le menu selon un paramètre qui dépend des boutons cliqués par l'usager
    //GUI_DrawCircle(40, 87, 4); // Éventuellement le deuxième argument dépendra du paramètre
        GUI_DispStringAt("Modifier le courant des DELs ", 30, 30);
        GUI_DispStringAt("Regler l'alarme de l'accelerometre ", 30,60);
        GUI_DispStringAt("Programmer l'alarme de danger", 30,90);
        GUI_DispStringAt("Acceder a l'ecran des resultats", 30,120);
        UpdateDisplay(CY_EINK_FULL_4STAGE, true);
    }
    
    if (currentPage==PAGE_LED)
    {
        GUI_SetColor(GUI_BLACK);
        GUI_SetBkColor(GUI_WHITE);
        GUI_SetFont(GUI_FONT_24B_1);
        GUI_SetTextAlign(GUI_TA_CENTER);
        GUI_DispStringAt("REGLAGE DELs", 132, 5);
        char bufferDEL[50];
        GUI_SetFont(GUI_FONT_16_1);
        GUI_SetTextAlign(GUI_TA_LEFT);
        GUI_DispStringAt("Intensite des DELs (V): ", 10, 60); 
        sprintf(&bufferDEL[0], "%d", courantDEL);
        GUI_DispStringAt(bufferDEL, 150, 60);
        GUI_DispStringAt("augmenter : Bouton1 | diminuer : Bouton0", 10, 120);
        UpdateDisplay(CY_EINK_FULL_4STAGE, true);
    }
    
    if (currentPage==PAGE_ALARME_1)
    {
        GUI_SetColor(GUI_BLACK);
        GUI_SetBkColor(GUI_WHITE);
        GUI_SetFont(GUI_FONT_20B_1);
        GUI_SetTextAlign(GUI_TA_CENTER);
        GUI_DispStringAt("ALARME ACCELEROMETRE", 132, 5);
        GUI_SetFont(GUI_FONT_16_1);
        GUI_SetTextAlign(GUI_TA_LEFT);
        GUI_DispStringAt("Pour activer l'alarme : bouton B0 ", 10, 60); 
        GUI_DispStringAt("Pour desactiver l'alarme : bouton B1 ", 10, 90); 
        UpdateDisplay(CY_EINK_FULL_4STAGE, true);
        // si bouton0, flag alarme true. Si bouton B1, flag false - à faire dans tâche handler queue
    }
    if (currentPage==PAGE_ALARME_2)
    {
        char bufferSup[50];
        char bufferInf[50];
        GUI_SetColor(GUI_BLACK);
        GUI_SetBkColor(GUI_WHITE);
        GUI_SetFont(GUI_FONT_20B_1);
        GUI_SetTextAlign(GUI_TA_CENTER);
        GUI_DispStringAt("ALARME RYTHME", 132, 5);
        GUI_SetFont(GUI_FONT_16_1);
        GUI_SetTextAlign(GUI_TA_LEFT);
        GUI_DispStringAt("Borne superieure (bpm) : ", 10, 60); 
        sprintf(&bufferSup[0], "%d", borneSup);
        GUI_DispStringAt("Borne inferieure (bpm) : ", 10, 90);
        sprintf(&bufferInf[0], "%d", borneInf);
        GUI_DispStringAt("augmenter : Bouton1 | diminuer : Bouton0", 10, 120);
        UpdateDisplay(CY_EINK_FULL_4STAGE, true);
        // à modifier
    }
    
    if (currentPage==PAGE_AFFICHAGE)
    {
         drawSignal(pointeurAffichage);
    }
    
    /*
    if (flagSignalSwitch==true) // à mettre si on est à l'affichage 
    {
        SignalSwitch();
    }*/
    
    // écrire une fonction qui permet d'incrémenter la variable courantDEL selon le choix de l'utilisateur
    
 }   


/* [] END OF FILE */
