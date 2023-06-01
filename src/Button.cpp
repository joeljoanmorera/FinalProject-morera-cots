#include <Button.h>

using namespace std;

/** Button default constructor
 * 
 * @brief This function is the constructor of the button.
 *
 */
Button::Button(){}

/** Button constructor
 * 
 * @brief This function is the constructor of the button.
 * 
 * @param pPin Pin of the button.
 * 
 */
Button::Button(uint8_t pPin)
{
    pin = pPin;
    previousValue = 1;
    order = 0;
}

/** Button = operator
 * 
 * @brief This function is the = operator of the button.
 *
 * @param B Button.
 * 
 * @return Button.
 * 
 */
Button& Button::operator =(const Button& B)
{
    if (this != &B)
    {
        this -> pin = B.pin;
        this -> actualValue = B.actualValue;
        this -> previousValue = B.previousValue;
        this -> actualChange = B.actualChange;
        this -> previousValue = B.previousValue;
        this -> order = B.order;
    }
    return(*this);
}

// BUTTONS ARRAY //

/** buttonsArray default constructor
 * 
 * @brief This function is the default constructor of the buttonsArray.
 *
 */
buttonsArray::buttonsArray(){
    buttons.clear();
}

/** buttonsArray = operator
 * 
 * @brief This function is the = operator of the buttonsArray.
 *
 * @param B Button.
 * 
 * @return Button.
 * 
 */
buttonsArray& buttonsArray::operator =(const buttonsArray& B)
{
    if (this != &B)
    {
        this -> buttons = B.buttons;
    }
    return(*this);
}

/** buttonsArray [] operator
 * 
 * @brief This function is the [] operator of the buttonsArray.
 *
 * @param i Index.
 * 
 * @return Button.
 * 
 */
Button& buttonsArray::operator [](int i)
{
    return buttons[i];
}

/** buttonsArray begin function
 * 
 * @brief This function is the begin function of the buttonsArray.
 *
 * @param buttonPins Buttons' pins array.
 * @param defaultOrder Choice of the adding a default order
 * 
 */
void buttonsArray::begin(vector<int> buttonPins, bool defaultOrder)
{
    //Buttons definition
    for(uint8_t i = 0; i < buttonPins.size(); i++)
    {
        buttons.push_back(Button(uint8_t(buttonPins[i])));
    }
    //Buttons'pins initialization
    for(uint8_t i = 0; i < buttonPins.size(); i++)
    {
        pinMode(buttonPins[i], INPUT_PULLUP);
    }
    // Default order
    if(defaultOrder)buttons[0].order = 1;
}

/** Button management function
 * 
 * @brief This function reads the buttons.
 *  
 * @return void.
 *  
 * @details This function reads the buttons' values and changes the order of the buttons.
 *  
 * @note This function is called when the timer is activated. 
 * 
 */
void buttonsArray::readButtons()
{ 
    for(uint8_t i = 0; i < buttons.size(); i++)
    {
        buttons[i].actualValue = digitalRead(buttons[i].pin);                        // Read the value of the button
        buttons[i].actualChange = buttons[i].previousValue ^ buttons[i].actualValue; // XOR of actual value and last value
        if(buttons[i].actualChange == 1 && buttons[i].previousChange == 1)           // If both status changes are equal to 1
        {
            buttons[i].order = 1;                                                    // Order to 1
            for (uint8_t j = 0; j < buttons.size(); j++)
            {
                if (j != i)buttons[j].order = 0;                                     // Rest of orders to 0 
            }
            buttons[i].previousValue = buttons[i].actualValue;                       // Last value equal to actual value
            buttons[i].previousChange = 0;                                           // Last status change equal to 0
            return;
        }
        buttons[i].previousChange = buttons[i].actualChange;                         // Last status change is equal to acutal change
    }
}
