#ifndef DATAVISUALIZER_H
#define DATAVISUALIZER_H

#include "GlobalValues.h"
#include "Display.h"
#include "WebPage.h"
#include "Button.h"
namespace std {
    class globalDataVisualizer
    {
        public:
            globalDataVisualizer ( const u8g2_cb_t *rotation, uint8_t clock, 
                                   uint8_t data, uint8_t cs, uint8_t dc, uint8_t reset, int port = 80 );

            void setup ( vector<int> buttonPins, const char* ssid, const char* password );

            void generateVisualization( globalValues& globalValuesVar );

            void generateDisplayVisualization ( globalValues& globalValuesVar );

            void defaultDataVisualitzation ( globalValues& globalValuesVar, uint32_t windowSize, bool heartRateType);

            vector<uint32_t> defaultDiscretization ( vector<uint32_t> data, bool heartRateType );

            uint32_t getMaxValue( vector<uint32_t> data );

            void frequenciesDataVisualitzation ( globalValues& globalValuesVar );

            void getDisplayStyleFundamentalsFrequencies ( vector<fundamentalsFreqs> data, vector<String>& labels, vector<float>& amplitudes );

            String getLabeledFrequency(float data, bool withUnit = true);

            float getMaxAmplitude( const vector<fundamentalsFreqs>& freqs );

            String getJSON ( globalValues& globalValuesVar );

            buttonsArray buttons;

        private:

            Display display;
            webPage page;
    };
}

#endif  /* DATAVISUALIZER_H */
