#include "GlobalValues.h"

using namespace std;

/** Global values default constructor
 * 
 * @brief This function is the constructor of the global values.
 *
 */
globalValues::globalValues()
{
    this -> heartRateDataArray = {};
    this -> spo2DataArray = {};
    this -> beatsPerMinute = 0;
    this -> spo2Percentage = 0;
    this -> freqs = {};
}

/** Global values constructor
 * 
 * @brief This function is the constructor of the global values.
 *
 * @param heartRateDataArray Heart rate data array.
 * @param spo2DataArray SPO2 data array.
 * @param beatsPerMinute Beats per minute.
 * @param spo2Percentage SPO2 percentage.
 * @param freqs Fundamentals frequencies.
 * 
 */
globalValues::globalValues( vector<uint32_t> heartRateDataArray, vector<uint32_t> spo2DataArray, int32_t beatsPerMinute,
                            int32_t spo2Percentage, vector<fundamentalsFreqs> freqs )
{
    this -> heartRateDataArray = heartRateDataArray;
    this -> spo2DataArray = spo2DataArray;
    this -> beatsPerMinute = beatsPerMinute;
    this -> spo2Percentage = spo2Percentage;
    this -> freqs = freqs;
}

/** Set heart rate data array function
 * 
 * @brief This function sets the heart rate data array.
 * 
 * @param heartRateDataArray Heart rate data array.
 * 
 */
void globalValues::setHeartRateDataArray ( vector<uint32_t> heartRateDataArray )
{
    this -> heartRateDataArray = heartRateDataArray;
}

/** Push back heart rate data array function
 * 
 * @brief This function pushes back the heart rate data array.
 * 
 * @param pHeartRateDataArray Heart rate data array.
 * @param size Size of the heart rate data array.
 * 
 */
void globalValues::pushBackHeartRateDataArray ( uint32_t* pHeartRateDataArray, uint32_t size )
{  
    for (uint32_t i = 0; i < size; i++)
    {
        this -> heartRateDataArray.push_back ( pHeartRateDataArray[i] );
    }
}

/** Set SPO2 data array function
 * 
 * @brief This function sets the SPO2 data array.
 * 
 * @param spo2DataArray SPO2 data array.
 * 
 */
void globalValues::setSpo2DataArray ( vector<uint32_t> spo2DataArray )
{
    this -> spo2DataArray = spo2DataArray;
}

/** Push back SPO2 data array function
 * 
 * @brief This function pushes back the SPO2 data array.
 * 
 * @param spo2DataArray SPO2 data array.
 * @param size Size of the SPO2 data array.
 * 
 */
void globalValues::pushBackSpo2DataArray ( uint32_t* pSpo2DataArray, uint32_t size )
{
    for (uint32_t i = 0; i < size; i++)
    {
        this -> spo2DataArray.push_back ( pSpo2DataArray[i] );
    }
}

/** Set beats per minute function
 * 
 * @brief This function sets the beats per minute.
 * 
 * @param beatsPerMinute Beats per minute.
 * 
 */
void globalValues::setBeatsPerMinute ( int32_t beatsPerMinute )
{
    this -> beatsPerMinute = beatsPerMinute;
}

/** Set SPO2 percentage function
 * 
 * @brief This function sets the SPO2 percentage.
 * 
 * @param spo2Percentage SPO2 percentage.
 * 
 */
void globalValues::setSpo2Percentage ( int32_t spo2Percentage )
{
    this -> spo2Percentage = spo2Percentage;
}

/** Set fundamentals frequencies function
 * 
 * @brief This function sets the fundamentals frequencies.
 * 
 * @param freqs Fundamentals frequencies.
 * 
 */
void globalValues::setFreqs ( vector<fundamentalsFreqs> freqs )
{
    this -> freqs = freqs;
}

/** Get heart rate data array function
 * 
 * @brief This function gets the heart rate data array.
 * 
 * @return Heart rate data array.
 * 
 */
vector<uint32_t> globalValues::getHeartRateDataArray()
{
    return heartRateDataArray;
}

/** Get heart rate data array function
 * 
 * @brief This function returns the N first values of the heart rate data array.
 * 
 * @param N Size of the returned heart rate data array.
 * 
 * @return Heart rate data array.
 * 
 */
vector<uint32_t> globalValues::getHeartRateDataArray ( uint32_t N )
{
    if ( N > heartRateDataArray.size() )return heartRateDataArray;
    vector<uint32_t> result;
    for (uint32_t i = 0; i < N; i++)
    {
        result.push_back ( heartRateDataArray[i] );
    }
    return result;
}

/** Get first value and shift heart rate function
 * 
 * @brief This function gets the first value of the heartRate array and shifts it.
 * 
 * @return First value of the heartRate array before shifting.
 * 
 */
uint32_t globalValues::getFirstValueAndShiftHeartRate()
{
    uint32_t firstValue = heartRateDataArray[0];
    heartRateDataArray.erase( heartRateDataArray.begin() );
    return firstValue;
}

/** Get SPO2 data array function
 * 
 * @brief This function gets the SPO2 data array.
 * 
 * @return SPO2 data array.
 * 
 */
vector<uint32_t> globalValues::getSpo2DataArray()
{
    return spo2DataArray;
}

/** Get SPO2 data array function
 * 
 * @brief This function returns the N first values of the SPO2 data array.
 * 
 * @param N Size of the returned SPO2 data array.
 *
 * @return SPO2 data array.
 *  
 */
vector<uint32_t> globalValues::getSpo2DataArray ( uint32_t N )
{
    if ( N > spo2DataArray.size() ) return spo2DataArray;
    vector<uint32_t> result;
    for (uint32_t i = 0; i < N; i++)
    {
        result.push_back ( spo2DataArray[i] );
    }
    return result;
}

/** Get first value and shift SPO2 function
 * 
 * @brief This function gets the first value of the spo2 array and shifts it.
 *
 * @return First value of the spo2 array before shifting. 
 * 
 */
uint32_t globalValues::getFirstValueAndShiftSpo2()
{
    uint32_t firstValue = spo2DataArray[0];
    spo2DataArray.erase( spo2DataArray.begin() );
    return firstValue;
}

/** Get beats per minute function
 * 
 * @brief This function gets the beats per minute.
 * 
 * @return Beats per minute.
 * 
 */
int32_t globalValues::getBeatsPerMinute()
{
    return beatsPerMinute;
}

/** Get SPO2 percentage function
 * 
 * @brief This function gets the SPO2 percentage.
 * 
 * @return SPO2 percentage.
 * 
 */
int32_t globalValues::getSpo2Percentage()
{
    return spo2Percentage;
}

/** Get fundamentals frequencies function
 * 
 * @brief This function gets the fundamentals frequencies.
 * 
 * @return Fundamentals frequencies.
 * 
 */
vector<fundamentalsFreqs> globalValues::getFreqs()
{
    return freqs;
}
