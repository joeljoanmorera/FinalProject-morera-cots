#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <SPIFFS.h>

class webPage{
    AsyncWebServer server;
    AsyncWebSocket ws;
    AsyncWebSocketClient * globalClient;

    public:

        webPage();

        void begin(const char* ssid, const char* password);

        void initWiFi(const char* ssid, const char* password);

        void initServer();

        void onWsEvent (AsyncWebSocket * server, 
                            AsyncWebSocketClient * client,
                            AwsEventType type, 
                            void * arg,
                            uint8_t *data,
                            size_t len);

        void sendWsMessage(String message);
};

#endif /* WEBPAGE_H */