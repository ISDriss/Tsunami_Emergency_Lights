#include <WiFi.h>
#include <ESPmDNS.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keypad.h>
#include "../credentials.h"

WiFiServer server(8080);
WiFiClient client;
bool connected = false;

// Declaring the pins of the columns and rows of the button matrix
const byte ROWS = 3;
const byte COLS = 3;
byte ROW_PINS[] = {20, 10, 0};  // CHANGE according to connections
byte COL_PINS[] = {7, 8, 1};    // CHANGE according to connections

// Declaring the button actions in a 2D array
const char keys[ROWS][COLS] = {
  {'U',  'L', 'D'},   // Row 0: ↑, →, A
  {'R',  'S', '0'},  // Row 1: ←, Select, B
  {'A',  'B', '1'}  // Row 2: ↓,
};
Keypad keypad = Keypad(makeKeymap(keys), ROW_PINS, COL_PINS, ROWS, COLS);

bool OH_NO_ITS_HAPPENING = false;

void WiFiSetup() {
    WiFi.begin(ssid,password);
    Serial.print("Connecting to Wifi");
    while(WiFi.status() != WL_CONNECTED){
        delay(1000);
        Serial.print(".");
    }
    Serial.println("Connected to Wifi");
    Serial.print("At IP:");
    Serial.println(WiFi.localIP());
}

IPAddress MDNSSetup() {
    // Start mDNS
    if (!MDNS.begin("esp32server")) {  // Register the device as "esp32server.local"
      Serial.println("Error setting up MDNS responder!");
    } else {
      Serial.println("mDNS responder started");
    }
    // Start server
    server.begin();
    return IPAddress();
}

void ComsSetup(IPAddress serverIP){
    client = server.available();
    if (client) {
      Serial.println("Client connected.");

      String message = client.readStringUntil('\n');  // Read the message sent from the client
      Serial.println("Received message: " + message);
      // Send a response
      client.println("Hello from Server");
    }
}

void GetKeyboard(){
  char input = keypad.getKey();
  //Serial.println(input);
  if(keypad.isPressed('U')){
    OH_NO_ITS_HAPPENING = true;
  } else if(keypad.isPressed('D')){
    OH_NO_ITS_HAPPENING = false;
  }
}


void setup() {
    //base Setup
    Serial.begin(9600);
    WiFiSetup();

    //Card connection
    IPAddress SIP = MDNSSetup();
    ComsSetup(SIP);
}

void loop() {
    if(client){
        GetKeyboard();
        Serial.println(OH_NO_ITS_HAPPENING);
        if(OH_NO_ITS_HAPPENING){
            client.println("Alert");
        } else{
            client.println("Stop");
        }
    }
    else{  
      //server does not detect the disconnection of the client
      //but can reconnect to it if client attempt reconnection
      Serial.println("lost client");
      client = server.available();
    }
    delay(20);
}
