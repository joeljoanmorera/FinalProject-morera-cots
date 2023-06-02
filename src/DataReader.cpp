#include "DataReader.h"

using namespace std;

/** Data reader default constructor
 * 
 * @brief This function is the constructor of the data reader.
 *
 */
globalDataReader::globalDataReader ( int pEnoughSamples )
{
    this -> enoughSamples = pEnoughSamples;
   
    uint32_t pIrBuffer[enoughSamples];
    this -> irBuffer = pIrBuffer;
   
    uint32_t pRedBuffer[enoughSamples];
    this -> redBuffer = pRedBuffer;
   
    float pVCoefs[enoughSamples + 1];
    this -> vCoefs = pVCoefs;
}

/** Setup function
 * 
 * @brief This function sets up the data reader.
 * 
 */
void globalDataReader::setup ( )
{
    readFile();
    initMAX30102();
}

/** Init MAX30102 function
 * 
 * @brief This function initializes the MAX30102 sensor.
 * 
 * @param ledBrightness LED brightness.
 * @param sampleAverage Sample average.
 * @param ledMode LED mode.
 * @param sampleRate Sample rate.
 * @param pulseWidth Pulse width.
 * @param adcRange ADC range.
 * 
 * @note The parameters are set to the default values. The different options are:
 *      - ledBrightness: 0 = Off to 255 = 50mA
 *      - sampleAverage: 1, 2, 4, 8, 16, 32
 *      - ledMode: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
 *      - sampleRate: 50, 100, 200, 400, 800, 1000, 1600, 3200
 *      - pulseWidth: 69, 118, 215, 411
 *      - adcRange: 2048, 4096, 8192, 16384
 * 
 * @see setup();
 * 
 */
void globalDataReader::initMAX30102 ( byte ledBrightness, byte sampleAverage, byte ledMode, 
                                      int sampleRate, int pulseWidth, int adcRange )
{
    // Initialize sensor
    if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
    {
        Serial.println("MAX30105 was not found. Please check wiring/power. ");
        while (1);
    }
    particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); 
}

/** Read file function
 * 
 * @brief This function reads the coeficients for the filter from a file in the ESP32 
 *        file system and assign its coefficients to the vCoefs array.
 * 
 * @param fileName Name of the file to read. By default is "/coeficients2.txt".
 * 
 * @see setup();
 * 
 */
void globalDataReader::readFile ( String fileName)
{
    File file = SPIFFS.open(fileName);
    if(!file)
    {
        Serial.println("Failed to open file for reading");
        return;
    }

    for (int i = 0; i <= enoughSamples; i++)
    {
        string stringCoef = file.readStringUntil('\n').c_str();
        vCoefs[i] = stof(stringCoef);
        delay(50);
    }
  
    file.close(); 
}

/** Read data function
 * 
 * @brief This function reads the data from the sensor.
 *
 * @param globalValuesVar Global values variable.
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
    Serial.println(resultOfIR);
        // delete the oldest sample from the list
    inputIRData.erase(inputIRData.begin()); 
    inputRedData.erase(inputRedData.begin());
}

/** Set the global values
 * 
 * @brief This functions sets the global values with the calculated parameters.
 * 
 * @param globalValuesVar global values variable
 * 
 * @details This function sends the samples to the heart rate algorithm and, if the
 *         heart rate and the SPO2 are valid, it stores them in the global values 
 *        variable. If not, it stores a default value.
 * 
 * @see readData().
 * 
 */
void globalDataReader::setGlobalValues ( globalValues& globalValuesVar )
{
    // send samples to the heart rate algorithm
    maxim_heart_rate_and_oxygen_saturation( irBuffer, enoughSamples /*200*/, redBuffer, 
                                            &spo2Percentage, &validSPO2, &heartRate, &validHeartRate);
    if ( validHeartRate && validSPO2 ){
        globalValuesVar.setBeatsPerMinute(heartRate);
        globalValuesVar.setSpo2Percentage(spo2Percentage);
    } else {
        globalValuesVar.setSpo2Percentage(96);
    }
    globalValuesVar.pushBackHeartRateDataArray( irBuffer, enoughSamples /*200*/ );
}

/** Print data function
 * 
 * @brief This function prints the results calculated.
 * 
 * @see setGlobalValues(), readData().
 * 
*/
void globalDataReader::printData ( )
{
    Serial.print("Heart rate: ");
    Serial.print(heartRate);
    Serial.print(" bpm / SpO2: ");
    Serial.print(spo2Percentage);
    Serial.println(" %"); 
}

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
   
    // // FFT from GITHUB COPILOT
    // FFT.Windowing(vReal, enoughSamples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    // FFT.Compute(vReal, vImag, enoughSamples, FFT_FORWARD);
    // FFT.ComplexToMagnitude(vReal, vImag, enoughSamples);
    // double peak = FFT.MajorPeak(vReal, enoughSamples, 3200);
    // Serial.print("Peak: ");
    // Serial.println(peak);
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

bool globalDataReader::isDataReady()
{
    return dataReady;
}