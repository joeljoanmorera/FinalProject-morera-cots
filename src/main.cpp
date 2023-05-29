#include <U8g2lib.h>
#include <SPI.h>
#include <vector>
#include "WiFi.h"
#include "SPIFFS.h"
#include "ESPAsyncWebServer.h"

#include "WebPage.h"
#include "Button.h"
#include "Display.h"
#include "GlobalValues.h"

using namespace std;

// DISPLAY PINS
#define SCL 18
#define SI 23
#define CS 5
#define RS 32
#define RSE 33

// CONSTANTS and VARIABLES
const int BUTTON_NUMBER = 3;
const int BPM_PIN = 25;
const int SPO2_PIN = 26;
const int FUNDAMENTALS_PIN = 27;
const char* ssid = "MiFibra-F392"; // SSID of the WiFi
const char* password =  "5QUisHGE"; // Password of the WiFi
webPage webPageVar;
globalValues globalValuesVar;
Display display(U8G2_R0, SCL, SI, CS, RS, RSE);
buttonsArray buttons(BUTTON_NUMBER);                            
hw_timer_t * timer = NULL;                  

// FUNCTIONS DECLARATION
void initButtons();
void IRAM_ATTR buttonManagement();
void initSPIFFS();
void fillDataTests();
void visualizeData(void * parameter);

/** Setup function
 * 
 * @brief This function is the setup of the program.
 *  
 * @return void.
 *  
 * @details This function initializes the serial communication, the display, the buttons, the web, the data and the timer.
 *  
 * @note This function is called once.
 * 
 * @see loop().
 * 
 */
void setup() 
{
    // Serial initialization
    Serial.begin(115200);

    // Display initialization
    display.init();

    //Initzialitaion of buttons
    initButtons();

    // SPIFFS initialization
    initSPIFFS();

    // Web initialization
    webPageVar.begin(ssid, password);

    // Create task for visualizing data
    xTaskCreatePinnedToCore(
                    visualizeData,   /* Task function. */
                    "visualizeData", /* name of task. */
                    10000,        /* Stack size of task */
                    NULL,         /* parameter of the task */
                    1,            /* priority of the task */
                    NULL,         /* Task handle to keep track of created task */
                    0);           /* pin task to core 0 */

    // Data initialization
    fillDataTests();
}

/** Loop function
 * 
 * @brief This function is the loop of the program.
 *  
 * @return void.
 *  
 * @details This function is empty because all the program is executed in the task.
 *  
 * @note This function is called once.
 * 
 * @see setup().
 * 
 */
void loop(){}

/** Visualize data function
 * 
 * @brief This function visualizes the data in the display and in the web page.
 *  
 * @return void.
 *  
 * @details This function visualizes the data in the display and in the web page. This
 * function is executed in one core of the ESP32.
 *  
 * @note This function is called once.
 * 
 * @see setup().
 * 
 */
void visualizeData(void * parameter)
{
    for(;;)
    {
        display.firstPage();
        Serial.print("Visualizing data - Button activated: ");
        if (buttons[0].orden)
            Serial.println("Heart Rate");
        else
        {
            if(buttons[1].orden)
                Serial.println("SpO2");
            else 
                Serial.println("Frequencies");
        }

        while(display.nextPage())
        {
            if (buttons[0].orden){
                display.updateData(globalValuesVar.getHeartRateDataArray(), globalValuesVar.getBeatsPerMinute(), true);
            }
            if(buttons[1].orden){
                display.updateData(globalValuesVar.getSpo2DataArray(), globalValuesVar.getSpo2Percentage(), false);
            }
            if(buttons[2].orden){
                display.updateFreqs(globalValuesVar.getFreqs());
            }        
        }

        webPageVar.sendWsMessage(globalValuesVar.getJson((display.xAxisEnd - display.xAxisBegin), (display.xAxisEnd - display.xAxisBegin)));

        delay(700); //Wait 1000ms
    }
}

/** Buttons initialization function
 * 
 * @brief This function initializes the buttons.
 *  
 * @return void.
 *  
 * @details This function initializes the buttons.
 *  
 * @note This function is called once.
 * 
 * @see buttonManagement().
 * 
 */
void initButtons(){
    // Initilization of buttons
    int* buttons_pins = new int[BUTTON_NUMBER];
    buttons_pins[0] = BPM_PIN;
    buttons_pins[1] = SPO2_PIN;
    buttons_pins[2] = FUNDAMENTALS_PIN;
    buttons.begin(buttons_pins);

    for(uint8_t i = 0; i < BUTTON_NUMBER; i++)
    {
        pinMode(buttons[i].pin, INPUT_PULLUP);
    }

    // Timer initialization
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &buttonManagement, true);
    timerAlarmWrite(timer, 100000, true);
    timerAlarmEnable(timer);
}

/** Button management function
 * 
 * @brief This function manages the buttons.
 *  
 * @return void.
 *  
 * @details This function reads the buttons' values and changes the order of the buttons.
 *  
 * @note This function is called when the timer is activated. 
 *       Therefore, it is called when an interrupt is generated.
 * 
 * @see initButtons().
 * 
 */
void IRAM_ATTR buttonManagement()
{ 
    for(uint8_t i = 0; i < BUTTON_NUMBER; i++)
    {
        buttons[i].val_act = digitalRead(buttons[i].pin);               // Read the value of the button
        buttons[i].cambioact = buttons[i].val_ant ^ buttons[i].val_act; // XOR of actual value and last value
        if(buttons[i].cambioact == 1 && buttons[i].cambioanterior == 1) // If both status changes are equal to 1
        {
            buttons[i].orden = 1;                                         // Order to 1
            for (uint8_t j = 0; j < BUTTON_NUMBER; j++)
            {
                if (j != i)buttons[j].orden = 0;                            // Rest of orders to 0 
            }
            buttons[i].val_ant = buttons[i].val_act;                      // Last value equal to actual value
            buttons[i].cambioanterior = 0;                                // Last status change equal to 0
            return;
        }
        buttons[i].cambioanterior = buttons[i].cambioact;               // Last status change is equal to acutal change
    }
}

/** SPIFFS initialization function
 * 
 * @brief This function initializes the SPIFFS.
 *  
 * @return void.
 *  
 * @details This function initializes the SPIFFS.
 *  
 * @note This function is called once.
 * 
 */
void initSPIFFS()
{
  if(!SPIFFS.begin()){
     Serial.println("An Error has occurred while mounting SPIFFS");
     for(;;);
  }
}

/** Data tests initialization function
 * 
 * @brief This function initializes the data tests.
 *  
 * @return void.
 *  
 * @details This function initializes the data tests.
 *  
 * @note This function is called once.
 * 
 */
void fillDataTests()
{   
    uint32_t* heartRateData_temp = new uint32_t[display.xAxisEnd - display.xAxisBegin];
    uint32_t* spo2Data_temp = new uint32_t[display.xAxisEnd - display.xAxisBegin];

    uint32_t j = 0;
    for (uint32_t i = 0; i < (display.xAxisEnd - display.xAxisBegin); i++)
    {
        // Little mountain in the middle with a value of yAxisEnd
        if (i < display.xAxisEnd/4)
        {
            spo2Data_temp[i] = 0;
        }
        else if (i < display.xAxisEnd/2)
        {
            spo2Data_temp[i] =  j;
            j+=3;
        }
        else if (i < 3*display.xAxisEnd/4)
        {
            j-=3;
            spo2Data_temp[i] = j;
        }
        else
        {
            spo2Data_temp[i] = 0;
        }
    }

    for (uint32_t i = 0; i < (display.xAxisEnd - display.xAxisBegin); i++)
    {
        // Little mountain in the middle with a value of yAxisEnd
        if (i < display.xAxisEnd/4)
        {
            heartRateData_temp[i] = 0;
        }
        else if (i < 3*display.xAxisEnd/8)
        {
            heartRateData_temp[i] =  j;
            j+=3;
        }
        else if (i < 5*display.xAxisEnd/8)
        {
            if(j >= 3)j-=3;
            heartRateData_temp[i] = j;
        }
        else if (i < 3*display.xAxisEnd/4)
        {
            j+=3;
            heartRateData_temp[i] = j;
        }
        else
        {
            heartRateData_temp[i] = 0;
        }
    }

    int32_t *beatsPerMinute_temp = new int32_t[5];
    int32_t *spo2Percentage_temp = new int32_t[5];

    beatsPerMinute_temp[0] = 60;
    beatsPerMinute_temp[1] = 70;
    beatsPerMinute_temp[2] = 80;
    beatsPerMinute_temp[3] = 90;
    beatsPerMinute_temp[4] = 100;


    spo2Percentage_temp[0] = 90;
    spo2Percentage_temp[1] = 91;
    spo2Percentage_temp[2] = 92;
    spo2Percentage_temp[3] = 93;
    spo2Percentage_temp[4] = 94;


    vector<fundamentalsFreqs> freqs_temp(7);

    freqs_temp[0].freqsHz = 100;
    freqs_temp[0].amplitude = 640;
    freqs_temp[1].freqsHz = 500;
    freqs_temp[1].amplitude = 240;
    freqs_temp[2].freqsHz = 1000;
    freqs_temp[2].amplitude = 440;
    freqs_temp[3].freqsHz = 10000;
    freqs_temp[3].amplitude = 129;
    freqs_temp[4].freqsHz = 20000;
    freqs_temp[4].amplitude = 100;
    freqs_temp[5].freqsHz = 30000;
    freqs_temp[5].amplitude = 50;
    freqs_temp[6].freqsHz = 40000;
    freqs_temp[6].amplitude = 220;

    globalValuesVar.setHeartRateDataArray(heartRateData_temp);
    globalValuesVar.setSpo2DataArray(spo2Data_temp);
    globalValuesVar.setBeatsPerMinute(beatsPerMinute_temp[0]);
    globalValuesVar.setSpo2Percentage(spo2Percentage_temp[0]);
    globalValuesVar.setFreqs(freqs_temp);
}