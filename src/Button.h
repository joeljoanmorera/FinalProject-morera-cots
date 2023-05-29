#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

/** Button class
 * 
 * @brief This class is the button of the device.
 *
 */
class Button{
  public:
    uint8_t pin;                                              
    bool val_act, val_ant, cambioact, cambioanterior, orden;  
    
    Button();

    Button(uint8_t PPIN);

    Button& operator =(const Button& B);
};

/** Buttons array class
 * 
 * @brief This class is the buttons array of the device.
 *
 */
class buttonsArray{
    public:
        Button* buttons; 
        uint8_t numButtons;
    
        buttonsArray();
    
        buttonsArray(uint8_t pNumButtons);
    
        buttonsArray& operator =(const buttonsArray& B);

        Button& operator [](int i);

        void begin(int* buttonPins);

};

#endif /* BUTTON_H */