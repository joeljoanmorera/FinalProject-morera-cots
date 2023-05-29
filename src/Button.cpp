#include <Button.h>

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
 * @param PPIN Pin of the button.
 */
Button::Button(uint8_t PPIN)
{
      pin = PPIN;
      val_ant = 1;
      orden = 0;
}

/** Button = operator
 * 
 * @brief This function is the = operator of the button.
 *
 * @param B Button.
 * @return Button.
 */
Button& Button::operator =(const Button& B)
{
    if (this != &B)
      {
        this -> pin = B.pin;
        this -> val_act = B.val_act;
        this -> val_ant = B.val_ant;
        this -> cambioact = B.cambioact;
        this -> cambioanterior = B.cambioanterior;
        this -> orden = B.orden;
      }
      return(*this);
}

/** buttonsArray default constructor
 * 
 * @brief This function is the constructor of the buttonsArray.
 *
 */
buttonsArray::buttonsArray(){}

/** buttonsArray constructor
 * 
 * @brief This function is the constructor of the buttonsArray.
 *
 * @param pNumButtons Number of buttons.
 */
buttonsArray::buttonsArray(uint8_t pNumButtons)
{
    numButtons = pNumButtons;
    buttons = new Button[numButtons];
}

/** buttonsArray = operator
 * 
 * @brief This function is the = operator of the buttonsArray.
 *
 * @param B Button.
 * @return Button.
 */
buttonsArray& buttonsArray::operator =(const buttonsArray& B)
{
    if (this != &B)
      {
        this -> numButtons = B.numButtons;
        this -> buttons = B.buttons;
      }
      return(*this);
}

/** buttonsArray [] operator
 * 
 * @brief This function is the [] operator of the buttonsArray.
 *
 * @param i Index.
 * @return Button.
 */
Button& buttonsArray::operator [](int i)
{
    return buttons[i];
}

/** buttonsArray begin function
 * 
 * @brief This function is the begin function of the buttonsArray.
 *
 */
void buttonsArray::begin(int* buttonPins)
{
    //Buttons definition
    Button *buttons_temp = new Button[numButtons];
    for(uint8_t i = 0; i < numButtons; i++)
    {
        buttons_temp[i] = Button(buttonPins[i]);
    }
    buttons = buttons_temp;

    //Buttons'pins initialization
    for(uint8_t i = 0; i < numButtons; i++)
    {
        pinMode(buttonPins[i], INPUT_PULLUP);
    }

    // Default order
    buttons[0].orden = 1;
}


