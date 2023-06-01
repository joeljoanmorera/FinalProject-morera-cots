#include "SPIFFS.h"
#include "DataVisualizer.h"
#include "DataReader.h"
#include "GlobalValues.h"

using namespace std;

// MAX30105 VARIABLES
#define I2C_SPEED_FAST 400000 // Set I2C frequency to 400kHz
#define MAX_BRIGHTNESS 255    // Set maximum brightness

// FILTER and FFT variables
#define SAMPLES 64            // Número de muestras para la FFT
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
const char *ssid = "iPhone de JJ"; // SSID of the WiFi
const char *password = "onayago1"; // Password of the WiFi
globalValues dataStorage;
globalDataVisualizer dataVisualizer(U8G2_R0, SCL, SI, CS, RS, RSE);
globalDataReader dataReader;
hw_timer_t *timer = NULL;

// FUNCTIONS DECLARATION
void initSPIFFS();
void initGlobalVisualizer();
void IRAM_ATTR readButtonsWrapper();
void readData(void *parameter);
void visualizeData(void *parameter);
void fillDataTests(void *parameter);

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

    // Data reader initialization
    dataReader.setup();

    // Create task for reading
    xTaskCreatePinnedToCore(
        readData,   /* Task function. */
        "readData", /* name of task. */
        10000,      /* Stack size of task */
        NULL,       /* parameter of the task */
        1,          /* priority of the task */
        NULL,       /* Task handle to keep track of created task */
        0);         /* pin task to core 0 */

    // Create task for visualization
    xTaskCreatePinnedToCore(
        visualizeData,   /* Task function. */
        "visualizeData", /* name of task. */
        10000,           /* Stack size of task */
        NULL,            /* parameter of the task */
        1,               /* priority of the task */
        NULL,            /* Task handle to keep track of created task */
        1);              /* pin task to core 1 */

    // Create task for data tests
    // xTaskCreatePinnedToCore(
    //                 fillDataTests,   /* Task function. */
    //                 "fillDataTests", /* name of task. */
    //                 10000,        /* Stack size of task */
    //                 NULL,         /* parameter of the task */
    //                 1,            /* priority of the task */
    //                 NULL,         /* Task handle to keep track of created task */
    //                 0);           /* pin task to core 1 */
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
void loop() {}

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
    if (!SPIFFS.begin())
    {
        Serial.println("An Error has occurred while mounting SPIFFS");
        for (;;);
    }
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
void visualizeData(void *parameter)
{
    while(!dataReader.isDataReady());
    for (;;)
    {
            dataVisualizer.generateVisualization(dataStorage);
    }
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
void readData(void *parameter)
{
    for (;;)
    {
        if (dataVisualizer.buttons[2].order)
            dataReader.fft(dataStorage, SAMPLING_FREQUENCY, SAMPLES);
        else
            dataReader.readData(dataStorage);
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
void fillDataTests(void *parameter)
{
    uint32_t margin = 8;
    uint32_t xAxisEnd = 128 - 128 / 2;
    uint32_t xAxisBegin = margin / 4;
    uint32_t size = xAxisEnd - xAxisBegin;
    uint32_t *heartRateData_temp = new uint32_t[size];
    uint32_t *spo2Data_temp = new uint32_t[size];
    uint32_t j = 0;
    for (uint32_t i = 0; i < size; i++)
    {
        // Little mountain in the middle with a value of yAxisEnd
        if (i < xAxisEnd / 4)
        {
            spo2Data_temp[i] = 0;
        }
        else if (i < xAxisEnd / 2)
        {
            spo2Data_temp[i] = j;
            j += 3;
        }
        else if (i < 3 * xAxisEnd / 4)
        {
            j -= 3;
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
        if (i < xAxisEnd / 4)
        {
            heartRateData_temp[i] = 0;
        }
        else if (i < 3 * xAxisEnd / 8)
        {
            heartRateData_temp[i] = j;
            j += 3;
        }
        else if (i < 5 * xAxisEnd / 8)
        {
            if (j >= 3)
                j -= 3;
            heartRateData_temp[i] = j;
        }
        else if (i < 3 * xAxisEnd / 4)
        {
            j += 3;
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

    dataStorage.setBeatsPerMinute(beatsPerMinute_temp[0]);
    dataStorage.setSpo2Percentage(spo2Percentage_temp[0]);
    dataStorage.setFreqs(freqs_temp);
    dataStorage.pushBackHeartRateDataArray(heartRateData_temp, size);
    dataStorage.pushBackSpo2DataArray(spo2Data_temp, size);
    for (;;)
    {
        dataStorage.setBeatsPerMinute(beatsPerMinute_temp[0]);
        dataStorage.setSpo2Percentage(spo2Percentage_temp[0]);
        dataStorage.setFreqs(freqs_temp);
        dataStorage.pushBackHeartRateDataArray(heartRateData_temp, size);
        dataStorage.pushBackSpo2DataArray(spo2Data_temp, size);
        delay(7500);
    }
}
