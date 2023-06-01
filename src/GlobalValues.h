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
     * @param spo2DataArray Array of spo2 data
     * @param beatsPerMinute Beats per minute
     * @param spo2Percentage Spo2 percentage
     * @param freqs Fundamentals frequencies
     *
     */
    class globalValues {
        vector<uint32_t> heartRateDataArray;
        vector<uint32_t> spo2DataArray;
        vector<fundamentalsFreqs> freqs;
        int32_t beatsPerMinute, spo2Percentage;

        public:
            globalValues ();
            
            globalValues ( vector<uint32_t> heartRateDataArray, vector<uint32_t> spo2DataArray, int32_t beatsPerMinute, 
                        int32_t spo2Percentage, vector<fundamentalsFreqs> freqs );

            void setHeartRateDataArray ( vector <uint32_t>heartRateDataArray );
            
            void pushBackHeartRateDataArray ( uint32_t* pHeartRateDataArray, uint32_t size);

            void setSpo2DataArray ( vector<uint32_t> spo2DataArray );

            void pushBackSpo2DataArray ( uint32_t* pSpo2DataArray, uint32_t size );
            
            void setBeatsPerMinute ( int32_t beatsPerMinute );

            void setSpo2Percentage ( int32_t spo2Percentage );

            void setFreqs ( vector<fundamentalsFreqs> freqs);

            vector<uint32_t> getHeartRateDataArray();

            vector<uint32_t> getHeartRateDataArray( uint32_t N );
                        
            uint32_t getFirstValueAndShiftHeartRate();

            vector<uint32_t> getSpo2DataArray();

            vector<uint32_t> getSpo2DataArray( uint32_t N );

            uint32_t getFirstValueAndShiftSpo2();

            int32_t getBeatsPerMinute();

            int32_t getSpo2Percentage();
        
            vector<fundamentalsFreqs> getFreqs();
    };
}
#endif /* GLOBALVALUES_H */