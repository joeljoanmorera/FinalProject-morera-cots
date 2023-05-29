#ifndef GLOBALVALUES_H
#define GLOBALVALUES_H

#include <vector>
#include <Arduino.h>

using namespace std;

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
    int pointerHeartRateDataArray = 0;
    int pointerSpo2DataArray = 0;

    public:
        globalValues();
        
        globalValues(uint32_t* heartRateDataArray, uint32_t* spo2DataArray, int32_t beatsPerMinute, int32_t spo2Percentage, vector<fundamentalsFreqs> freqs);

        void setHeartRateDataArray(uint32_t* heartRateDataArray);

        void setSpo2DataArray(uint32_t* spo2DataArray);

        void setBeatsPerMinute(int32_t beatsPerMinute);

        void setSpo2Percentage(int32_t spo2Percentage);

        void setFreqs(vector<fundamentalsFreqs> freqs);

        uint32_t* getHeartRateDataArray();

        uint32_t* getSpo2DataArray();

        int32_t getBeatsPerMinute();

        int32_t getSpo2Percentage();
    
        vector<fundamentalsFreqs> getFreqs();

        String getJson(int size_of_HR, int size_of_SPO2);
        
        String getJsonSingleValues(int size_of_heartrate, int size_of_spo2);
};

#endif /* GLOBALVALUES_H */