#include <math.h>
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

#define ThermistorPIN 2                 // Analog Pin 0

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
static byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
static byte ip[] = { 192, 168, 3, 251 };
static byte gateway[] = { 192, 168, 3, 1 };
static byte subnet[] = { 255, 255, 0, 0 };
static byte server[] = { 192, 168, 3, 101 };
//IPAddress ip(192, 168, 4, 101);
//IPAddress server(192, 168, 3, 101);

EthernetClient ethClient;
PubSubClient client(ethClient);


//溫度部
float vcc = 5.00;                       // only used for display purposes, if used
                                        // set to the measured Vcc.
float pad = 10100;                       // balance/pad resistor value, set this to
                                        // the measured resistance of your pad resistor
float thermr = 9900;                   // thermistor nominal resistance

float Thermistor(int RawADC) {
  long Resistance;  
  float Temp;  // Dual-Purpose variable to save space.

  Resistance=pad*((1024.0 / RawADC) - 1);
  Temp = log(Resistance); // Saving the Log(resistance) so not to calculate  it 4 times later
  Temp = 1 / (0.001129148 + (0.000234125 * Temp) + (0.0000000876741 * Temp * Temp * Temp));
  Temp = Temp - 273.15;  // Convert Kelvin to Celsius                      

  // BEGIN- Remove these lines for the function not to display anything
  //Serial.print("ADC: ");
  //Serial.print(RawADC);
  //Serial.print("/1024");                           // Print out RAW ADC Number
  //Serial.print(", vcc: ");
  //Serial.print(vcc,2);
  //Serial.print(", pad: ");
  //Serial.print(pad/1000,3);
  //Serial.print(" Kohms, Volts: ");
  //Serial.print(((RawADC*vcc)/1024.0),3);  
  //Serial.print(", Resistance: ");
  //Serial.print(Resistance);
  //Serial.print(" ohms, ");
  // END- Remove these lines for the function not to display anything

  // Uncomment this line for the function to return Fahrenheit instead.
  //temp = (Temp * 9.0)/ 5.0 + 32.0;                  // Convert to Fahrenheit
  return Temp;                                      // Return the Temperature
}
/*
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
*/
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("temp001","connected");
      // ... and resubscribe
      //client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(57600);

  client.setServer(server, 1883);
  //client.setCallback(callback);

  Ethernet.begin(mac, ip, gateway, subnet);
  // Allow the hardware to sort itself out
  delay(1500);
}

long lastMsg = 0;
void loop()
{
  float temp;
  temp=Thermistor(analogRead(ThermistorPIN));
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis();
  if (now - lastMsg > 1000) {
    lastMsg = now;
    Serial.println(String(temp).c_str());
    client.publish("temp001",String(temp).c_str(), true);
    delay(5000);
    }

}
