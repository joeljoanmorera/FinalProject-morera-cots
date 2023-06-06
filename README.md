# Proyecto final : Pulsioxímetro 

En el siguiente repositorio se describe brevemente el código de un pulsioxímetro implementado en un ESP32. Este proyecto se ha realizado para la asignatura de Procesadores Digitales (PD) de la Universitat Politécnica de Catalunya (UPC).

La presentación del proyecto se puede encontrar pinchando [aquí](https://docs.google.com/presentation/d/1E0ODB6eI-Wz4NGNXGugeu6bnHJXOKb55/edit?usp=sharing&ouid=104627258973508044844&rtpof=true&sd=true), ya que por limitaciones de espacio no sea podido añadir en el repositorio.
> **NOTA**: La explicación que se da a continuación consiste en un breve resumen del proyecto. Para más detalles se puede consultar directamente el código, el cual esta documentado mediante [Doxygen](https://www.doxygen.nl/index.html).

## **Funcionamiento**

El funcionamiento de este proyecto se basa en la lectura y filtrado de los datos del sensor *MAX30102*, la obtención y procesamiento de los datos de interes y su visualización mediante un *display* y una página web.

###### **Pipeline**

```mermaid
  stateDiagram-v2
    direction LR
    [*] --> Lectura
    Lectura --> Filtrado
    Filtrado --> Procesamiento
    Procesamiento --> Visualizacion
    Visualizacion --> Display
    Visualizacion --> WebPage
```

###### **Código principal**

```cpp
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

void loop() {}

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

void readData(void *parameter)
{
    for (;;)
    {
        dataReader.readData(dataStorage, SAMPLES, SAMPLING_FREQUENCY);
    }
}

void initSPIFFS()
{
    if (!SPIFFS.begin())
    {
        Serial.println("An Error has occurred while mounting SPIFFS");
        for (;;);
    }
}

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

void IRAM_ATTR readButtonsWrapper()
{
    dataVisualizer.buttons.readButtons();
}

```

###### **Diagrama de flujo**

```mermaid
    flowchart LR;
    S --> R & V
    subgraph S[Setup]
        SPIFFS[Inicializacion del File System del ESP32] --> SV
        SV --> RV
        RV --> T[Configuracion de tareas]

        subgraph SV [Configuracion de la variable global de visualizacion]
            B[Configuracion del WiFi] --> C[Configuracion del servidor web]
            C --> D[Configuracion del display]
            D --> E[Configuracion de los botones]
        end

        subgraph RV [Configuracion de la variable global de lectura]
            F[Lectura fichero coeficientes] --> G[Configuracion del sensor]
            G --> H[Configuracion del filtro]
        end
    end
    
    subgraph R[Lectura de datos]
        I[Lectura de valores] --> ifRI{Suficientes \n muestras?}
        ifRI --> J[Filtrado de valores]
        J --> ifConv{Suficientes iteraciones \n de convolución?}
        ifConv --> K[Calculo de frecuencia cardiaca y sus frecuencias fundamentales]
        K --> P[Guardado y muestra de los datos por el puerto serie]
    end

    subgraph V[Visualizacion de los datos]
        ifLS{Primera inserccion \n de datos?} --No--> PS[Mostrar mensaje de cargando en puerto serie y en display]
        ifLS --No--> LP
        subgraph LP[Generar visualizaciones]
            VG --> WG --> ShD[Desplazar los datos al eliminar el primero]
            subgraph VG[Generar visualizacion del display]
                ifO{Orden de la visualizacion \n correspondiente a 1?} --> doV
                subgraph doV[Ejecutar funcion de la visualizacion correspodiente]
                    oD[Obtener los datos calculados] --> N
                    N[Procesar los datos para la visualizacion] --> eD[Mostrar los datos por el Display]
                end
            end
            subgraph WG[Generar visualizacion de la pàgina web]
                OJSON[Obtener mensaje del tipo JSON] --> SWS[Enviar mensaje JSON]
            end
        end
    end
```

### Lectura de los datos

***

Para la lectura de los datos se ha utilizado la libreria `Wire` y `MAX30102` para la comunicación I2C con el sensor *MAX30102*. Además, se ha creado una clase `globalDataReader` que contiene las funciones necesarias para esta tarea. Las funciones de esta clase se han implementado en el núcleo 0 del ESP32.

| MAX30102 PINS| ESP32 PINS |
|--------------|------------|
| SCL          | GPIO 22    |
| SDA          | GPIO 21    |
| VCC          | 3V3        |
| GND          | GND        |

###### **Cabecera de la clase**

```cpp
#ifndef DATAREADER_H
#define DATAREADER_H

#include <Arduino.h>
#include <Wire.h>
#include <vector>
#include <SPIFFS.h>

#include "MAX30105.h"
#include "heartRate.h"
#include "arduinoFFT.h"
#include "spo2_algorithm.h"

#include "GlobalValues.h"

namespace std
{
    /** Global data reader class
     *
     * @brief This class is the global data reader of the device.
     *
     * @details This class is used to manage the global data reader of the device.
     *
     * @param particleSensor MAX30105 object
     * @param enoughSamples Number of samples to be taken
     * @param irBuffer IR buffer
     * @param redBuffer Red buffer
     * @param vCoefs Coefficients of the filter
     * @param bufferLenght Buffer length
     * @param spo2Percentage SPO2 percentage
     * @param heartRate Heart rate
     * @param validSPO2 Valid SPO2
     * @param validHeartRate Valid heart rate
     * @param inputIRData Input IR data
     * @param inputRedData Input red data
     * @param filteringIterations Filtering iterations
     * @param dataReady Data ready
     *
     */
    class globalDataReader {
        // sensor
        MAX30105 particleSensor;

        // variables for data reading
        int enoughSamples;
        uint32_t* irBuffer;
        uint32_t* redBuffer;
        float* vCoefs;
        int32_t bufferLenght, spo2Percentage, heartRate;
        int8_t validSPO2, validHeartRate;

        // filter variables
        vector<float> inputIRData;
        vector<float> inputRedData;
        int filteringIterations = 0; 

        // Confrimation variables
        bool dataReady = false;

        public:
            globalDataReader ( int pEnoughSamples = 200 );

            void setup ();

            void initMAX30102 ( byte ledBrightness = 0x1F, byte sampleAverage = 4, byte ledMode = 3, 
                                int sampleRate = 3200, int pulseWidth = 411, int adcRange = 4096 );

            void readFile ( String fileName = "/coefficients.txt" );

            void readData ( globalValues& globalValuesVar, uint8_t SAMPLES, uint8_t SAMPLING_FREQUENCY );

            void readValuesFromSensor ();

            void doFiltering ( float& resultOfIR, float& resultOfRed );

            void setGlobalValues ( globalValues& globalValuesVar );
            
            void printData ();

            void fft ( globalValues& globalValuesVar, uint8_t SAMPLES, uint8_t SAMPLING_FREQUENCY );

            vector<fundamentalsFreqs> getFFTResults ( double* vReal, uint8_t SAMPLES, uint8_t SAMPLING_FREQUENCY );

            bool isDataReady ();
    };
}

#endif /* DATAREADER_H *
```


#### Filtrado de los datos

Para el filtrado de los datos se ha utilizado la librería `arduinoFFT` para la transformada de Fourier y la librería `spo2_algorithm` y `heartRate` para el cálculo de la saturación de oxígeno en sangre y el ritmo cardíaco respectivamente.

La función del filtrado, implementada dentro de la clase `globalDataReader`, consiste en la lectura y filtrado de los datos del sensor. Con tal de establecer los coeficientes del filtro que se usa en el programa, previamente se realizó un estudio de la señal recibida y mediante `MatLab` se determino el filtro que se usaria. 

Para el filtrado de los datos se ha utilizado un filtro FIR, el cual se ha implementado en el nucleo 0 del ESP32. El filtro FIR se ha implementado usando el comando fir1() de MATLAB i exportando el resultado de los coeficientes a un archivo.txt para poder leerlo desde el ESP32. El filtro consiste en un filtro pasa-banda con una frecuencia de corte inferior de 0.5Hz y una frecuencia de corte superior de 3Hz. Podemos ver el codigo en la rama Filtres de git. 

Con los coeficientes del filtro hemos aplicado una convolución con los datos de entrada para obtener los datos filtrados. Estos datos han sido almacenados en un buffer para poder obtener el sop2 y el ritmo cardiaco. También hemos aplicado una fft para obtener la respuesta en freqüència de la señal.

> **NOTA**: se pueden ver más detalles al respecto del codigo de MATLAB y las herramientas usadas para el filtrado en la rama `Filtres`

###### **Código del programa**

```cpp
/** Read data function
 * 
 * @brief This function reads the data from the sensor.
 *
 * @param globalValuesVar Global values variable.
 * @param SAMPLES Number of samples.
 * @param SAMPLING_FREQUENCY Sampling frequency.
 * 
 * @details This functions reads the data from the sensor and, when it has enough samples,
 *          it applies the filter and sends the data to the heart rate algorithm. Finally,
 *          the output data is stored in the global values variable and printed.
 * 
 * @see readValuesFromSensor(), doFiltering(), setGlobalValues(), printData().
 *  
 */
void globalDataReader::readData ( globalValues& globalValuesVar, uint8_t SAMPLES, uint8_t SAMPLING_FREQUENCY )
{
    float resultOfIR = 0.0;
    float resultOfRed = 0.0;

    readValuesFromSensor();
    // we have enough samples to apply the filter
    if ( inputIRData.size() > enoughSamples )
    {
        doFiltering(resultOfIR, resultOfRed);

        //we have enough samples to send to the heart rate algorithm
        if ( filteringIterations >= enoughSamples )
        {
            filteringIterations = 0;
            setGlobalValues(globalValuesVar);
            printData();
            fft(globalValuesVar, SAMPLES, SAMPLING_FREQUENCY);
            dataReady = true;
        }
        irBuffer[filteringIterations] = resultOfIR;
        redBuffer[filteringIterations] = resultOfRed;    
        filteringIterations++;
    }
}

/** Read values from sensor function
 * 
 * @brief This function reads the values from the sensor and stores the values in its
 *        respective arrays.
 * 
 * @see readData().
 * 
 */
void globalDataReader::readValuesFromSensor ( )
{
    // read the value from the sensor
    float valueIR = particleSensor.getIR();
    float valueRed = particleSensor.getRed();

    // add the new sample to the input data vector
    inputIRData.push_back(valueIR); 
    inputRedData.push_back(valueRed);
}

/** Do filtering function
 * 
 * @brief This function does the filtering of the input data and stores it.
 * 
 * @param resultOfIR Result of the convolution of the filter and the IR array.
 * @param resultOfRed Result of the convolution of the filter and the Red array. 
 * 
 */
void globalDataReader::doFiltering ( float& resultOfIR, float& resultOfRed )
{
    for (int n = enoughSamples; n >= 0; n--) 
    {
        resultOfIR += vCoefs[n] * inputIRData[inputIRData.size() - n - 1];
        resultOfRed += vCoefs[n] * inputRedData[inputRedData.size() - n - 1];
        
    }
    // delete the oldest sample from the list
    inputIRData.erase(inputIRData.begin()); 
    inputRedData.erase(inputRedData.begin());
}
```

#### FFT

La función de la FFT, implementada dentro de la clase `globalDataReader`, consiste en la aplicación de la FFT a los datos y la obtención de las frecuencias fundamentales.

###### **Código del programa**

```cpp
/** FFT function
 * 
 * @brief This function applies the FFT to the data.
 * 
 * @param globalValuesVar Global values variable.
 * @param SAMPLING_FREQUENCY Sampling frequency.
 * @param SAMPLES Number of samples.
 * 
 * @details This function applies the FFT to the data and stores the results in the
 *         global values variable.
 * 
 */
void globalDataReader::fft ( globalValues& globalValuesVar, uint8_t SAMPLES, uint8_t SAMPLING_FREQUENCY )
{
    Serial.println("Computing FFT...");
    
    /***TODO: GET 4 MAX AND SHOW ITS HZ IN DISPLAY*/

    arduinoFFT FFT = arduinoFFT();
    double vReal[SAMPLES];
    double vImag[SAMPLES];
    for (int i = 0; i < SAMPLES; i++)
    {
        vReal[i] = irBuffer[i];
        vImag[i] = 0;
    }

    FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
    vReal[0] = 0; // Remove the DC component

    vector<fundamentalsFreqs> fftResults = getFFTResults( vReal, SAMPLES, SAMPLING_FREQUENCY );
    globalValuesVar.setFreqs( fftResults );
}

/** Get FFT results function
 * 
 * @brief This function gets the FFT results.
 * 
 * @param vReal Real part of the FFT.
 * @param SAMPLES Number of samples.
 * @param SAMPLING_FREQUENCY Sampling frequency.
 * 
 * @return Vector of fundamentals frequencies.
 * 
 */
vector<fundamentalsFreqs> globalDataReader::getFFTResults ( double* vReal, uint8_t SAMPLES, uint8_t SAMPLING_FREQUENCY )
{
    Serial.println("FFT results:");

    vector<fundamentalsFreqs> freqs;
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
    return freqs;
}
```

### Guardado de los datos

***

Con la finalidad de facilitar el traspaso de datos entre las dos tareas principales, se ha implementado una clase para acceder y modificar los datos filtrados del sensor. Esta clase lleva el nombre de `globalValues` y incluye principalmente funciones de acceso y modificación de datos, por lo que no se entrara en detalle sobre las funciones que la describen.

###### **Cabecera de la clase**

```cpp
#ifndef GLOBALVALUES_H
#define GLOBALVALUES_H

#include <Arduino.h>
#include <vector>

namespace std
{
    /** Fundamentals frequencies struct
     * 
     * @brief This struct is the fundamentals frequencies of the device.
     * 
     * @details This struct is used to store the fundamentals frequencies of the heart rate.
     * 
     * @param freqsHz Frequency in Hz
     * @param amplitude Amplitude of the frequency
     *
     */
    struct fundamentalsFreqs{
        float freqsHz;
        float amplitude;
    };

    /** Global values class
     * 
     * @brief This class is the global values of the device.
     * 
     * @details This class is used to store the global values of the device.
     * 
     * @param heartRateDataArray Array of heart rate data
     * @param beatsPerMinute Beats per minute
     * @param spo2Percentage Spo2 percentage
     * @param freqs Fundamentals frequencies
     *
     */
    class globalValues {
        vector<uint32_t> heartRateDataArray;
        vector<fundamentalsFreqs> freqs;
        int32_t beatsPerMinute, spo2Percentage;

        public:
            globalValues ();
            
            globalValues ( vector<uint32_t> heartRateDataArray, int32_t beatsPerMinute, 
                        int32_t spo2Percentage, vector<fundamentalsFreqs> freqs );

            void pushBackHeartRateDataArray ( uint32_t* pHeartRateDataArray, uint32_t size);

            void setHeartRateDataArray ( vector <uint32_t>heartRateDataArray );
            
            void setBeatsPerMinute ( int32_t beatsPerMinute );

            void setSpo2Percentage ( int32_t spo2Percentage );

            void setFreqs ( vector<fundamentalsFreqs> freqs);
            
            vector<uint32_t> getHeartRateDataArray();

            vector<uint32_t> getHeartRateDataArray( uint32_t N );
                        
            uint32_t getFirstValueHeartRate();
            
            void shiftHeartRate();

            int32_t getBeatsPerMinute();

            int32_t getSpo2Percentage();
        
            vector<fundamentalsFreqs> getFreqs();
    };
}
#endif /* GLOBALVALUES_H */
```

### Visualización de los datos

***

Para la visualización de los datos se ha utilizado un display y una pagina web. Para la implementacion del *display* se ha utilizado la librería `SPI` y `U8g2lib` y para la pagina web las librerias `WiFi` y `ESPAsyncWebServer`. Ambos métodos de visualización se han implementado en una clase `globalDataVisualizer` que contiene como atributos un clase `Display` y una clase `WebPage`. Las funciones de esta clase se han implementado en el núcleo 1 del ESP32.

#### Display

El *display* con el que trabajaremos consiste en un `GMG-12864-06D`, de 128x64 píxeles de resolución. Este se comunica con el ESP32 mediante el protocolo SPI, por lo que se ha utilizado la librería `SPI` y `U8g2lib` para facilitar la comunicación.

El *display* se connecta al ESP32 mediante 7 pines, los cuales se muestran en la siguiente tabla con su correspondiente descripción:

|PIN GMG-12864-06D| PIN ESP32 | Descripción |
|-----------------|-----------|-------------|
|CS               |GPIO 5     |Chip Select  |
|RSE              |GPIO 32    |Reset        |
|RS               |GPIO 33    |DC           |
|SCL              |GPIO 18    |SPI Clock    |
|SI               |GPIO 23    |SPI Data     |
|VDD              |3V3        |VCC          |
|VSS              |GND        |GND          |
|A                |47Ω -> 3V3 |Anode        |
|K                |GND        |Cathode      |

El *display* se ha programado para que muestre tres tipos de datos, el ritmo cardíaco, el porcentaje de saturación de oxígeno en sangre (ambos con una gráfica en la parte izquierda donde se ve la evolución de la frecuencia cardíaca) y las frecuencias fundamentales. Para ello se ha creado una clase `Display` que hereda de la clase `U8G2_ST7565_ERC12864_1_4W_SW_SPI` de la librería `U8g2lib`. Esta clase contiene las funciones necesarias para mostrar los datos en la pantalla.

Dentro de la clase `Display` se han implementado las siguientes funciones:

- `init()`: Esta funcion inicializa el display.
- `drawAxis()`: Esta funcion dibuja los ejes del display.
- `drawData()`: Esta funcion dibuja los datos en el display.
- `printMeasurements()`: Esta funcion imprime los datos en el display.
- `drawBars()`: Esta funcion dibuja las barras en el display.
- `getYAxisBias()`: Esta funcion devuelve el valor del eje Y.
- `getDataWindowSize()`: Esta funcion devuelve el tamaño de la ventana de datos.

Se puede alternar entre los modos de visualizacion mediante tres pulsadores que cambian entre los estados descritos. Para la implementacion de estos se ha definido una clase `Button` y otra clase `buttonsArray`, que contiene la función `readButtons`, la cual se ejecuta mediante interrupciones por timer y se encarga de filtrar los pulsos de los botones y cambiar el estado de la variable `order` a 1 si se pasa el filtro anti-rebote. Esta funcion se ha implementado como su puede ver a continuación:

```cpp
/** Button management function
 * 
 * @brief This function reads the buttons.
 *  
 * @return void.
 *  
 * @details This function reads the buttons' values and changes the order of the buttons.
 *  
 * @note This function is called when the timer is activated. 
 * 
 */
void buttonsArray::readButtons()
{ 
    for(uint8_t i = 0; i < buttons.size(); i++)
    {
        buttons[i].actualValue = digitalRead(buttons[i].pin);                        // Read the value of the button
        buttons[i].actualChange = buttons[i].previousValue ^ buttons[i].actualValue; // XOR of actual value and last value
        if(buttons[i].actualChange == 1 && buttons[i].previousChange == 1)           // If both status changes are equal to 1
        {
            buttons[i].order = 1;                                                    // Order to 1
            for (uint8_t j = 0; j < buttons.size(); j++)
            {
                if (j != i)buttons[j].order = 0;                                     // Rest of orders to 0 
            }
            buttons[i].previousValue = buttons[i].actualValue;                       // Last value equal to actual value
            buttons[i].previousChange = 0;                                           // Last status change equal to 0
            return;
        }
        buttons[i].previousChange = buttons[i].actualChange;                         // Last status change is equal to acutal change
    }
}
```

> **NOTA**: En caso de necesitar más información sobre la implementación de los botones, se puede consultar el archivo `Button.cpp` y `buttonsArray.cpp`.

###### **Cabecera de la clase `Display`**

```cpp
#ifndef DISPLAY_H
#define DISPLAY_H

#include <vector>
#include <U8g2lib.h>
#include <SPI.h>

namespace std
{
    /** Display class
     *
     * @brief This class is the display of the device.
     *
     * @details This class is used to manage the display of the device.
     *
     * @param xAxisBegin X axis begin
     * @param xAxisEnd X axis end
     * @param yAxisBegin Y axis begin
     * @param yAxisEnd Y axis end
     * @param halfHeight Half height of the display
     * @param margin Margin of the display
     *
     */
    class Display : public U8G2_ST7565_ERC12864_1_4W_SW_SPI{
        uint32_t xAxisBegin, xAxisEnd, yAxisBegin, yAxisEnd, halfHeight, margin;
        
        public:
            using U8G2_ST7565_ERC12864_1_4W_SW_SPI::U8G2_ST7565_ERC12864_1_4W_SW_SPI;

            void init ();

            void drawAxis ( bool longAxis = false );

            void drawData ( vector<uint32_t> dataVector );

            void printMeasurements (int32_t value, bool heartRateType );

            void drawBars ( const vector<String>& labels, const vector<float>& amplitudes );
            
            uint32_t getYAxisBias ( );

            uint32_t getDataWindowSize ();
    };
}
#endif /* DISPLAY_H */
```

#### Página web

En lo que respecta a la implementación de la página web, se ha utilizado la librería `ESPAsyncWebServer` para la creación del servidor web. Para la creación de la página web se ha utilizado el lenguaje HTML, CSS y JavaScript. En ella se pueden visualizar dos gráficas: una para las frecuencias fundamentales de la frecuencia cardíaca y otra para la evolución de la frecuencia cardíaca, ademas, de los valores calculados de porcentaje de saturación en sangre y frecuencia cardíaca media.

El código en *JavaScript* de la página web principalmente se encarga de la creación de las gráficas y de la comunicación con el servidor mediante *WebSockets*. Para la creación de las gráficas se ha utilizado la librería `Chart.js`. Los mensajes que se establecen entre el servidor y la aplicación en `Javascript` són del tipo *JSON*.

En pocas palabras, los datos calculados se agrupan en un mensaje *JSON* (se puede ver un ejemplo en la documentación del código) y se envian al cliente mediante *WebSockets*. La aplicación en `JavaScript` recibe el mensaje y actualiza los datos de la página web.

###### **Cabecera de la clase**

```cpp
#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <SPIFFS.h>

/** Web page class 
 * @brief Class to manage the web page
 * 
 * @details This class is used to manage the web page
 * 
 * @param webServer AsyncWebServer object
 * @param webSocket AsyncWebSocket object
 * @param globalClient AsyncWebSocketClient object
 * 
 */
class webPage{
    AsyncWebServer webServer;
    AsyncWebSocket webSocket;
    AsyncWebSocketClient * globalClient;

    public:
        webPage(int port = 80);

        void begin(const char* ssid, const char* password);

        void initWiFi(const char* ssid, const char* password);

        void initServer();

        void onWsEvent (AsyncWebSocket * server, AsyncWebSocketClient * client, 
                        AwsEventType type, void * arg, uint8_t *data, size_t len);

        void sendWsMessage(String message);
};

#endif /* WEBPAGE_H */
```

###### **Código principal en Javascript de la página web**

```javascript
// Constants
const maxDataLength = 32;
// Arrays
let heartRateArray = [];
let timeArray = [];
let freqsHz = [];
let freqsAmplitude = [];

// Last values
let lastFreqsHz = [];
let lastFreqsAmplitude = [];
let lastRawTime = 0;

// Values vars
let beatsPerMinuteValue = "Calculating BPM...";
let spo2PercentageValue = "Calculating SPO2...";

// Conectar al WebSocket del ESP32
var socket = new WebSocket('ws://' + location.hostname + '/ws');
socket.onmessage = function (event) {
    // get new data
    var jsonData = JSON.parse(event.data);

    // update beats per minute and spo2
    var beatsPerMinute = jsonData.beatsPerMinute;
    beatsPerMinuteValue = beatsPerMinute + " BPM";
    document.getElementById('heartrate').innerHTML = beatsPerMinuteValue;
    
    var spo2Percentage = jsonData.spo2Percentage;
    spo2PercentageValue = spo2Percentage + " %";
    document.getElementById('spo2').innerHTML = spo2PercentageValue;

    // update heartRateArray, spo2Array and timeArray
    var newHeartRateData = jsonData.heartRateData;
    heartRateArray.push(newHeartRateData);
    if (heartRateArray.length >= maxDataLength) heartRateArray.shift();

    if(lastRawTime != 0)
    {
        var actualTime = Date.now();
        var millisTimeDifference = actualTime - lastRawTime;
        var timeDifferenceBetweenData = millisTimeDifference/1000;
        
        // calculate accomulated time and round it to 3 decimals
        var accomulatedTime = Math.floor((timeArray[timeArray.length - 1] + timeDifferenceBetweenData)*1000)/1000;        
        lastRawTime = actualTime;
        timeArray.push(accomulatedTime);
    } else {
        lastRawTime = Date.now();
        timeArray.push(0);
    }
    if ( timeArray.length > maxDataLength ) timeArray.shift();
    
    cardiogramaChart.update();
    
    // update freqsHz and freqsAmplitude
    var freqsDataHz = jsonData.freqsHz;
    var similarityBetweenHzArrays =  (lastFreqsHz.length == freqsDataHz.length) 
        && lastFreqsHz.every(function(element, index) {
        return element === freqsDataHz[index]; 
    });

    if (!similarityBetweenHzArrays){
        freqsHz.length = 0;
        for(var i = 0; i < freqsDataHz.length; i++)
        {
            freqsHz.push(freqsDataHz[i]);
        }
        lastFreqsHz = freqsDataHz;
    }
    
    var freqsDataAmplitude = jsonData.freqsAmplitude;
    var similarityBetweenAmplitudeArrays = (lastFreqsAmplitude.length == freqsDataAmplitude.length) 
        && lastFreqsAmplitude.every(function(element, index) {
        return element === freqsDataAmplitude[index]; 
    });
    if (!similarityBetweenAmplitudeArrays){
        freqsAmplitude.length = 0;
        for(var i = 0; i < freqsDataAmplitude.length; i++){
            freqsAmplitude.push(freqsDataAmplitude[i]);
        }
        lastFreqsAmplitude = freqsDataAmplitude;
        
        freqsChart.update();
    }
} 
```

## **Esquema de connexiones**

Para la conexión de los dispositivos se ha utilizado el siguiente pinaje:

| Dispositivo      | Uso         | GPIO |
|------------------|-------------|------|
| GMG-12864-06D    | Chip Select | 5    |
| GMG-12864-06D    | Reset       | 32   | 
| GMG-12864-06D    | DC(RS)      | 33   |
| GMG-12864-06D    | SPI Clock   | 18   | 
| GMG-12864-06D    | SPI Data    | 23   |
| GMG-12864-06D    | VCC         | 3V3  |    
| GMG-12864-06D    | GND         | GND  |
| GMG-12864-06D    | Anode       | 3V3  |
| GMG-12864-06D    | Cathode     | GND  |
| MAX30102         | SCL         | 22   |
| MAX30102         | SDA         | 21   |
| MAX30102         | VCC         | 3V3  |
| MAX30102         | GND         | GND  |
| Boton BPM        | PIN         | 25   |
| Boton BPM        | GND         | GND  |
| Boton SPO2       | PIN         | 26   |
| Boton SPO2       | GND         | GND  |
| Boton Frecuencia | PIN         | 27   |
| Boton Frecuencia | GND         | GND  |


###### **Esquema de connexiones**

![Esquema de connexiones](./images/connections_esquema.png)

## **Montaje**

Respecto al montaje del proyecto, se ha utilizado una placa de prototipado para connectar los dispositivos y el ESP32. Todos los dispositivos han sido colocados dentro de una caja de madera con tal de facilitar su portabilidad. Cabe destacar que todos los cables de connexiones han sido soldados para evitar que se desconecten durante el uso del dispositivo.

El resultado final es el siguiente:

<img src="./images/upper_view.png" width="50%" ><img src="./images/bow_integraton.png" width="50%" >

> **NOTA**: Se pueden consultar más imagenes del montaje del proyecto en la carpeta `images`.

## **Resultados**

Se pueden consultar los resultados del proyecto en la carpeta `images/results`, donde se pueden visualizar la salida de los datos en el *display* y en la página web.
