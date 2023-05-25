 #include <U8g2lib.h>
#include <SPI.h>
#include "WiFi.h"
#include "SPIFFS.h"
#include "ESPAsyncWebServer.h"
#include <vector>

using namespace std;

// Pin's definition
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
//Server vars.
const char* ssid = "*****";
const char* password =  "*****";

// CLASSES AND STRUCTS
/** Fundamentals frequencies struct
 * 
 * @brief This struct is the fundamentals frequencies of the device.
 *
 */
struct fundamentalsFreqs{
    String freqsHz;
    int amplitude;
};

/** Global values class
 * 
 * @brief This class is the global values of the device.
 *
 */
class globalValues {
    int32_t* heartRateDataArray;
    int32_t* spo2DataArray;
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
        globalValues(int32_t* heartRateDataArray, int32_t* spo2DataArray, int32_t beatsPerMinute, int32_t spo2Percentage, vector<fundamentalsFreqs> freqs)
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
        void setHeartRateDataArray(int32_t* heartRateDataArray)
        {
            this -> heartRateDataArray = heartRateDataArray;
        }

        /** Set SPO2 data array function
         * 
         * @brief This function sets the SPO2 data array.
         * 
         * @param spo2DataArray SPO2 data array.
         */
        void setSpo2DataArray(int32_t* spo2DataArray)
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
        int32_t* getHeartRateDataArray()
        {
            return heartRateDataArray;
        }

        /** Get SPO2 data array function
         * 
         * @brief This function gets the SPO2 data array.
         * 
         * @return SPO2 data array.
         */
        int32_t* getSpo2DataArray()
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
        String getJson()
        {
            String json = "{";
            json += "\"heartRateDataArray\": [";
            for(int i = 0; i < 100; i++)
            {
                json += String(heartRateDataArray[i]);
                if(i != 99)
                {
                    json += ",";
                }
            }
            json += "],";
            json += "\"spo2DataArray\": [";
            for(int i = 0; i < 100; i++)
            {
                json += String(spo2DataArray[i]);
                if(i != 99)
                {
                    json += ",";
                }
            }
            json += "],";
            json += "\"beatsPerMinute\": " + String(beatsPerMinute) + ",";
            json += "\"spo2Percentage\": " + String(spo2Percentage) + ",";
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
        int8_t xAxisBegin, xAxisEnd, yAxisBegin, yAxisEnd, halfHeight;
        int8_t margin = 8;

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
        int32_t getMaxValue(int32_t* dataVector)
        {
            int max = 0;
            for (uint8_t i = 0; i < xAxisEnd - xAxisBegin; i++)
            {
                if (abs(dataVector[i]) > max)
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
        int32_t* discretizeData(int32_t* dataVector, bool choiceBPM)
        {
            int32_t* discretizedDataVector = new int32_t[xAxisEnd - xAxisBegin];
            int32_t max = getMaxValue(dataVector);
            int32_t yAxisScale;
            if (choiceBPM){
                yAxisScale =int(max/(halfHeight - margin));
            } else {
                yAxisScale = int(max/(yAxisEnd - margin)); 
            } 

            if (yAxisScale == 0)yAxisScale = 1;

            for (uint8_t i = 0; i < xAxisEnd - xAxisBegin; i++)
            {
                discretizedDataVector[i] = int(dataVector[i]/yAxisScale);
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
        void drawData(int32_t* dataVector, bool choiceBPM)
        {
            int8_t lastHeight = 0;
            if (choiceBPM)
            {
                for (uint8_t i = 0; i < xAxisEnd - xAxisBegin; i++)
                {
                    this -> drawLine(this -> xAxisBegin + i, lastHeight, this -> xAxisBegin + i, halfHeight - dataVector[i]);
                    lastHeight = halfHeight - dataVector[i];
                }   
            }
            else
            {
                for (uint8_t i = 0; i < xAxisEnd - xAxisBegin; i++)
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
        void updateData(int32_t* array, int32_t value, bool choiceBPM)
        {
            this -> drawAxis();
            int32_t* discretizedArray = this -> discretizeData(array, choiceBPM);
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
            int yAxisScale = max/yAxisEnd;
            int xAxisScale = (xAxisEnd + margin/2)/freqs.size();
            int yAxisStep = yAxisEnd/freqs.size();
            this -> setFont(u8g2_font_tinyunicode_tf);

            for (uint8_t i = 0; i < freqs.size() ; i++)
            {
                // Plot amplitude of each freq
                int xAxisPlotBegin = xAxisScale*i + xAxisScale/2;
                int xWidth = int(xAxisPlotBegin + xAxisScale/5);
                int8_t totalPixelValues = int(freqs[i].amplitude/yAxisScale);

                for (int8_t j = 0; j < totalPixelValues; j++)
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

// Web functions declaration
void initWeb();
void initWiFi();
void initServer();
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);
void sendWsMessage(String message);

// Data for tests functions declaration
void fillDataTests();
void getNewData();  

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

    // Data initialization
    fillDataTests();
}

/** Loop function
 * 
 * @brief This function is the loop of the program.
 *  
 * @return void.
 *  
 * @details This function updates the data in the display.
 *  
 * @note This function is called continuously.
 * 
 * @see setup().
 * 
 */
void loop() 
{
    display.firstPage(); // First page of the display
    do {
        if (buttons[0].orden){
            display.updateData(globalValuesVar.getHeartRateDataArray(), globalValuesVar.getBeatsPerMinute(), true);
            Serial.println("Heart rate");
        }
        if(buttons[1].orden){
            display.updateData(globalValuesVar.getSpo2DataArray(), globalValuesVar.getSpo2Percentage(), false);
            Serial.println("SPO2");
        }
        if(buttons[2].orden){
            display.updateFreqs(globalValuesVar.getFreqs());
            Serial.println("Freqs");
        }        
    } while(display.nextPage());

    sendWsMessage(globalValuesVar.getJson());
    
    delay(700); //Wait 1000ms
}

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
    uint8_t *button_pin = new uint8_t [BUTTON_NUMBER];
    button_pin[0] = BPM_PIN;                                // Heart rate button
    button_pin[1] = SPO2_PIN;                               // SPO2 button
    button_pin[2] = FUNDAMENTALS_PIN;                       // Freqs button

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

    // Default order
    buttons[0].orden = 1;

    // Timer initialization
    timer = timerBegin(0, 80, true);                            //Initiation of timer
    timerAttachInterrupt(timer, &buttonManagement, true);       //Relate function with timer
    timerAlarmWrite(timer, 50000, true);                        //Specify time betweem interrupts
    timerAlarmEnable(timer);                                    //Enable timer
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
  initSPIFFS();
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

// Data tests
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
    int32_t* heartRateData_temp = new int32_t[display.xAxisEnd - display.xAxisBegin];
    int32_t* spo2Data_temp = new int32_t[display.xAxisEnd - display.xAxisBegin];

    int j = 0;
    for (uint8_t i = 0; i < display.xAxisEnd; i++)
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

    for (uint8_t i = 0; i < display.xAxisEnd; i++)
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

    freqs_temp[0].freqsHz = "100 Hz";
    freqs_temp[0].amplitude = 640;
    freqs_temp[1].freqsHz = "500 Hz";
    freqs_temp[1].amplitude = 240;
    freqs_temp[2].freqsHz = "1 kHz";
    freqs_temp[2].amplitude = 440;
    freqs_temp[3].freqsHz = "10 kHz";
    freqs_temp[3].amplitude = 129;
    freqs_temp[4].freqsHz = "20 kHz";
    freqs_temp[4].amplitude = 100;
    freqs_temp[5].freqsHz = "30 kHz";
    freqs_temp[5].amplitude = 50;
    freqs_temp[6].freqsHz = "40 kHz";
    freqs_temp[6].amplitude = 220;

    globalValuesVar.setHeartRateDataArray(heartRateData_temp);
    globalValuesVar.setSpo2DataArray(spo2Data_temp);
    globalValuesVar.setBeatsPerMinute(beatsPerMinute_temp[0]);
    globalValuesVar.setSpo2Percentage(spo2Percentage_temp[0]);
    globalValuesVar.setFreqs(freqs_temp);
}