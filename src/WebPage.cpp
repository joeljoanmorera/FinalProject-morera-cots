#include "WebPage.h"

using namespace std;

/** webPage default constructor
 * 
 * @brief This function is the constructor of the webPage.
 *
 */
webPage::webPage():server(80), ws("/ws"){
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
 * @note This function is called once.
 * 
 * @see initWiFi(), initServer().
 * 
 */
void webPage::begin(const char* ssid, const char* password){
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
 * @note This function is called once.
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
 * @note This function is called once.
 * 
 * @see begin().
 * 
 */
void webPage::initServer()
{
    ws.onEvent([this](  AsyncWebSocket *server, AsyncWebSocketClient *client, 
                        AwsEventType type, void *arg, uint8_t *data, size_t len){
        // Call the onWsEvent member function of the current instance of the webPage class
        this->onWsEvent(server, client, type, arg, data, len);
    });

    server.addHandler(&ws);
    
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/index.html", "text/html");
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
 * @note This function is called when the websocket receives a message.
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
 * @note This function is called when a websocket message is generated.
 * 
 */
void webPage::sendWsMessage(String message)
{
    if(globalClient != NULL && globalClient->status() == WS_CONNECTED)
    {
        globalClient->text(message);
    }
}