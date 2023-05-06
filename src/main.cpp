 #include <U8g2lib.h>
#include <SPI.h>

// Pin's definition
#define SCL 18
#define SI 23
#define CS 5
#define RS 32
#define RSE 33

// U8g2 Contructor
U8G2_ST7565_ERC12864_1_4W_SW_SPI u8g2(U8G2_R0, SCL, SI, CS, RS, RSE);

// Display size
const uint8_t width = u8g2.getDisplayWidth(); // 128 pixels
const uint8_t height = u8g2.getDisplayHeight(); // 64 pixels

// Axis
int8_t xAxisBegin, xAxisEnd, yAxisBegin, yAxisEnd, heartRateYBias, spo2YBias;

// Data
int8_t* heartRateData;
int8_t* spo2Data;

//Function declaration
void plot();
void fillData();
void plotData();
void printMeasurements();

void setup(void) 
{
    // Serial initialization
    Serial.begin(115200);

    // Axis
    xAxisBegin  = width  - 126;
    xAxisEnd    = width  - width/2;
    yAxisBegin  = height - 62;
    yAxisEnd    = height - 4;
    heartRateYBias = height/3; //- 40
    spo2YBias = height - 6;

    // Data initialization
    fillData();

    // Display initialization
    u8g2.begin(); // Inicializa
    u8g2.setContrast (10); //contraste
    u8g2.enableUTF8Print(); //Visuliza caracteres UTF-8
}

void loop(void) 
{
    u8g2.firstPage(); // First page
    do {
        plot();
        printMeasurements();
    }while(u8g2.nextPage());

    delay(1000); //Wait 1000ms
}

void printMeasurements()
{
    u8g2.setFont(u8g2_font_luBS10_tf );
    u8g2.setCursor(xAxisEnd + 8, heartRateYBias );
    u8g2.print("60 BPM");
    u8g2.setCursor(xAxisEnd + 20, spo2YBias );
    u8g2.print("98 \% ");
}

void plot()
{
    u8g2.drawLine(xAxisBegin, yAxisBegin, xAxisBegin, yAxisEnd);                // Y-axis
    u8g2.drawLine(xAxisBegin, yAxisEnd, xAxisEnd, yAxisEnd);                    // X-axis
    plotData();
}

void fillData()
{   
    int8_t* heartRateData_temp = new int8_t[xAxisEnd - xAxisBegin];
    int8_t* spo2Data_temp = new int8_t[xAxisEnd - xAxisBegin];

    int j = 0;
    for (uint8_t i = 0; i < xAxisEnd; i++)
    {
        // Little mountain in the middle with a value of yAxisEnd
        if (i < xAxisEnd/4)
        {
            spo2Data_temp[i] = 0;
        }
        else if (i < xAxisEnd/2)
        {
            spo2Data_temp[i] =  j;
            j++;
        }
        else if (i < 3*xAxisEnd/4)
        {
            j--;
            spo2Data_temp[i] = j;
        }
        else
        {
            spo2Data_temp[i] = 0;
        }
    }

    for (uint8_t i = 0; i < xAxisEnd; i++)
    {
        // Little mountain in the middle with a value of yAxisEnd
        if (i < xAxisEnd/4)
        {
            heartRateData_temp[i] = 0;
        }
        else if (i < 3*xAxisEnd/8)
        {
            heartRateData_temp[i] =  j;
            j+=2;
        }
        else if (i < 5*xAxisEnd/8)
        {
            j-=2;
            heartRateData_temp[i] = j;
            
        }
        else if (i < 3*xAxisEnd/4)
        {
            j+=2;
            heartRateData_temp[i] = j;
        }
        else
        {
            heartRateData_temp[i] = 0;
        }
    }

    heartRateData = heartRateData_temp;
    spo2Data = spo2Data_temp;
}

void plotData()
{
    for (uint8_t i = 0; i < xAxisEnd; i++)
    {
        u8g2.drawPixel(xAxisBegin + i, heartRateYBias - heartRateData[i]);
        u8g2.drawPixel(xAxisBegin + i, spo2YBias - spo2Data[i]);
    }
}

//Haz hasta que se devuelva 1
        //u8g2.setFont(u8g2_font_luBS10_tf );
        // establece font Lúcida a 10 pixel
        //u8g2.drawFrame(0,0,128,64);
        // Dibujar un borde de 64 x 128 pixels
        //u8g2.setCursor(6, 25);
        // Pon cursor en x = 6, y = 25
        //u8g2.print("HOLA MUNDO !");
        //Muestra esta cadena
        //u8g2.setCursor(6, 40);
        // Set en x = 6, y = 40
        //u8g2.setCursor(14, 55);
        // Set en x = 14, y = 55
        //u8g2.print("BUENOS DIAS ");
        //Muestra la cadena