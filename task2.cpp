

  #include <Arduino.h>
  #include <Wire.h>
  #include "MAX30105.h"
  #include "SPIFFS.h"
  #include "heartRate.h"
  #include "spo2_algorithm.h"
  #include <vector>
  #include<queue>

  using namespace std;

  MAX30105 particleSensor;

  //static unsigned long lastTime1=0;
  //static unsigned long lastTime2=0;

  vector<float> input_data;//vector per guardar les dades del sensor
  vector<float> input_data2;//vector per guardar les dades del sensor

  vector<float> vcoefs1;//vector per guardar els coeficients del filtre
  vector<float> vcoefs2;//vector per guardar els coeficients del filtre 
  int i =0;//variable per recorrer el vector de coeficients
  bool DSR = true;

  uint32_t irBuffer[201]; //infrared LED sensor data
  uint32_t redBuffer[201];  //red LED sensor data

  int32_t bufferLength; //data length
  int32_t spo2; //SPO2 value
  int8_t validSPO2; //indicator to show if the SPO2 calculation is valid
  int32_t heartRate; //heart rate value
  int8_t validHeartRate; //indicator to show if the heart rate calculation is valid


  void initMAX30102();
  void determineSignalRate();
  void takingSamples();

  void datacalc(void *parameter) {

    
    
    while (1) {
   if(DSR)determineSignalRate();
    
  Serial.print(F("\t Heart Rate : "));
  Serial.print(heartRate, DEC);

  Serial.print(F("\t HRvalid : "));
  Serial.print(validHeartRate, DEC);

  Serial.print(F("\t Saturation in oxygen : "));
  Serial.print(spo2, DEC);

  Serial.print(F("\t SPO2Valid : "));
  Serial.println(validSPO2, DEC);
  
  takingSamples();
}

}
  

  void setup()
  {
  

    Serial.begin(115200);
    Serial.println("Initializing..."); 

    xTaskCreatePinnedToCore(datacalc, "datacalc", 1000, NULL, 1, NULL,1);

    // Obrir fitxers SPIFFS
     if(!SPIFFS.begin(true)){
      Serial.println("An Error has occurred while mounting SPIFFS");
      return;
    }

    File file = SPIFFS.open("/coeficients2.txt");
    if(!file){
      Serial.println("Failed to open file for reading");
      return;
    }
    
    
   // Carregar fitxer al vector del filtre
    while(i<201){
      delay(50);
      if(i<201){
      String x=file.readStringUntil('\n'); 
      string s=x.c_str();
      vcoefs1.push_back(stof(s));
      }
    
      i++;
    }
    i=0;

    file.close();
     // Codi per visualitzar el vector
    for(int e=0;e<=200;e++)
    {
      Serial.print("Vcoefs1[");
      Serial.print(e);
      Serial.print("]: ");
      Serial.println(vcoefs1[e],6);
    }    

    initMAX30102();
  }

  void loop()
  {}

 
  


void determineSignalRate()
{
  bufferLength = 201; //buffer length of 100 stores 4 seconds of samples running at 25sps
  Serial.print("Determining signal rate ..");
  //read the first 100 samples, and determine the signal range
  for (byte i = 0 ; i < bufferLength ; i++)
  {
    while (particleSensor.available() == false) //do we have new data?
    
    particleSensor.check(); //Check the sensor for new data

    float valueRed = particleSensor.getRed(); // llegim el valor Red
    float value = particleSensor.getIR(); // llegim el valor IR
    input_data.push_back(value); // afegim el valor a la llista d'entrada
    input_data2.push_back(valueRed); // afegim el valor a la llista d'entrada

    for (int n = 200; n >= 0; n--) {
          redBuffer[i] += vcoefs1[n] * input_data2[input_data.size()-1-n];
          irBuffer[i]  += vcoefs1[n] * input_data[input_data.size()-1-n];// aplicar el filtre
      }
      input_data.erase(input_data.begin()); // eliminem la mostra més antiga de la llista
      input_data2.erase(input_data2.begin()); // eliminem la mostra més antiga de la llista

    particleSensor.nextSample(); //We're finished with this sample so move to next sample
    if(i%25 == 0)
    {  
      Serial.print('.');
    }
  }
  //calculate heart rate and SpO2 after first 100 samples (first 4 seconds of samples)
  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
  DSR = false;
  Serial.println("\nSignal determining done!");
}


void takingSamples()//Continuously taking samples from MAX30102.  Heart rate and SpO2 are calculated every 1 second
{
  //dumping the first 25 sets of samples in the memory and shift the last 75 sets of samples to the top
  for (byte i = 25; i < 201; i++)
  {
    redBuffer[i - 25] = redBuffer[i];
    irBuffer[i - 25] = irBuffer[i];
  }

  //take 25 sets of samples before calculating the heart rate.
  for (byte i = 176; i < 201; i++)
  {
    while (particleSensor.available() == false) //do we have new data?
      particleSensor.check(); //Check the sensor for new data

    float valueRed = particleSensor.getRed(); // llegim el valor Red
    float value = particleSensor.getIR(); // llegim el valor IR
    input_data.push_back(value); // afegim el valor a la llista d'entrada
    input_data2.push_back(valueRed); // afegim el valor a la llista d'entrada

    for (int n = 200; n >= 0; n--) {
          redBuffer[i] += vcoefs1[n] * input_data2[input_data.size()-1-n];
          irBuffer[i]  += vcoefs1[n] * input_data[input_data.size()-1-n];// aplicar el filtre
      }
      input_data.erase(input_data.begin()); // eliminem la mostra més antiga de la llista
      input_data2.erase(input_data2.begin()); // eliminem la mostra més antiga de la llista
    particleSensor.nextSample(); //We're finished with this sample so move to next sample
  }

  //After gathering 25 new samples recalculate HR and SP02
  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
}



void initMAX30102()
{
  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println(F("MAX30105 was not found. Please check wiring/power."));
    while (1);
  }
  
  byte ledBrightness = 0x1F; //Options: 0=Off to 255=50mA
    byte sampleAverage = 4; //Options: 1, 2, 4, 8, 16, 32
    byte ledMode = 3; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
    int sampleRate = 3200; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
    int pulseWidth = 411; //Options: 69, 118, 215, 411
    int adcRange = 4096; //Options: 2048, 4096, 8192, 16384

    particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
}



