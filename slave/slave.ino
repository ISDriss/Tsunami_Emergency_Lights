#include <WiFi.h>
#include <ESPmDNS.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "../credentials.h"

#define LED 2

//WiFiServer server(8080);
WiFiClient client;
bool connected = false;
bool alerted = false;

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
    // Use mDNS to find the server
    if (!MDNS.begin("esp32client")) {
      Serial.println("Error starting MDNS on client");
    }
    // Resolve server's mDNS hostname to IP
    Serial.println("Resolving server...");
    IPAddress serverIP = MDNS.queryHost("esp32server");
    return serverIP;
}

void ComsSetup(IPAddress serverIP){
    if(serverIP != IPAddress()) {
        Serial.print("Server found at: ");
        Serial.println(serverIP);
    
        if (client.connect(serverIP, 8080)) {
            Serial.println("Connected to server.");

            // Send a message to the server
            client.println("Hello from Client");

            // Wait for a response
            String response = client.readStringUntil('\n');
            Serial.println("Server response: " + response);
        }
    } else {
    Serial.println("Server not found.");
    }
}

void Checkmessage(){
    String response = client.readStringUntil('\n');
    if(response == "Alert"){
        alerted = true;
    }
    else if (response == "Stop")
    {
        alerted = false;
    }
}

void Alert(){
    digitalWrite(LED, HIGH);
    delay(500);
    digitalWrite(LED, LOW);
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
    if(alerted){
        Alert();
    }
    while(!connected){
        if(client.connected()){
            connected = true;
        }
    }   
    while(connected){
        Checkmessage();
        delay(20);
    }
}