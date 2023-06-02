#ifndef DISPLAY_H
#define DISPLAY_H

#include <vector>
#include <U8g2lib.h>
#include <SPI.h>

namespace std
{
    /** Display class
     *
     * @brief This class is the display of the device.
     *
     * @details This class is used to manage the display of the device.
     *
     * @param xAxisBegin X axis begin
     * @param xAxisEnd X axis end
     * @param yAxisBegin Y axis begin
     * @param yAxisEnd Y axis end
     * @param halfHeight Half height of the display
     * @param margin Margin of the display
     *
     */
    class Display : public U8G2_ST7565_ERC12864_1_4W_SW_SPI{
        uint32_t xAxisBegin, xAxisEnd, yAxisBegin, yAxisEnd, halfHeight, margin;
        
        public:
            using U8G2_ST7565_ERC12864_1_4W_SW_SPI::U8G2_ST7565_ERC12864_1_4W_SW_SPI;

            void init ();

            void drawAxis ( bool longAxis = false );

            void drawData ( vector<uint32_t> dataVector );

            void printMeasurements (int32_t value, bool heartRateType );

            void drawBars ( const vector<String>& labels, const vector<float>& amplitudes );
            
            uint32_t getYAxisBias ( );

            uint32_t getDataWindowSize ();
    };
}
#endif /* DISPLAY_H */