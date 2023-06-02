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
 * 
 * @see Display(), webPage(), buttonsArray().
 * 
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
 * 
 * @see buttonsArray::begin(), Display::init(), webPage::begin().
 * 
 */
void globalDataVisualizer::setup ( vector<int> buttonPins, const char* ssid, const char* password )
{
    buttons.begin(buttonPins);
    display.init();
    page.begin(ssid, password);
}

void globalDataVisualizer::workInProgressMessage()
{
    display.setFont(u8g2_font_luBS10_tf);
    display.setCursor(display.getDisplayWidth()/4, display.getDisplayHeight()/2);
    display.print("Calculating...");
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
    globalValuesVar.shiftHeartRate();
    delay(700);
}

/** Generate display visualization function
 * 
 * @brief This function generates the display visualization.
 *
 * @param globalValuesVar Global values.
 * 
 * @see defaultDataVisualitzation(), frequenciesDataVisualitzation().
 */
void globalDataVisualizer::generateDisplayVisualization ( globalValues& globalValuesVar )
{
    Serial.println("Display visuializing: ");
    if (buttons[0].order){
        Serial.println("Heart Rate");
        defaultDataVisualitzation( globalValuesVar, display.getDataWindowSize(), true );
    } else {
        if(buttons[1].order){
            Serial.println("SPO2");
            defaultDataVisualitzation( globalValuesVar, display.getDataWindowSize(), false );
        } else {
            if(buttons[2].order){
                Serial.println("Frequencies");
                frequenciesDataVisualitzation(globalValuesVar);
            }
        }
    }   
}

/** Default data visualitzation function
 * 
 * @brief This function visualizes the default data.
 *
 * @param globalValuesVar Global values.
 * @param windowSize Size of the window.( N first values that will be visualized)
 * @param heartRateType Choice of the data to visualize.
 * 
 * @details This function discretizes the data and visualizes it.
 * 
 * @see defaultDiscretization(), getLabeledFrequency(), getDisplayStyleFundamentalsFrequencies().
 */
void globalDataVisualizer::defaultDataVisualitzation ( globalValues& globalValuesVar, uint32_t windowSize, bool heartRateType )
{
    display.drawAxis();
    vector<uint32_t> data = globalValuesVar.getHeartRateDataArray(windowSize);
    uint32_t value;
    if ( heartRateType ) value = globalValuesVar.getBeatsPerMinute();
    else value = globalValuesVar.getSpo2Percentage();

    vector<uint32_t> discretizedDataVector = defaultDiscretization( data );
    display.drawData( discretizedDataVector );
    display.printMeasurements(value, heartRateType);
}

/** Default discretization function
 * 
 * @brief This function discretizes the data.
 *
 * @param data Data to discretize.
 * 
 * @details This function discretizes the data by normalizing it in realation to the maximum value divided by the Y axis bias.
 *          From that we will obtain the height of each value in pixels, being the maximum value the highest permitted value.
 * 
 * @return Discretized data.
 */
vector<uint32_t> globalDataVisualizer::defaultDiscretization ( vector<uint32_t> data )
{
    vector<uint32_t> discretizedDataVector;
    uint32_t max = getMaxValue(data);
    uint32_t yBias = display.getYAxisBias();
    uint32_t yAxisScale = uint32_t(max/yBias);

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
 * 
 * @see defaultDiscretization().
 * 
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
 * 
 * @see getDisplayStyleFundamentalsFrequencies(), getLabeledFrequency().
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
 * @details This function normalizes the amplitudes and adds them to the amplitudes vector. The normalized amplitude should be
 *          a float number between 0 and 1. The labels vector is filled with the frequencies in Hz, kHz or MHz depending on the
 *         magnitude of the frequency.
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
String globalDataVisualizer::getLabeledFrequency ( float data )
{
    String labelHz = String(data) + " ";
    if (data/1000 >= 1)
    {
        labelHz = String(data/1000) + " K";
    }
    if (data/1000000 >= 1)
    {
        labelHz = String(data/1000000) + " M";
    }
    labelHz += "Hz";
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
 * @details This function returns the JSON of the global values. The JSON is structured as follows:
 *         {
 *              "heartRateData": firstHeartRateData,
 *              "spo2Data": firstSpo2Data,
 *              "beatsPerMinute": beatsPerMinute,
 *              "spo2Percentage": spo2Percentage,
 *              "freqsAmplitude": [freqsAmplitude],
 *              "freqsHz": [labeledFreqsHz]
 *          }
 * 
 * @see getLabeledFrequency().
 * 
 */
String globalDataVisualizer::getJSON ( globalValues& globalValuesVar )
{
    String json = "{";
    json += "\"heartRateData\":" + String(globalValuesVar.getFirstValueHeartRate()) + ",";
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