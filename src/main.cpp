
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>  //segun la libreria se diseño para el esp8266, pero lo admite el esp32

const char *ssid = "";
const char *password = "";

const char *mqtt_server = "";//http://broker.mqtt-dashboard.com/index.html
const int mqtt_port = 1883;//TCP_URL
const char *mqtt_user = "esp32_client";
const char *mqtt_pass = "121212";

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[100];

int cedula = 117541900;
//int temp2 = 1;
//int volts = 2;

//*****************************
//*** DECLARACION FUNCIONES ***
//*****************************
void setup_wifi();
void callback(char *topic, byte *payload, unsigned int length);
void reconnect();

void setup()
{
    pinMode(BUILTIN_LED, OUTPUT);
    Serial.begin(9600);
    randomSeed(micros());
    setup_wifi();
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
}

void loop()
{
    if (!client.connected())
    {
        reconnect();
    }

    client.loop();

    long now = millis();
    if (now - lastMsg > 3000)
    {
        lastMsg = now;
        cedula++;
        
        String to_send = "5e992a6806505c5262396ff0," + String(cedula) + "," + String(37);
        to_send.toCharArray(msg, 100);
        Serial.print("Publicamos mensaje -> ");
        Serial.println(msg);
        client.publish("porvenir", msg);
    }
}

//*****************************
//***    CONEXION WIFI      ***
//*****************************
void setup_wifi()
{
    delay(10);
    // Nos conectamos a nuestra red Wifi
    Serial.println();
    Serial.print("Conectando a ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("Conectado a red WiFi!");
    Serial.println("Dirección IP: ");
    Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *payload, unsigned int length)
{
    String incoming = "";
    Serial.print("Mensaje recibido desde -> ");
    Serial.print(topic);
    Serial.println("");
    for (int i = 0; i < length; i++)
    {
        incoming += (char)payload[i];
    }
    incoming.trim();
    Serial.println("Mensaje -> " + incoming);

    if (incoming == "on")
    {
        digitalWrite(BUILTIN_LED, HIGH);
    }
    else
    {
        digitalWrite(BUILTIN_LED, LOW);
    }
}

void reconnect()
{

    while (!client.connected())
    {
        Serial.print("Intentando conexión Mqtt...");
        // Creamos un cliente ID
        String clientId = "esp32_";
        clientId += String(random(0xffff), HEX);
        // Intentamos conectar
        if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass))
        {
            Serial.println("Conectado!");
            // Nos suscribimos
            client.subscribe("led");
        }
        else
        {
            Serial.print("falló :( con error -> ");
            Serial.print(client.state());
            Serial.println(" Intentamos de nuevo en 5 segundos");

            delay(5000);
        }
    }
}