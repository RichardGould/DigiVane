/* Men in Sheds Wind Direction  */
/*
  Version:      0.0
  Date:         01/09/2025
  Changes       <- Date -> <-Time-> <-V->	<-    Comment                                 >
  Latest Edit:  06/09/2025 11:00    0.1		Creation
                23/02/2026 16:00    1.0		Production
*/

/*
 Library Includes
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_PCF8574.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <WiFiUdp.h>
#include "param.h";
/*
  ESP8266WiFi at version 1.0
  PubSubClient at version 2.8
  Wire at version 1.0
  Adafruit PCF8574 at version 1.1.2
  Adafruit BusIO at version 1.17.2
  SPI at version 1.0 
  ESP8266mDNS at version 1.2 
  ArduinoOTA at version 1.0 
*/

/* Boards */
/*
    esp8266 by ESP8266 Community v3.12
    esp32   by Espressif v3.1.0
    Arduino AVR Boards by Arduino v1.8.6
 */

/*
 *	Preferences
 */
 /*
    http://arduino.esp8266.com/stable/package_esp8266com_index.json
    https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
 */


/*  Instance creation */

WiFiClient espClient;              // WiFi instance

PubSubClient client(espClient);    // MQTT instance

Adafruit_PCF8574 pcf;             // 8574 Instance

/*
 *	Setup
 */
void setup() {
  Serial.begin( 115200 );
  while (!Serial) delay(1000);
/*
 *  Setup 8 bit 8574 interface
 */
  if (!pcf.begin(PCF8574, &Wire)) {
    Serial.println("Couldn't find PCF8574");
    while (1);
  }
  Serial.println( "8574 found" );
/*
 *	All pins of 8574 are inputs
 */
  for ( i=0; i<8; i++) pcf.pinMode(i, INPUT_PULLUP);

/*
 *    Construct MQTT tokens
 */ 
  sprintf( MiS_HEAD,      "%s/%s", MiS_BASE, MiS_DEVICE );
  sprintf( MQTT_IN,       "%s/%s", MiS_HEAD, IN_TOPIC );
  sprintf( MQTT_OUT,      "%s/%s", MiS_HEAD, OUT_TOPIC );
  sprintf( MQTT_RST,      "%s/%s", MiS_HEAD, RST_TOPIC );
  sprintf( MQTT_STATIN,   "%s/%s", MiS_HEAD, STAT_IN_TOPIC );
  sprintf( MQTT_STATOUT,  "%s/%s", MiS_HEAD, STAT_OUT_TOPIC );
  sprintf( MQTT_DEVICES,  "%s/%s/%s", MiS_BASE, DEV_TOPIC, MiS_DEVICE );
  
/*
 *  decrypt passwords
 */
  for (i = 0; i < (int)strlen(Home_Pass); i++) Home_Pass[i] = Home_Pass[i] + 3 - i;
  for (i = 0; i < (int)strlen(MiS_Pass);  i++) MiS_Pass[i]  = MiS_Pass[i]  + 3 - i;
  for (i = 0; i < (int)strlen(Jim_Pass);  i++) Jim_Pass[i]  = Jim_Pass[i]  + 3 - i;
  for (i = 0; i < (int)strlen(Rich_Pass); i++) Rich_Pass[i] = Rich_Pass[i] + 3 - i;
/*
 *  determine where the WiFi is
 */
  net = fn_WiFiScan();

  if (net == -2) fn_ReStart();  // restart wemo
  
  Serial.print( "Network : " );
  Serial.println( net );

/*
 *  Connect to recognised WiFi
 */
  count = 0;
  Serial.print(" WiFi Connecting ");
  fn_WiFi_Connect( net );

  strcpy(my_IP_Address, WiFi.localIP().toString().c_str());
  strcpy(my_MAC_Address, WiFi.macAddress().c_str());
/*
 *  MQTT setup
 */
  client.setServer(MQTT_server, MQTT_PORT);
  client.setCallback(MQTT_CB);
  delay(1000);
  client.loop();
  fn_MQTT_Connect();
/*
 *	Arduino OTA Setup
 */ 
  ArduinoOTA.setHostname("newvane");
  ArduinoOTA.setPassword("starwest");
/*
 *  Arduino OTA Callbacks
 */
  ArduinoOTA.onStart([]() {
    Serial.println("Start");
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_FS
      type = "filesystem";
    }
  });
  
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR)   Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR)     Serial.println("End Failed");
	});

  ArduinoOTA.begin();

  epoch = millis();      //  start the clock

  strcpy( MQTT_PUB, "ESP 8266,");
  strcat( MQTT_PUB, my_IP_Address );
  strcat( MQTT_PUB, "," );
   strcat( MQTT_PUB, my_MAC_Address );
  /*  publish to MQTT  */
      client.publish(MQTT_DEVICES, (const uint8_t*)MQTT_PUB, strlen(MQTT_PUB), false);
}
/*
 *  END of SETUP
 */ 

/*
 *  Scan WiFi for networks
 */
int fn_WiFiScan() {
  networks = WiFi.scanNetworks();
  Serial.print("Networks found : ");
  Serial.println( networks );

  if (networks == 0) return (-2);               // No Networks found

  for (network = 0; network <= networks; network++) {
    Serial.print( "Network : " );
    Serial.print( network );
    Serial.print( " : " );
    Serial.print( WiFi.SSID(network).c_str());
    Serial.print( " : " );
  	Serial.println( WiFi.RSSI() ); // Get the RSSI value
  }
  for (network = 0; network <= networks; network++) {
    if (strcmp(WiFi.SSID(network).c_str(), Rich_WiFi) == 0) {
      strcpy(WiFi_Pass, Rich_Pass);
      strcpy(MQTT_server, Rich_MQTT);
      return (network);
    }
    if (strcmp(WiFi.SSID(network).c_str(), Home_WiFi) == 0) {
      strcpy(WiFi_Pass, Home_Pass);
      strcpy(MQTT_server, Home_MQTT);
      return (network);
    }
    if (strcmp(WiFi.SSID(network).c_str(), MiS_WiFi) == 0) {
      strcpy(WiFi_Pass, MiS_Pass);
      strcpy(MQTT_server, MiS_MQTT);
      return (network);
    }
    if (strcmp(WiFi.SSID(network).c_str(), Jim_WiFi) == 0) {
      strcpy(WiFi_Pass, Jim_Pass);
      strcpy(MQTT_server, Jim_MQTT);
      return (network);
    }
  }
/* if we get here the network found is not recognised */
  fn_ReStart();
  return(0);
}

/*	Connect to Wifi */
int fn_WiFi_Connect(int network) {
  int j = 0;
  delay(2000);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WiFi.SSID(network), WiFi_Pass);

  while (j++ < 20) {
    if (WiFi.status() == WL_CONNECTED) return(0);
    Serial.print(".");
	delay(1000);
  }

  fn_ReStart();
  return(0);
}

/*
 *  MQTT callback
 */
int MQTT_CB(char* topic, byte* payload, uint8_t length) {
  Serial.println( "Message Arrived" );
  int k;
  memset(MQTT_in_topic, 0, sizeof(MQTT_in_topic));
  memset(MQTT_in_buffer, 0, sizeof(MQTT_in_buffer));

  strcpy(MQTT_in_topic, topic);
  MQTT_in_length = length;
  for (k = 0; k < length; k++) MQTT_in_buffer[k] = (char)payload[k];
  MQTT_in_buffer[k] = 0;
  Serial.print( "MQTT : " );
  Serial.print( MQTT_in_topic );
  Serial.print( " : " );
  Serial.println( MQTT_in_buffer );
  
  if (strcmp(MQTT_in_topic, MQTT_STATIN) == 0)  // Status Enquiry
  {
    if (strcmp(MQTT_in_buffer, "ASK") == 0)
    {
      strcpy( MQTT_PUB, "OK - ");
      strcat( MQTT_PUB, Version );
/*  publish status to MQTT  */
      client.publish(MQTT_STATOUT, (const uint8_t*)MQTT_PUB, strlen(MQTT_PUB), false);
    }
  }
  if (strcmp(MQTT_in_topic, MQTT_RST) == 0)
  {
    if (strcmp(MQTT_in_buffer, "Yes") == 0) fn_ReStart();
  }
  return (1);
}

/*	connect to MQTT broker */
int fn_MQTT_Connect() {
  int j = 0;
  while (j < 10) {
    if (client.connect(MiS_DEVICE) == 1) {
      delay(2000);
      client.subscribe(MQTT_RST,  1);
      client.subscribe(MQTT_STATIN, 1);
      return (0);
    }
    Serial.print(F("."));
    delay( 3000 );
    j++;
  }
  fn_ReStart();
  return( 1 );
}

void fn_ReStart(void) {
  ESP.restart();
}

void fn_sample( void ) {
  uint8_t p = 0;
  epoch = millis();
  memset( dir, 0, sizeof(dir));
  
  while ( ( millis() - epoch ) < 15000 )
  {
    p = pcf.digitalReadByte();
//    Serial.print( "8574 : " );
//  	Serial.println( p );
    switch( p )
    {
      case 1:     pa = 1;     break;    //  1
      case 3:     pa = 2;     break;    //  1 + 2
      case 2:     pa = 3;     break;    //  2
      case 6:     pa = 4;     break;    //  2 + 4
      case 4:     pa = 5;     break;    //  4
      case 12:    pa = 6;     break;    //  4 + 8
      case 8:     pa = 7;     break;    //  8
      case 24:    pa = 8;     break;    //  8 + 16
      case 16:    pa = 9;     break;    // 16
      case 48:    pa = 10;    break;    // 16 + 32
      case 32:    pa = 11;    break;    // 32
      case 96:    pa = 12;    break;    // 32 + 64
      case 64:    pa = 13;    break;    // 64
      case 192:   pa = 14;    break;    // 64 + 128
      case 128:   pa = 15;    break;    //128
      case 129:   pa = 16;    break;    //128 + 1
      default:    pa = 0;     break;
    }    
    dir[ pa ] = dir[ pa ] + 1;
    delay( 100 );
    client.loop();
  }
  period = millis() - epoch;
}

void loop() {
  ArduinoOTA.handle();
  sequence++;
  fn_sample();
  sprintf( MQTT_PUB, "$%05i,%05i", sequence, period);
  for ( i=0; i<=16; i++)
  {
    strcat( MQTT_PUB, "," );
    sprintf( my_dir, "%03i", dir[ i ] );
    strcat( MQTT_PUB, my_dir );
  }
  strcat( MQTT_PUB, "#" );
  /*	publish <UID> to MQTT  */
  if (!client.connected()) fn_MQTT_Connect();
  Serial.print( "MQTT Publish : ");
  Serial.println( MQTT_PUB );
  client.publish(MQTT_OUT, (const uint8_t*)MQTT_PUB, strlen(MQTT_PUB), false);
}
