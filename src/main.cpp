#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>
#include <vector>
#include "WiFi.h"
#include "SPIFFS.h"
#include "ESPAsyncWebServer.h"

#include "DataVisualizer.h"
#include "GlobalValues.h"

#include "MAX30105.h"
#include "heartRate.h"
#include "spo2_algorithm.h"
#include "arduinoFFT.h"

using namespace std;

// MAX30105 VARIABLES
#define I2C_SPEED_FAST 400000 // Set I2C frequency to 400kHz
#define MAX_BRIGHTNESS 255 // Set maximum brightness

// FILTER and FFT variables
#define SAMPLES 64 // Número de muestras para la FFT
#define SAMPLING_FREQUENCY 25 // Frecuencia de muestreo en Hz

// DISPLAY PINS
#define SCL 18
#define SI 23
#define CS 5
#define RS 32
#define RSE 33

// CONSTANTS and VARIABLES
// Variables for data visualization
const int BUTTONS_NUMBER = 3;
const int BPM_PIN = 26;
const int SPO2_PIN = 25;
const int FUNDAMENTALS_PIN = 27;
const int enoughSamples = 200;
const char* ssid = "iPhone de JJ";      // SSID of the WiFi
const char* password = "onayago1"; // Password of the WiFi
globalValues globalValuesVar;
globalDataVisualizer dataVisualizer( U8G2_R0, SCL, SI, CS, RS, RSE );
hw_timer_t* timer = NULL;

// Variables for data processing
MAX30105 particleSensor;
float vcoefs1[(enoughSamples + 1)];
vector<fundamentalsFreqs> freqs;
uint32_t irBuffer[enoughSamples];     //infrared LED sensor data
uint32_t redBuffer[enoughSamples];    //red LED sensor data

// FUNCTIONS DECLARATION
void readData(void * parameter);
void initGlobalVisualizer();
void visualizeData(void * parameter);
void IRAM_ATTR readButtonsWrapper();
void initSPIFFS();
void readFile();
void initMAX30102();
void fft();
void fillDataTests();

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

    // SPIFFS initialization
    initSPIFFS();

    // Visualizer init
    initGlobalVisualizer();
    
    // Data tests initialization
    fillDataTests();

    // Data initialization
    // readFile();

    //MAX30102 initialization
    // initMAX30102();
    
    // Create tasks 
    // xTaskCreatePinnedToCore(
    //                 readData,    /* Task function. */
    //                 "readData",  /* name of task. */
    //                 10000,       /* Stack size of task */
    //                 NULL,        /* parameter of the task */
    //                 1,           /* priority of the task */
    //                 NULL,        /* Task handle to keep track of created task */
    //                 0);          /* pin task to core 0 */
    xTaskCreatePinnedToCore(
                    visualizeData,   /* Task function. */
                    "visualizeData", /* name of task. */
                    100000,        /* Stack size of task */
                    NULL,         /* parameter of the task */
                    1,            /* priority of the task */
                    NULL,         /* Task handle to keep track of created task */
                    1);           /* pin task to core 1 */
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
void visualizeData ( void * parameter )
{
    for(;;)dataVisualizer.generateVisualization(globalValuesVar);
}

/** Global visualizer initialization function
 * 
 * @brief This function initializes the global visualizer.
 *  
 * @return void.
 *  
 * @details This function initializes the global visualizer.
 *  
 * @note This function is called once.
 * 
 * @see setup().
 * 
 */
void initGlobalVisualizer()
{
    // Definition of pins array
    vector<int> buttons_pins(BUTTONS_NUMBER);
    buttons_pins[0] = BPM_PIN; 
    buttons_pins[1] = SPO2_PIN;
    buttons_pins[2] = FUNDAMENTALS_PIN;
    
    // Data visualization initialization
    dataVisualizer.setup(buttons_pins, ssid, password);

    // Timer inizialization
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &readButtonsWrapper, true);
    timerAlarmWrite(timer, 100000, true);
    timerAlarmEnable(timer);
}

/** Read buttons wrapper function
 * 
 * @brief This function is the wrapper of the read buttons function.
 *  
 * @return void.
 *  
 * @details This function is the wrapper of the read buttons function.
 *  
 * @note This function is called when the timer is activated. 
 * 
 * @see initGlobalVisualizer(), buttonsArray::readButtons().
 * 
 */
void IRAM_ATTR readButtonsWrapper()
{
    dataVisualizer.buttons.readButtons();
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
void readData ( void * parameter )
{
    vector<float> input_data;
    vector<float> input_data2;
    int i=0;
    int n=0;
    int32_t bufferLength,   //data length
            spo2,           //SPO2 value
            heartRate;      //heart rate value
    int8_t  validSPO2,      //indicator to show if the SPO2 calculation is valid
            validHeartRate; //indicator to show if the heart rate calculation is valid
    for(;;)
    {
      float y = 0.0;
      float y2 = 0.0;
        //lastTime1 = millis();
        
        // read the value from the sensor
        float value = particleSensor.getIR();
        //Serial.println(millis() - lastSampleTime); // envia el temps des de l'última mostra a la consola sèrie
        float value2 = particleSensor.getRed();
        
        //lastTime2 = millis();
        //Serial.print("Temps de mostreig: ");
        //Serial.println(lastTime2-lastTime1);

        // add the new sample to the input data vector
        input_data.push_back(value); 
        input_data2.push_back(value2);

        // if we have enough samples to apply the filter
        if (input_data.size() > enoughSamples/*>= 201*/)
        {
            y=0; y2=0;
            for (int n = enoughSamples/*200*/; n >= 0; n--) 
            {
                y += vcoefs1[n] * input_data[input_data.size()-1-n];
                y2 += vcoefs1[n] * input_data2[input_data2.size()-1-n];
            }

            // delete the oldest sample from the list
            input_data.erase(input_data.begin()); 
            input_data2.erase(input_data2.begin());

            if (i>=enoughSamples/*200*/)
            {
                i=0;
                
                maxim_heart_rate_and_oxygen_saturation(irBuffer, enoughSamples /*200*/, redBuffer, 
                                                       &spo2, &validSPO2, &heartRate, &validHeartRate);
                if ( validHeartRate && validSPO2 ) {
                    globalValuesVar.setBeatsPerMinute(heartRate);
                    globalValuesVar.setSpo2Percentage(spo2);
                } else {
                    globalValuesVar.setSpo2Percentage(96);
                }

                globalValuesVar.setHeartRateDataArray(irBuffer, enoughSamples /*200*/);
                globalValuesVar.setSpo2DataArray(redBuffer, enoughSamples /*200*/);
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
void initSPIFFS ()
{
  if(!SPIFFS.begin()){
     Serial.println("An Error has occurred while mounting SPIFFS");
     for(;;);
  }
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
void initMAX30102 ()
{
    // Initialize sensor
    if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
    {
        Serial.println("MAX30105 was not found. Please check wiring/power. ");
        while (1);
    }

    //Setup to sense a nice looking saw tooth on the plotter
    byte ledBrightness = 0x1F;  //Options: 0=Off to 255=50mA
    byte sampleAverage = 4;     //Options: 1, 2, 4, 8, 16, 32
    byte ledMode = 3;           //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
    int sampleRate = 3200;      //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
    int pulseWidth = 411;       //Options: 69, 118, 215, 411
    int adcRange = 4096;        //Options: 2048, 4096, 8192, 16384

    particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); 
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
void readFile()
{
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

    for (int i = 0; i <= enoughSamples; i++)
    {
        string stringCoef = file.readStringUntil('\n').c_str();
        vcoefs1[i] = stof(stringCoef);
        delay(50);
    }
    file.close(); 
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
    Serial.println("Computing FFT...");
    FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
    vReal[0] = 0; // Remove the DC component

    // print results
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
void fillDataTests ()
{  
    uint32_t margin = 8;
    uint32_t xAxisEnd = 128-128/2;
    uint32_t xAxisBegin  = margin/4;
    uint32_t size = xAxisEnd - xAxisBegin;
    uint32_t* heartRateData_temp = new uint32_t[size];
    uint32_t* spo2Data_temp = new uint32_t[size];
    uint32_t j = 0;
    for (uint32_t i = 0; i < size; i++)
    {
        // Little mountain in the middle with a value of yAxisEnd
        if (i < xAxisEnd/4)
        {
            spo2Data_temp[i] = 0;
        }
        else if (i < xAxisEnd/2)
        {
            spo2Data_temp[i] =  j;
            j+=3;
        }
        else if (i < 3*xAxisEnd/4)
        {
            j-=3;
            spo2Data_temp[i] = j;
        }
        else
        {
            spo2Data_temp[i] = 0;
        }
    }

    for (uint32_t i = 0; i < size; i++)
    {
        // Little mountain in the middle with a value of yAxisEnd
        if (i < xAxisEnd/4)
        {
            heartRateData_temp[i] = 0;
        }
        else if (i < 3*xAxisEnd/8)
        {
            heartRateData_temp[i] =  j;
            j+=3;
        }
        else if (i < 5*xAxisEnd/8)
        {
            if(j >= 3)j-=3;
            heartRateData_temp[i] = j;
        }
        else if (i < 3*xAxisEnd/4)
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


    globalValuesVar.setBeatsPerMinute(beatsPerMinute_temp[0]);
    globalValuesVar.setSpo2Percentage(spo2Percentage_temp[0]); 
    globalValuesVar.setFreqs(freqs_temp);
    globalValuesVar.setHeartRateDataArray(heartRateData_temp, size);
    globalValuesVar.setSpo2DataArray(spo2Data_temp, size);
}
