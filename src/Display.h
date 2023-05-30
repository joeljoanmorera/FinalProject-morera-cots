#ifndef DISPLAY_H
#define DISPLAY_H

#include <vector>
#include "GlobalValues.h"
#include <U8g2lib.h>

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

        using U8G2_ST7565_ERC12864_1_4W_SW_SPI::U8G2_ST7565_ERC12864_1_4W_SW_SPI;

        void init ();

        void updateData ( uint32_t *array, int32_t value, bool choiceBPM );

        void drawAxis ( bool longAxis = false );

        uint32_t *discretizeData ( uint32_t *dataVector, bool choiceBPM );
        
        uint32_t getMaxValue ( uint32_t *dataVector ) ;

        void drawData ( uint32_t *dataVector, bool choiceBPM );

        void printMeasurements (int32_t value, bool choiceBPM );

        void updateFreqs ( const vector<fundamentalsFreqs> &freqs );

        void drawFreqs ( const vector<fundamentalsFreqs> &freqs );

        int getMaxAmplitude ( const vector<fundamentalsFreqs> &freqs );
};

#endif /* DISPLAY_H */