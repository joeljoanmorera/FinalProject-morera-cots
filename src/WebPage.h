#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <SPIFFS.h>

/** Web page class 
 * @brief Class to manage the web page
 * 
 * @details This class is used to manage the web page
 * 
 * @param webServer AsyncWebServer object
 * @param webSocket AsyncWebSocket object
 * @param globalClient AsyncWebSocketClient object
 * 
 */
class webPage{
    AsyncWebServer webServer;
    AsyncWebSocket webSocket;
    AsyncWebSocketClient * globalClient;

    public:
        webPage(int port = 80);

        void begin(const char* ssid, const char* password);

        void initWiFi(const char* ssid, const char* password);

        void initServer();

        void onWsEvent (AsyncWebSocket * server, AsyncWebSocketClient * client, 
                        AwsEventType type, void * arg, uint8_t *data, size_t len);

        void sendWsMessage(String message);
};

#endif /* WEBPAGE_H */