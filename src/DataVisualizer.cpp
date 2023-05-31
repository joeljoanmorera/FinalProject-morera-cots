#include "DataVisualizer.h"

using namespace std;

/** Global data visualizer constructor
 * 
 * @brief This constructor initializes the global data visualizer.
 *
 * @param numButtons Number of buttons.
 * @param rotation Rotation of the display.
 * @param clock Clock pin of the display.
 * @param data Data pin of the display.
 * @param cs CS pin of the display.
 * @param dc DC  pin of the display.
 * @param reset Reset pin of the display.
 */
globalDataVisualizer::globalDataVisualizer ( const u8g2_cb_t *rotation, uint8_t clock, 
                                             uint8_t data, uint8_t cs, uint8_t dc, uint8_t reset, int port ):
                                             display(rotation, clock, data, cs, dc, reset ), page(port), buttons(){}

/** Setup function
 * 
 * @brief This function setups the global data visualizer.
 *
 * @param buttonPins Pins of the buttons.
 * @param ssid SSID of the network.
 * @param password Password of the network.
 */
void globalDataVisualizer::setup ( vector<int> buttonPins, const char* ssid, const char* password )
{
    buttons.begin(buttonPins);
    display.init();
    page.begin(ssid, password);
}

/** Generate visualization function
 * 
 * @brief This function generates the visualization.
 *
 * @param globalValuesVar Global values.
 */
void globalDataVisualizer::generateVisualization( globalValues& globalValuesVar )
{
    // Visualize in display
    display.firstPage();
    while (display.nextPage()) generateDisplayVisualization(globalValuesVar);
    // Visualize in web page
    String jsonMessage = getJSON(globalValuesVar);
    page.sendWsMessage(jsonMessage);  
    delay(700);
}

/** Generate display visualization function
 * 
 * @brief This function generates the display visualization.
 *
 * @param globalValuesVar Global values.
 */
void globalDataVisualizer::generateDisplayVisualization ( globalValues& globalValuesVar )
{
    Serial.println("Display visuializing: ");
    if (buttons[0].order){
        Serial.println("Heart Rate");
        defaultDataVisualitzation( globalValuesVar, display.getDataWindowSize(), true );
    }
    if(buttons[1].order){
        Serial.println("SPO2");
        defaultDataVisualitzation( globalValuesVar, display.getDataWindowSize(), false );
    }
    if(buttons[2].order){
        Serial.println("Frequencies");
        frequenciesDataVisualitzation(globalValuesVar);
    }       
}

/** Default data visualitzation function
 * 
 * @brief This function visualizes the default data.
 *
 * @param globalValuesVar Global values.
 * @param windowSize Size of the window.( N first values that will be visualized)
 * @param heartRateType Choice of the data to visualize.
 */
void globalDataVisualizer::defaultDataVisualitzation ( globalValues& globalValuesVar, uint32_t windowSize, bool heartRateType )
{
    display.drawAxis();
    vector<uint32_t> data;
    uint32_t value;
    if (heartRateType)
    {
        data = globalValuesVar.getHeartRateDataArray(windowSize);
        value = globalValuesVar.getBeatsPerMinute();
    }
    else
    {
        data = globalValuesVar.getSpo2DataArray(windowSize);
        value = globalValuesVar.getSpo2Percentage();
    }

    vector<uint32_t> discretizedDataVector = defaultDiscretization(data, heartRateType);
    display.drawData(discretizedDataVector, heartRateType);
    display.printMeasurements(value, heartRateType);
}

/** Default discretization function
 * 
 * @brief This function discretizes the data.
 *
 * @param data Data to discretize.
 * @param heartRateType Choice of the data to discretize.
 * 
 * @return Discretized data.
 */
vector<uint32_t> globalDataVisualizer::defaultDiscretization ( vector<uint32_t> data, bool heartRateType )
{
    vector<uint32_t> discretizedDataVector;
    uint32_t max = getMaxValue(data);
    uint32_t yAxisScale = 1;
    if (heartRateType){
        yAxisScale =uint32_t(max/(display.halfHeight - display.margin));
    } else {
        yAxisScale = uint32_t(max/(display.yAxisEnd - display.margin)); 
    } 

    if (yAxisScale == 0)yAxisScale = 1;
    
    for (uint32_t i = 0; i < data.size(); i++)
    {
        discretizedDataVector.push_back(uint32_t(data[i]/yAxisScale));
    }
    return discretizedDataVector;
}

/** Get max value function
 * 
 * @brief This function gets the max value.
 *
 * @param data Data to get the max value.
 * 
 * @return Max value.
 */
uint32_t globalDataVisualizer::getMaxValue( vector<uint32_t> data )
{
    uint32_t max = 0;
    for (uint8_t i = 0; i < data.size(); i++)
    {
        if (data[i] > max)
        {
            max = data[i];
        }
    }
    return max;
}

/** Frequencies data visualitzation function
 * 
 * @brief This function visualizes the frequencies data.
 *
 * @param globalValuesVar Global values.
 */
void globalDataVisualizer::frequenciesDataVisualitzation ( globalValues& globalValuesVar )
{
    vector<String> labels;
    vector<float> amplitudes;
    getDisplayStyleFundamentalsFrequencies(globalValuesVar.getFreqs(), labels, amplitudes);
    display.drawBars(labels, amplitudes);
    bool longAxis = true;
    display.drawAxis(longAxis);
}

/** Get display style fundamentals frequencies function
 * 
 * @brief This function returns the normalized frequencies'amplitude vector and the labels'vector.
 *
 * @param data Data to get the scaled fundamentals frequencies.
 * @param labels Vector of labels.
 * @param amplitudes Vector of amplitudes.
 * 
 * @see getLabeledFrequency(), frequenciesDataVisualitzation().
 * 
 */
void globalDataVisualizer::getDisplayStyleFundamentalsFrequencies ( vector<fundamentalsFreqs> data, vector<String>& labels, vector<float>& amplitudes )
{
    float max = getMaxAmplitude(data);
    for (uint8_t i = 0; i < data.size() ; i++)
    {
        // normalize and add amplitudes
        float normalizeFrequencyAmplitude = data[i].amplitude/max;
        amplitudes.push_back(normalizeFrequencyAmplitude);

        // casting to string and add labels
        String label = getLabeledFrequency(data[i].freqsHz);
        labels.push_back(label);
    }
}

/** Get labeled frequency
 * 
 * @brief This function returns a string with an appropiate style realted to its magnitude
 * 
 * @param data Data to convert into string
 * 
 * @return String of the input data
 * 
 * @see getDisplayStyleFudamentalsFrequencies(), getJSON().
 * 
*/
String globalDataVisualizer::getLabeledFrequency(float data, bool withUnit)
{

    String labelHz = String(data);
    if(withUnit)labelHz += " ";
    if (data/1000 >= 1)
    {
        //labelHz = "";
        labelHz = String(data/1000);
        if (withUnit)labelHz += " K";
    }
    if (data/1000000 >= 1)
    {
        //labelHz = "";
        labelHz = String(data/1000000);
        if (withUnit) labelHz +=" M";
    }
    if (withUnit)labelHz += "Hz";
    return labelHz;
}

/** Get max amplitude function
 * 
 * @brief This function gets the max amplitude.
 *
 * @param freqs Vector of frequencies.
 * 
 * @return Max amplitude.
 */
float globalDataVisualizer::getMaxAmplitude ( const vector<fundamentalsFreqs>& freqs )
{
    float max = 0;
    for (uint8_t i = 0; i < freqs.size(); i++)
    {
        if (freqs[i].amplitude > max)
        {
            max = freqs[i].amplitude;
        }
    }
    return max;
}

/** Get JSON function
 * 
 * @brief This function gets the JSON of the global values.
 * 
 * @param globalValuesVar Global values variable.
 * 
 * @return JSON of the global values.
 * 
 * @see getLabeledFrequency().
 * 
 */
String globalDataVisualizer::getJSON ( globalValues& globalValuesVar )
{
    String json = "{";
    json += "\"heartRateData\":" + String(globalValuesVar.getFirstValueAndShiftHeartRate()) + ",";
    json += "\"spo2Data\": " + String(globalValuesVar.getFirstValueAndShiftSpo2()) + ", ";
    json += "\"beatsPerMinute\": " + String(globalValuesVar.getBeatsPerMinute()) + ", ";
    json += "\"spo2Percentage\": " + String(globalValuesVar.getSpo2Percentage()) + ", ";
    json += "\"freqsAmplitude\": [";

    vector<fundamentalsFreqs> freqs = globalValuesVar.getFreqs();
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
    bool withUnit = false;
    for(int i = 0; i < freqs.size(); i++)
    {
        json += getLabeledFrequency(freqs[i].freqsHz, withUnit);
        if(i != freqs.size() - 1)
        {
            json += ", ";
        }
    }
    json += "]";
    json += "}";
    
    return json;
}