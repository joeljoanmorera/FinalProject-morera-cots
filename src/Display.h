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
        public:
            uint32_t xAxisBegin, xAxisEnd, yAxisBegin, yAxisEnd, halfHeight;
            uint32_t margin = 8;
            // TODO: set the margin as a percentage of the total width, ex: 12.5% == 8

            using U8G2_ST7565_ERC12864_1_4W_SW_SPI::U8G2_ST7565_ERC12864_1_4W_SW_SPI;

            void init ();

            void drawAxis ( bool longAxis = false );

            void drawData ( vector<uint32_t> dataVector, bool choiceBPM );

            void printMeasurements (int32_t value, bool choiceBPM );

            void drawBars ( const vector<String>& labels, const vector<float>& amplitudes );

            uint32_t getDataWindowSize();
    };
}
#endif /* DISPLAY_H */