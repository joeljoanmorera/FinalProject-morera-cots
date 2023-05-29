#include <GlobalValues.h>

/** Global values default constructor
 * 
 * @brief This function is the constructor of the global values.
 *
 */
globalValues::globalValues()
{
    this -> heartRateDataArray = 0;
    this -> spo2DataArray = 0;
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
 */
globalValues::globalValues(uint32_t* heartRateDataArray, uint32_t* spo2DataArray, int32_t beatsPerMinute, int32_t spo2Percentage, vector<fundamentalsFreqs> freqs)
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
 */
void globalValues::setHeartRateDataArray(uint32_t* heartRateDataArray)
{
    this -> heartRateDataArray = heartRateDataArray;
}

/** Set SPO2 data array function
 * 
 * @brief This function sets the SPO2 data array.
 * 
 * @param spo2DataArray SPO2 data array.
 */
void globalValues::setSpo2DataArray(uint32_t* spo2DataArray)
{
    this -> spo2DataArray = spo2DataArray;
}

/** Set beats per minute function
 * 
 * @brief This function sets the beats per minute.
 * 
 * @param beatsPerMinute Beats per minute.
 */
void globalValues::setBeatsPerMinute(int32_t beatsPerMinute)
{
    this -> beatsPerMinute = beatsPerMinute;
}

/** Set SPO2 percentage function
 * 
 * @brief This function sets the SPO2 percentage.
 * 
 * @param spo2Percentage SPO2 percentage.
 */
void globalValues::setSpo2Percentage(int32_t spo2Percentage)
{
    this -> spo2Percentage = spo2Percentage;
}

/** Set fundamentals frequencies function
 * 
 * @brief This function sets the fundamentals frequencies.
 * 
 * @param freqs Fundamentals frequencies.
 */
void globalValues::setFreqs(vector<fundamentalsFreqs> freqs)
{
    this -> freqs = freqs;
}

/** Get heart rate data array function
 * 
 * @brief This function gets the heart rate data array.
 * 
 * @return Heart rate data array.
 */
uint32_t* globalValues::getHeartRateDataArray()
{
    return heartRateDataArray;
}

/** Get SPO2 data array function
 * 
 * @brief This function gets the SPO2 data array.
 * 
 * @return SPO2 data array.
 */
uint32_t* globalValues::getSpo2DataArray()
{
    return spo2DataArray;
}

/** Get beats per minute function
 * 
 * @brief This function gets the beats per minute.
 * 
 * @return Beats per minute.
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
 */
vector<fundamentalsFreqs> globalValues::getFreqs()
{
    return freqs;
}

/** Get JSON function
 * 
 * @brief This function gets the JSON of the global values.
 * 
 * @return JSON of the global values.
 */
String globalValues::getJson(int size_of_HR, int size_of_SPO2)
{
    String json = "{";
    json += "\"heartRateDataArray\": [";
    for(int i = 0; i < size_of_HR; i++)
    {
        json += String(heartRateDataArray[i]);
        if(i != size_of_HR - 1)
        {
            json += ", ";
        }
    }
    json += "],";
    json += "\"spo2DataArray\": [";
    for(int i = 0; i < size_of_SPO2; i++)
    {
        json += String(spo2DataArray[i]);
        if(i != size_of_SPO2 - 1)
        {
            json += ", ";
        }
    }
    json += "],";
    json += "\"beatsPerMinute\": " + String(beatsPerMinute) + ",";
    json += "\"spo2Percentage\": " + String(spo2Percentage) + ",";
    json += "\"freqsAmplitude\": [";
    for(int i = 0; i < freqs.size(); i++)
    {
        json += String(freqs[i].amplitude);
        if(i != freqs.size() - 1)
        {
            json += ", ";
        }
    }
    json += "],";
    json += "\"freqsHz\": [";
    for(int i = 0; i < freqs.size(); i++)
    {
        json += String(freqs[i].freqsHz);
        if(i != freqs.size() - 1)
        {
            json += ", ";
        }
    }
    json += "]";
    json += "}";

    return json;
}

/** Get JSON single values function
 * 
 * @brief This function gets the JSON of the global values.
 * 
 * @return JSON of the global values.
 */
String globalValues::getJsonSingleValues(int size_of_heartrate, int size_of_spo2)
{
    if (pointerHeartRateDataArray == (size_of_heartrate - 1))
    {
        //pointerHeartRateDataArray --;
        pointerHeartRateDataArray = 0;
    }
    if (pointerSpo2DataArray == (size_of_spo2 - 1)){
        //pointerSpo2DataArray --;
        pointerSpo2DataArray = 0; 
    }
    String json = "{";
    json += "\"heartRateData\":" + String(heartRateDataArray[pointerHeartRateDataArray]) + ",";
    json += "\"spo2Data\": " + String(spo2DataArray[pointerSpo2DataArray]) + ", ";
    json += "\"beatsPerMinute\": " + String(beatsPerMinute) + ", ";
    json += "\"spo2Percentage\": " + String(spo2Percentage) + ", ";
    // Freqs
    json += "\"freqsAmplitude\": [";
    for(int i = 0; i < freqs.size(); i++)
    {
        json += String(freqs[i].amplitude);
        if(i != freqs.size() - 1)
        {
            json += ", ";
        }
    }
    json += "], ";
    json += "\"freqsHz\": [";
    for(int i = 0; i < freqs.size(); i++)
    {
        json += String(freqs[i].freqsHz);
        if(i != freqs.size() - 1)
        {
            json += ", ";
        }
    }
    json += "]";
    json += "}";

    //Increment pointers
    pointerHeartRateDataArray ++;
    pointerSpo2DataArray++;
    
    return json;
}