#include <U8g2lib.h>
#include <SPI.h>
#include "WiFi.h"
#include "SPIFFS.h"
#include "ESPAsyncWebServer.h"
#include <vector>
#include "heartRate.h"
#include "spo2_algorithm.h"
#include "arduinoFFT.h"
#include <Wire.h>
#include "MAX30105.h"

using namespace std;

// Max30102 definitions
#define I2C_SPEED_FAST 400000 // Set I2C frequency to 400kHz
#define MAX_BRIGHTNESS 255 // Set maximum brightness

// Filter and fft definitions
#define SAMPLES 64 // Número de muestras para la FFT
#define SAMPLING_FREQUENCY 25 // Frecuencia de muestreo en Hz

// Display pin's definition
#define SCL 18
#define SI 23
#define CS 5
#define RS 32
#define RSE 33

// CONSTANTS
//Pin distribution variables
const int BUTTON_NUMBER = 3;
const int BPM_PIN = 25;
const int SPO2_PIN = 26;
const int FUNDAMENTALS_PIN = 27;
uint8_t* button_pin;
//Server vars.
const char* ssid = "*****";
const char* password =  "*****";
// Max30102 object
MAX30105 particleSensor;
//Coeficients for the filter
float vcoefs1[201];
//Ir and Red led data
uint32_t irBuffer[200]; //infrared LED sensor data
uint32_t redBuffer[200];  //red LED sensor 

// CLASSES AND STRUCTS
/** Fundamentals frequencies struct
 * 
 * @brief This struct is the fundamentals frequencies of the device.
 *
 */
struct fundamentalsFreqs{
    int freqsHz;
    int amplitude;
};

/** Global values class
 * 
 * @brief This class is the global values of the device.
 *
 */
class globalValues {
    uint32_t* heartRateDataArray;
    uint32_t* spo2DataArray;
    int32_t beatsPerMinute, spo2Percentage;
    vector <fundamentalsFreqs> freqs;
    
    public:
        /** Global values default constructor
         * 
         * @brief This function is the constructor of the global values.
         *
         */
        globalValues()
        {
            this -> heartRateDataArray = 0;
            this -> spo2DataArray = 0;
            this -> beatsPerMinute = 0;
            this -> spo2Percentage = 0;
            this -> freqs = {};
        }
        
        /** Global values constructor
         * 
         * @brief This function is the constructor of the global values.
         *
         * @param heartRateDataArray Heart rate data array.
         * @param spo2DataArray SPO2 data array.
         * @param beatsPerMinute Beats per minute.
         * @param spo2Percentage SPO2 percentage.
         * @param freqs Fundamentals frequencies.
         */
        globalValues(uint32_t* heartRateDataArray, uint32_t* spo2DataArray, int32_t beatsPerMinute, int32_t spo2Percentage, vector<fundamentalsFreqs> freqs)
        {
            this -> heartRateDataArray = heartRateDataArray;
            this -> spo2DataArray = spo2DataArray;
            this -> beatsPerMinute = beatsPerMinute;
            this -> spo2Percentage = spo2Percentage;
            this -> freqs = freqs;
        }

        /** Set heart rate data array function
         * 
         * @brief This function sets the heart rate data array.
         * 
         * @param heartRateDataArray Heart rate data array.
         */
        void setHeartRateDataArray(uint32_t* heartRateDataArray)
        {
            this -> heartRateDataArray = heartRateDataArray;
        }

        /** Set SPO2 data array function
         * 
         * @brief This function sets the SPO2 data array.
         * 
         * @param spo2DataArray SPO2 data array.
         */
        void setSpo2DataArray(uint32_t* spo2DataArray)
        {
            this -> spo2DataArray = spo2DataArray;
        }

        /** Set beats per minute function
         * 
         * @brief This function sets the beats per minute.
         * 
         * @param beatsPerMinute Beats per minute.
         */
        void setBeatsPerMinute(int32_t beatsPerMinute)
        {
            this -> beatsPerMinute = beatsPerMinute;
        }

        /** Set SPO2 percentage function
         * 
         * @brief This function sets the SPO2 percentage.
         * 
         * @param spo2Percentage SPO2 percentage.
         */
        void setSpo2Percentage(int32_t spo2Percentage)
        {
            this -> spo2Percentage = spo2Percentage;
        }

        /** Set fundamentals frequencies function
         * 
         * @brief This function sets the fundamentals frequencies.
         * 
         * @param freqs Fundamentals frequencies.
         */
        void setFreqs(vector<fundamentalsFreqs> freqs)
        {
            this -> freqs = freqs;
        }

        /** Get heart rate data array function
         * 
         * @brief This function gets the heart rate data array.
         * 
         * @return Heart rate data array.
         */
        uint32_t* getHeartRateDataArray()
        {
            return heartRateDataArray;
        }

        /** Get SPO2 data array function
         * 
         * @brief This function gets the SPO2 data array.
         * 
         * @return SPO2 data array.
         */
        uint32_t* getSpo2DataArray()
        {
            return spo2DataArray;
        }

        /** Get beats per minute function
         * 
         * @brief This function gets the beats per minute.
         * 
         * @return Beats per minute.
         */
        int32_t getBeatsPerMinute()
        {
            return beatsPerMinute;
        }

        /** Get SPO2 percentage function
         * 
         * @brief This function gets the SPO2 percentage.
         * 
         * @return SPO2 percentage.
         */
        int32_t getSpo2Percentage()
        {
            return spo2Percentage;
        }

        /** Get fundamentals frequencies function
         * 
         * @brief This function gets the fundamentals frequencies.
         * 
         * @return Fundamentals frequencies.
         */
        vector<fundamentalsFreqs> getFreqs()
        {
            return freqs;
        }

        /** Get JSON function
         * 
         * @brief This function gets the JSON of the global values.
         * 
         * @return JSON of the global values.
         */
        String getJson(int size_of_HR, int size_of_SPO2)
        {
            String json = "{";
            json += "\"heartRateDataArray\": [";
            for(int i = 0; i < size_of_HR; i++)
            {
                json += String(heartRateDataArray[i]);
                if(i != size_of_HR - 1)
                {
                    json += ", ";
                }
            }
            json += "],";
            json += "\"spo2DataArray\": [";
            for(int i = 0; i < size_of_SPO2; i++)
            {
                json += String(spo2DataArray[i]);
                if(i != size_of_SPO2 - 1)
                {
                    json += ", ";
                }
            }
            json += "],";
            json += "\"beatsPerMinute\": " + String(beatsPerMinute) + ",";
            json += "\"spo2Percentage\": " + String(spo2Percentage) + ",";
            json += "\"freqsAmplitude\": [";
            for(int i = 0; i < freqs.size(); i++)
            {
                json += String(freqs[i].amplitude);
                if(i != freqs.size() - 1)
                {
                    json += ", ";
                }
            }
            json += "],";
            json += "\"freqsHz\": [";
            for(int i = 0; i < freqs.size(); i++)
            {
                json += String(freqs[i].freqsHz);
                if(i != freqs.size() - 1)
                {
                    json += ", ";
                }
            }
            json += "]";
            json += "}";

            return json;
        }
};

/** Button class
 * 
 * @brief This class is the button of the device.
 *
 */
class Button{
  public:
    //VARS
    uint8_t pin;                                              
    bool val_act, val_ant, cambioact, cambioanterior, orden;  
    //API

    /** Button default constructor
     * 
     * @brief This function is the constructor of the button.
     *
     */
    Button(){}

    /** Button constructor
     * 
     * @brief This function is the constructor of the button.
     *
     * @param PPIN Pin of the button.
     */
    Button(uint8_t PPIN)                                      
    {
      pin = PPIN;
      val_ant = 1;
      orden = 0;
    }

    /** Button = operator
     * 
     * @brief This function is the = operator of the button.
     *
     * @param B Button.
     * @return Button.
     */
    Button& operator =(const Button& B)                             
    {
      if (this != &B)
      {
        this -> pin = B.pin;
        this -> val_act = B.val_act;
        this -> val_ant = B.val_ant;
        this -> cambioact = B.cambioact;
        this -> cambioanterior = B.cambioanterior;
        this -> orden = B.orden;
      }
      return(*this);
    } 
};

/** Display class
 * 
 * @brief This class is the display of the device.
 *
 */
class Display : public U8G2_ST7565_ERC12864_1_4W_SW_SPI {
    public:
        uint32_t xAxisBegin, xAxisEnd, yAxisBegin, yAxisEnd, halfHeight;
        uint32_t margin = 8;

        /** Display constructor
         * 
         * @brief This function is the constructor of the display.
         *
         * @param rotation Rotation of the display.
         * @param clockPin Clock pin of the display.
         * @param dataPin Data pin of the display.
         * @param csPin Chip select pin of the display.
         * @param dcPin Data/Command pin of the display.
         * @param resetPin Reset pin of the display.
         */
        using U8G2_ST7565_ERC12864_1_4W_SW_SPI::U8G2_ST7565_ERC12864_1_4W_SW_SPI;    

        /** Init function
         * 
         * @brief This function initializes the display.
         *
         */
        void init()
        {
            // Display initialization
            this -> begin();                         // Inicialitzate
            this -> setContrast (10);                // Contraste
            this -> enableUTF8Print();               // Visualize UTF-8 characters
            this -> setFont(u8g2_font_6x10_tf);      // Font

            //Axis
            uint8_t height = this -> getDisplayHeight(); // Get display height : 64
            uint8_t width = this -> getDisplayWidth(); // Get display width : 128
            
            this -> xAxisBegin      = margin/4;
            this -> xAxisEnd        = width  - width/2;
            this -> yAxisBegin      = margin/4;
            this -> yAxisEnd        = height - margin/2;
            this -> halfHeight      = height/2;
        }

        /** Draw axis in the display function
         * 
         * @brief This function draws the axis in the display.
         *
         */
        void drawAxis()
        {
            this -> drawLine(xAxisBegin, yAxisBegin, xAxisBegin, yAxisEnd);                // Y-axis
            this -> drawLine(xAxisBegin, yAxisEnd, xAxisEnd, yAxisEnd);                    // X-axis
        }

        /** Draw long axis in the display function
         * 
         * @brief This function draws the long axis in the display.
         *
         */
        void drawLongAxis()
        {
            this -> drawLine(xAxisBegin, yAxisBegin, xAxisBegin, yAxisEnd);                // Y-axis
            this -> drawLine(xAxisBegin, yAxisEnd, xAxisEnd + margin, yAxisEnd);                  // X-axis
        }
        
        /** Print measurements function
         * 
         * @brief This function prints the measurements in the display.
         *
         * @param value Value to print.
         * @param choiceBPM Choice of the measurement to print.
         */
        void printMeasurements(int32_t value, bool choiceBPM)
        {
            this -> setFont(u8g2_font_luBS10_tf);
            String valueString = String(value);

            if (choiceBPM){
                this -> setCursor(this -> xAxisEnd + 20, this -> halfHeight + 2*margin);
                this -> print("BPM");
            }else {
                valueString += " %";
                this -> setCursor(this -> xAxisEnd + 20, this -> halfHeight + 2*margin);
                this -> print("SPO2");
            }

            if (valueString.length() >= 3){
                this -> setCursor(this -> xAxisEnd + 20, this -> halfHeight );
            }else {
                this -> setCursor(this -> xAxisEnd + 20 + margin, this -> halfHeight);
            }
            this -> print(valueString);
        }
        
        /** Get max value function
         * 
         * @brief This function gets the max value.
         *
         * @param dataVector Data to get the max value.
         * @return Max value.
         */
        uint32_t getMaxValue(uint32_t* dataVector)
        {
            uint32_t max = 0;
            for (uint8_t i = 0; i < xAxisEnd - xAxisBegin; i++)
            {
                if (dataVector[i] > max)
                {
                    max = dataVector[i];
                }
            }
            return max;
        }

        /** Discretize data function
         * 
         * @brief This function discretizes the data.
         *
         * @param dataVector Data to discretize.
         * @return Discretized data.
         */
        uint32_t* discretizeData(uint32_t* dataVector, bool choiceBPM)
        {
            uint32_t* discretizedDataVector = new uint32_t[xAxisEnd - xAxisBegin];
            uint32_t max = getMaxValue(dataVector);
            uint32_t yAxisScale = 1;
            if (choiceBPM){
                yAxisScale = uint32_t(max/(halfHeight - margin));
            } else {
                yAxisScale = uint32_t(max/(yAxisEnd - margin)); 
            } 

            if (yAxisScale == 0)yAxisScale = 1;

            for (uint32_t i = 0; i < xAxisEnd - xAxisBegin; i++)
            {
                discretizedDataVector[i] = uint32_t(dataVector[i]/yAxisScale);
            }
            return discretizedDataVector;
        }

        /** Draw data function
         * 
         * @brief This function draws the data in the display.
         *
         * @param dataVector Data to draw.
         * @param choiceBPM Choice of the data to draw.
         */
        void drawData(uint32_t* dataVector, bool choiceBPM)
        {
            uint32_t lastHeight = 0;
            if (choiceBPM)
            {
                for (uint32_t i = 0; i < xAxisEnd - xAxisBegin; i++)
                {
                    this -> drawLine(this -> xAxisBegin + i, lastHeight, this -> xAxisBegin + i, halfHeight - dataVector[i]);
                    lastHeight = halfHeight - dataVector[i];
                }   
            }
            else
            {
                for (uint32_t i = 0; i < xAxisEnd - xAxisBegin; i++)
                {
                    this -> drawLine(xAxisBegin + i, lastHeight, xAxisBegin + i, yAxisEnd - margin - dataVector[i]);
                    lastHeight = yAxisEnd - margin - dataVector[i];
                }
            }
        }
        
        /** Update data function
         * 
         * @brief This function updates the data in the display.
         *
         * @param array Array of data to update.
         * @param value Value to update.
         * @param choiceBPM Choice of the data to update.
         */
        void updateData(uint32_t* array, int32_t value, bool choiceBPM)
        {
            this -> drawAxis();
            uint32_t* discretizedArray = this -> discretizeData(array, choiceBPM);
            this -> drawData(discretizedArray, choiceBPM);
            this -> printMeasurements(value, choiceBPM);
        }
        
        /** Get max amplitude function
         * 
         * @brief This function gets the max amplitude.
         *
         * @param freqs Vector of frequencies.
         * @return Max amplitude.
         */
        int getMaxAmplitude(const vector<fundamentalsFreqs>& freqs)
        {
            int max = 0;
            for (uint8_t i = 0; i < freqs.size(); i++)
            {
                if (freqs[i].amplitude > max)
                {
                    max = freqs[i].amplitude;
                }
            }
            return max;
        }

        /** Draw frequencies function
         * 
         * @brief This function draws the frequencies in the display.
         *
         * @param freqs Vector of frequencies to draw.
         */
        void drawFreqs(const vector<fundamentalsFreqs>& freqs)
        {
            int max = getMaxAmplitude(freqs);
            uint32_t yAxisScale = max/yAxisEnd;
            uint32_t xAxisScale = (xAxisEnd + margin/2)/freqs.size();
            uint32_t yAxisStep = yAxisEnd/freqs.size();
            this -> setFont(u8g2_font_tinyunicode_tf);

            for (uint8_t i = 0; i < freqs.size() ; i++)
            {
                // Plot amplitude of each freq
                uint32_t xAxisPlotBegin = xAxisScale*i + xAxisScale/2;
                uint32_t xWidth = int(xAxisPlotBegin + xAxisScale/5);
                uint32_t totalPixelValues = int(freqs[i].amplitude/yAxisScale);

                for (uint32_t j = 0; j < totalPixelValues; j++)
                {
                    this -> drawLine(xAxisPlotBegin, yAxisEnd -j, xWidth ,yAxisEnd -j);
                }

                // Plot freq in Hz
                this -> setCursor(xAxisEnd + 2*margin, yAxisStep * i + 4/3*margin);
                this -> print(freqs[i].freqsHz);
            }
        }

        /** Update frequencies function
         * 
         * @brief This function updates the frequencies in the display.
         *
         * @param freqs Vector of frequencies to update.
         */
        void updateFreqs(const vector<fundamentalsFreqs>& freqs)
        {
            this -> drawLongAxis();
            this -> drawFreqs(freqs);
        }
};

// GLOBAL VARIABLES
// Global values
globalValues globalValuesVar;
vector<fundamentalsFreqs> freqs;
// U8g2
Display display(U8G2_R0, SCL, SI, CS, RS, RSE);
// Buttons
Button* buttons;                            
hw_timer_t * timer = NULL;                  
// Web variables
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncWebSocketClient * globalClient = NULL;
String message = "";

// Button functions declaration
void initButtons();
void IRAM_ATTR buttonManagement(); 

// SPIFFS functions declaration
void initSPIFFS();
void readfile();
// Web functions declaration
void initWeb();
void initWiFi();
void initServer();
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);
void sendWsMessage(String message); 

// Data functions declaration
void iniMAX30102();
void fft();
void data(void *pvParameters);

// Visualize data functions declaration
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

    // Web initialization
    initWeb();

    // SPIFFS initialization
    initSPIFFS();

    // Data initialization
    readfile();

    //MAX30102 initialization
    iniMAX30102();

    xTaskCreatePinnedToCore(
        data,   /* Task function. */
        "Data task",     /* name of task. */
        10000,       /* Stack size of task */
        NULL,        /* parameter of the task */
        1,           /* priority of the task */
        NULL,      /* Task handle to keep track of created task */
        0);          /* pin task to core 0 */

    // Create task for visualizing data
    xTaskCreatePinnedToCore(
                    visualizeData,   /* Task function. */
                    "visualizeData", /* name of task. */
                    10000,        /* Stack size of task */
                    NULL,         /* parameter of the task */
                    1,            /* priority of the task */
                    NULL,         /* Task handle to keep track of created task */
                    0);           /* pin task to core 0 */
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

        sendWsMessage(globalValuesVar.getJson((display.xAxisEnd - display.xAxisBegin), (display.xAxisEnd - display.xAxisBegin)));

        delay(700); //Wait 1000ms
    }
}

// VISUALIZE DATA FUNCTIONS

// Button functions
/** Init buttons function
 * 
 * @brief This function initializes the buttons.
 *  
 * @return void.
 *  
 * @details This function initializes the buttons' pins, the buttons' definition and the timer.
 *  
 * @note This function is called once.
 * 
 * @see setup().
 * 
 */
void initButtons()
{
    //Buttons pins
    uint8_t *button_pin_temp = new uint8_t [BUTTON_NUMBER];
    button_pin_temp[0] = BPM_PIN;                                // Heart rate button
    button_pin_temp[1] = SPO2_PIN;                               // SPO2 button
    button_pin_temp[2] = FUNDAMENTALS_PIN;                       // Freqs button

    button_pin = button_pin_temp;

    //Buttons definition
    Button *buttons_temp = new Button[BUTTON_NUMBER];
    for(uint8_t i = 0; i < BUTTON_NUMBER; i++)
    {
        buttons_temp[i] = Button(button_pin[i]);
    }
    buttons = buttons_temp;


    //Buttons'pins initialization
    for(uint8_t i = 0; i < BUTTON_NUMBER; i++)
    {
        pinMode(button_pin[i], INPUT_PULLUP);
    }

    // Timer initialization
    timer = timerBegin(0, 80, true);                            //Initiation of timer
    timerAttachInterrupt(timer, &buttonManagement, true);       //Relate function with timer
    timerAlarmWrite(timer, 50000, true);                        //Specify time betweem interrupts
    timerAlarmEnable(timer);                                    //Enable timer

    // Default order
    buttons[0].orden = 1;
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

// Web functions
/** Web initialization function
 * 
 * @brief This function initializes the web.
 *  
 * @return void.
 *  
 * @details This function initializes the SPIFFS, the WiFi and the server.
 *  
 * @note This function is called once.
 * 
 * @see initSPIFFS(), initWiFi(), initServer().
 * 
 */
void initWeb()
{
  initWiFi();
  initServer();
}

/** WiFi initialization function
 * 
 * @brief This function initializes the WiFi.
 *  
 * @return void.
 *  
 * @details This function initializes the WiFi.
 *  
 * @note This function is called once.
 * 
 * @see initWeb().
 * 
 */
void initWiFi()
{
  WiFi.begin(ssid, password);
 
  Serial.print("Connecting to WiFi..");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("IP: ");
  Serial.print(WiFi.localIP());
  Serial.println("");
}

/** Server initialization function
 * 
 * @brief This function initializes the server.
 *  
 * @return void.
 *  
 * @details This function initializes the server.
 *  
 * @note This function is called once.
 * 
 * @see initWeb().
 * 
 */
void initServer()
{
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
 
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });
 
  server.begin();
}

/** Websocket event function
 * 
 * @brief This function manages the websocket events.
 *  
 * @return void.
 *  
 * @details This function manages the websocket events.
 *  
 * @note This function is called when a websocket event is generated.
 * 
 * @see initServer().
 * 
 */
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len)
{
  if(type == WS_EVT_CONNECT)
  {
    Serial.println("Websocket client connection received");
    globalClient = client;
  }
  else if(type == WS_EVT_DISCONNECT)
  {
    Serial.println("Websocket client connection finished");
    globalClient = NULL;
  }
}

/** Send websocket message function
 * 
 * @brief This function sends a websocket message.
 *  
 * @return void.
 *  
 * @details This function sends a websocket message.
 *  
 * @note This function is called when a websocket message is generated.
 * 
 * @see loop().
 * 
 */
void sendWsMessage(String message)
{
  if(globalClient != NULL && globalClient->status() == WS_CONNECTED)
    {
        globalClient -> text(message); // '{"tiempo":X, "amplitud":Y, "spo2":Z}'
        message = "";
    }
}

// DATA FUNCTIONS

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
            j-=3;
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

/** MAX30102 initialization function
 * 
 * @brief This initializes MAX30102.
 *  
 * @return void.
 *  
 * @details This initializes MAX30102.
 *  
 * @note This function is called in setup.
 * 
 * @see setup().
 * 
 */
void iniMAX30102()
{
    // Initialize sensor
    if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
    {
        Serial.println("MAX30105 was not found. Please check wiring/power. ");
        while (1);
    }
    //Setup to sense a nice looking saw tooth on the plotter
    byte ledBrightness = 0x1F; //Options: 0=Off to 255=50mA
    byte sampleAverage = 4; //Options: 1, 2, 4, 8, 16, 32
    byte ledMode = 3; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
    int sampleRate = 3200; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
    int pulseWidth = 411; //Options: 69, 118, 215, 411
    int adcRange = 4096; //Options: 2048, 4096, 8192, 16384

    particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); //Configure sensor with these settings
}

/** Read file function
 * 
 * @brief This function reads the file.
 *  
 * @return void.
 *  
 * @details This function reads the file.
 *  
 * @note This function is called in setup.
 * 
 * @see setup().
 * 
 */
void readfile()
{
  int i =0;
    if(!SPIFFS.begin(true))
    {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }

    File file = SPIFFS.open("/coeficients2.txt");
    if(!file)
    {
        Serial.println("Failed to open file for reading");
        return;
    }
  
    while(i<201)
    {
        delay(50);
        if(i<201)
        {
           String x=file.readStringUntil('\n'); 
            string s=x.c_str();
            vcoefs1[i]=stof(s);
        }
        i++;
    }
    i=0;
    file.close(); 
}

/** Data function
 * 
 * @brief This function gets the data.
 *  
 * @return void.
 *  
 * @details This function gets the data.
 *  
 * @note This function is called in setup.
 * 
 * @see setup().
 * 
 */
void data(void *pvParameters)
{
  vector<float> input_data;
  vector<float> input_data2;
  int i=0;
  int n=0;
  int32_t bufferLength; //data length
  int32_t spo2; //SPO2 value
  int8_t validSPO2; //indicator to show if the SPO2 calculation is valid
  int32_t heartRate; //heart rate value
  int8_t validHeartRate; //indicator to show if the heart rate calculation is valid
    for(;;)
    {
      float y = 0.0;
      float y2 = 0.0;
        //lastTime1 = millis();
        float value = particleSensor.getIR(); // llegim el valor IR Serial.println(millis() - lastSampleTime); // envia el temps des de l'última mostra a la consola sèrie
        float value2 = particleSensor.getRed();
        //lastTime2 = millis();
        //Serial.print("Temps de mostreig: ");
        //Serial.println(lastTime2-lastTime1);

        input_data.push_back(value); // afegim el valor a la llista d'entrada
        input_data2.push_back(value2);

        if (input_data.size() >= 201) // si tenim suficients mostres per aplicar el filtre
        {
            y=0;
            y2=0;

            for (int n = 200; n >= 0; n--) 
            {
                y += vcoefs1[n] * input_data[input_data.size()-1-n];
                y2 += vcoefs1[n] * input_data2[input_data2.size()-1-n];
            }

            input_data.erase(input_data.begin()); // eliminem la mostra més antiga de la llista
            input_data2.erase(input_data2.begin());

            if (i>=200)
            {
                i=0;
                
                maxim_heart_rate_and_oxygen_saturation(irBuffer, 200, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
                if(validHeartRate && validSPO2){
                globalValuesVar.setBeatsPerMinute(heartRate);
                globalValuesVar.setSpo2Percentage(spo2);}
                else {
                  globalValuesVar.setSpo2Percentage(96);}
                globalValuesVar.setHeartRateDataArray(irBuffer);
                globalValuesVar.setSpo2DataArray(redBuffer);
                Serial.print("Heart rate: ");
                Serial.print(heartRate);
                Serial.print(" bpm / SpO2: ");
                Serial.print(spo2);
                Serial.println(" %"); 
                n++;
            }

            irBuffer[i]=y;
            redBuffer[i]=y2;    

            i++;

            //Serial.println(y); // enviem el resultat a la consola sèrie

        
        }
    }
}

/** FFT function
 * 
 * @brief This function does the FFT.
 *  
 * @return void.
 *  
 * @details This function does the FFT.
 *  
 * @note This function is called in setup.
 * 
 * @see setup().
 * 
 */
void fft()
{
  arduinoFFT FFT = arduinoFFT();
  double vReal[SAMPLES];
  double vImag[SAMPLES];
    for (int i = 0; i < SAMPLES; i++)
    {
        vReal[i] = irBuffer[i];
        vImag[i] = 0;
    }

    //FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING);
    Serial.println("Computing FFT. Please wait...");
    FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
    vReal[0] = 0; // Remove the DC component

    //Normalitzem
    float max=0;
    for(int i=0;i<SAMPLES;i++)
    {
        if(vReal[i]>max)
        {
            max=vReal[i];
        }
    }
    for(int i=0;i<SAMPLES;i++)
    {
        vReal[i]=vReal[i]/max;
    }

    //Print the results of the FFT calculation to the Arduino Serial monitor
    Serial.println("FFT results:");
    for (int i = 0; i < SAMPLES / 2; i++)
    {
        float frequency = float(i) * SAMPLING_FREQUENCY / SAMPLES;
        float magnitude = vReal[i];
        fundamentalsFreqs x;
        x.amplitude=magnitude;
        x.freqsHz=frequency;
        freqs.push_back(x);
        Serial.print("Frequency: ");
        Serial.print(frequency);
        Serial.print(" Hz, Magnitude: ");
        Serial.println(magnitude);
    }
    globalValuesVar.setFreqs(freqs);               
}