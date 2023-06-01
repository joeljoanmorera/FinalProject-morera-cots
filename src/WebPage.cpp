#include "WebPage.h"

/** webPage default constructor
 * 
 * @brief This function is the constructor of the webPage.
 * 
 * @param port Port number.
 *
 */
webPage::webPage(int port):webServer(port), webSocket("/ws")
{
    globalClient = NULL;
}

/** webPage begin function
 * 
 * @brief This function initializes the web.
 * 
 * @param ssid SSID of the network.
 * @param password Password of the network.
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
 * @param  ssid SSID of the network.
 * @param  password Password of the network.
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
 * @details This function defines the websocket and the html, css and js files.
 *  
 * @see begin(), onWsEvent().
 * 
 */
void webPage::initServer()
{
    // websocker definition
    webSocket.onEvent([this](  AsyncWebSocket *server, AsyncWebSocketClient *client, 
                        AwsEventType type, void *arg, uint8_t *data, size_t len){
        // Call the onWsEvent member function of the current instance of the webPage class
        this->onWsEvent(server, client, type, arg, data, len);
    });

    webServer.addHandler(&webSocket);
    
    // define html file
    webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/index.html", "text/html");
    });

    // define css files
    webServer.on("/css/stylesheet.css", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/css/stylesheet.css", "text/css");
    });
    
    // define js files
    webServer.on("/js/main.js", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/js/main.js", "text/javascript");
    });
    webServer.on("/js/heartrate-chart.js", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/js/heartrate-chart.js", "text/javascript");
    });
    webServer.on("/js/frequencies-chart.js", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/js/frequencies-chart.js", "text/javascript");
    });
    webServer.on("/js/spo2-chart.js", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/js/spo2-chart.js", "text/javascript");
    });

    webServer.begin();
}

/** Websocket event function
 * 
 * @brief This function is the event function of the websocket.
 * 
 * @param server AsyncWebSocket object.
 * @param client AsyncWebSocketClient object.
 * @param type AwsEventType object.
 * @param arg void pointer.
 * @param data uint8_t pointer.
 * @param len size_t object.
 *  
 * @details This function defines what to do when a websocket event occurs depending on the type of event.
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
 * @param message Message to send.
 *  
 * @details This function sends a websocket message if there is a client connected.
 * 
 */
void webPage::sendWsMessage(String message)
{
    if(globalClient != NULL && globalClient->status() == WS_CONNECTED)
    {
        globalClient->text(message);
    }
}