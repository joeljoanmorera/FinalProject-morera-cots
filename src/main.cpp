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
const char *ssid = "*****"; // SSID of the WiFi
const char *password = "*****"; // Password of the WiFi
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
    Serial.println(" Calculating... ");
    while(!dataReader.isDataReady())
    {
        dataVisualizer.workInProgressMessage();
    }
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
        dataReader.readData(dataStorage, SAMPLES, SAMPLING_FREQUENCY);
    }
}
