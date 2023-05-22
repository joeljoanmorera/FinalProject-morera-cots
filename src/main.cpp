 //CODI 1
 
  #include <Arduino.h>
  #include <Wire.h>
  #include "MAX30105.h"
  #include "SPIFFS.h"
  #include <vector>
  #include "heartRate.h"
  #include "spo2_algorithm.h"
  #include "arduinoFFT.h"


  using namespace std;

#define I2C_SPEED_FAST 400000 // Set I2C frequency to 400kHz
#define MAX_BRIGHTNESS 255 // Set maximum brightness

#define SAMPLES 64 // Número de muestras para la FFT
#define SAMPLING_FREQUENCY 25 // Frecuencia de muestreo en Hz

double vReal[SAMPLES];
double vImag[SAMPLES];

arduinoFFT FFT = arduinoFFT();

  MAX30105 particleSensor;

  static unsigned long lastTime1=0;
  static unsigned long lastTime2=0;

  
  vector<float> input_data;
  vector<float> input_data2;


  float vcoefs1[201];
  float vcoefs2[201];
  int i =0;
  int n = 0;
  float y = 0.0;
  float y2 = 0.0;
  bool fft=true;


uint32_t irBuffer[100]; //infrared LED sensor data
uint32_t redBuffer[100];  //red LED sensor 



int32_t bufferLength; //data length
int32_t spo2; //SPO2 value
int8_t validSPO2; //indicator to show if the SPO2 calculation is valid
int32_t heartRate; //heart rate value
int8_t validHeartRate; //indicator to show if the heart rate calculation is valid

struct fundamentalsFreqs{
    String freqsHz;
    int amplitude;
};


class globalValues {
    uint32_t* heartRateDataArray;
    uint32_t* spo2DataArray;
    int32_t beatsPerMinute, spo2Percentage;
    vector <fundamentalsFreqs> freqs;
    
    public:
        /** Global values default constructor
         * 
         * @brief This function is the constructor of the global values.
         *
         */
        globalValues()
        {
            this -> heartRateDataArray = 0;
            this -> spo2DataArray = 0;
            this -> beatsPerMinute = 0;
            this -> spo2Percentage = 0;
            this -> freqs = {};
        }
        
        /** Global values constructor
         * 
         * @brief This function is the constructor of the global values.
         *
         * @param heartRateDataArray Heart rate data array.
         * @param spo2DataArray SPO2 data array.
         * @param beatsPerMinute Beats per minute.
         * @param spo2Percentage SPO2 percentage.
         * @param freqs Fundamentals frequencies.
         */
        globalValues(uint32_t* heartRateDataArray, uint32_t* spo2DataArray, int32_t beatsPerMinute, int32_t spo2Percentage, vector<fundamentalsFreqs> freqs)
        {
            this -> heartRateDataArray = heartRateDataArray;
            this -> spo2DataArray = spo2DataArray;
            this -> beatsPerMinute = beatsPerMinute;
            this -> spo2Percentage = spo2Percentage;
            this -> freqs = freqs;
        }

        /** Set heart rate data array function
         * 
         * @brief This function sets the heart rate data array.
         * 
         * @param heartRateDataArray Heart rate data array.
         */
        void setHeartRateDataArray(uint32_t* heartRateDataArray)
        {
            this -> heartRateDataArray = heartRateDataArray;
        }

        /** Set SPO2 data array function
         * 
         * @brief This function sets the SPO2 data array.
         * 
         * @param spo2DataArray SPO2 data array.
         */
        void setSpo2DataArray(uint32_t* spo2DataArray)
        {
            this -> spo2DataArray = spo2DataArray;
        }

        /** Set beats per minute function
         * 
         * @brief This function sets the beats per minute.
         * 
         * @param beatsPerMinute Beats per minute.
         */
        void setBeatsPerMinute(int32_t beatsPerMinute)
        {
            this -> beatsPerMinute = beatsPerMinute;
        }

        /** Set SPO2 percentage function
         * 
         * @brief This function sets the SPO2 percentage.
         * 
         * @param spo2Percentage SPO2 percentage.
         */
        void setSpo2Percentage(int32_t spo2Percentage)
        {
            this -> spo2Percentage = spo2Percentage;
        }

        /** Set fundamentals frequencies function
         * 
         * @brief This function sets the fundamentals frequencies.
         * 
         * @param freqs Fundamentals frequencies.
         */
        void setFreqs(vector<fundamentalsFreqs> freqs)
        {
            this -> freqs = freqs;
        }

        /** Get heart rate data array function
         * 
         * @brief This function gets the heart rate data array.
         * 
         * @return Heart rate data array.
         */
        uint32_t* getHeartRateDataArray()
        {
            return heartRateDataArray;
        }

        /** Get SPO2 data array function
         * 
         * @brief This function gets the SPO2 data array.
         * 
         * @return SPO2 data array.
         */
        uint32_t* getSpo2DataArray()
        {
            return spo2DataArray;
        }

        /** Get beats per minute function
         * 
         * @brief This function gets the beats per minute.
         * 
         * @return Beats per minute.
         */
        int32_t getBeatsPerMinute()
        {
            return beatsPerMinute;
        }

        /** Get SPO2 percentage function
         * 
         * @brief This function gets the SPO2 percentage.
         * 
         * @return SPO2 percentage.
         */
        int32_t getSpo2Percentage()
        {
            return spo2Percentage;
        }

        /** Get fundamentals frequencies function
         * 
         * @brief This function gets the fundamentals frequencies.
         * 
         * @return Fundamentals frequencies.
         */
        vector<fundamentalsFreqs> getFreqs()
        {
            return freqs;
        }
};

globalValues globalValuesObject;
vector<fundamentalsFreqs> freqs;


  void data(void *pvParameters){
    for(;;){
   //lastTime1 = millis();
    float value = particleSensor.getIR(); // llegim el valor IR Serial.println(millis() - lastSampleTime); // envia el temps des de l'última mostra a la consola sèrie
    float value2 = particleSensor.getRed();
    //lastTime2 = millis();
    //Serial.print("Temps de mostreig: ");
    //Serial.println(lastTime2-lastTime1);

    input_data.push_back(value); // afegim el valor a la llista d'entrada
    input_data2.push_back(value2);

   
    
    if (input_data.size() >= 201) { // si tenim suficients mostres per aplicar el filtre
      y=0;
      y2=0;



      for (int n = 200; n >= 0; n--) {
        y += vcoefs1[n] * input_data[input_data.size()-1-n];
        y2 += vcoefs1[n] * input_data2[input_data2.size()-1-n];
        
      }

      input_data.erase(input_data.begin()); // eliminem la mostra més antiga de la llista
      input_data2.erase(input_data2.begin());
      
      
      if (i>=100)
      {
        i=0;
        
        maxim_heart_rate_and_oxygen_saturation(irBuffer, 100, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
        globalValuesObject.setBeatsPerMinute(heartRate);
        globalValuesObject.setSpo2Percentage(spo2);
        globalValuesObject.setHeartRateDataArray(irBuffer);
        globalValuesObject.setSpo2DataArray(redBuffer);
        Serial.print("Heart rate: ");
        Serial.print(heartRate);
        Serial.print(" bpm / SpO2: ");
        Serial.print(spo2);
        Serial.println(" %"); 
        n++;
      }

     
      irBuffer[i]=y;
      redBuffer[i]=y2;    
     
      i++;
      
     //Serial.println(y); // enviem el resultat a la consola sèrie

    if (n==5)
    {
        for (int i = 0; i < SAMPLES; i++)
      {
        vReal[i] = irBuffer[i];
        vImag[i] = 0;
      }

      //FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING);
      Serial.println("Computing FFT. Please wait...");
      FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
      FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
      vReal[0] = 0; // Remove the DC component
      
      //Normalitzem
      float max=0;
      for(int i=0;i<SAMPLES;i++){
        if(vReal[i]>max){
          max=vReal[i];
        }
      }
        for(int i=0;i<SAMPLES;i++){
            vReal[i]=vReal[i]/max;
        }

      //Print the results of the FFT calculation to the Arduino Serial monitor
      Serial.println("FFT results:");
      for (int i = 0; i < SAMPLES / 2; i++)
      {
        float frequency = float(i) * SAMPLING_FREQUENCY / SAMPLES;
        float magnitude = vReal[i];
        fundamentalsFreqs x;
        x.amplitude=magnitude;
        x.freqsHz=frequency;
        freqs.push_back(x);
        Serial.print("Frequency: ");
        Serial.print(frequency);
        Serial.print(" Hz, Magnitude: ");
        Serial.println(magnitude);
      }
      n++;
      globalValuesObject.setFreqs(freqs);

    }

    
    }


  }
  }

  void readfile();// dona errors al implementar-ho
  void iniMAX30102();// deixa de funcionar si s'implementa.



  void setup()
  {
    Serial.begin(115200);
    Serial.println("Initializing...");


    if(!SPIFFS.begin(true)){
      Serial.println("An Error has occurred while mounting SPIFFS");
      return;
    }

    File file = SPIFFS.open("/coeficients2.txt");
    if(!file){
      Serial.println("Failed to open file for reading");
      return;
    }
    
    
  
    while(i<201){
      delay(50);
      if(i<201){
      String x=file.readStringUntil('\n'); 
      string s=x.c_str();
      vcoefs1[i]=stof(s);
      }
      i++;
    }
    i=0;

    file.close(); 
   // Initialize sensor
    if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
    {
      Serial.println("MAX30105 was not found. Please check wiring/power. ");
      while (1);
    }

    //Setup to sense a nice looking saw tooth on the plotter
    byte ledBrightness = 0x0F; //Options: 0=Off to 255=50mA
    byte sampleAverage = 4; //Options: 1, 2, 4, 8, 16, 32
    byte ledMode = 3; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
    int sampleRate = 3200; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
    int pulseWidth = 411; //Options: 69, 118, 215, 411
    int adcRange = 4096; //Options: 2048, 4096, 8192, 16384

    particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); //Configure sensor with these settings
    
    for(int e=0;e<=200;e++)
    {
      Serial.print("Vcoefs1[");
      Serial.print(e);
      Serial.print("]: ");
      Serial.println(vcoefs1[e],6);
    }


   
    i=0;
    n=0;
   

   xTaskCreatePinnedToCore(
                    data,   /* Task function. */
                    "Task1",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    NULL,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */

  

   //attachInterrupt(button1.PIN, isr, FALLING); //attach interrupt to our button pin, to do fft.

  }

  void loop()
  {
   
    

  
  }

void iniMAX30102(){
 // Initialize sensor
    if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
    {
      Serial.println("MAX30105 was not found. Please check wiring/power. ");
      while (1);
    }

    //Setup to sense a nice looking saw tooth on the plotter
    byte ledBrightness = 0x0F; //Options: 0=Off to 255=50mA
    byte sampleAverage = 4; //Options: 1, 2, 4, 8, 16, 32
    byte ledMode = 3; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
    int sampleRate = 3200; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
    int pulseWidth = 411; //Options: 69, 118, 215, 411
    int adcRange = 4096; //Options: 2048, 4096, 8192, 16384

    particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); //Configure sensor with these settings

}




void readfile(){

 

}
