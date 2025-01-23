#include <WiFi.h>
#include <ESPmDNS.h>
#include "../credentials.h"

// Configuration des pins
#define LED1 4  // Première LED du chemin lumineux
#define LED2 2  // Deuxième LED du chemin lumineux
#define LED_MORSE 9  // LEDs en série pour le code Morse

// Variables globales
IPAddress SIPA;
WiFiClient client;
bool connected = false;
bool alerted = false;

// Variables pour le chemin lumineux
unsigned long pathPreviousMillis = 0;
const long pathInterval = 500; // Intervalle pour alterner les LEDs du chemin
int pathState = 0;

// Variables pour le code Morse SOS
unsigned long morsePreviousMillis = 0;
int morseIndex = 0;
bool morseState = false;
const int morsePattern[] = {1, 0, 1, 0, 1, 0, 3, 0, 3, 0, 3, 0, 1, 0, 1, 0, 1, 0}; // "SOS" en Morse
const int morseUnit = 1000; // Durée d'une unité Morse (1000 ms)

void WiFiSetup() {
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}

IPAddress MDNSSetup() {
    if (!MDNS.begin("esp32client")) {
        Serial.println("Error starting MDNS on client");
    }
    Serial.println("Resolving server...");
    IPAddress serverIP = MDNS.queryHost("esp32server");
    return serverIP;
}

void ComsSetup(IPAddress serverIP) {
    if (serverIP != IPAddress()) {
        Serial.print("Server found at: ");
        Serial.println(serverIP);

        if (client.connect(serverIP, 8080)) {
            Serial.println("Connected to server.");
            client.println("Hello from Client");
            String response = client.readStringUntil('\n');
            Serial.println("Server response: " + response);
        }
    } else {
        Serial.println("Server not found.");
    }
}

void Checkmessage() {
    if (client.available()) {
        String response = client.readStringUntil('\n');
        response.trim();
        Serial.println(response);
        if (response == "Alert") {
            alerted = true;
        }
        if (response == "Stop") {
            alerted = false;
        }
    }
}

// Fonction pour le chemin lumineux
void PathLights() {
    unsigned long currentMillis = millis();
    if (currentMillis - pathPreviousMillis >= pathInterval) {
        pathPreviousMillis = currentMillis;

        // Alterner l'état des LEDs
        if (pathState == 0) {
            digitalWrite(LED1, HIGH);
            digitalWrite(LED2, LOW);
        } else if (pathState == 1) {
            digitalWrite(LED1, LOW);
            digitalWrite(LED2, HIGH);
        } else if (pathState == 2) {
            digitalWrite(LED1, LOW);
            digitalWrite(LED2, LOW);
        }
        pathState = (pathState + 1) % 3; // Passer à l'état suivant
    }
}

// Fonction pour le code Morse SOS
void MorseSOS() {
    unsigned long currentMillis = millis();
    if (currentMillis - morsePreviousMillis >= morseUnit) {
        morsePreviousMillis = currentMillis;

        // Appliquer l'état du pattern Morse
        if (morsePattern[morseIndex] == 1) { // Allumer pour une unité courte
            digitalWrite(LED_MORSE, HIGH);
        } else if (morsePattern[morseIndex] == 3) { // Allumer pour une unité longue
            digitalWrite(LED_MORSE, HIGH);
            delay(morseUnit); // Temps supplémentaire pour un long
        } else { // Éteindre
            digitalWrite(LED_MORSE, LOW);
        }

        morseIndex++;
        if (morseIndex >= sizeof(morsePattern) / sizeof(morsePattern[0])) {
            morseIndex = 0; // Recommencer le pattern
        }
    }
}

// Fonction principale d'alerte
void Alert() {
    PathLights();
    MorseSOS();
}

void setup() {
    // Configuration de base
    Serial.begin(9600);
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED_MORSE, OUTPUT);

    WiFiSetup();
    SIPA = MDNSSetup();
    ComsSetup(SIPA);
}

void loop() {
    if (client.connected()) {
        Checkmessage();
    } else {
        ComsSetup(SIPA);
    }

    if (alerted) {
        Alert();
    } else {
        // Éteindre les LEDs si aucune alerte n'est active
        digitalWrite(LED1, LOW);
        digitalWrite(LED2, LOW);
        digitalWrite(LED_MORSE, LOW);
    }

    delay(20);
}
