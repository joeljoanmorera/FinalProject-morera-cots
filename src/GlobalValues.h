#ifndef GLOBALVALUES_H
#define GLOBALVALUES_H

#include <vector>
#include <Arduino.h>

using namespace std;

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
    int freqsHz;
    int amplitude;
};

/** Global values class
 * 
 * @brief This class is the global values of the device.
 * 
 * @details This class is used to store the global values of the device.
 * 
 * @param heartRateDataArray Array of heart rate data
 * @param spo2DataArray Array of spo2 data
 * @param beatsPerMinute Beats per minute
 * @param spo2Percentage Spo2 percentage
 * @param freqs Fundamentals frequencies
 * @param pointerHeartRateDataArray Pointer to heart rate data array
 * @param pointerSpo2DataArray Pointer to spo2 data array
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
        
        globalValues(uint32_t* heartRateDataArray, uint32_t* spo2DataArray, int32_t beatsPerMinute, 
                    int32_t spo2Percentage, vector<fundamentalsFreqs> freqs);

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
};

#endif /* GLOBALVALUES_H */