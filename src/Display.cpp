#include "Display.h"

/** Init function
 * 
 * @brief This function initializes the display.
 *
 */
void Display::init()
{
    // Display initialization
    begin();                         // Inicialitzate
    setContrast (10);                // Contraste
    enableUTF8Print();               // Visualize UTF-8 characters
    setFont(u8g2_font_6x10_tf);      // Font

    //Axis
    uint8_t height = getDisplayHeight(); // Get display height : 64
    uint8_t width = getDisplayWidth(); // Get display width : 128
    
    xAxisBegin      = margin/4;
    xAxisEnd        = width  - width/2;
    yAxisBegin      = margin/4;
    yAxisEnd        = height - margin/2;
    halfHeight      = height/2;
}

/** Draw axis in the display function
 * 
 * @brief This function draws the axis in the display.
 *
 */
void Display::drawAxis()
{
    this -> drawLine(xAxisBegin, yAxisBegin, xAxisBegin, yAxisEnd);                // Y-axis
    this -> drawLine(xAxisBegin, yAxisEnd, xAxisEnd, yAxisEnd);                    // X-axis
}

/** Draw long axis in the display function
 * 
 * @brief This function draws the long axis in the display.
 *
 */
void Display::drawLongAxis()
{
    this -> drawLine(xAxisBegin, yAxisBegin, xAxisBegin, yAxisEnd);                // Y-axis
    this -> drawLine(xAxisBegin, yAxisEnd, xAxisEnd + margin, yAxisEnd);                  // X-axis
}

/** Print measurements function
 * 
 * @brief This function prints the measurements in the display.
 *
 * @param value Value to print.
 * @param choiceBPM Choice of the measurement to print.
 */
void Display::printMeasurements(int32_t value, bool choiceBPM)
{
    this -> setFont(u8g2_font_luBS10_tf);
    String valueString = String(value);

    if (choiceBPM){
        this -> setCursor(this -> xAxisEnd + 20, this -> halfHeight + 2*margin);
        this -> print("BPM");
    }else {
        valueString += " %";
        this -> setCursor(this -> xAxisEnd + 20, this -> halfHeight + 2*margin);
        this -> print("SPO2");
    }

    if (valueString.length() >= 3){
        this -> setCursor(this -> xAxisEnd + 20, this -> halfHeight );
    }else {
        this -> setCursor(this -> xAxisEnd + 20 + margin, this -> halfHeight);
    }
    this -> print(valueString);
}

/** Get max value function
 * 
 * @brief This function gets the max value.
 *
 * @param dataVector Data to get the max value.
 * @return Max value.
 */
uint32_t Display::getMaxValue(uint32_t* dataVector)
{
    uint32_t max = 0;
    for (uint8_t i = 0; i < xAxisEnd - xAxisBegin; i++)
    {
        if (dataVector[i] > max)
        {
            max = dataVector[i];
        }
    }
    return max;
}

/** Discretize data function
 * 
 * @brief This function discretizes the data.
 *
 * @param dataVector Data to discretize.
 * @return Discretized data.
 */
uint32_t* Display::discretizeData(uint32_t* dataVector, bool choiceBPM)
{
    uint32_t* discretizedDataVector = new uint32_t[xAxisEnd - xAxisBegin];
    uint32_t max = getMaxValue(dataVector);
    uint32_t yAxisScale = 1;
    if (choiceBPM){
        yAxisScale =uint32_t(max/(halfHeight - margin));
    } else {
        yAxisScale = uint32_t(max/(yAxisEnd - margin)); 
    } 

    if (yAxisScale == 0)yAxisScale = 1;

    for (uint32_t i = 0; i < xAxisEnd - xAxisBegin; i++)
    {
        discretizedDataVector[i] = uint32_t(dataVector[i]/yAxisScale);
    }
    return discretizedDataVector;
}

/** Draw data function
 * 
 * @brief This function draws the data in the display.
 *
 * @param dataVector Data to draw.
 * @param choiceBPM Choice of the data to draw.
 */
void Display::drawData(uint32_t* dataVector, bool choiceBPM)
{
    uint32_t lastHeight = 0;
    if (choiceBPM)
    {
        for (uint32_t i = 0; i < xAxisEnd - xAxisBegin; i++)
        {
            this -> drawLine(this -> xAxisBegin + i, lastHeight, this -> xAxisBegin + i, halfHeight - dataVector[i]);
            lastHeight = halfHeight - dataVector[i];
        }   
    }
    else
    {
        for (uint32_t i = 0; i < xAxisEnd - xAxisBegin; i++)
        {
            this -> drawLine(xAxisBegin + i, lastHeight, xAxisBegin + i, yAxisEnd - margin - dataVector[i]);
            lastHeight = yAxisEnd - margin - dataVector[i];
        }
    }
}

/** Update data function
 * 
 * @brief This function updates the data in the display.
 *
 * @param array Array of data to update.
 * @param value Value to update.
 * @param choiceBPM Choice of the data to update.
 */
void Display::updateData(uint32_t* array, int32_t value, bool choiceBPM)
{
    this -> drawAxis();
    uint32_t* discretizedArray = this -> discretizeData(array, choiceBPM);
    this -> drawData(discretizedArray, choiceBPM);
    this -> printMeasurements(value, choiceBPM);
}

/** Get max amplitude function
 * 
 * @brief This function gets the max amplitude.
 *
 * @param freqs Vector of frequencies.
 * @return Max amplitude.
 */
int Display::getMaxAmplitude(const vector<fundamentalsFreqs>& freqs)
{
    int max = 0;
    for (uint8_t i = 0; i < freqs.size(); i++)
    {
        if (freqs[i].amplitude > max)
        {
            max = freqs[i].amplitude;
        }
    }
    return max;
}

/** Draw frequencies function
 * 
 * @brief This function draws the frequencies in the display.
 *
 * @param freqs Vector of frequencies to draw.
 */
void Display::drawFreqs(const vector<fundamentalsFreqs>& freqs)
{
    int max = getMaxAmplitude(freqs);
    uint32_t yAxisScale = max/yAxisEnd;
    uint32_t xAxisScale = (xAxisEnd + margin/2)/freqs.size();
    uint32_t yAxisStep = yAxisEnd/freqs.size();
    this -> setFont(u8g2_font_tinyunicode_tf);

    for (uint8_t i = 0; i < freqs.size() ; i++)
    {
        // Plot amplitude of each freq
        uint32_t xAxisPlotBegin = xAxisScale*i + xAxisScale/2;
        uint32_t xWidth = int(xAxisPlotBegin + xAxisScale/5);
        uint32_t totalPixelValues = int(freqs[i].amplitude/yAxisScale);

        for (uint32_t j = 0; j < totalPixelValues; j++)
        {
            this -> drawLine(xAxisPlotBegin, yAxisEnd -j, xWidth ,yAxisEnd -j);
        }

        // Plot freq in Hz
        this -> setCursor(xAxisEnd + 2*margin, yAxisStep * i + 4/3*margin);
        this -> print(freqs[i].freqsHz);
    }
}

/** Update frequencies function
 * 
 * @brief This function updates the frequencies in the display.
 *
 * @param freqs Vector of frequencies to update.
 */
void Display::updateFreqs(const vector<fundamentalsFreqs>& freqs)
{
    this -> drawLongAxis();
    this -> drawFreqs(freqs);
}
