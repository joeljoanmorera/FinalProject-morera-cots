#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>
#include <vector>

namespace std{
    /** Button class
     * 
     * @brief This class is the button of the device.
     * 
     * @details This class is used to manage the button of the device.
     * 
     * @param pin Pin of the button
     * @param actualValue Actual value of the button
     * @param previousValue Previous value of the button
     * @param actualChange Actual change of the button
     * @param previousChange Previous change of the button
     * @param order Order of the button
     *
     */
    class Button{
        public:
            uint8_t pin;
            uint8_t actualValue;
            uint8_t previousValue;
            uint8_t actualChange;
            uint8_t previousChange;
            uint8_t order;

            Button();

            Button(uint8_t pPin);

            Button& operator =(const Button& B);
    };

    /** Buttons array class
     * 
     * @brief This class is the buttons array of the device.
     *
     * @details This class is used to manage the buttons array of the device.
     * 
     * @param buttons Buttons array
     * @param numButtons Number of buttons
     * @param timer Timer for the buttons
     * 
     */
    class buttonsArray{
        public:

            buttonsArray();

            buttonsArray& operator =(const buttonsArray& B);

            Button& operator [](int i);

            void begin(vector<int> buttonPins, bool defaultOrder = true);

            void readButtons();
        
        private:
            vector<Button> buttons;
    };
}

#endif /* BUTTON_H */