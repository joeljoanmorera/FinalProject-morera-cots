

  #include <Arduino.h>
  #include <Wire.h>
  #include "MAX30105.h"
  #include "SPIFFS.h"
  #include <vector>
  #include<queue>

  using namespace std;

  MAX30105 particleSensor;

  static unsigned long lastTime1=0;
  static unsigned long lastTime2=0;

  double x;
  vector<float> input_data;

  queue<float> input_queue;
  float vcoefs1[201];
  float vcoefs2[201];
  int i =0;
  int n = 0;
  float y = 0.0;

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
      if (i>=201){
      String x=file.readStringUntil('\n'); 
      string s=x.c_str();
      vcoefs2[n]=stof(s);
      n++;
      }
      i++;
    }
    i=0;

    file.close();

    
    for(int e=0;e<=200;e++)
    {
      Serial.print("Vcoefs1[");
      Serial.print(e);
      Serial.print("]: ");
      Serial.println(vcoefs1[e],6);
    }

  for(int e=0;e<=200;e++)
    {
      Serial.print("Vcoefs2[");
      Serial.print(e);
      Serial.print("]: ");
      Serial.println(vcoefs2[e],6);
    }

    // Initialize sensor
    if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
    {
      Serial.println("MAX30105 was not found. Please check wiring/power. ");
      while (1);
    }

    //Setup to sense a nice looking saw tooth on the plotter
    byte ledBrightness = 0x1F; //Options: 0=Off to 255=50mA
    byte sampleAverage = 8; //Options: 1, 2, 4, 8, 16, 32
    byte ledMode = 3; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
    int sampleRate = 3200; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
    int pulseWidth = 411; //Options: 69, 118, 215, 411
    int adcRange = 4096; //Options: 2048, 4096, 8192, 16384

    particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); //Configure sensor with these settings

    //Arduino plotter auto-scales annoyingly. To get around this, pre-populate
    //the plotter with 500 of an average reading from the sensor

    //Take an average of IR readings at power up
    const byte avgAmount = 64;
    long baseValue = 0;
    for (byte x = 0 ; x < avgAmount ; x++)
    {
      baseValue += particleSensor.getIR(); //Read the IR value
    }
    baseValue /= avgAmount;

    //Pre-populate the plotter so that the Y scale is close to IR values
    for (int x = 0 ; x < 500 ; x++)
      Serial.println(baseValue);
    

  
  }

  void loop()
  {
    //lastTime1 = millis();
    float value = particleSensor.getIR(); // llegim el valor IR Serial.println(millis() - lastSampleTime); // envia el temps des de l'última mostra a la consola sèrie
    //lastTime2 = millis();
    //Serial.print("Temps de mostreig: ");
    //Serial.println(lastTime2-lastTime1);

    input_data.push_back(value); // afegim el valor a la llista d'entrada
    
    if (input_data.size() >= 201) { // si tenim suficients mostres per aplicar el filtre
      float y = 0.0; // variable per guardar el resultat del filtre
      for (int n = 200; n >= 0; n--) {
        y += vcoefs1[n] * input_data[input_data.size()-1-n];
        //y += vcoefs2[n] * input_data[input_data.size()-1-n];// aplicar el filtre
      }
      input_data.erase(input_data.begin()); // eliminem la mostra més antiga de la llista
      Serial.println(y); // enviem el resultat a la consola sèrie
    }
    
    

  }
