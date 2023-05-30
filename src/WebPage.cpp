#include "WebPage.h"

/** webPage default constructor
 * 
 * @brief This function is the constructor of the webPage.
 * 
 * @param port Port number.
 *
 */
webPage::webPage(int port = 80):server(port), ws("/ws")
{
    globalClient = NULL;
}

/** webPage begin
 * 
 * @brief This function initializes the web.
 *  
 * @return void.
 *  
 * @details This function initializes the WiFi and the server.
 *  
 * @see initWiFi(), initServer().
 * 
 */
void webPage::begin(const char* ssid, const char* password)
{
    initWiFi(ssid, password);
    initServer();
}

/** WiFi initialization function
 * 
 * @brief This function initializes the WiFi.
 *  
 * @return void.
 *  
 * @details This function initializes the WiFi.
 *  
 * @see begin().
 * 
 */
void webPage::initWiFi(const char* ssid, const char* password)
{
    WiFi.begin(ssid, password);

    Serial.print("Connecting to WiFi..");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("IP: ");
    Serial.print(WiFi.localIP());
    Serial.println("");
}

/** Server initialization function
 * 
 * @brief This function initializes the server.
 *  
 * @return void.
 *  
 * @details This function initializes the server.
 *  
 * @see begin(), onWsEvent().
 * 
 */
void webPage::initServer()
{
    // websocker definition
    ws.onEvent([this](  AsyncWebSocket *server, AsyncWebSocketClient *client, 
                        AwsEventType type, void *arg, uint8_t *data, size_t len){
        // Call the onWsEvent member function of the current instance of the webPage class
        this->onWsEvent(server, client, type, arg, data, len);
    });

    server.addHandler(&ws);
    
    // define html file
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/index.html", "text/html");
    });

    // define css files
    server.on("/css/stylesheet.css", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/css/stylesheet.css", "text/css");
    });
    
    // define js files
    server.on("/js/main.js", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/js/main.js", "text/javascript");
    });
    server.on("/js/heartrate-chart.js", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/js/heartrate-chart.js", "text/javascript");
    });
    server.on("/js/frequencies-chart.js", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/js/frequencies-chart.js", "text/javascript");
    });
    server.on("/js/spo2-chart.js", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/js/spo2-chart.js", "text/javascript");
    });

    server.begin();
}

/** Websocket event function
 * 
 * @brief This function is the event function of the websocket.
 *  
 * @return void.
 *  
 * @details This function is the event function of the websocket.
 *  
 * @see begin(), initServer().
 * 
 */
void webPage::onWsEvent (AsyncWebSocket * server, AsyncWebSocketClient * client, 
                         AwsEventType type, void * arg, uint8_t *data, size_t len)
{
    if (type == WS_EVT_CONNECT)
    {
        Serial.println("Websocket client connection received");
        globalClient = client;
    } 
    else if (type == WS_EVT_DISCONNECT)
    {
        Serial.println("Client disconnected");
        globalClient = NULL;
    }
}

/** Send websocket message function
 * 
 * @brief This function sends a websocket message.
 *  
 * @return void.
 *  
 * @details This function sends a websocket message.
 * 
 */
void webPage::sendWsMessage(String message)
{
    if(globalClient != NULL && globalClient->status() == WS_CONNECTED)
    {
        globalClient->text(message);
    }
}