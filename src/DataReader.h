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
     * @param n N
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

#endif /* DATAREADER_H */