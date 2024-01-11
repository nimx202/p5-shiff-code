#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Servo.h>
#include <ArduinoJson.h>
#include <Arduino.h>

const char *ssid = "wifi";
const char *password = "nassr123";
const char *serverName = "http://p1.nimx.me:5001/coordinates";
const int ESC_pin = 13;
const int SERVO_pin = 12;
const int MIN_PWM = 0;
const int MAX_PWM = 2000;

Servo ESC;
Servo SERVO;

void setup()
{
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());

    ESC.attach(ESC_pin, MIN_PWM, MAX_PWM);
    SERVO.attach(SERVO_pin);
}

void loop()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        String response = httpGETRequest(serverName);

        DynamicJsonDocument jsonDocument(256);
        DeserializationError error = deserializeJson(jsonDocument, response);

        if (error)
        {
            Serial.print("JSON Parsing Error: ");
            Serial.println(error.c_str());
        }
        else
        {
            int degree = jsonDocument["degree"];
            int motorSpeed = jsonDocument["motorSpeed"];

            Serial.print("Received Degree: ");
            Serial.println(degree);
            Serial.print("Received Motor Speed: ");
            Serial.println(motorSpeed);

            int mappedMotorSpeed = map(motorSpeed, 0, 100, MIN_PWM, MAX_PWM);

            if(motorSpeed > 1){
                ESC.write(MAX_PWM);
                Serial.print("speed");
            }
            SERVO.write(degree);
        }
    }
    else
    {
        Serial.println("WiFi Disconnected");
    }

    delay(500);
}

String httpGETRequest(const char *serverName)
{
    WiFiClient client;
    HTTPClient http;

    http.begin(client, serverName);

    int httpResponseCode = http.GET();

    String response = "{}";

    if (httpResponseCode > 0)
    {
        response = http.getString();
    }
    else
    {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
    }

    http.end();

    return response;
}
