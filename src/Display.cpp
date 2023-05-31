#include "Display.h"

using namespace std;

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
 * @param longAxis Choice of the X axis to draw. By default is false.
 *
 */
void Display::drawAxis(bool longAxis)
{
    this -> drawLine(xAxisBegin, yAxisBegin, xAxisBegin, yAxisEnd);                     // Y-axis
    if (longAxis) this -> drawLine(xAxisBegin, yAxisEnd, xAxisEnd + margin, yAxisEnd);  // X-long-axis
    else this -> drawLine(xAxisBegin, yAxisEnd, xAxisEnd, yAxisEnd);                    // X-axis
}

/** Draw data function
 * 
 * @brief This function draws the data in the display.
 *
 * @param dataVector Data to draw.
 * @param choiceBPM Choice of the data to draw.
 */
void Display::drawData(vector<uint32_t> dataVector, bool choiceBPM)
{
    uint32_t lastHeight = 0;
    uint32_t windowSize = this -> getDataWindowSize();
    if (choiceBPM)
    {
        for (uint32_t i = 0; i < windowSize; i++)
        {
            this -> drawLine(this -> xAxisBegin + i, lastHeight, this -> xAxisBegin + i, halfHeight - dataVector[i]);
            lastHeight = halfHeight - dataVector[i];
        }
    }
    else
    {
        for (uint32_t i = 0; i < windowSize; i++)
        {
            this -> drawLine(xAxisBegin + i, lastHeight, xAxisBegin + i, yAxisEnd - margin - dataVector[i]);
            lastHeight = yAxisEnd - margin - dataVector[i];
        }
    }
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

/** Draw bars function
 * 
 * @brief This function draws the bars in the display.
 *
 * @param labels Labels to print.
 * @param normalizedAmplitudes Normalized amplitudes to print.
 * 
 * @note This function expects the labels and the normalized amplitudes to be in the same order.
 *       Also, amplitudes must be normalized between 0 and 1.
 *
 */
void Display::drawBars ( const vector<String>& labels, const vector<float>& normalizedAmplitudes )
{
    this -> setFont(u8g2_font_tinyunicode_tf);

    if(labels.size() != normalizedAmplitudes.size())
    {
        Serial.println("Error: labels and normalizedAmplitudes must have the same size");
        return;
    }

    uint32_t xAxisScale = (xAxisEnd + margin/2)/labels.size();
    uint32_t yAxisStep = yAxisEnd/labels.size();
    for (uint8_t i = 0; i < labels.size() ; i++)
    {
        // Plot bar
        uint32_t xAxisPlotBegin = xAxisScale*i + xAxisScale/2;
        uint32_t xWidth = int(xAxisPlotBegin + xAxisScale/5);
        uint32_t amplitude = uint32_t(yAxisEnd*normalizedAmplitudes[i]);

        for (uint32_t j = 0; j < amplitude; j++)
        {
            this -> drawLine(xAxisPlotBegin, yAxisEnd -j, xWidth ,yAxisEnd -j);
        }
        // Plot label
        this -> setCursor(xAxisEnd + 2*margin, yAxisStep * i + 4/3*margin);
        this -> print(labels[i]);
    }
}

/** Get data window size function
 * 
 * @brief This function returns the size that the data should have in order to be correctly visualized.
 *
 * @return Data window size.
 */
uint32_t Display::getDataWindowSize()
{
    return xAxisEnd - xAxisBegin;
}