#include <vector>
#include <Arduino.h>

using namespace std;

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
        /** Global values default constructor
         * 
         * @brief This function is the constructor of the global values.
         *
         */
        globalValues(){}
        
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
        globalValues(uint32_t* heartRateDataArray, uint32_t* spo2DataArray, int32_t beatsPerMinute, int32_t spo2Percentage, vector<fundamentalsFreqs> freqs){}

        /** Set heart rate data array function
         * 
         * @brief This function sets the heart rate data array.
         * 
         * @param heartRateDataArray Heart rate data array.
         */
        void setHeartRateDataArray(uint32_t* heartRateDataArray){}

        /** Set SPO2 data array function
         * 
         * @brief This function sets the SPO2 data array.
         * 
         * @param spo2DataArray SPO2 data array.
         */
        void setSpo2DataArray(uint32_t* spo2DataArray){}

        /** Set beats per minute function
         * 
         * @brief This function sets the beats per minute.
         * 
         * @param beatsPerMinute Beats per minute.
         */
        void setBeatsPerMinute(int32_t beatsPerMinute){}

        /** Set SPO2 percentage function
         * 
         * @brief This function sets the SPO2 percentage.
         * 
         * @param spo2Percentage SPO2 percentage.
         */
        void setSpo2Percentage(int32_t spo2Percentage){}

        /** Set fundamentals frequencies function
         * 
         * @brief This function sets the fundamentals frequencies.
         * 
         * @param freqs Fundamentals frequencies.
         */
        void setFreqs(vector<fundamentalsFreqs> freqs){}

        /** Get heart rate data array function
         * 
         * @brief This function gets the heart rate data array.
         * 
         * @return Heart rate data array.
         */
        uint32_t* getHeartRateDataArray(){}

        /** Get SPO2 data array function
         * 
         * @brief This function gets the SPO2 data array.
         * 
         * @return SPO2 data array.
         */
        uint32_t* getSpo2DataArray(){}

        /** Get beats per minute function
         * 
         * @brief This function gets the beats per minute.
         * 
         * @return Beats per minute.
         */
        int32_t getBeatsPerMinute(){}

        /** Get SPO2 percentage function
         * 
         * @brief This function gets the SPO2 percentage.
         * 
         * @return SPO2 percentage.
         */
        int32_t getSpo2Percentage(){}

        /** Get fundamentals frequencies function
         * 
         * @brief This function gets the fundamentals frequencies.
         * 
         * @return Fundamentals frequencies.
         */
        vector<fundamentalsFreqs> getFreqs(){}

        /** Get JSON function
         * 
         * @brief This function gets the JSON of the global values.
         * 
         * @return JSON of the global values.
         */
        String getJson(int size_of_HR, int size_of_SPO2){}
        
        /** Get JSON single values function
         * 
         * @brief This function gets the JSON of the global values, but returns only the last value of the arrays.
         * 
         * @return JSON of the global values.
         */
        String getJsonSingleValues(int size_of_heartrate, int size_of_spo2){}
};
