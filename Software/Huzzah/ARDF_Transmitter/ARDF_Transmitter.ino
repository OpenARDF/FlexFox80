/**********************************************************************************************
    Copyright © 2022 Digital Confections LLC

    Permission is hereby granted, free of charge, to any person obtaining a copy of
    this software and associated documentation files (the "Software"), to deal in the
    Software without restriction, including without limitation the rights to use, copy,
    modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
    and to permit persons to whom the Software is furnished to do so, subject to the
    following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
    INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
    PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
    FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
    OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

 **********************************************************************************************

     Basic WiFi Functionality for Receiver Development Platform

     Hardware Target: Adafruit HUZZAH ESP8266

     This sketch provides the following functionality on the target hardware:

       1. Receives commands over the UART0 port. Commands must start with $$$ as the escape sequence.

       2. $$$m,#,val;  <- this command allows the ATmega328p to set variables that the WiFi board
          will use; such as the SSID and password for a hotspot for Internet access.

       3. $$$t; <- this tells the WiFi to connect to an Internet hotspot and attempt to read the current
          NIST time; if successful the WiFi sends a command to the ATmega328p telling it to update the time
          stored in the real-time clock.

       4. $$$b; <- this tells the WiFi to set itself up as a UART-to-TCP bridge, and accept TCP connections
          from any clients (like a smartphone or PC)

       5. $$$w; <- starts a web server that accepts connections from WiFi devices.

*/

#include <Arduino.h>
/*#include <GDBStub.h> */
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFiMulti.h>
/*#include <ESP8266mDNS.h> */
#include <user_interface.h>
#include "esp8266.h"
/*#include <ArduinoOTA.h> */
#include <LittleFS.h> // LittleFS is declared
#include <Hash.h>
#include <WebSocketsServer.h>
#include <WebSocketsClient.h>
#include <ESP8266WiFiType.h>
#include <time.h>
#include "Transmitter.h"
#include "Event.h"
/* #include <Wire.h> */
#include "Helpers.h"
#include "CircularStringBuff.h"
#include "Blinkies.h"

/* Global variables are always prefixed with g_ */

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
bool g_debug_prints_enabled = DEBUG_PRINTS_ENABLE_DEFAULT;
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

bool g_LEDs_enabled = LEDS_ENABLE_DEFAULT;
bool g_baud_sync_success = false;

/*
    TCP to UART Bridge
*/

#define MAX_SRV_CLIENTS 3   /*how many clients should be able to telnet to this ESP8266 */
/*WiFiServer g_tcpServer(BRIDGE_TCP_PORT_DEFAULT.toInt());
  WiFiClient g_tcpServerClients[MAX_SRV_CLIENTS]; */

// String g_bridgeIPaddr = BRIDGE_IP_ADDR_DEFAULT;
///String g_bridgeSSID = BRIDGE_SSID_DEFAULT;
String g_bridgePW = BRIDGE_PW_DEFAULT;  /* minimum 8 characters */
// String g_bridgeTCPport = BRIDGE_TCP_PORT_DEFAULT;


/* Access Point */
String g_masterSSID = MASTER_SSID;
String g_masterSSID_PWD = BRIDGE_PW_DEFAULT;
bool g_IamMaster = MASTER_ENABLE_DEFAULT;

/* Time Retrieval */
/*
    NIST Time Sync
    See https://www.arduino.cc/en/Tutorial/UdpNTPClient
    See http://tf.nist.gov/tf-cgi/servers.cgi
*/
// String g_timeHost = TIME_HOST_DEFAULT;
// String g_timeHTTPport = TIME_HTTP_PORT_DEFAULT;

/* UDP time query */
#define NTP_PACKET_SIZE 48                                  /* NTP time stamp is in the first 48 bytes of the message */
WiFiUDP g_UDP;                                              /* A UDP instance to let us send and receive packets over UDP */
unsigned int g_localPort = 2390;                            /* local port to listen for UDP  packets */

/* HTTP web server */
ESP8266WebServer g_http_server(80);                         /* HTTP server on port 80 */
WebSocketsServer g_webSocketServer = WebSocketsServer(81);  /* create a websocket server on port 81 */
String g_AP_NameString;
WebSocketsClient g_webSocketLocalClient;

typedef struct webSocketClient
{
  int8_t webID;
  int8_t socketID;
  String macAddr;
} WebSocketClient;

WebSocketClient g_webSocketClient[MAX_NUMBER_OF_WEB_CLIENTS];   /* Keep track of active clients and their MAC addresses */
uint8_t g_numberOfSocketClients = 0;
uint8_t g_numberOfWebClients = 0;
uint8_t g_socket_timeout = 0;

ESP8266WiFiMulti wifiMulti; /* Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti' */
File fsUploadFile;          /* a File variable to temporarily store the received file */

String g_softAP_IP_addr;

void handleRoot();          /* function prototypes for HTTP handlers */
void handleNotFound();

/*
    Main Program Support
*/

//unsigned long g_lastAccessToNISTServers = 0;
//bool g_timeWasSet = false;
bool g_justPoweredUp = true;

WebSocketSlaveState g_webSocketSlaveState = WSClientConnecting;
bool g_master_has_connected_slave = false;
bool g_slave_released = true;
bool g_slave_received_new_event_file = false;
int g_slave_socket_number = -1;
int g_files_sent_to_slave = 0;
bool g_onlyUpdateEvent = false;

#define NO_ACTIVITY_TIMEOUT (60 * 5)
int g_noActivityTimeoutSeconds = NO_ACTIVITY_TIMEOUT;

unsigned long g_timeOfDayFromTx = 0;

static WiFiEventHandler e1, e2;

Transmitter *g_xmtr = NULL;
Event* g_activeEvent = NULL;
String g_selectedEventName = String("");
String g_atmega_sw_version = String("");
int g_activeEventIndex = 0;
EventFileRef g_eventList[20];
int g_numberOfEventFilesFound = 0;
int g_numberOfScheduledEvents = 0;
TxCommState g_ESP_Comm_State = TX_WAKE_UP;
TxCommState g_Hold_Comm_State = TX_INVALID_STATE;

CircularStringBuff *g_LBOutputBuff = NULL;
CircularStringBuff *g_fileDataBuff = NULL;
int g_linkBusAckPending = 0;
int g_linkBusNacksReceived = 0;
bool g_linkBusAckTimoutOccurred = false;

Blinkies *lights;

void httpWebServerLoop(int blinkRate);
bool populateEventFileList(void);
bool readDefaultsFile(void);
void saveDefaultsFile(void);
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
void showSettings(void);
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
void handleFileUpload(void);
void handleFileDownload(void);
void fileDelete(void);
void fileDeleteWithMessage(String msg);
void handleFileDelete(void);
void handleLBMessage(String message);
void handleFS(void);
bool linkbusLoop(void);
bool clientConnectLoop();
bool clientUpdateEventFilesLoop();
bool waitForTimeLoop();
bool sendEventToATMEGA(String * errorTxt);
bool loadActiveEventFile(String updatedFileName);
int numberOfEventsScheduled(unsigned long epoch);
void shutdownSlave(void);

void setup()
{
  Serial.begin(SERIAL_BAUD_RATE);

  lights = new Blinkies();

  startLittleFS();                  /* Start the LittleFS and list all contents */
  if (!readDefaultsFile())        /* Read default settings from file system */
  {
    saveDefaultsFile();
  }

#if WIFI_DEBUG_PRINTS_ENABLED
  WiFi.onEvent(eventWiFi);    /* Handle WiFi event */
#endif /* WIFI_DEBUG_PRINTS_ENABLED */

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
  g_xmtr = new Transmitter(g_debug_prints_enabled);
#else
  g_xmtr = new Transmitter(false);
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

#define LB_OUTPUT_BUFF_SIZE 25
  g_LBOutputBuff = new CircularStringBuff(LB_OUTPUT_BUFF_SIZE);
}

/*******************************************************
    Handle WiFi events
 ********************************************************/
#if WIFI_DEBUG_PRINTS_ENABLED
void eventWiFi(WiFiEvent_t event)
{
  String e = String(event);

  switch (event)
  {
    case WIFI_EVENT_STAMODE_CONNECTED:
      {
        Serial.println(String("[WiFi] " + e + ", Connected"));
      }
      break;

    case WIFI_EVENT_STAMODE_DISCONNECTED:
      {
        Serial.println(String("[WiFi] " + e + ", Disconnected - Status " + String( WiFi.status()) + String(connectionStatus( WiFi.status() ).c_str()) ));
      }
      break;

    case WIFI_EVENT_STAMODE_AUTHMODE_CHANGE:
      {
        Serial.println(String("[WiFi] " + e + ", AuthMode Change"));
      }
      break;

    case WIFI_EVENT_STAMODE_GOT_IP:
      {
        Serial.println(String("[WiFi] " + e + ", Got IP"));
      }
      break;

    case WIFI_EVENT_STAMODE_DHCP_TIMEOUT:
      {
        Serial.println(String("[WiFi] " + e + ", DHCP Timeout"));
      }
      break;

    case WIFI_EVENT_SOFTAPMODE_STACONNECTED:
      {
        Serial.println(String("[WiFi] " + e + ", Client Connected"));
      }
      break;

    case WIFI_EVENT_SOFTAPMODE_STADISCONNECTED:
      {
        Serial.println(String("[AP] " + e + ", Client Disconnected"));
      }
      break;

    case WIFI_EVENT_SOFTAPMODE_PROBEREQRECVED:
      {
        Serial.println(String("[AP] " + e + ", Probe Request Recieved"));
      }
      break;

    case WIFI_EVENT_ANY:
      {
        Serial.println(String("[AP] " + e + ", WIFI_EVENT_ANY Recieved"));
      }
      break;

    case WIFI_EVENT_MODE_CHANGE:
      {
        Serial.println(String("[AP] " + e + ", WIFI_EVENT_MODE_CHANGE Recieved"));
      }
      break;

    default:
      {
        /*    case WIFI_EVENT_MAX: */
        Serial.println(String("[AP] " + e + ", WIFI_EVENT_MAX Recieved"));
      }
      break;
  }
}
#endif  /* WIFI_DEBUG_PRINTS_ENABLED */


/*******************************************************
    WiFi Connection Status
 ********************************************************/
String connectionStatus( int which )
{
  switch ( which )
  {
    case WL_CONNECTED:
      {
        return ( "Connected");
      }
      break;

    case WL_NO_SSID_AVAIL:
      {
        return ( "Network not availible");
      }
      break;

    case WL_CONNECT_FAILED:
      {
        return ( "Wrong password");
      }
      break;

    case WL_IDLE_STATUS:
      {
        return ( "Idle status");
      }
      break;

    case WL_DISCONNECTED:
      {
        return ( "Disconnected");
      }
      break;

    default:
      {
        return ( "Unknown");
      }
      break;
  }
}

/*===============================================================
   This routine is executed when you open its IP in browser
  =============================================================== */
void handleRoot()
{
  g_http_server.send(200, "text/html", "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><h2 style=\"font-family:verdana; font-size:30px; color:Black; text-align:left;\">Options</h2><p style=\"font-family:verdana; font-size:20px; color:Black; text-align:left;\">Configure events: <a href=\"/events.html\">73.73.73.73/events.html</a></p> <p style=\"font-family:verdana; font-size:20px; color:Black; text-align:left;\">Upload a file: <a href=\"/upload.html\">73.73.73.73/upload.html</a></p> <p style=\"font-family:verdana; font-size:20px; color:Black; text-align:left;\">Download a file: <a href=\"/download.html\">73.73.73.73/download.html</a></p> <p style=\"font-family:verdana; font-size:20px; color:Black; text-align:left;\">Delete a file: <a href=\"/delete.html\">73.73.73.73/delete.html</a> <- Use with caution!</p> <p style=\"font-family:verdana; font-size:20px; color:Black; text-align:left;\">Testing support: <a href=\"/test.html\">73.73.73.73/test.html</a></p> <p style=\"font-family:verdana; font-size:20px; color:Black; text-align:left;\">Radio Mode: <a href=\"/radio.html\">73.73.73.73/radio.html</a></p> <p style=\"font-size:12\">ESP8266 SW Date: 27-May-2022</p> ");
}

void handleFS()
{
  String message = "<p style=\"text-align:left;\"><a href=\\ \"73.73.73.73\">[HOME]</a></p> <h2 style=\"font-family:verdana; font-size:30px; color:Black; text-align:left;\">File System Contents</h2>";
  String line;
  Dir dir = LittleFS.openDir("/");

  while (dir.next())
  { /* List the file system contents */
    String fileName = dir.fileName();
    size_t fileSize = dir.fileSize();
    line = String("<p>" + fileName + ", size: " + formatBytes(fileSize) + "</p>");
    message += line;
  }

  message += "<form method=\"post\" enctype=\"multipart/form-data\">\r\n";
  message += "File name:<input type=\"text\" name=\"name\">\r\n";
  message += "<input class=\"button\" type=\"submit\" value=\"Download\">\r\n";
  message += "</form>";

  g_http_server.send(200, "text/html", message);
}

void handleUpload()
{
  String message = "<p style=\"text-align:left;\"><a href=\\ \"73.73.73.73\">[HOME]</a></p><form method=\"post\" enctype=\"multipart/form-data\"><input type=\"file\" name=\"name\"><input class=\"button\" type=\"submit\" value=\"Upload\"></form>";

  g_http_server.send(200, "text/html", message);
}

void handleSuccess()
{
  String message = "<html><head><title>Success</title></head><body><p><a href=\\ \"73.73.73.73\">[HOME]</a></p><h1 style=\"font-family:verdana;font-size:30px;color:Black;text-align:left;\">File uploaded successfully!</h1></body></html>";

  g_http_server.send(200, "text/html", message);
}

void fileDelete()
{
  fileDeleteWithMessage("");
}

void fileDeleteWithMessage(String msg)
{
  String message = "<p style=\"text-align:left;\"><a href=\\ \"73.73.73.73\">[HOME]</a></p> <h2 style=\"font-family:verdana; font-size:30px; color:Black; text-align:left;\">File System Contents</h2>";
  String line;
  Dir dir = LittleFS.openDir("/");

  while (dir.next())
  { /* List the file system contents */
    String fileName = dir.fileName();
    size_t fileSize = dir.fileSize();
    line = String("<p>" + fileName + ", size: " + formatBytes(fileSize) + "</p>");
    message += line;
  }

  message += "<form method=\"post\" enctype=\"multipart/form-data\">\r\n";
  message += "File name:<input type=\"text\" name=\"name\">\r\n";
  message += "<input class=\"button\" type=\"submit\" value=\"Delete\">\r\n";
  message += "</form>";

  if (msg.length() > 0)
  {
    message += msg;
  }

  g_http_server.send(200, "text/html", message);
}

void handleNotFound()
{
  /* if the requested file or page doesn't exist, return a 404 not found error */
  if (!handleFileRead(g_http_server.uri()))
  { /* check if the file exists in the flash memory (LittleFS), if so, send it */
    String message = "<p style=\"text-align:left;\"><a href=\\ \"73.73.73.73\">[HOME]</a></p><p>File Not Found</p>";
    message += "URI: ";
    message += g_http_server.uri();
    message += "\nMethod: ";
    message += (g_http_server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += g_http_server.args();
    message += "\n";
    for (uint8_t i = 0; i < g_http_server.args(); i++)
    {
      message += " " + g_http_server.argName(i) + ": " + g_http_server.arg(i) + "\n";
    }
    g_http_server.send(404, "text/html", message);
  }
  else
  {
    String filename = String(g_http_server.uri());

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (g_debug_prints_enabled)
    {
      Serial.println("File read:" + filename);
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
  }
}


bool setupHTTP_AP()
{
  bool success = false;

  g_numberOfSocketClients = 0;
  g_numberOfWebClients = 0;
  Serial.setDebugOutput(false);

#if WIFI_DEBUG_PRINTS_ENABLED
  Serial.printf("Initial connection status: %d\n", WiFi.status());
  WiFi.printDiag(Serial);
#endif

  WiFi.mode(WIFI_AP_STA);
  /* Event subscription */
  e1 = WiFi.onSoftAPModeStationConnected(onNewStation);
  e2 = WiFi.onSoftAPModeStationDisconnected(onStationDisconnect);

  /*  WiFi.mode(WIFI_AP); */

  if (g_IamMaster)
  {
    g_AP_NameString = MASTER_SSID;
  }
  else
  {
    /* Create a somewhat unique SSID */
    uint8_t mac[WL_MAC_ADDR_LENGTH];
    WiFi.softAPmacAddress(mac);
    String macID = String(mac[WL_MAC_ADDR_LENGTH - 4], HEX) +
                   String(mac[WL_MAC_ADDR_LENGTH - 3], HEX) +
                   String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                   String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
    macID.toUpperCase();
    g_AP_NameString = String("Tx_" + macID);
  }

  IPAddress apIP = stringToIP(g_softAP_IP_addr);

  if (apIP == IPAddress(-1, -1, -1, -1))
  {
    apIP = stringToIP(String(SOFT_AP_IP_ADDR_DEFAULT)); /* some reasonable default address */
  }

  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

  if (g_bridgePW.length() > 0)
  {
    success = WiFi.softAP(stringObjToConstCharString(&g_AP_NameString), stringObjToConstCharString(&g_bridgePW));
  }
  else
  {
    success = WiFi.softAP(stringObjToConstCharString(&g_AP_NameString));
  }

  if (success)
  {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    IPAddress myIP = WiFi.softAPIP();   /*Get IP address */
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

    /* Start TCP listener on port TCP_PORT */
    g_http_server.on("/", HTTP_GET, handleRoot);                        /* Call the 'handleRoot' function when a client requests URI "/" */

    g_http_server.on("/download", HTTP_GET, handleFS);                        /* Provide utility to help manage the file system */
    g_http_server.on("/download.html", HTTP_GET, handleFS);                   /* Provide utility to help manage the file system */

    g_http_server.on("/download", HTTP_POST, handleFileDownload);             /* go to 'handleFileDownload' */
    g_http_server.on("/download.html", HTTP_POST, handleFileDownload);        /* go to 'handleFileDownload' */

    g_http_server.on("/delete", HTTP_GET, fileDelete);              /* Provide utility to help manage the file system */
    g_http_server.on("/delete.html", HTTP_GET, fileDelete);         /* Provide utility to help manage the file system */

    g_http_server.on("/delete", HTTP_POST, handleFileDelete);       /* go to 'handleFileDelete' */
    g_http_server.on("/delete.html", HTTP_POST, handleFileDelete);  /* go to 'handleFileDelete' */

    g_http_server.on("/upload", HTTP_GET, handleUpload);
    g_http_server.on("/upload.html", HTTP_GET, handleUpload);

    g_http_server.on("/upload.html", HTTP_POST, [] ()
    { /* If a POST request is sent to the /upload.html address, */
      g_http_server.send(200, "text/plain", "");
    }, handleFileUpload);                       /* go to 'handleFileUpload' */

    g_http_server.on("/upload", HTTP_POST, [] ()
    { /* If a POST request is sent to the /upload.html address, */
      g_http_server.send(200, "text/plain", "");
    }, handleFileUpload);                       /* go to 'handleFileUpload' */

    g_http_server.onNotFound(handleNotFound);   /* When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound" */
    g_http_server.begin();

    /*    startWebSocketServer(); // Start a WebSocket server */

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (g_debug_prints_enabled)
    {
      Serial.print(g_AP_NameString + " server started: ");
      Serial.println(myIP);
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
  }

  return (success);
}


/* Manage incoming device connection on ESP access point */
void onNewStation(WiFiEventSoftAPModeStationConnected sta_info)
{
  char newMac[20];

  sprintf(newMac, MACSTR, MAC2STR(sta_info.mac));
  String newMacStr = String(newMac);
  newMacStr.toUpperCase();
  bool found = false;

  for (int i = 0; i < MAX_NUMBER_OF_WEB_CLIENTS; i++)
  {
    if (newMacStr.equals(g_webSocketClient[i].macAddr))
    {
      found = true;
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
      if (g_debug_prints_enabled)
      {
        Serial.println("Station found in existing list.");
      }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
      g_webSocketClient[i].webID = sta_info.aid;
      g_webSocketClient[i].socketID = WEBSOCKETS_SERVER_CLIENT_MAX;
      break;
    }
  }

  g_numberOfWebClients = WiFi.softAPgetStationNum();
  g_numberOfSocketClients = g_webSocketServer.connectedClients(false);

  if (!found)
  {
    int top = max(0, g_numberOfWebClients - 1);
    g_webSocketClient[top].macAddr = newMacStr;
    g_webSocketClient[top].webID = sta_info.aid;
    g_webSocketClient[top].socketID = WEBSOCKETS_SERVER_CLIENT_MAX;
  }

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
  if (g_debug_prints_enabled)
  {
    Serial.println("New Station: " + newMacStr);
    Serial.println("  Total stations: " + String(g_numberOfWebClients) + "; Socket clients: " + String(g_numberOfSocketClients));

    for (int i = 0; i < g_numberOfSocketClients; i++)
    {
      Serial.printf("%d. WebSocketClient: WebID# %d. MAC address : %s\n", i, g_webSocketClient[i].webID, (g_webSocketClient[i].macAddr).c_str());
    }
  }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

  g_noActivityTimeoutSeconds = NO_ACTIVITY_TIMEOUT;
  startWebSocketServer(); /* Start a WebSocket server */
}

void onStationDisconnect(WiFiEventSoftAPModeStationDisconnected sta_info)
{
  char newMac[20];

  sprintf(newMac, MACSTR, MAC2STR(sta_info.mac));
  String oldMacStr = String(newMac);
  oldMacStr.toUpperCase();

  for (int i = 0; i < MAX_NUMBER_OF_WEB_CLIENTS; i++)
  {
    if (oldMacStr.equals(g_webSocketClient[i].macAddr))
    {
      g_webSocketClient[i].webID = 0;
      g_webSocketClient[i].macAddr = "";
      g_webSocketClient[i].socketID = WEBSOCKETS_SERVER_CLIENT_MAX;

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
      if (g_debug_prints_enabled)
      {
        Serial.println("Station removed from list.");
      }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
      break;
    }
  }

  g_numberOfWebClients = WiFi.softAPgetStationNum();
  g_numberOfSocketClients = g_webSocketServer.connectedClients(false);

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
  if (g_debug_prints_enabled)
  {
    Serial.println("Station Exit:");
    Serial.printf("  Remaining stations: %d\n", g_numberOfWebClients);
  }

  if (g_debug_prints_enabled)
  {
    for (int i = 0; i < MAX_NUMBER_OF_WEB_CLIENTS; i++)
    {
      if ((g_webSocketClient[i].macAddr).length())
      {
        Serial.printf("WebID# %d. sockID# %d. MAC address : %s\n", g_webSocketClient[i].webID, g_webSocketClient[i].socketID, (g_webSocketClient[i].macAddr).c_str());
      }
    }
  }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
}


bool setupWiFiAPConnection()
{
  bool err = false;
  int tries = 0;
  bool sentComsOFF = false;

  lights->setLEDs(RED_BLUE_TOGETHER, g_LEDs_enabled);

  /* We start by connecting to a WiFi network */

#ifdef MULTI_ACCESS_POINT_SUPPORT
  if (strlen(stringObjToConstCharString(&g_masterSSID_PWD)) > 0)
  {
    wifiMulti.addAP(stringObjToConstCharString(&g_masterSSID), stringObjToConstCharString(&g_masterSSID_PWD));    /* add Wi-Fi networks you want to connect to */
  }
  else
  {
    wifiMulti.addAP(stringObjToConstCharString(&g_masterSSID));
  }

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
  if (g_debug_prints_enabled)
  {
    Serial.print("Connecting ...");
  }
#endif // #if TRANSMITTER_COMPILE_DEBUG_PRINTS

  unsigned long holdMillis = millis();

  while (wifiMulti.run() != WL_CONNECTED)
  {
    linkbusLoop();

    if ((unsigned long)(millis() - holdMillis) > 250)
    {
      holdMillis = millis();
      if (lights->blinkLEDs(100, RED_BLUE_TOGETHER, true))
      {
        if (!sentComsOFF)
        {
          lights->setLEDs(RED_BLUE_TOGETHER, true);
          Serial.printf(LB_MESSAGE_WIFI_COMS_OFF);    /* send immediate */
          sentComsOFF = true;
          if (g_slave_released)
          {
            g_webSocketSlaveState = WSClientCleanup;
          }
          err = true;
          break;
        }
      }
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
      if (g_debug_prints_enabled)
      {
        Serial.print(".");
      }
#endif // #if TRANSMITTER_COMPILE_DEBUG_PRINTS

      tries++;

      if (tries > 60)
      {
        err = true;
        break;
      }
    }
  }

#else // MULTI_ACCESS_POINT_SUPPORT

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
  if (g_debug_prints_enabled)
  {
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(g_masterSSID);
  }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

  if (WiFi.status() == WL_CONNECTED)
  {
    WiFi.disconnect();
    delay(2500);
  }

  if (g_masterSSID_PWD.length() < 1)
  {
    WiFi.begin(stringObjToConstCharString(&g_masterSSID);
  }
  else
  {
    WiFi.begin(stringObjToConstCharString(&g_masterSSID), stringObjToConstCharString(&g_masterSSID_PWD));
  }

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(250);
    if (lights->blinkLEDs(100, RED_BLUE_TOGETHER, true))
    {
      if (!sentComsOFF)
      {
        lights->setLEDs(RED_BLUE_TOGETHER, true);
        Serial.printf(LB_MESSAGE_WIFI_COMS_OFF);    /* send immediate */
        sentComsOFF = true;
        if (g_slave_released)
        {
          g_webSocketSlaveState = WSClientCleanup;
        }
        err = true;
        break;
      }
    }

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (g_debug_prints_enabled)
    {
      Serial.print(".");
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

    tries++;

    if (tries > 60)
    {
      err = true;
      break;
    }
    else if ((tries == 20) || (tries == 40))
    {
      if (g_hotspotPW.length() < 1)
      {
        WiFi.begin(stringObjToConstCharString(&g_masterSSID));
      }
      else
      {
        WiFi.begin(stringObjToConstCharString(&g_masterSSID), stringObjToConstCharString(&g_masterSSID_PWD));
      }
    }
  }
#endif  /* MULTI_ACCESS_POINT_SUPPORT */


  if (err)
  {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (g_debug_prints_enabled)
    {
      Serial.println('\n');
      Serial.println("Unable to find master tx.");
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
    lights->setLEDs(RED_BLUE_TOGETHER, true);
  }
  else
  {
    lights->setLEDs(BLUE_LED_ONLY, true);

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (g_debug_prints_enabled)
    {
      Serial.println('\n');
      Serial.print("Connected to ");
      Serial.println(WiFi.SSID());    /* Tell us what network we're connected to */
      Serial.print("IP address:\t");
      Serial.println(WiFi.localIP()); /* Send the IP address of the ESP8266 to the computer */
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
  }

  return (err);
}


void loop()
{
  String arg1, arg2;
  int blinkPeriodMillis = 500;

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
  g_debug_prints_enabled = DEBUG_PRINTS_ENABLE_DEFAULT;   /* kluge override of saved settings */

  /* HTTP Server */
  if (g_debug_prints_enabled)
  {
    Serial.println("Starting loop()...");
  }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

  lights->setLEDs(LEDS_ON, g_LEDs_enabled);

  waitForTimeLoop(); /* Wait for baud calibration and time update from ATMEGA */

  if (g_IamMaster)
  {
    populateEventFileList();   /* read files into the event list */
    g_numberOfScheduledEvents = numberOfEventsScheduled(g_timeOfDayFromTx);
    g_LBOutputBuff->put(LB_MESSAGE_ESP_KEEPALIVE);
    linkbusLoop();

    if (g_numberOfScheduledEvents)
    {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
      if (g_debug_prints_enabled)
      {
        String msg = String("Scheduled event(s): ") + g_numberOfScheduledEvents;
        Serial.println(msg);
      }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

      if (!g_activeEvent)
      {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
        g_activeEvent = new Event(g_debug_prints_enabled);
#else
        g_activeEvent = new Event(false);
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
      }

      g_activeEvent->readEventFile(g_eventList[0].path);
      g_LBOutputBuff->put(LB_MESSAGE_ESP_KEEPALIVE);

      if (!loadActiveEventFile(g_eventList[0].path))
      {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
        if (g_debug_prints_enabled)
        {
          String msg = String("Scheduled event loaded: ");
          msg = msg + g_activeEvent->getEventName();
          Serial.println(msg);
        }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
      }
      else
      {
        blinkPeriodMillis = 100;
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
        if (g_debug_prints_enabled)
        {
          Serial.println("Scheduled event load failed.");
        }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
      }
    }
    else
    {
      blinkPeriodMillis = 100;
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
      if (g_debug_prints_enabled)
      {
        Serial.println("No scheduled events found.");
      }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
    }
  }
  else /* IamSlave */
  {
    if (!g_onlyUpdateEvent && !setupWiFiAPConnection())
    {
      g_webSocketLocalClient.begin("73.73.73.73", 81);
      g_webSocketLocalClient.onEvent(webSocketClientEvent);

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
      if (g_debug_prints_enabled)
      {
        Serial.println("Client started\n");
      }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

      g_slave_released = true;

      if (!clientConnectLoop())
      {
        Serial.println(LB_MESSAGE_ESP_KEEPALIVE);

        if (!clientUpdateEventFilesLoop())
        {
          g_LBOutputBuff->put(LB_MESSAGE_ESP_KEEPALIVE);
          populateEventFileList();   /* read files into the event list */
          g_numberOfScheduledEvents = numberOfEventsScheduled(g_timeOfDayFromTx);
          linkbusLoop();

          if (g_numberOfScheduledEvents)
          {
            g_LBOutputBuff->put(LB_MESSAGE_ESP_KEEPALIVE);
            if (!loadActiveEventFile(g_eventList[0].path))
            {
              g_activeEventIndex = 0;
              String msg = String(String(SOCK_COMMAND_SLAVE_UPDATE_SUCCESS) + "," + g_activeEvent->getTxDescriptiveName(g_activeEvent->getTxAssignment()) + "," + g_activeEvent->getEventName());
              g_webSocketLocalClient.sendTXT(stringObjToConstCharString(&msg)); /* Send to Master */
              shutdownSlave();
              blinkPeriodMillis = 500;
            }
            else
            {
              blinkPeriodMillis = 100;
            }

            shutdownSlave();
          }
          else
          {
            String msg = String(SOCK_COMMAND_SLAVE_UPDATE_ERROR);
            msg = msg + ",No events scheduled to run";
            g_webSocketLocalClient.sendTXT(stringObjToConstCharString(&msg)); /* Send to Master */
            blinkPeriodMillis = 100;
            shutdownSlave();
          }

          g_webSocketServer.loop();
        }
        else
        {
          blinkPeriodMillis = 100;
          shutdownSlave();
        }
      }
      else
      {
        blinkPeriodMillis = 100;
      }
    }
    else
    {
      /* Load the next active event */
      populateEventFileList();   /* read files into the event list */
      g_numberOfScheduledEvents = numberOfEventsScheduled(g_timeOfDayFromTx);
      linkbusLoop();

      if (g_numberOfScheduledEvents)
      {
        if (loadActiveEventFile(g_eventList[0].path))
        {
          blinkPeriodMillis = 100;
        }
      }
      else
      {
        blinkPeriodMillis = 100;
      }
    }
  }

  if (g_onlyUpdateEvent)
  {
    unsigned long last = 0;

    while (1)
    {
      linkbusLoop();
      yield();
      if ((unsigned long)(millis() - last) > 5000)
      {
        last = millis();
        String msgOut = String(LB_MESSAGE_ESP_SHUTDOWN); /* Tell ATMEGA to shut down WiFi */
        g_LBOutputBuff->put(msgOut);
      }
    }
  }
  else if (setupHTTP_AP())
  {
    httpWebServerLoop(blinkPeriodMillis); /* Pass the blink rate to the next loop */
  }

  Serial.println("Escaped!");

  lights->setLEDs(LEDS_OFF, g_LEDs_enabled);
}

void shutdownSlave()
{
  int times2try = 2;
  bool ready = false;
  unsigned long last = millis();
  int state = 0;

  while (!ready)
  {
    g_webSocketLocalClient.loop();
    linkbusLoop();
    yield();

    switch (state)
    {
      case 0:
        {
          if (times2try)
          {
            if ((unsigned long)(millis() - last) > 1000)
            {
              last = millis();
              times2try--;
            }
          }
          else
          {
            String msg = String(SOCK_COMMAND_SLAVE) + "," + SLAVE_FREE;
            g_webSocketLocalClient.sendTXT(stringObjToConstCharString(&msg)); /* Send to Master */
            times2try = 5;
            state++;
          }
        }
        break;

      case 1:
        {
          if (times2try)
          {
            if ((unsigned long)(millis() - last) > 1000)
            {
              last = millis();
              times2try--;
            }

            if (g_slave_released)
            {
              g_webSocketLocalClient.disconnect();
              WiFi.disconnect();
              wifiMulti.cleanAPlist();
              ready = true;
            }
          }
          else
          {
            g_webSocketLocalClient.disconnect();
            WiFi.disconnect();
            wifiMulti.cleanAPlist();
            ready = true;
          }
        }
        break;
    }
  }

  return;
}


bool waitForTimeLoop()
{
  bool failure = true;
  int times2try = 5; /* Allow time for baud calibration */
  bool ready = false;
  unsigned long last = millis();
  bool baudSet = false;
  bool sentComsOFF = false;

//  while (times2try)
//  {
//    g_webSocketLocalClient.loop();
//    linkbusLoop();
//    yield();
//
//    if ((unsigned long)(millis() - last) > 1000)
//    {
//      last = millis();
//      times2try--;
//      Serial.write(0);
//      Serial.write(85);
//      Serial.flush();
//    }
//  }

  /* Inform the ATMEGA that WiFi power up is complete */
  g_LBOutputBuff->put(LB_MESSAGE_ESP_WAKEUP);

  while (!ready)
  {
    g_webSocketLocalClient.loop();
    linkbusLoop();
    yield();
    if (lights->blinkLEDs(500, RED_BLUE_TOGETHER, true))
    {
      if (!sentComsOFF)
      {
        lights->setLEDs(RED_BLUE_TOGETHER, true);
        Serial.printf(LB_MESSAGE_WIFI_COMS_OFF);    /* send immediate */
        sentComsOFF = true;
      }
    }

    if (!baudSet && g_baud_sync_success)
    {
      baudSet = true;
      times2try = 10;
    }

    if (g_timeOfDayFromTx)
    {
      g_numberOfScheduledEvents = numberOfEventsScheduled(g_timeOfDayFromTx);
      ready = true;
      failure = false;
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
      if (g_debug_prints_enabled)
      {
        Serial.println(String("Received time from tx: ") + g_timeOfDayFromTx);
      }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

    }
    else if (times2try)
    {
      if ((unsigned long)(millis() - last) > 1000)
      {
        last = millis();
        times2try--;
        g_LBOutputBuff->put(LB_MESSAGE_ESP_WAKEUP);
      }
    }
    else
    {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
      if (g_debug_prints_enabled)
      {
        Serial.println("Timeout waiting for Tx time.");
      }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

      ready = true;
    }
  }

  return failure;
}


bool clientConnectLoop()
{
  bool failure = true;
  String temp;
  LEDPattern ledPattern = RED_BLUE_TOGETHER;
  String errorMessage = String("");

  int blinkPeriodMillis = 100;
  String msg;
  int times2try = 5;
  bool syncFailed = true;
  bool busy = true;
  static unsigned long last = 0;

  while (busy)
  {
    g_webSocketLocalClient.loop();
    linkbusLoop();
    yield();

    lights->blinkLEDs(blinkPeriodMillis, ledPattern, true);

    switch (g_webSocketSlaveState)
    {
      case WSClientConnecting:
        {
          if (g_slave_released && times2try)
          {
            if ((unsigned long)(millis() - last) > 2000)
            {
              msg = String(SOCK_COMMAND_SLAVE) + "," + SLAVE_CONNECT;
              g_webSocketLocalClient.sendTXT(stringObjToConstCharString(&msg)); /* Connect to Master */
              last = millis();
              times2try--;
            }
          }
          else
          {
            if (!g_slave_released) /* Success */
            {
              times2try = 5;
              g_webSocketSlaveState = WSClientSyncClock;

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
              if (g_debug_prints_enabled)
              {
                Serial.println("WSc: Active");
              }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
            }
            else
            {
              g_webSocketSlaveState = WSClientClose;

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
              if (g_debug_prints_enabled)
              {
                Serial.println("WSc: failed connection (1)");
              }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
            }
          }
        }
        break;

      case WSClientSyncClock:
        {
          if (g_slave_released)
          {
            g_webSocketSlaveState = WSClientClose;

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
            if (g_debug_prints_enabled)
            {
              Serial.println("WSc: failed connection (2)");
            }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
          }
          else
          {
            g_linkBusAckTimoutOccurred = false;
            times2try = 5;
            /* Wait in this state until an incoming sync message changes the state */
          }
        }
        break;

      case WSClientWaitForSyncAck:
        {
          if (g_slave_released)
          {
            blinkPeriodMillis = 100;
            g_webSocketSlaveState = WSClientClose;

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
            if (g_debug_prints_enabled)
            {
              Serial.println("WSc: failed connection (3)");
            }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
          }
          else if (g_LBOutputBuff->empty() && !g_linkBusAckPending && !g_linkBusAckTimoutOccurred)
          {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
            if (g_debug_prints_enabled)
            {
              Serial.println("WSc: Sync ack received.");
            }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
            g_webSocketSlaveState = WSClientExitSuccess;
            failure = false;
          }
          else
          {
            if (times2try)
            {
              if ((unsigned long)(millis() - last) > 1000)
              {
                last = millis();
                times2try--;
              }
            }
            else
            {
              g_webSocketSlaveState = WSClientClose;

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
              if (g_debug_prints_enabled)
              {
                Serial.println("WSc: ACK timeout");
              }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
            }
          }
        }
        break;

      case WSClientExitSuccess:
        {
          if (failure)
          {
            g_webSocketSlaveState = WSClientCleanup;
          }
          else
          {
            busy = false;
          }
        }
        break;

      case WSClientClose:
        {
          if (syncFailed)
          {
            msg = String(SOCK_COMMAND_SLAVE_UPDATE_ERROR);
            if (errorMessage.length())
            {
              msg = msg + "," + errorMessage;
            }
            g_webSocketLocalClient.sendTXT(stringObjToConstCharString(&msg)); /* Send to Master */
            g_webSocketServer.loop();
            times2try = 3;
            syncFailed = false;
          }

          if (times2try)
          {
            if ((unsigned long)(millis() - last) > 1000)
            {
              last = millis();
              times2try--;
            }
          }
          else
          {
            g_webSocketLocalClient.disconnect();

            if (g_fileDataBuff)
            {
              delete g_fileDataBuff;
            }

            g_slave_released = true;
            times2try = 0;
            WiFi.disconnect(false);
            g_webSocketSlaveState = WSClientCleanup;
          }
        }
        break;

      default: /* WSClientCleanup */
        {
          if (times2try)
          {
            if ((unsigned long)(millis() - last) > 1000)
            {
              last = millis();
              times2try--;
            }
          }
          else
          {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
            if (g_debug_prints_enabled)
            {
              Serial.println("WSc: Closed (1)");
            }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
            WiFi.disconnect();
            wifiMulti.cleanAPlist();
            busy = false;
          }
        }
        break;
    }
  }

  return (failure);
}


bool loadActiveEventFile(String updatedFileName)
{
  bool failure = true;
  File tempFile;
  String temp;
  String errorMessage = String("");

  LEDPattern ledPattern = RED_BLUE_TOGETHER;
  int blinkPeriodMillis = 500;
  String msg;
  int times2try = 5;
  bool syncFailed = true;
  bool busy = true;
  unsigned long last = 0;

  g_webSocketSlaveState = WSClientLoadEventFile;

  while (busy)
  {
    g_webSocketLocalClient.loop();
    linkbusLoop();
    yield();

    lights->blinkLEDs(blinkPeriodMillis, ledPattern, true);

    switch (g_webSocketSlaveState)
    {
      case WSClientLoadEventFile:
        {
          if (updatedFileName.length() > 0)
          {
            if (!g_activeEvent)
            {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
              g_activeEvent = new Event(g_debug_prints_enabled);
#else
              g_activeEvent = new Event(false);
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
              g_activeEventIndex = 0;
            }

            if (g_activeEvent)
            {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
              if (g_debug_prints_enabled)
              {
                Serial.println(String("updatedFileName: " + updatedFileName));
                Serial.println(String("g_activeEvent: " + g_activeEvent->myPath));
              }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
              if (g_activeEvent->getPath().equals(updatedFileName))
              {
                g_selectedEventName = g_activeEvent->getEventName();
                g_slave_received_new_event_file = true;
              }
              else
              {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                if (g_debug_prints_enabled)
                {
                  Serial.println("Loading new file");
                }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

                g_LBOutputBuff->put(LB_MESSAGE_ESP_KEEPALIVE);
                if (g_activeEvent->readEventFile(updatedFileName))
                {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                  if (g_debug_prints_enabled)
                  {
                    Serial.println("Error loading new file");
                  }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
                  errorMessage = "Could not read new event file";
                  g_webSocketSlaveState = WSClientClose;
                }
                else
                {
                  g_selectedEventName = g_activeEvent->getEventName();
                  g_slave_received_new_event_file = true;
                }
              }
            }
            else
            {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
              if (g_debug_prints_enabled)
              {
                Serial.println("g_activeEvent is NULL");
              }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
              errorMessage = "Could not create activeEvent";
              g_webSocketSlaveState = WSClientClose;
            }
          }
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
          else if (g_debug_prints_enabled)
          {
            Serial.println("Invalid event file name: " + updatedFileName);
          }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

          if (g_activeEvent->isNotDisabledEvent(g_timeOfDayFromTx))
          {
            g_webSocketSlaveState = WSClientProgramATMEGA;
          }
          else
          {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
            if (g_debug_prints_enabled)
            {
              Serial.println("Event finished in the past");
            }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
            errorMessage = "Event finished in the past";
            g_webSocketSlaveState = WSClientClose;
          }
        }
        break;

      case WSClientProgramATMEGA:
        {
          if (g_slave_received_new_event_file)
          {
            String err;
            sendEventToATMEGA(&err);

            if (err.length())
            {
              errorMessage = "Failed to send data to ATMEGA";
              g_webSocketSlaveState = WSClientClose;
              blinkPeriodMillis = 100;
            }
            else
            {
              blinkPeriodMillis = 500;
              syncFailed = false;
              failure = false;
              times2try = 0;
              g_webSocketSlaveState = WSClientExitSuccess;
            }
          }
          else
          {
            g_webSocketSlaveState = WSClientClose;
            errorMessage = "No event received";
          }
          g_webSocketServer.loop();
        }
        break;

      case WSClientClose:
        {
          if (g_IamMaster)
          {
            busy = false;
          }
          else
          {
            if (syncFailed)
            {
              msg = String(SOCK_COMMAND_SLAVE_UPDATE_ERROR);
              if (errorMessage.length())
              {
                msg = msg + "," + errorMessage;
              }
              g_webSocketLocalClient.sendTXT(stringObjToConstCharString(&msg)); /* Send to Master */
              g_webSocketServer.loop();
              times2try = 3;
              syncFailed = false;
            }

            if (times2try)
            {
              if ((unsigned long)(millis() - last) > 1000)
              {
                last = millis();
                times2try--;
              }
            }
            else
            {
              g_webSocketLocalClient.disconnect();

              if (g_fileDataBuff)
              {
                delete g_fileDataBuff;
              }

              if (tempFile)
              {
                tempFile.close();
              }

              g_slave_released = true;
              times2try = 0;
              WiFi.disconnect(false);
              g_webSocketSlaveState = WSClientCleanup;
            }
          }
        }
        break;

      case WSClientExitSuccess:
        {
          if (failure && !g_IamMaster)
          {
            g_webSocketSlaveState = WSClientCleanup;
          }
          else
          {
            busy = false;
          }
        }
        break;

      default: /* WSClientCleanup */
        {
          if (times2try)
          {
            if ((unsigned long)(millis() - last) > 1000)
            {
              last = millis();
              times2try--;
            }
          }
          else
          {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
            if (g_debug_prints_enabled)
            {
              Serial.println("WSc: Closed (2)");
            }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
            WiFi.disconnect();
            wifiMulti.cleanAPlist();
            busy = false;
          }
        }
        break;
    }
  }

  return (failure);
}


bool clientUpdateEventFilesLoop()
{
  bool failure = true;
  File tempFile;
  String updatedFileName;
  String temp;
  LEDPattern ledPattern = RED_BLUE_TOGETHER;
  String errorMessage = String("");

  int blinkPeriodMillis = 100;
  String msg;
  int times2try = 5;
  bool syncFailed = true;
  bool busy = true;
  static unsigned long last;

  last = millis();
  g_webSocketSlaveState = WSClientWaitingForFiles;
  msg = String(SOCK_COMMAND_SLAVE) + "," + SLAVE_WAITING_FOR_FILES;
  g_webSocketLocalClient.sendTXT(stringObjToConstCharString(&msg));

  while (busy)
  {
    g_webSocketLocalClient.loop();
    linkbusLoop();
    yield();

    lights->blinkLEDs(blinkPeriodMillis, ledPattern, true);

    switch (g_webSocketSlaveState)
    {
      case WSClientWaitingForFiles:
        {
          if (g_slave_released)
          {
            g_webSocketSlaveState = WSClientClose;

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
            if (g_debug_prints_enabled)
            {
              Serial.println("WSc: failed connection (21)");
            }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
          }
          else
          {
            if (times2try)
            {
              if ((unsigned long)(millis() - last) > 2000)
              {
                msg = String(SOCK_COMMAND_SLAVE) + "," + SLAVE_WAITING_FOR_FILES;
                g_webSocketLocalClient.sendTXT(stringObjToConstCharString(&msg));
                last = millis();
                times2try--;
              }
            }
            else
            {
              blinkPeriodMillis = 100;
              g_webSocketSlaveState = WSClientClose;

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
              if (g_debug_prints_enabled)
              {
                Serial.println("WSc: TO waiting for files");
              }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
            }
          }
        }
        break;

      case WSClientReceiveFileUpdate:
        {
          if (g_slave_released)
          {
            g_webSocketSlaveState = WSClientClose;

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
            if (g_debug_prints_enabled)
            {
              Serial.println("WSc: failed connection (6)");
            }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
          }
          else
          {
            if (g_fileDataBuff == NULL)
            {
              g_fileDataBuff = new CircularStringBuff(2 + EVENT_FILE_DATA_SIZE);
            }

            if (g_fileDataBuff)
            {
              if (!tempFile)
              {
                tempFile = LittleFS.open("/Temp", "w");
              }

              if (tempFile)
              {
                g_webSocketSlaveState = WSClientReceiveFileData;
                times2try = 5;

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                if (g_debug_prints_enabled)
                {
                  Serial.println("Temp file ready...");
                }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
              }
              else
              {
                g_webSocketSlaveState = WSClientClose;
              }
            }
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
            else if (g_debug_prints_enabled)
            {
              Serial.println("File data buff error!");
            }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
          }
        }
        break;

      case WSClientReceiveFileData:
        {
          if (g_slave_released)
          {
            g_webSocketSlaveState = WSClientClose;

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
            if (g_debug_prints_enabled)
            {
              Serial.println("WSc: failed connection (5)");
            }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
          }
          else
          {
            if (tempFile && g_fileDataBuff)
            {
              if (g_fileDataBuff->empty())
              {
                if (times2try)
                {
                  if ((unsigned long)(millis() - last) > 1000)
                  {
                    last = millis();
                    times2try--;
                  }
                }
                else
                {
                  g_webSocketSlaveState = WSClientClose;
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                  if (g_debug_prints_enabled)
                  {
                    Serial.println("WSc: Timeout waiting for data");
                  }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
                }
              }
              else
              {
                times2try = 5;
                temp = g_fileDataBuff->get();
                if (temp.equals("EOF"))
                {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                  if (g_debug_prints_enabled)
                  {
                    Serial.println("Closing file");
                  }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
                  tempFile.close();
                  g_webSocketSlaveState = WSClientValidateFile;
                }
                else if (temp.length() > 0)
                {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                  if (g_debug_prints_enabled)
                  {
                    Serial.println(temp);
                  }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
                  tempFile.println(temp);
                }
              }

            }
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
            else if (g_debug_prints_enabled)
            {
              Serial.println("File or buff error!");
            }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
          }
        }
        break;


      case WSClientValidateFile:
        {
          if (g_slave_released)
          {
            g_webSocketSlaveState = WSClientClose;

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
            if (g_debug_prints_enabled)
            {
              Serial.println("WSc: failed connection (7)");
            }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
          }
          else
          {
            String path = String("/Temp");
            if (Event::validEventFile(path, &updatedFileName))
            {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
              if (g_debug_prints_enabled)
              {
                Serial.println("Temp is valid");
              }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

              if (updatedFileName.length() > 0)
              {
                if (LittleFS.exists(updatedFileName))
                {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                  if (g_debug_prints_enabled)
                  {
                    Serial.println("Removing old file");
                  }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
                  LittleFS.remove(updatedFileName);
                }

                LittleFS.rename(path, updatedFileName);

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                if (g_debug_prints_enabled)
                {
                  Serial.println(String("Renamed /Temp to " + updatedFileName));
                }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
                g_webSocketSlaveState = WSClientWaitingForFiles;
                times2try = 5;
              }
              else
              {
                g_webSocketSlaveState = WSClientClose;
              }
            }
            else
            {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
              if (g_debug_prints_enabled)
              {
                Serial.println("Temp file is corrupt");
              }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
              g_webSocketSlaveState = WSClientClose;
            }
          }
        }
        break;

      case WSClientFilesComplete:
        {
          syncFailed = false;
          failure = false;
          busy = false;
        }
        break;

      case WSClientClose:
        {
          if (syncFailed)
          {
            msg = String(SOCK_COMMAND_SLAVE_UPDATE_ERROR);
            if (errorMessage.length())
            {
              msg = msg + "," + errorMessage;
            }
            g_webSocketLocalClient.sendTXT(stringObjToConstCharString(&msg)); /* Send to Master */
            g_webSocketServer.loop();
            times2try = 3;
            syncFailed = false;
          }

          if (times2try)
          {
            if ((unsigned long)(millis() - last) > 1000)
            {
              last = millis();
              times2try--;
            }
          }
          else
          {
            g_webSocketLocalClient.disconnect();

            if (g_fileDataBuff)
            {
              delete g_fileDataBuff;
            }

            if (tempFile)
            {
              tempFile.close();
            }

            g_slave_released = true;
            times2try = 0;
            WiFi.disconnect(false);
            g_webSocketSlaveState = WSClientCleanup;
          }
        }
        break;

      default: /* WSClientCleanup */
        {
          if (times2try)
          {
            if ((unsigned long)(millis() - last) > 1000)
            {
              last = millis();
              times2try--;
            }
          }
          else
          {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
            if (g_debug_prints_enabled)
            {
              Serial.println("WSc: Closed (3)");
            }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
            WiFi.disconnect();
            wifiMulti.cleanAPlist();
            busy = false;
          }
        }
        break;
    }
  }

  return (failure);
}


void httpWebServerLoop(int blinkRate)
{
  //  bool server_done = false;
  File activeFile;
  int checksum = 0;
  bool eventFileStartFound = false;
  bool firstPageLoad = true;
  int holdWebClients = 0;
  int holdWebSocketClients = 0;

  int serialIndex = 0;
  String msgOut;

  bool sentComsOFF = false;
  int blinkPeriodMillis = blinkRate;

  unsigned long relativeTimeSeconds;
  unsigned long holdSeconds = 0;

  LEDPattern ledPattern = RED_LED_ONLY;

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
  if (g_debug_prints_enabled)
  {
    Serial.println("httpWebServerLoop() started");
  }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

  g_numberOfWebClients = WiFi.softAPgetStationNum();

  if (g_numberOfWebClients > 0)
  {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (g_debug_prints_enabled)
    {
      Serial.println(String("Stations already connected:" + String(g_numberOfWebClients)));
      Serial.println(String("Existing socket connections:" + String(g_numberOfSocketClients)));
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

    /*    WiFi.disconnect();
          ESP.restart();
          WiFi.mode(WIFI_OFF); */
    WiFi.mode(WIFI_STA);
    WiFi.mode(WIFI_AP_STA);
    /* Event subscription */
    e1 = WiFi.onSoftAPModeStationConnected(onNewStation);
    e2 = WiFi.onSoftAPModeStationDisconnected(onStationDisconnect);

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    uint8_t i = WiFi.softAPgetStationNum();

    if (g_debug_prints_enabled)
    {
      Serial.println(String("Stations already connected:" + String(i)));
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
  }

  //  while (!server_done)
  while (1)
  {
    /*check if there are any new clients */
    g_http_server.handleClient();
    g_webSocketServer.loop();

    g_numberOfWebClients = WiFi.softAPgetStationNum();
    g_numberOfSocketClients = g_webSocketServer.connectedClients(false);

    if ((g_numberOfWebClients != holdWebClients) || (g_numberOfSocketClients != holdWebSocketClients ))
    {
      holdWebClients = g_numberOfWebClients;
      holdWebSocketClients = g_numberOfSocketClients;

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
      if (g_debug_prints_enabled)
      {
        Serial.println("Web Clients: " + String(g_numberOfWebClients));
        Serial.println("Socket Clients: " + String(g_numberOfSocketClients));
      }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

      if (g_numberOfWebClients == 0)
      {
        g_webSocketServer.close(); /* ensure all web socket clients are disconnected - this might not happen if WiFi connection was broken */
        g_numberOfSocketClients = 0;
        holdWebSocketClients = 0;
        g_LBOutputBuff->put(LB_MESSAGE_ESP_IDLE);
      }
    }

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (linkbusLoop())
    {
      if (g_debug_prints_enabled)
      {
        Serial.println("lb timeout!");
      }
    }
#else
    linkbusLoop();
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

    relativeTimeSeconds = millis() / 1000;

    if (relativeTimeSeconds != holdSeconds)
    {
      holdSeconds = relativeTimeSeconds;

      if (g_numberOfWebClients)
      {
        if (!(holdSeconds % 25))   /* Ask ATMEGA to wait longer before shutting down WiFi */
        {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
          if (g_debug_prints_enabled)
          {
            Serial.println("Sending keep-alive");
          }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
          g_LBOutputBuff->put(LB_MESSAGE_ESP_KEEPALIVE);
        }
      }

      if (!g_master_has_connected_slave)
      {
        if (g_numberOfSocketClients)
        {
          if (!(holdSeconds % 61))
          {
            g_LBOutputBuff->put(LB_MESSAGE_TEMP_REQUEST);
          }
          else if (!(holdSeconds % 17))
          {
            g_LBOutputBuff->put(LB_MESSAGE_BATTERY_REQUEST);
          }
        }
      }

      if (!g_master_has_connected_slave) /* Never timeout if a slave is connected */
      {
        if (g_noActivityTimeoutSeconds)
        {
          g_noActivityTimeoutSeconds--;

          if (!g_noActivityTimeoutSeconds)
          {
            g_LBOutputBuff->put(LB_MESSAGE_ESP_SHUTDOWN);   /* shut down immediately */
          }
        }
      }

      if (!g_master_has_connected_slave) /* Never timeout if a slave is connected */
      {
        if (g_socket_timeout)
        {
          g_socket_timeout--;

          if (!g_socket_timeout)
          {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
            if (g_debug_prints_enabled)
            {
              Serial.println("Socket TO!");
            }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

            g_webSocketServer.disconnect(); /* ensure all web socket clients are disconnected */
          }
        }
      }
    }

    if (g_numberOfSocketClients)
    {
      ledPattern = RED_BLUE_ALTERNATING;
    }
    else if (g_numberOfWebClients)
    {
      ledPattern = BLUE_LED_ONLY;
    }
    else
    {
      ledPattern = RED_LED_ONLY;
    }

    if (lights->blinkLEDs(blinkPeriodMillis, ledPattern, g_LEDs_enabled))
    {
      if (!sentComsOFF)
      {
        Serial.printf(LB_MESSAGE_WIFI_COMS_OFF);    /* send immediate */
        sentComsOFF = true;
      }
    }

    switch (g_ESP_Comm_State)
    {
      case TX_WAKE_UP:
        {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
          if (g_debug_prints_enabled)
          {
            Serial.println("s: TX_WAKE_UP");
          }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

          g_ESP_Comm_State = TX_WAITING_FOR_INSTRUCTIONS;
          g_LBOutputBuff->put(LB_MESSAGE_VER_REQUEST);
        }
        break;

      case TX_READ_ALL_EVENTS_FILES:
        {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
          if (g_debug_prints_enabled)
          {
            Serial.println("S=TX_READ_ALL_EVENTS_FILES");
          }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
          populateEventFileList();
          g_numberOfScheduledEvents = numberOfEventsScheduled(g_timeOfDayFromTx);
          g_ESP_Comm_State = TX_WAITING_FOR_INSTRUCTIONS;
        }
        break;

      case TX_HTML_SAVE_CHANGES:
        {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
          if (g_debug_prints_enabled)
          {
            Serial.println("S=TX_HTML_SAVE_CHANGES");
          }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

          if (g_activeEvent)
          {
            g_LBOutputBuff->put(LB_MESSAGE_ESP_KEEPALIVE);
            g_socket_timeout = 20;  /* allow extra time before declaring socket dead */

            g_activeEvent->writeEventFile();

            String msg = String(SOCK_MESSAGE_EVENT_SAVED);
            g_webSocketServer.broadcastTXT(stringObjToConstCharString(&msg), msg.length());
          }
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
          else
          {
            if (g_debug_prints_enabled)
            {
              Serial.println("Could not save changes: activeEvent is NULL");
            }
          }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

          g_ESP_Comm_State = TX_WAITING_FOR_INSTRUCTIONS;
        }
        break;


      case TX_HTML_PAGE_SERVED:
        {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
          if (g_debug_prints_enabled)
          {
            Serial.println("S=TX_HTML_PAGE_SERVED");
          }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

          firstPageLoad = true;
        }
      /* Intentional Fall-through
         break; */

      case TX_HTML_NEXT_EVENT:
        {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
          if (g_debug_prints_enabled)
          {
            Serial.println("S=TX_HTML_NEXT_EVENT");
          }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

          if (g_activeEvent != NULL)
          {
            bool fail = g_activeEvent->writeEventFile();    /* save any changes made to the active event */

            if (fail)
            {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
              if (g_debug_prints_enabled)
              {
                Serial.println("Write event failed");
              }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
            }
          }

          populateEventFileList();                /* read any values that might have changed into the event list */
          g_numberOfScheduledEvents = numberOfEventsScheduled(g_timeOfDayFromTx);

          if (firstPageLoad)
          {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
            if (g_debug_prints_enabled)
            {
              Serial.println("firstPageLoad = true");
            }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
            if (g_numberOfEventFilesFound)
            {
              if (g_activeEvent == NULL)
              {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                g_activeEvent = new Event(g_debug_prints_enabled);
#else
                g_activeEvent = new Event(false);
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
              }
              g_activeEventIndex = 0;
              g_activeEvent->readEventFile(g_eventList[0].path);
            }
          }

          if (g_numberOfSocketClients)
          {
            String msg;

            g_http_server.handleClient();
            g_webSocketServer.loop();

            if (g_numberOfEventFilesFound)
            {
              if (g_numberOfEventFilesFound > 1)
              {
                if ((g_selectedEventName.length() > 0) && (g_activeEvent != NULL))
                {
                  bool found = false;

                  for (int i = 0; i < g_numberOfEventFilesFound; i++)
                  {
                    if (g_selectedEventName.equals(g_eventList[i].ename))
                    {
                      g_activeEventIndex = i;
                      found = true;
                      break;
                    }
                  }

                  if (!found)
                  {
                    g_activeEventIndex = (g_activeEventIndex + 1) % g_numberOfEventFilesFound;
                  }
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                  if (g_debug_prints_enabled)
                  {
                    Serial.println("g_selectedEventName");
                  }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

                  g_activeEvent->readEventFile(g_eventList[g_activeEventIndex].path);
                }
                else
                {
                  if (g_activeEvent == NULL)
                  {
                    g_activeEventIndex = 0;
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                    g_activeEvent = new Event(g_debug_prints_enabled);
#else
                    g_activeEvent = new Event(false);
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                    if (g_debug_prints_enabled)
                    {
                      Serial.println("(2) g_activeEvent == NULL");
                    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

                    g_activeEvent->readEventFile(g_eventList[0].path);
                    g_selectedEventName = g_activeEvent->getEventName();
                  }
                  else if (!firstPageLoad)
                  {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                    if (g_debug_prints_enabled)
                    {
                      Serial.println("!firstPageLoad");
                    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
                    g_activeEventIndex = (g_activeEventIndex + 1) % g_numberOfEventFilesFound;
                    g_activeEvent->readEventFile(g_eventList[g_activeEventIndex].path);
                    g_selectedEventName = g_activeEvent->getEventName();
                  }
                  else
                  {
                    firstPageLoad = false;
                  }
                }
              }

              String msg = String(String(SOCK_COMMAND_EVENT_NAME) + "," + g_activeEvent->getEventName() );
              g_webSocketServer.broadcastTXT(stringObjToConstCharString(&msg), msg.length());

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
              if (g_debug_prints_enabled)
              {
                Serial.println(msg);
              }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

              g_http_server.handleClient();
              g_webSocketServer.loop();

              for (int i = 0; i < g_numberOfEventFilesFound; i++)
              {
                msg = String(String(SOCK_COMMAND_EVENT_DATA) + "," + g_eventList[i].ename + "," + g_eventList[i].vers + "," +  g_eventList[i].startDateTimeEpoch + "," +  g_eventList[i].finishDateTimeEpoch + ",*," + g_eventList[i].callsign + ",*,*");
                g_webSocketServer.broadcastTXT(stringObjToConstCharString(&msg), msg.length());
              }

              g_http_server.handleClient();
              g_webSocketServer.loop();

              for (int i = 0; i < g_activeEvent->getEventNumberOfTxTypes(); i++)
              {
                for (int j = 0; j < g_activeEvent->getNumberOfTxsForRole(i); j++)
                {
                  String role_tx = String(String(i) + ":" + String(j));
                  msg = String(String(SOCK_COMMAND_TYPE_NAME) + "," + g_activeEvent->getTxDescriptiveName(role_tx) + "," + role_tx);
                  g_webSocketServer.broadcastTXT(stringObjToConstCharString(&msg), msg.length());
                  g_webSocketServer.loop();
                  g_http_server.handleClient();
                }
              }

              msg = String(String(SOCK_COMMAND_TYPE_NAME) + ",Done,Done");
              g_webSocketServer.broadcastTXT(stringObjToConstCharString(&msg), msg.length());
              g_webSocketServer.loop();
              g_http_server.handleClient();

              for (int i = 0; i < g_activeEvent->getEventNumberOfTxTypes(); i++)
              {
                msg = String(String(SOCK_COMMAND_TYPE_FREQ) + "," + String(g_activeEvent->getFrequencyForRole(i)) + "," + g_activeEvent->getRolename(i));
                g_webSocketServer.broadcastTXT(stringObjToConstCharString(&msg), msg.length());
                g_webSocketServer.loop();
              }

              g_http_server.handleClient();


              for (int i = 0; i < g_activeEvent->getEventNumberOfTxTypes(); i++)
              {
                msg = String(String(SOCK_COMMAND_TYPE_PWR) + "," + String(g_activeEvent->getPowerlevelForRole(i)) + "," + g_activeEvent->getRolename(i));
                g_webSocketServer.broadcastTXT(stringObjToConstCharString(&msg), msg.length());
                g_webSocketServer.loop();
              }

              g_http_server.handleClient();

              msg = String(String(SOCK_COMMAND_TX_ROLE) + "," + g_activeEvent->getTxAssignment());
              g_webSocketServer.broadcastTXT(stringObjToConstCharString(&msg), msg.length());

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
              if (g_debug_prints_enabled)
              {
                Serial.println(msg);
              }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
            }
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
            else if (g_debug_prints_enabled)
            {
              Serial.println("No events found!");
            }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

          }
          g_ESP_Comm_State = TX_WAITING_FOR_INSTRUCTIONS;
        }
        break;


      case TX_SEND_NEXT_EVENT_TO_ATMEGA:
        {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
          if (g_debug_prints_enabled)
          {
            Serial.println("case TX_SEND_NEXT_EVENT_TO_ATMEGA");
          }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
          g_LBOutputBuff->put(LB_MESSAGE_ESP_KEEPALIVE);
          g_numberOfScheduledEvents = numberOfEventsScheduled(g_timeOfDayFromTx);

          if (g_numberOfScheduledEvents)
          {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
            if (g_debug_prints_enabled)
            {
              String msg = String("Scheduled event(s): ") + g_numberOfScheduledEvents;
              Serial.println(msg);
            }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

            if (!g_activeEvent)
            {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
              g_activeEvent = new Event(g_debug_prints_enabled);
#else
              g_activeEvent = new Event(false);
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
            }

            g_activeEvent->readEventFile(g_eventList[0].path);

            if (!loadActiveEventFile(g_eventList[0].path))
            {
              blinkPeriodMillis = 500;
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
              if (g_debug_prints_enabled)
              {
                String msg = String("Scheduled event loaded: ");
                msg = msg + g_activeEvent->getEventName();
                Serial.println(msg);
              }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
            }
            else
            {
              blinkPeriodMillis = 100;
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
              if (g_debug_prints_enabled)
              {
                Serial.println("Scheduled event load failed.");
              }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
            }

            g_ESP_Comm_State = TX_WAITING_FOR_INSTRUCTIONS;
          }
          else
          {
            g_ESP_Comm_State = TX_POWER_DOWN_NOW;
          }
        }
        break;


      case TX_RECD_START_EVENT_REQUEST:
        {
          g_http_server.handleClient();
          g_webSocketServer.loop();
          yield();
          blinkPeriodMillis = 100;

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
          if (g_debug_prints_enabled)
          {
            Serial.println("Sending data to ATMEGA");
          }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

          String err = String("");
          sendEventToATMEGA(&err);

          if (err.length())
          {
            if (g_numberOfSocketClients)
            {
              String msg = String(String(SOCK_COMMAND_ERROR) + "," + ERROR_CODE_ATMEGA_NOT_RESPONDING);
              g_webSocketServer.broadcastTXT(stringObjToConstCharString(&msg), msg.length());
            }

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
            if (g_debug_prints_enabled)
            {
              Serial.println("Error setting ATMEGA: " + err);
            }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
            g_ESP_Comm_State = TX_WAITING_FOR_INSTRUCTIONS;
          }
          else
          {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
            if (g_debug_prints_enabled)
            {
              Serial.println("ATMEGA setting successful");
            }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

            blinkPeriodMillis = 500;
            g_ESP_Comm_State = TX_WAITING_FOR_INSTRUCTIONS;
          }
        }
        break;

      case TX_MASTER_SEND_EVENT_FILES:
        {
          g_http_server.handleClient();
          g_webSocketServer.loop();
          yield();

          switch (serialIndex)
          {
            case 0: /* Send File Name and open file for read */
              {
                if (g_numberOfEventFilesFound > g_files_sent_to_slave)
                {
                  String fn = g_eventList[g_files_sent_to_slave].path;;
                  g_files_sent_to_slave++;

                  if (fn.length() > 0)
                  {
                    activeFile = LittleFS.open(fn, "r"); /* Open the file for reading */

                    if (!activeFile)
                    {
                      serialIndex = 100; /* abort */

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                      if (g_debug_prints_enabled)
                      {
                        Serial.println(String("File error: " + fn));
                      }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
                    }
                    else
                    {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                      if (g_debug_prints_enabled)
                      {
                        Serial.println(String("Sending file: " + fn));
                      }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

                      blinkPeriodMillis = 100;
                      msgOut = String(SOCK_COMMAND_FILE_DATA) + "," + EVENT_FILE_NAME + "," + fn;
                      g_webSocketServer.broadcastTXT(stringObjToConstCharString(&msgOut), msgOut.length());
                      checksum = 0;
                      eventFileStartFound = false;
                    }
                  }
                  else
                  {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                    if (g_debug_prints_enabled)
                    {
                      Serial.println("Err: Corrupt event file name");
                    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

                    serialIndex = 100; /* abort */
                  }
                }
                else
                {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                  if (g_debug_prints_enabled)
                  {
                    Serial.println(String("Err: No event files to send."));
                  }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

                  serialIndex = 100; /* abort */
                }
              }
              break;

            case 1: /* Read each line of the file and send it */
              {
                if (activeFile)
                {
                  String s = activeFile.readStringUntil('\n');

                  if (s.length() > 0)
                  {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                    if (g_debug_prints_enabled)
                    {
                      Serial.println(s);
                    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

                    s.trim();
                    if (!eventFileStartFound)
                    {
                      eventFileStartFound = s.equals(EVENT_FILE_START);
                      if (eventFileStartFound)
                      {
                        checksum += s.length();
                      }
                    }
                    else
                    {
                      checksum += s.length();
                    }

                    msgOut = String(SOCK_COMMAND_FILE_DATA) + "," + s;
                    g_webSocketServer.broadcastTXT(stringObjToConstCharString(&msgOut), msgOut.length());

                    if (!s.equalsIgnoreCase(String(EVENT_FILE_END)))
                    {
                      serialIndex = 0;
                    }
                  }
                  else
                  {
                    activeFile.close();
                    serialIndex = 100;

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                    if (g_debug_prints_enabled)
                    {
                      Serial.println("Err: event file corrupt");
                    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
                  }
                }
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                else if (g_debug_prints_enabled)
                {
                  serialIndex = 100;
                  Serial.println("Err: No open file");
                }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
              }
              break;

            case 2: /* Send checksum and close file */
              {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                if (g_debug_prints_enabled)
                {
                  Serial.println("Sending checksum");
                }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
                msgOut = String(SOCK_COMMAND_FILE_DATA) + "," + EVENT_FILE_CHECKSUM + "," + String(checksum);
                g_webSocketServer.broadcastTXT(stringObjToConstCharString(&msgOut), msgOut.length());
                activeFile.close();
              }
              break;

            case 3:  /* Send EOF */
              {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                if (g_debug_prints_enabled)
                {
                  Serial.println("Sending EOF");
                }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
                msgOut = String(SOCK_COMMAND_FILE_DATA) + ",EOF";
                g_webSocketServer.broadcastTXT(stringObjToConstCharString(&msgOut), msgOut.length());
              }
              break;

            default:    /* Done */
              {
                if (serialIndex >= 100) /* Abort flag */
                {
                  msgOut = String(SOCK_COMMAND_SLAVE) + "," + SLAVE_FREE;
                  g_webSocketServer.broadcastTXT(stringObjToConstCharString(&msgOut), msgOut.length());
                  blinkPeriodMillis = 100;
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                  if (g_debug_prints_enabled)
                  {
                    Serial.println("With errors");
                  }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
                }
                else
                {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                  if (g_debug_prints_enabled)
                  {
                    Serial.println("No errors");
                  }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
                  blinkPeriodMillis = 500;
                }

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                if (g_debug_prints_enabled)
                {
                  Serial.println("Done");
                }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

                g_ESP_Comm_State = TX_WAITING_FOR_INSTRUCTIONS;
              }
              break;
          }

          serialIndex++;
        }
        break;

      case TX_MASTER_SEND_ACTIVE_FILE:
        {
          g_http_server.handleClient();
          g_webSocketServer.loop();
          yield();

          switch (serialIndex)
          {
            case 0: /* Send File Name and open file for read */
              {
                if (g_activeEvent)
                {
                  String fn = String(g_activeEvent->myPath);

                  if (fn.length() > 0)
                  {
                    activeFile = LittleFS.open(fn, "r"); /* Open the file for reading */

                    if (!activeFile)
                    {
                      serialIndex = 100; /* abort */

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                      if (g_debug_prints_enabled)
                      {
                        Serial.println(String("File error: " + fn));
                      }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
                    }
                    else
                    {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                      if (g_debug_prints_enabled)
                      {
                        Serial.println(String("Sending file: " + fn));
                      }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

                      blinkPeriodMillis = 100;
                      msgOut = String(SOCK_COMMAND_FILE_DATA) + "," + EVENT_FILE_NAME + "," + fn;
                      g_webSocketServer.broadcastTXT(stringObjToConstCharString(&msgOut), msgOut.length());
                      checksum = 0;
                      eventFileStartFound = false;
                    }
                  }
                  else
                  {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                    if (g_debug_prints_enabled)
                    {
                      Serial.println("Err: No active event");
                    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

                    serialIndex = 100; /* abort */
                  }
                }
                else
                {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                  if (g_debug_prints_enabled)
                  {
                    Serial.println(String("Err: No file name."));
                  }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

                  serialIndex = 100; /* abort */
                }
              }
              break;

            case 1: /* Read each line of the file and send it */
              {
                if (activeFile)
                {
                  String s = activeFile.readStringUntil('\n');

                  if (s.length() > 0)
                  {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                    if (g_debug_prints_enabled)
                    {
                      Serial.println(s);
                    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

                    s.trim();
                    if (!eventFileStartFound)
                    {
                      eventFileStartFound = s.equals(EVENT_FILE_START);
                      if (eventFileStartFound)
                      {
                        checksum += s.length();
                      }
                    }
                    else
                    {
                      checksum += s.length();
                    }

                    msgOut = String(SOCK_COMMAND_FILE_DATA) + "," + s;
                    g_webSocketServer.broadcastTXT(stringObjToConstCharString(&msgOut), msgOut.length());

                    if (!s.equalsIgnoreCase(String(EVENT_FILE_END)))
                    {
                      serialIndex = 0;
                    }
                  }
                  else
                  {
                    activeFile.close();
                    serialIndex = 100;

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                    if (g_debug_prints_enabled)
                    {
                      Serial.println("Err: event file corrupt");
                    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
                  }
                }
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                else if (g_debug_prints_enabled)
                {
                  Serial.println("Err: No open file");
                }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
              }
              break;

            case 2: /* Send checksum and close file */
              {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                if (g_debug_prints_enabled)
                {
                  Serial.println("Sending checksum");
                }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
                msgOut = String(SOCK_COMMAND_FILE_DATA) + "," + EVENT_FILE_CHECKSUM + "," + String(checksum);
                g_webSocketServer.broadcastTXT(stringObjToConstCharString(&msgOut), msgOut.length());
                activeFile.close();
              }
              break;

            case 3:  /* Send EOF */
              {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                if (g_debug_prints_enabled)
                {
                  Serial.println("Sending EOF");
                }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
                msgOut = String(SOCK_COMMAND_FILE_DATA) + ",EOF";
                g_webSocketServer.broadcastTXT(stringObjToConstCharString(&msgOut), msgOut.length());
              }
              break;

            default:    /* Done */
              {
                if (serialIndex >= 100) /* Abort flag */
                {
                  msgOut = String(SOCK_COMMAND_SLAVE) + "," + SLAVE_FREE;
                  g_webSocketServer.broadcastTXT(stringObjToConstCharString(&msgOut), msgOut.length());
                }
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                if (g_debug_prints_enabled)
                {
                  Serial.println("Done");
                }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

                g_ESP_Comm_State = TX_WAITING_FOR_INSTRUCTIONS;
              }
              break;
          }

          serialIndex++;
        }
        break;


      case TX_SET_ENUNCIATORS_FAILURE:
        {
          blinkPeriodMillis = 100;
          g_ESP_Comm_State = TX_WAITING_FOR_INSTRUCTIONS;
        }
        break;


      case TX_SET_ENUNCIATORS_SUCCESS:
        {
          blinkPeriodMillis = 500;
          g_ESP_Comm_State = TX_WAITING_FOR_INSTRUCTIONS;
        }
        break;


      case TX_POWER_DOWN_NOW:
        {
          if (g_webSocketServer.connectedClients(false))
          {
            g_webSocketServer.disconnect();
          }

          if (activeFile) activeFile.close();

          g_http_server.stop();
          g_http_server.close();

          int secs2try = 5;
          unsigned long last = 0;

          while (secs2try)
          {
            if ((unsigned long)(millis() - last) > 1000)
            {
              last = millis();
              g_LBOutputBuff->put(LB_MESSAGE_ESP_SHUTDOWN);
              linkbusLoop();
              secs2try--;
            }
          }

          blinkPeriodMillis = 100;
        }
        break;


      default:
      case TX_WAITING_FOR_INSTRUCTIONS:
        {
          serialIndex = 0;
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
          if (g_debug_prints_enabled)
          {
            static int secs2try = 3;
            static unsigned long last = 0;
            if (secs2try)
            {
              if ((unsigned long)(millis() - last) > 1000)
              {
                last = millis();
                secs2try--;

                if (!secs2try)
                {
                  Serial.println("Waiting");
                  secs2try = 3;
                }
              }
            }
          }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
        }
        break;
    }
  }
}

bool linkbusLoop()
{
  static int linkBusAckTimeoutCountdown = 3;
  static int messageLength = 0;
  static String lb_message = "";
  static char buf[1024];
  static unsigned long holdSentTime = 0;
  static String msg = "";
  static int linkBusTimeoutCount = 0;
  size_t bytesAvail, bytesIn;
  uint8_t j;
  int timeout = 1000;
  unsigned long lbSendTimeSeconds;
  bool sendMessages = false;

  if (!g_linkBusAckPending)
  {
    linkBusAckTimeoutCountdown = 10;

    sendMessages = !g_LBOutputBuff->empty();

    if (sendMessages)
    {
      msg = g_LBOutputBuff->get();
      Serial.println(stringObjToConstCharString(&msg));
      g_linkBusAckPending++;
      holdSentTime = millis() / 1000;
      linkBusAckTimeoutCountdown = 3;
      linkBusTimeoutCount = 0;
    }
  }
  else
  {
    lbSendTimeSeconds = millis() / 1000;

    if (holdSentTime != lbSendTimeSeconds)
    {
      holdSentTime = lbSendTimeSeconds;

      if (linkBusAckTimeoutCountdown)
      {
        linkBusAckTimeoutCountdown--;

        if (!linkBusAckTimeoutCountdown) /* Ack timeout occurred */
        {
          linkBusTimeoutCount++;

          if (linkBusTimeoutCount < 3) /* Try sending the last message again */
          {
            g_linkBusAckTimoutOccurred = true;
            linkBusAckTimeoutCountdown = 3;
            holdSentTime = millis() / 1000;
            Serial.println(stringObjToConstCharString(&msg));
          }
          else
          {
            g_linkBusAckPending = 0;
            g_linkBusNacksReceived = 0;
          }

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
          if (g_debug_prints_enabled)
          {
            Serial.println("ACK Timeout");
            if (g_linkBusNacksReceived)
            {
              Serial.println("NACKs Received");
            }
          }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
        }
      }
    }
  }

  while (((bytesAvail = Serial.available()) > 0) && timeout) /*check UART for data */
  {
    yield(); /* Avoids WDT reset for long LB messages */
    timeout--;
    bytesIn = Serial.readBytes(buf, min(sizeof(buf), bytesAvail));

    if (bytesIn > 0)
    {
      buf[bytesIn] = '\0';

      for (j = 0; j < bytesIn; j++)
      {
        if (buf[j] == '$')
        {
          messageLength = 1;
          lb_message = "$";

        }
        else if (buf[j] == '!')
        {
          lb_message = "!";
          messageLength = 1;
        }
        else if ( messageLength > 0 )
        {
          lb_message += buf[j];
          messageLength++;

          if (buf[j] == ';')
          {
            messageLength = 0;
            handleLBMessage(lb_message);
          }
        }
      }
    }
  }

  return (timeout == 0);
}

void startLittleFS()
{ /* Start the LittleFS and list all contents */
  LittleFS.begin(); /* Start the SPI Flash File System (LittleFS) */

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
  if (g_debug_prints_enabled)
  {
    Serial.println("LittleFS started. Contents:");
    {
      Dir dir = LittleFS.openDir("/");
      while (dir.next())
      { /* List the file system contents */
        String fileName = dir.fileName();
        size_t fileSize = dir.fileSize();
        Serial.printf("\tFS File: %s, size: %s\r\n", fileName.c_str(), formatBytes(fileSize).c_str());
      }

      Serial.printf("\n");
    }
  }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
}

void startWebSocketServer()
{ /* Start a WebSocket server */
  if (g_webSocketServer.isRunning())
  {
    return;                                 /* Don't attempt to start a webSocket server that has already begun */
  }

  g_webSocketServer.begin();                  /* start the websocket server */
  /*  g_webSocketServer.beginSSL(); // start secure wss support? */
  g_webSocketServer.onEvent(webSocketServerEvent);  /* if there's an incoming websocket message, go to function 'webSocketServerEvent' */

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
  if (g_debug_prints_enabled)
  {
    Serial.println("WebSocket server start tasks complete.");
  }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
}

void webSocketClientEvent(WStype_t type, uint8_t * payload, size_t length)
{
  switch (type)
  {
    case WStype_DISCONNECTED:
      {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
        if (g_debug_prints_enabled)
        {
          Serial.printf("[WSc] Disconnected!\n");
        }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
        g_slave_released = true;
      }
      break;

    case WStype_CONNECTED:
      {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
        if (g_debug_prints_enabled)
        {
          if (!payload)
          {
            Serial.println("Null payload");
          }
          else
          {
            Serial.printf("[WSc] Connected to url: %s\n",  payload);
          }
        }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
      }
      break;

    case WStype_TEXT:
      {
        if (!payload)
        {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
          if (g_debug_prints_enabled)
          {
            Serial.println("Null payload");
          }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
        }
        else
        {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
          if (g_debug_prints_enabled)
          {
            Serial.printf("[WSc] got text: %s\n", payload);
          }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

          String p = String((char*)payload);
          int commaIndex = p.indexOf(',');

          String msgHeader;

          if (commaIndex >= 0)
          {
            msgHeader = p.substring(0, p.indexOf(','));
          }
          else
          {
            msgHeader = p;
          }

          if (msgHeader.equalsIgnoreCase(SOCK_COMMAND_SLAVE))
          {
            p = p.substring(p.indexOf(',') + 1);

            if (p.equals(SLAVE_FREE))
            {
              g_slave_released = true;
            }
            else if (p.equals(SLAVE_CONFIRMED))
            {
              g_slave_released = false;

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
              if (g_debug_prints_enabled)
              {
                Serial.println("WSc: sync clock");
              }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
            }
            else if (p.equals(SLAVE_WAITING_FOR_FILES))
            {
              if (g_webSocketSlaveState == WSClientWaitingForFiles)
              {
                g_webSocketSlaveState = WSClientReceiveFileUpdate;

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                if (g_debug_prints_enabled)
                {
                  Serial.println("WSc: rcv file");
                }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
              }
            }
            else if (p.equals(SLAVE_NO_MORE_FILES))
            {
              if (g_webSocketSlaveState == WSClientWaitingForFiles)
              {
                g_webSocketSlaveState = WSClientFilesComplete;

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                if (g_debug_prints_enabled)
                {
                  Serial.println("WSc: no more files");
                }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
              }
            }
          }
          else if (msgHeader.equalsIgnoreCase(SOCK_COMMAND_SYNC_TIME)) /* From connected Master */
          {
            if (g_webSocketSlaveState == WSClientSyncClock)
            {
              p = p.substring(p.indexOf(',') + 1);

              if ((p.length() > 9) && (p.length() < 12))
              {
                time_t t = strtol(stringObjToConstCharString(&p), NULL, 10);

                if (t > 962452800) /* Avoid obviously wrong dates */
                {
                  g_timeOfDayFromTx = t;
                  p = convertEpochToTimeString(g_timeOfDayFromTx);

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                  if (g_debug_prints_enabled)
                  {
                    Serial.println(String("WSc: Calcd time string: \"" + p + "\" from ") + String((unsigned)g_timeOfDayFromTx));
                  }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

                  String msgOut = String(String(LB_MESSAGE_TIME_SET) + p + ";");
                  Serial.printf(stringObjToConstCharString(&msgOut));
                  g_webSocketSlaveState = WSClientWaitForSyncAck;
                }
                else
                {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                  if (g_debug_prints_enabled)
                  {
                    Serial.println(String("WSc: Rcvd invalid time: \"" + p + "\""));
                  }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

                  g_webSocketSlaveState = WSClientClose;
                }
              }
              else
              {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                if (g_debug_prints_enabled)
                {
                  Serial.println(String("WSc: Rcvd invalid time: \"" + p + "\""));
                }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

                g_webSocketSlaveState = WSClientClose;
              }
            }
          }
          else if (msgHeader.equalsIgnoreCase(SOCK_COMMAND_FILE_DATA))
          {
            if (g_webSocketSlaveState == WSClientReceiveFileData)
            {
              if (g_fileDataBuff)
              {
                p = p.substring(p.indexOf(',') + 1);

                if ((p.length() > 0) && (p.length() < 100))
                {
                  g_fileDataBuff->put(p);
                }
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                else
                {
                  Serial.println("BD");
                }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
              }
              else
              {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                if (g_debug_prints_enabled)
                {
                  Serial.println(String("WSc: File buff not ready!"));
                }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
                g_webSocketSlaveState = WSClientClose;
              }
            }
          }
        }
      }
      break;

    case WStype_BIN:
      {
        Serial.printf("[WSc] got binary length: %u\n", length);
      }
      break;

    case WStype_ERROR:
      {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
        if (g_debug_prints_enabled)
        {
          Serial.printf("[WSc] error\n");
        }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
      }
      break;

    default:
      break;
  }
}


void webSocketServerEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length)
{
  switch (type)
  {
    case WStype_DISCONNECTED:
      {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
        if (g_debug_prints_enabled)
        {
          Serial.printf("[%u] Web Socket disconnected!\r\n", num);
        }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

        /* Invalidate the socket ID of the disconnected client */
        if (g_numberOfSocketClients)
        {
          for (uint8_t i = 0; i < MAX_NUMBER_OF_WEB_CLIENTS; i++)
          {
            if (g_webSocketClient[i].socketID == num)
            {
              g_webSocketClient[i].socketID = WEBSOCKETS_SERVER_CLIENT_MAX;
              break;
            }
          }
        }

        g_numberOfSocketClients = g_webSocketServer.connectedClients(false);

        if (num == g_slave_socket_number)
        {
          g_master_has_connected_slave = false;
          g_slave_socket_number = -1;
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
          if (g_debug_prints_enabled)
          {
            Serial.println("Slave disconnected!");
          }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
        }

        if (!g_numberOfSocketClients)
        {
          g_socket_timeout = 0;
          g_master_has_connected_slave = false;
        }
      }
      break;

    case WStype_CONNECTED:
      {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
        IPAddress ip = g_webSocketServer.remoteIP(num);

        if (g_debug_prints_enabled && payload)
        {
          Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\r\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

        /* Assign the socket ID to the web client that does not yet have a socket ID assigned */
        if (g_numberOfWebClients)
        {
          for (uint8_t i = 0; i < MAX_NUMBER_OF_WEB_CLIENTS; i++)
          {
            if ((g_webSocketClient[i].socketID >= WEBSOCKETS_SERVER_CLIENT_MAX) && ((g_webSocketClient[i].macAddr).length() > 0))
            {
              g_webSocketClient[i].socketID = num;
              break;
            }
          }
        }

        g_numberOfSocketClients = g_webSocketServer.connectedClients(false);
        g_LBOutputBuff->put(LB_MESSAGE_TEMP_REQUEST);
        g_LBOutputBuff->put(LB_MESSAGE_BATTERY_REQUEST);
        g_noActivityTimeoutSeconds = NO_ACTIVITY_TIMEOUT;
      }
      break;

    case WStype_TEXT:
      {
        g_socket_timeout = 10;
        if (!payload)
        {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
          if (g_debug_prints_enabled)
          {
            Serial.println("Null payload");
          }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
        }
        else
        {
          g_noActivityTimeoutSeconds = NO_ACTIVITY_TIMEOUT;

          String p = String((char*)payload);
          int commaIndex = p.indexOf(',');

          String msgHeader;

          if (commaIndex >= 0)
          {
            msgHeader = p.substring(0, p.indexOf(','));
          }
          else
          {
            msgHeader = p;
          }

          if (msgHeader.equalsIgnoreCase(SOCK_COMMAND_ALIVE))
          {
            g_socket_timeout = 10;
          }
          else if (msgHeader.equalsIgnoreCase(SOCK_COMMAND_SLAVE))
          {
            p = p.substring(p.indexOf(',') + 1);

            String msg = "";

            if (p.equals(SLAVE_CONNECT))
            {
              if (!g_master_has_connected_slave)
              {
                g_master_has_connected_slave = true;
                g_files_sent_to_slave = 0;
                g_slave_socket_number = num;
                msg = String(String(SOCK_COMMAND_SLAVE) + "," + SLAVE_CONFIRMED);
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                if (g_debug_prints_enabled)
                {
                  Serial.println("WSs: ack slave");
                }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
              }
            }
            else if (p.equals(SLAVE_FREE))
            {
              g_master_has_connected_slave = false;
              if (g_numberOfSocketClients == 1)
              {
                g_webSocketServer.close();
              }
              msg = String(String(SOCK_COMMAND_SLAVE) + "," + SLAVE_FREE);
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
              if (g_debug_prints_enabled)
              {
                Serial.println("WSs: freed slave");
              }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
            }
            else if (p.equals(SLAVE_WAITING_FOR_FILES))
            {
              /* Check if event files need to be sent */
              if (g_numberOfEventFilesFound > g_files_sent_to_slave)
              {
                if (g_ESP_Comm_State == TX_WAITING_FOR_INSTRUCTIONS)
                {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                  if (g_debug_prints_enabled)
                  {
                    Serial.println(String("WSs: sending file #") + (g_files_sent_to_slave + 1) + " of " + g_numberOfEventFilesFound);
                  }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

                  msg = String(SOCK_COMMAND_SLAVE) + "," + SLAVE_WAITING_FOR_FILES;
                  g_ESP_Comm_State = TX_MASTER_SEND_EVENT_FILES;
                }
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                else
                {
                  if (g_debug_prints_enabled)
                  {
                    Serial.println("In wrong state for sending files.");
                  }
                }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
              }
              else /* No more files to send */
              {
                msg = String(String(SOCK_COMMAND_SLAVE) + "," + SLAVE_NO_MORE_FILES);

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                if (g_debug_prints_enabled)
                {
                  Serial.println("WSs: no more event files to send (1)");
                }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
              }
            }

            if (msg.length()) g_webSocketServer.broadcastTXT(stringObjToConstCharString(&msg), msg.length());
          }
          else if (msgHeader.equalsIgnoreCase(SOCK_COMMAND_SLAVE_UPDATE_SUCCESS))
          {
            int firstComma = p.indexOf(',');
            int lastComma = p.lastIndexOf(',');

            if ((firstComma > 0) && (lastComma > firstComma))
            {
              String eventName = p.substring(lastComma + 1);
              String roleName = p.substring(firstComma + 1, lastComma);

              if ((eventName.length() > 0) && (roleName.length() > 0)) /* ROLENAME,EVENTNAME */
              {
                g_webSocketServer.broadcastTXT(stringObjToConstCharString(&p), p.length());

                if (g_ESP_Comm_State == TX_WAITING_FOR_INSTRUCTIONS)
                {
                  g_ESP_Comm_State = TX_SET_ENUNCIATORS_SUCCESS;
                }

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                if (g_debug_prints_enabled)
                {
                  Serial.println(String("Slave success: ") + roleName + ", " + eventName);
                }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
              }
              else
              {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                if (g_debug_prints_enabled)
                {
                  Serial.println("Slave success msg error (1)");
                }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
              }
            }
          }
          else if (msgHeader.equalsIgnoreCase(SOCK_COMMAND_SLAVE_UPDATE_ERROR))
          {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
            if (g_debug_prints_enabled)
            {
              Serial.println("Error during slave file-update.");
            }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
            if (g_ESP_Comm_State == TX_WAITING_FOR_INSTRUCTIONS)
            {
              g_ESP_Comm_State = TX_SET_ENUNCIATORS_FAILURE;
            }

            g_webSocketServer.broadcastTXT(stringObjToConstCharString(&p), p.length());
          }
          else if (msgHeader.equalsIgnoreCase(SOCK_COMMAND_SYNC_TIME)) /* From connected browser */
          {
            p = p.substring(p.indexOf(',') + 1);

            /* Lop off milliseconds */
            if ((p.length() > 20) && (p.lastIndexOf('.') > 0))
            {
              int index = p.lastIndexOf('.');
              p = p.substring(0, index);
              p = p + "Z";
            }

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
            if (g_debug_prints_enabled)
            {
              Serial.println(String("Time string: \"" + p + "\""));
            }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

            String msgOut = String(String(LB_MESSAGE_TIME_SET) + p + ";");
            Serial.printf(stringObjToConstCharString(&msgOut));  /* Send time to Transmitter for synchronization */
          }
          else if (msgHeader.equalsIgnoreCase(SOCK_COMMAND_TX_ROLE))
          {
            int c = p.indexOf(':');
            p = p.substring(c - 1, c + 2);

            if (g_activeEvent != NULL)
            {
              g_activeEvent->setTxAssignment(p);
              g_ESP_Comm_State = TX_HTML_SAVE_CHANGES;
            }
          }
          else if (msgHeader.equalsIgnoreCase(SOCK_COMMAND_EVENT_NAME))
          {
            if (p.indexOf("NEW!") >= 0)
            {
              if (g_slave_received_new_event_file)
              {
                g_ESP_Comm_State = TX_HTML_NEXT_EVENT;
              }
              else
              {
                //              g_selectedEventName = "";
                g_ESP_Comm_State = TX_HTML_PAGE_SERVED;
              }
            }
            else
            {
              p = p.substring(p.indexOf(',') + 1);
              g_selectedEventName = p;
              g_ESP_Comm_State = TX_HTML_NEXT_EVENT;
            }
          }
          else if (msgHeader.equalsIgnoreCase(SOCK_COMMAND_CALLSIGN))
          {
            p = p.substring(p.indexOf(',') + 1);
            p.toUpperCase();

            if (g_activeEvent != NULL)
            {
              g_activeEvent->setCallsign(p);
              g_ESP_Comm_State = TX_HTML_SAVE_CHANGES;
            }
            else
            {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
              if (g_debug_prints_enabled)
              {
                Serial.printf(String("Callsign: \"" + p + "\"\n").c_str());
              }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

              String msgOut = String(LB_MESSAGE_CALLSIGN_SET + p + ";");
              g_LBOutputBuff->put(msgOut);
            }

            String msg = String(String(SOCK_COMMAND_CALLSIGN) + "," + p);

            g_webSocketServer.broadcastTXT(stringObjToConstCharString(&msg), msg.length());
            g_activeEvent->setCallsign(p);
          }
          else if (msgHeader.equalsIgnoreCase(SOCK_COMMAND_PATTERN))
          {
            int firstComma = p.indexOf(',');
            int lastComma = p.lastIndexOf(',');

            String pat = p.substring(lastComma + 1);
            pat.toUpperCase();

            p = p.substring(firstComma + 1, lastComma);

            firstComma = p.indexOf(',');

            if (firstComma > 0) /* PATTERN,type,tx,pattern */
            {
              int typeIndex = p.substring(0, firstComma).toInt();
              int txIndex = p.substring(firstComma + 1).toInt();

              if (g_activeEvent != NULL)
              {
                g_activeEvent->setPatternForTx(typeIndex, txIndex, pat);
                g_ESP_Comm_State = TX_HTML_SAVE_CHANGES;
              }
            }
            else if (g_activeEvent == NULL) /* PATTERN,pattern */
            {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
              if (g_debug_prints_enabled)
              {
                Serial.printf(String("Pattern: \"" + pat + "\"\n").c_str());
              }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

              String msgOut = String(LB_MESSAGE_PATTERN_SET + pat + ";");
              g_LBOutputBuff->put(msgOut);
            }
          }
          else if (msgHeader.startsWith(SOCK_COMMAND_START_TIME))
          {
            String timeStr = p.substring(p.indexOf(',') + 1);

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
            if (g_debug_prints_enabled)
            {
              Serial.println(String("Start Time: \"" + timeStr + "\""));
            }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

            if (g_activeEvent)
            {
              g_activeEvent->setEventStartDateTime(timeStr);
              g_ESP_Comm_State = TX_HTML_SAVE_CHANGES;
            }
          }
          else if (msgHeader.startsWith(SOCK_COMMAND_FINISH_TIME))
          {
            String timeStr = p.substring(p.indexOf(',') + 1);

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
            if (g_debug_prints_enabled)
            {
              Serial.println(String("Finish Time: \"" + timeStr + "\""));
            }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

            if (g_activeEvent)
            {
              g_activeEvent->setEventFinishDateTime(timeStr);
              g_ESP_Comm_State = TX_HTML_SAVE_CHANGES;
            }

          }
          else if (msgHeader.equalsIgnoreCase(SOCK_COMMAND_CLEAR_ACTIVE_EVENT))
          {
            g_LBOutputBuff->put(LB_MESSAGE_PREP4DATA);
            if (g_activeEvent)
            {
              delete(g_activeEvent);
            }
            g_activeEvent = NULL;
          }
          else if (msgHeader.equalsIgnoreCase(SOCK_COMMAND_TYPE_FREQ))
          {
            int typeIndex;
            long freq;
            int firstComma = p.indexOf(',');
            int secondComma = p.lastIndexOf(',');
            bool indexOnly = (secondComma == firstComma);

            if (indexOnly)
            {
              typeIndex = (p.substring(firstComma + 1)).toInt();
            }
            else
            {
              typeIndex = (p.substring(firstComma + 1, secondComma)).toInt();
              freq = (p.substring(secondComma + 1)).toInt();
            }

            if (indexOnly)  /* request for frequency of typeIndex */
            {
              if (g_activeEvent != NULL)
              {
                freq = g_activeEvent->getFrequencyForRole(typeIndex);

                String msg = String(String(SOCK_COMMAND_TYPE_FREQ) + "," + freq);
                g_webSocketServer.broadcastTXT(stringObjToConstCharString(&msg), msg.length());

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                if (g_debug_prints_enabled)
                {
                  Serial.println("Freq" + String(typeIndex) + ": " + freq);
                }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
              }
            }
            else if ((freq >= TX_MIN_ALLOWED_FREQUENCY_HZ) && (freq <= TX_MAX_ALLOWED_FREQUENCY_HZ) && (typeIndex >= 0) && (typeIndex < MAXIMUM_NUMBER_OF_EVENT_TX_TYPES))
            {
              if (g_activeEvent != NULL)
              {
                g_activeEvent->setFrequencyForRole(typeIndex, freq);
                g_ESP_Comm_State = TX_HTML_SAVE_CHANGES;
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                if (g_debug_prints_enabled)
                {
                  Serial.println("Freq" + String(typeIndex) + ": " + String(freq));
                }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
              }
              else
              {
                String msgOut = String(LB_MESSAGE_TX_FREQ_SET + String(freq) + ";");
                g_LBOutputBuff->put(msgOut);
              }
            }
          }
          else if (msgHeader.equalsIgnoreCase(SOCK_COMMAND_TYPE_PWR))
          {
            int typeIndex;
            String pwr;
            int firstComma = p.indexOf(',');
            int secondComma = p.lastIndexOf(',');
            bool indexOnly = (secondComma == firstComma);

            if (indexOnly)
            {
              typeIndex = (p.substring(firstComma + 1)).toInt();
            }
            else
            {
              typeIndex = (p.substring(firstComma + 1, secondComma)).toInt();
              pwr = (p.substring(secondComma + 1));
            }

            if (indexOnly)  /* request for power level of typeIndex */
            {
              if (g_activeEvent != NULL)
              {
                pwr = g_activeEvent->getPowerlevelForRole(typeIndex);

                String msg = String(String(SOCK_COMMAND_TYPE_PWR) + "," + pwr);
                g_webSocketServer.broadcastTXT(stringObjToConstCharString(&msg), msg.length());
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                if (g_debug_prints_enabled)
                {
                  Serial.println("Pwr" + String(typeIndex) + ": " + pwr);
                }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
              }
            }
            else if ((pwr.toInt() >= 0) && (pwr.toInt() < TX_MAX_ALLOWED_POWER_MW) && (typeIndex >= 0) && (typeIndex < MAXIMUM_NUMBER_OF_EVENT_TX_TYPES))
            {
              if (g_activeEvent != NULL)
              {
                g_activeEvent->setPowerlevelForRole(typeIndex, pwr);
                g_ESP_Comm_State = TX_HTML_SAVE_CHANGES;
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                if (g_debug_prints_enabled)
                {
                  Serial.println("Pwr" + String(typeIndex) + ": " + pwr);
                }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
              }
              else
              {
                String msgOut = String(LB_MESSAGE_TX_POWER_SET + pwr + ";");
                g_LBOutputBuff->put(msgOut);
              }
            }
            else
            {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
              if (g_debug_prints_enabled)
              {
                Serial.println("Illegal socket message: POWER," + pwr);
              }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
            }
          }
          else if (msgHeader.equalsIgnoreCase(SOCK_COMMAND_TYPE_MODULATION))
          {
            String mod;
            int lastComma = p.lastIndexOf(',');
            mod = (p.substring(lastComma + 1));

            if ((mod == "CW") || (mod == "AM"))
            {
              if (g_activeEvent != NULL)
              {
                g_activeEvent->setEventModulation(mod);
                g_ESP_Comm_State = TX_HTML_SAVE_CHANGES;
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                if (g_debug_prints_enabled)
                {
                  Serial.println("Modulation: " + mod);
                }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
              }
              else
              {
                String msgOut = String(LB_MESSAGE_TX_MOD_SET + mod + ";");
                g_LBOutputBuff->put(msgOut);
              }
            }
          }
          else if (msgHeader.equalsIgnoreCase(SOCK_COMMAND_CWSPEED))
          {
            String speed;
            int lastComma = p.lastIndexOf(',');
            speed = p.substring(lastComma + 1);
            int spd = speed.toInt();

            if ((spd > 4) && (spd < 21))
            {
              String msgOut = String(LB_MESSAGE_CODE_SPEED_SETPAT + speed + ";");
              g_LBOutputBuff->put(msgOut);
            }
          }
          else if (msgHeader.equalsIgnoreCase(SOCK_COMMAND_BAND))
          {
            String band;
            int lastComma = p.lastIndexOf(',');
            band = (p.substring(lastComma + 1));

            if ((band == "80") || (band == "2"))
            {
              if (g_activeEvent != NULL)
              {
                g_activeEvent->setEventBand(band);
                g_ESP_Comm_State = TX_HTML_SAVE_CHANGES;

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                if (g_debug_prints_enabled)
                {
                  Serial.println("Band: " + band);
                }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
              }
              else
              {
                String msgOut = String(LB_MESSAGE_TX_BAND_SET + band + ";");
                g_LBOutputBuff->put(msgOut);
              }
            }
          }
          else if (msgHeader.equalsIgnoreCase(SOCK_COMMAND_EXECUTE_EVENT))
          {
            if (g_activeEvent)
            {
              bool fail =  g_activeEvent->writeEventFile();    /* save any changes made to the active event */
              if (fail)
              {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                if (g_debug_prints_enabled)
                {
                  Serial.println("Write event failed");
                }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
              }

              saveDefaultsFile();

              if (g_activeEvent->isNotDisabledEvent(g_timeOfDayFromTx))
              {
                g_ESP_Comm_State = TX_RECD_START_EVENT_REQUEST;
              }
              else /* If event will never run then reply with an error message */
              {
                String msg = String( String(SOCK_COMMAND_ERROR) + "," + ERROR_CODE_EVENT_ENDED_IN_PAST);
                g_webSocketServer.broadcastTXT(stringObjToConstCharString(&msg), msg.length());
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                if (g_debug_prints_enabled)
                {
                  Serial.println(msg);
                }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
              }
            }
          }
          else if (msgHeader.equalsIgnoreCase(SOCK_COMMAND_MAC))
          {
            for (int i = 0; i < MAX_NUMBER_OF_WEB_CLIENTS; i++)
            {
              if ((g_webSocketClient[i].macAddr).length() && (g_webSocketClient[i].socketID < WEBSOCKETS_SERVER_CLIENT_MAX))
              {
                String msg = String( String(SOCK_COMMAND_MAC) + "," + g_webSocketClient[i].macAddr );
                g_webSocketServer.sendTXT(g_webSocketClient[i].socketID, stringObjToConstCharString(&msg), msg.length());

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
                if (g_debug_prints_enabled)
                {
                  Serial.println(msg);
                }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
              }
            }
          }
          else if (msgHeader.equalsIgnoreCase(SOCK_COMMAND_SW_VERSIONS))
          {
            /* Send the version of the ATMEGA software */
            String atmegaVersion;
            if (g_atmega_sw_version.length() > 0)
            {
              atmegaVersion = g_atmega_sw_version;
            }
            else
            {
              atmegaVersion = "?";
            }

            /* Send the version of this software */
            String msg = String( String(SOCK_COMMAND_SW_VERSIONS) + "," + WIFI_SW_VERSION + "," + atmegaVersion);
            g_webSocketServer.broadcastTXT(stringObjToConstCharString(&msg), msg.length());
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
            if (g_debug_prints_enabled)
            {
              Serial.println(msg);
            }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
          }
          else if (msgHeader.equalsIgnoreCase(SOCK_COMMAND_SSID))
          {
            String msg = String( String(SOCK_COMMAND_SSID) + "," + g_AP_NameString );
            g_webSocketServer.broadcastTXT(stringObjToConstCharString(&msg), msg.length());

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
            if (g_debug_prints_enabled)
            {
              Serial.println(msg);
            }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
          }
          else if (msgHeader.equalsIgnoreCase(SOCK_COMMAND_XMIT_NOW))
          {
            String msgOut = String(LB_MESSAGE_XMIT_NOW);
            g_LBOutputBuff->put(msgOut);
          }
          else if (msgHeader.equalsIgnoreCase(SOCK_COMMAND_PREP4DATA))
          {
            String msgOut = String(LB_MESSAGE_PREP4DATA);
            g_LBOutputBuff->put(msgOut);
          }
          else if (msgHeader.equalsIgnoreCase(SOCK_COMMAND_KEYDOWN))
          {
            String msgOut = String(LB_MESSAGE_KEYDOWN);
            g_LBOutputBuff->put(msgOut);
          }
          else if (msgHeader.equalsIgnoreCase(SOCK_COMMAND_KEYUP))
          {
            String msgOut = String(LB_MESSAGE_KEYUP);
            g_LBOutputBuff->put(msgOut);
          }
          else if (msgHeader == SOCK_COMMAND_WIFI_OFF)
          {
            String msgOut = String(LB_MESSAGE_WIFI_OFF);
            g_LBOutputBuff->put(msgOut);
          }
          else if (msgHeader.equalsIgnoreCase(SOCK_COMMAND_MASTER))
          {
            String setting;
            int lastComma = p.lastIndexOf(',');
            setting = (p.substring(lastComma + 1));

            if ((setting.equalsIgnoreCase("1") || setting.equalsIgnoreCase("TRUE")))
            {
              g_IamMaster = true;
              saveDefaultsFile();
              ESP.reset();
            }
            else if ((setting.equalsIgnoreCase("0") || setting.equalsIgnoreCase("FALSE")))
            {
              g_IamMaster = false;
              saveDefaultsFile();
            }

            String msg = String( String(SOCK_COMMAND_MASTER) + "," + String(g_IamMaster));
            g_webSocketServer.broadcastTXT(stringObjToConstCharString(&msg), msg.length());
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
            if (g_debug_prints_enabled)
            {
              Serial.println(msg);
            }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
          }
          else if (msgHeader.equalsIgnoreCase(SOCK_COMMAND_PASSTHRU))
          {
            int firstComma = p.indexOf(',');
            String msgOut = (p.substring(firstComma + 1));
            g_LBOutputBuff->put(msgOut);
          }
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
          else if (g_debug_prints_enabled)
          {
            Serial.println("[WS]err: " + p);
          }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
        }
      }
      break;

    case WStype_BIN:
      {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
        if (g_debug_prints_enabled)
        {
          Serial.printf("[%u] get binary length: %u\r\n", num, length);
        }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

        if (payload)
        {
          hexdump(payload, length);

          /* echo data back to browser */
          g_webSocketServer.sendBIN(num, payload, length);
        }
      }
      break;

    default:
      {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
        if (g_debug_prints_enabled)
        {
          Serial.printf("Invalid WStype [%d]\r\n", type);
        }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
      }
      break;
  }
}


bool handleFileRead(String path)
{
  if (path == NULL) return false;

  /* send the right file to the client (if it exists) */
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
  if (g_debug_prints_enabled)
  {
    Serial.println("\nhandleFileRead: " + path);
  }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

  if (path.endsWith("/"))
  {
    path += "index.html";                   /* If a folder is requested, send the index file */
  }
  String contentType = getContentType(path);  /* Get the MIME type */
  String pathWithGz = path + ".gz";
  String pathWithHTML = path + ".html";

  if (LittleFS.exists(stringObjToConstCharString(&pathWithGz)) || LittleFS.exists(stringObjToConstCharString(&path)) || LittleFS.exists(stringObjToConstCharString(&pathWithHTML)))
  { /* If the file exists, either as a compressed archive, or normal */
    if (LittleFS.exists(pathWithGz))          /* If there's a compressed version available */
    {
      path += ".gz";                      /* Use the compressed verion */

    }
    if (LittleFS.exists(pathWithHTML))
    {
      path += ".html";
      contentType = getContentType(path);
    }

    File file = LittleFS.open(path, "r");                             /* Open the file */
    if (file)
    {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
      size_t sent = g_http_server.streamFile(file, contentType);  /* Send it to the client */
#else
      g_http_server.streamFile(file, contentType);  /* Send it to the client */
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

      file.close();                                               /* Close the file again */

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
      if (g_debug_prints_enabled)
      {
        Serial.println(String("\tSent " + String(sent) + "bytes to file: ") + path);
      }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
    }
    return ( true);
  }
  else
  {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (g_debug_prints_enabled)
    {
      Serial.println(String("File not found in LittleFS: ") + path);
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
  }

  return ( false);
}


int numberOfEventsScheduled(unsigned long epoch)
{
  int numberScheduled = 0;
  EventFileRef a;

  if (g_numberOfEventFilesFound < 1)
  {
    return ( 0);    /* no events means none is scheduled */
  }

  if (g_numberOfEventFilesFound > 1)
  {
    /* binary sort the events list from soonest to latest */
    for (int i = 0; i < g_numberOfEventFilesFound; ++i)
    {
      for (int j = i + 1; j < g_numberOfEventFilesFound; ++j)
      {
        if (Event::isSoonerEvent(g_eventList[j], g_eventList[i], epoch))
        {
          a = g_eventList[i];
          g_eventList[i] = g_eventList[j];
          g_eventList[j] = a;
        }
      }
    }

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (g_debug_prints_enabled)
    {
      Serial.println("*********************");
      Serial.println("Ordered Events:");
      for (int i = 0; i < g_numberOfEventFilesFound; i++)
      {
        a = g_eventList[i];
        Serial.println( String(i) + ". " + a.path);
        if ((epoch < a.finishDateTimeEpoch) && (a.startDateTimeEpoch < a.finishDateTimeEpoch)) {
          Serial.println("    Enabled*");
        }
        Serial.println( "    Start: " + String(a.startDateTimeEpoch));
        Serial.println( "    Finish:" + String(a.finishDateTimeEpoch));
      }
      Serial.println("*********************");
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
  }

  for (int i = 0; i < g_numberOfEventFilesFound; i++)
  {
    a = g_eventList[i];
    if ((epoch < a.finishDateTimeEpoch) && (a.startDateTimeEpoch < a.finishDateTimeEpoch))
    {
      numberScheduled++;
    }
  }

  return ( numberScheduled);
}

bool readEventTimes(String path, EventFileRef * fileRef)
{
  fileRef->path = path;
  int data_count = 0;

  if (LittleFS.exists(path))
  {
    File file = LittleFS.open(path, "r"); /* Open the file for reading */

    if (file)
    {
      yield();
      String s = file.readStringUntil('\n');
      int count = 0;

      while (s.length() && (count++ < MAXIMUM_NUMBER_OF_EVENT_FILE_LINES) && (data_count < 5))
      {
        lights->blinkLEDs(500, RED_BLUE_TOGETHER, true);
        if (s.indexOf("EVENT_START_DATE_TIME") >= 0)
        {
          EventLineData data;
          Event::extractLineData(s, &data);
          fileRef->startDateTimeEpoch = convertTimeStringToEpoch(data.value);
          /*        startRead = true; */
          data_count++;

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
          if ( g_debug_prints_enabled )
          {
            Serial.println("Start epoch: " + String(fileRef->startDateTimeEpoch));
          }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
        }
        else if (s.indexOf("EVENT_FINISH_DATE_TIME") >= 0)
        {
          EventLineData data;
          Event::extractLineData(s, &data);
          fileRef->finishDateTimeEpoch = convertTimeStringToEpoch(data.value);
          data_count++;

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
          if ( g_debug_prints_enabled )
          {
            Serial.println("Finish epoch: " + String(fileRef->finishDateTimeEpoch));
          }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
        }
        else if ( s.indexOf("EVENT_NAME") >= 0)
        {
          EventLineData data;
          Event::extractLineData(s, &data);
          fileRef->ename = data.value;
          data_count++;

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
          if ( g_debug_prints_enabled )
          {
            Serial.println("Event name: " + fileRef->ename);
          }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
        }
        else if ( s.indexOf(EVENT_FILE_VERSION) >= 0)
        {
          EventLineData data;
          Event::extractLineData(s, &data);
          fileRef->vers = data.value;
          data_count++;

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
          if ( g_debug_prints_enabled )
          {
            Serial.println("File version: " + fileRef->vers);
          }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
        }
        else if (s.indexOf(EVENT_CALLSIGN) >= 0)
        {
          EventLineData data;
          Event::extractLineData(s, &data);
          fileRef->callsign = data.value;
          data_count++;

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
          if ( g_debug_prints_enabled )
          {
            Serial.println("Callsign: " + fileRef->callsign);
          }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
        }
        /* TODO: Role, Power, and Frequency also need to be read */

        yield();
        s = file.readStringUntil('\n');
      }

      file.close();   /* Close the file */
    }
    Event::extractMeFileData(path, fileRef);

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (g_debug_prints_enabled)
    {
      Serial.println(String("Times read for file: ") + path);
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
  }

  return (data_count != 5);
}

bool populateEventFileList(void)
{
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
  if ( g_debug_prints_enabled )
  {
    Serial.println("Reading events...");
  }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

  g_numberOfEventFilesFound = 0;
  Dir dir = LittleFS.openDir("/");
  while (dir.next())
  {
    lights->blinkLEDs(500, RED_BLUE_TOGETHER, true);
    String fileName = dir.fileName();

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    size_t fileSize = dir.fileSize();
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

    if (fileName.endsWith(".event"))
    {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
      if ( g_debug_prints_enabled )
      {
        Serial.printf("FS File: %s, size: %s\r\n", fileName.c_str(), formatBytes(fileSize).c_str());
      }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

      EventFileRef fileRef;
      if (!readEventTimes(fileName, &fileRef))
      {
        g_eventList[g_numberOfEventFilesFound] = fileRef;
        g_numberOfEventFilesFound++;
      }
    }
  }

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
  if ( g_debug_prints_enabled )
  {
    for (int i = 0; i < g_numberOfEventFilesFound; i++)
    {
      Serial.println( String(i) + ". " + g_eventList[i].path);
      Serial.println( "    " + String(g_eventList[i].startDateTimeEpoch));
      Serial.println( "    " + String(g_eventList[i].finishDateTimeEpoch));
      Serial.println( "    " + String(g_eventList[i].vers));
      Serial.println( "    " + String(g_eventList[i].ename));
      Serial.println( "    " + String(g_eventList[i].role));
      Serial.println( "    " + String(g_eventList[i].callsign));
      Serial.println( "    " + String(g_eventList[i].power));
      Serial.println( "    " + String(g_eventList[i].freq));
    }
    Serial.printf("\n");
  }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

  return ( true);
}


bool readDefaultsFile()
{
  /* send the right file to the client (if it exists) */
  String path = "/defaults.txt";
  bool success = true;

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
  if (g_debug_prints_enabled)
  {
    Serial.println("Reading defaults...");
  }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

  String contentType = getContentType(path);  /* Get the MIME type */
  if (LittleFS.exists(path))
  { /* If the file exists, either as a compressed archive, or normal
      open file for reading */
    File file = LittleFS.open(path, "r");     /* Open the file */
    if (file)
    {
      yield();
      String s = file.readStringUntil('\n');
      int count = 0;

      while (s.length() && count++ < NUMBER_OF_SETTABLE_VARIABLES)
      {
        if (s.indexOf(',') < 0)
        {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
          if (g_debug_prints_enabled)
          {
            Serial.println("Error: illegal entry found in defaults file at line " + count);
          }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

          break;  /* invalid line found */
        }

        String settingID = s.substring(0, s.indexOf(','));
        String value = s.substring(s.indexOf(',') + 1, s.indexOf('\n'));

        if (value.charAt(0) == '"')
        {
          if (value.charAt(1) == '"') /* handle empty string */
          {
            value = "";
          }
          else                        /* remove quotes */
          {
            value = value.substring(1, value.length() - 2);
          }
        }

        if (settingID.equalsIgnoreCase("MASTER_ENABLE"))
        {
          if (value.charAt(0) == 'T' || value.charAt(0) == 't' || value.charAt(0) == '1')
          {
            g_IamMaster = 1;
          }
          else
          {
            g_IamMaster = 0;
          }
        }
        else if (settingID.equalsIgnoreCase("LEDS_ENABLE"))
        {
          if (value.charAt(0) == 'T' || value.charAt(0) == 't' || value.charAt(0) == '1')
          {
            g_LEDs_enabled = 1;
          }
          else
          {
            g_LEDs_enabled = 0;
          }
        }
        //        else if (settingID.equalsIgnoreCase("LAST_EVENT_FILE_RUN"))
        //        {
        //          g_last_event_file_run = value;
        //        }
        else if (settingID.equalsIgnoreCase("DEBUG_PRINTS_ENABLE"))
        {
          /*        if (value.charAt(0) == 'T' || value.charAt(0) == 't' || value.charAt(0) == '1')
                    {
                      g_debug_prints_enabled = 1;
                    }
                    else
                    {
                      g_debug_prints_enabled = 0;
                    } */
        }
        else
        {
          success = false;
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
          if (g_debug_prints_enabled)
          {
            Serial.println("Error in file: SettingID = " + settingID + " Value = [" + value + "]");
          }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
        }

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
        if (g_debug_prints_enabled)
        {
          Serial.println("[" + s + "]");
        }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

        yield();
        s = file.readStringUntil('\n');
      }

      file.close();   /* Close the file */

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
      if (g_debug_prints_enabled)
      {
        Serial.println(String("\tRead file: ") + path);
      }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
    }

    return ( success);
  }

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
  if (g_debug_prints_enabled)
  {
    Serial.println(String("\tFile Not Found: ") + path);    /* If the file doesn't exist, return false */
  }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
  return ( false);
}

void saveDefaultsFile()
{
  String path = "/defaults.txt";

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
  if (g_debug_prints_enabled)
  {
    Serial.println("Writing defaults...");
  }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
  String contentType = getContentType(path);  /* Get the MIME type */
  /*  if(LittleFS.exists(path)) */
  { /* If the file exists, either as a compressed archive, or normal
      open file for reading */
    File file = LittleFS.open(path, "w");     /* Open the file for writing */
    if (file)
    {
      String s;

      for (int i = 0; i < NUMBER_OF_SETTABLE_VARIABLES; i++)
      {
        switch (i)
        {
          case MASTER_ENABLE:
            {
              if (g_IamMaster)
              {
                file.println("MASTER_ENABLE,TRUE");
              }
              else
              {
                file.println("MASTER_ENABLE,FALSE");
              }

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
              if (g_debug_prints_enabled)
              {
                Serial.println("Wrote MASTER_ENABLE");
              }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
            }
            break;

          case LEDS_ENABLE:
            {
              if (g_LEDs_enabled)
              {
                file.println("LEDS_ENABLE,TRUE");
              }
              else
              {
                file.println("LEDS_ENABLE,FALSE");
              }

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
              if (g_debug_prints_enabled)
              {
                Serial.println("Wrote LEDS_ENABLE");
              }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
            }
            break;

          case DEBUG_PRINTS_ENABLE:
            {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
              if (g_debug_prints_enabled)
              {
                file.println("DEBUG_PRINTS_ENABLE,TRUE");
              }
              else
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
              {
                file.println("DEBUG_PRINTS_ENABLE,TRUE");
              }

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
              if (g_debug_prints_enabled)
              {
                Serial.println("Wrote DEBUG_PRINTS_ENABLE");
              }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
            }
            break;

          case LAST_EVENT_FILE_RUN:
            {
              //              if (g_last_event_file_run.length() > 0)
              //              {
              //                file.println(String("LAST_EVENT_FILE_RUN,\"") + g_last_event_file_run + "\"");
              //              }
              //#if TRANSMITTER_COMPILE_DEBUG_PRINTS
              //              if (g_debug_prints_enabled)
              //              {
              //                Serial.println("Wrote LAST_EVENT_FILE_RUN_DEFAULT");
              //              }
              //#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
            }
            break;

          default:
            {
            }
            break;
        }
      }

      file.close();   /* Close the file */
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
      if (g_debug_prints_enabled)
      {
        Serial.println(String("\tWrote file: ") + path);
      }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
    }
  }

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
  if (g_debug_prints_enabled)
  {
    Serial.println(String("\tFile Not Found: ") + path);    /* If the file doesn't exist, return false */
  }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
}

void handleFileDelete()
{
  String path = g_http_server.arg(0);
  bool abort = false;

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
  if (g_debug_prints_enabled)
  {
    Serial.println("\nhandleFileDelete: " + path);
  }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

  if (path.endsWith("/"))
  {
    abort = true;   /* If a folder is requested consider it an error */

  }
  if (!abort && LittleFS.exists(stringObjToConstCharString(&path)))
  { /* If the file exists */
    LittleFS.remove(path);

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (g_debug_prints_enabled)
    {
      Serial.println(String(path + " Deleted."));
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

    fileDeleteWithMessage("<p>File deleted successfully!</p>");
  }
  else
  {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (g_debug_prints_enabled)
    {
      Serial.println(String("File not found in LittleFS: ") + path);
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

    fileDeleteWithMessage("<p>File deletion failed!</p>");
  }


  return;
}


void handleFileDownload()
{
  String path = g_http_server.arg(0);

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
  if (g_debug_prints_enabled)
  {
    Serial.println("\nhandleFileDownload: " + path);
  }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

  if (path.endsWith("/"))
  {
    path += "index.html";                                           /* If a folder is requested, send the index file */
  }

  String contentType = "text/plain";                                  /* Always use text MIME type */
  if (LittleFS.exists(stringObjToConstCharString(&path)))
  { /* If the file exists, either as a compressed archive, or normal */
    File file = LittleFS.open(path, "r");                             /* Open the file */

    if (file)
    {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
      size_t sent = g_http_server.streamFile(file, contentType);  /* Send it to the client */
#else
      g_http_server.streamFile(file, contentType);  /* Send it to the client */
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

      file.close();                                               /* Close the file */

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
      if (g_debug_prints_enabled)
      {
        Serial.println(String("\tSent " + String(sent) + "bytes from file: ") + path);
      }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
    }
  }
  else
  {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (g_debug_prints_enabled)
    {
      Serial.println(String("File not found in LittleFS: ") + path);
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
  }

  return;
}

void handleFileUpload()
{ /* upload a new file to the LittleFS */
  HTTPUpload& upload = g_http_server.upload();
  String path;

  if (upload.status == UPLOAD_FILE_START)
  {
    path = upload.filename;
    if (!path.endsWith(".gz"))
    { /* The file server always prefers a compressed version of a file */
      String pathWithGz = path + ".gz";   /* So if an uploaded file is not compressed, the existing compressed */
      if (LittleFS.exists(pathWithGz))      /* version of that file must be deleted (if it exists) */
      {
        LittleFS.remove(pathWithGz);
      }
    }
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (g_debug_prints_enabled)
    {
      Serial.print("handleFileUpload Name: "); Serial.println(path);
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
    fsUploadFile = LittleFS.open(path, "w");  /* Open the file for writing in LittleFS (create if it doesn't exist) */
    path = String();
  }
  else if (upload.status == UPLOAD_FILE_WRITE)
  {
    if (fsUploadFile)
    {
      fsUploadFile.write(upload.buf, upload.currentSize); /* Write the received bytes to the file */
    }
  }
  else if (upload.status == UPLOAD_FILE_END)
  {
    if (fsUploadFile)
    { /* If the file was successfully created */
      fsUploadFile.close();                                   /* Close the file again */
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
      if (g_debug_prints_enabled)
      {
        Serial.print("handleFileUpload Size: ");
        Serial.println(upload.totalSize);
      }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
      handleSuccess();

      if (path.endsWith(".event"))
      {
        g_ESP_Comm_State = TX_READ_ALL_EVENTS_FILES; /* have the transmitter re-initialize event file list */
      }
    }
    else
    {
      g_http_server.send(500, "text/plain", "500: couldn't create file");
    }
  }
}

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
void showSettings()
{
  int i;

  if (!g_debug_prints_enabled)
  {
    return;
  }

  Serial.println("RDP WiFi firmware - v" + String(WIFI_SW_VERSION));
  Serial.println("Baud Rate: " + String(SERIAL_BAUD_RATE));
  Serial.println();
  Serial.println("HUZZAH Settings");
  for (i = 0; i < NUMBER_OF_SETTABLE_VARIABLES; i++)
  {
    switch (i)
    {
      case MASTER_ENABLE:
        {
          Serial.println(String("Master Enabled (MASTER_ENABLE) = ") + String(g_IamMaster));
        }
        break;

      case LEDS_ENABLE:
        {
          Serial.println("LEDs Enabled (LEDS_ENABLE) = " + String(g_LEDs_enabled));
        }
        break;

      case DEBUG_PRINTS_ENABLE:
        {
          Serial.println("Debug Prints Enabled (DEBUG_PRINTS_ENABLE) = " + String(g_debug_prints_enabled));
        }
        break;

      case LAST_EVENT_FILE_RUN:
        {
          //          Serial.println("Last event file run = " + g_last_event_file_run);
        }
        break;

      default:
        {
        }
        break;
    }
  }
}
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

void handleLBMessage(String message)
{
  if (message == NULL) return;
  if (message.length() < 3) return;

  yield();

  /* e.g., "$EC,247;" */
  int firstDelimit = message.indexOf(',');

  if (firstDelimit < 0)
  {
    firstDelimit = message.indexOf(';');
  }

  if (firstDelimit < 0)
  {
    firstDelimit = message.length() - 1;
  }

  if (firstDelimit < 0)
  {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (g_debug_prints_enabled)
    {
      Serial.println("Bad LB msg rcvd!");
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
    return;
  }

  String type = message.substring(1, firstDelimit);
  String payload = "";
  if (message.indexOf(',') >= 0)
  {
    payload = message.substring(firstDelimit + 1, message.indexOf(';'));
  }

  if (!g_slave_released) /* If connected to Master ignore most messages */
  {
    if (!type.equals(LB_MESSAGE_ACK))
    {
      return;
    }
  }

  if (type.equals(LB_MESSAGE_ACK))
  {
    if (g_linkBusAckPending)
    {
      g_linkBusAckPending--;
    }

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (g_debug_prints_enabled)
    {
      Serial.println("ACK");
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
  }
  else if (type.equals(LB_MESSAGE_NACK))
  {
    if (g_linkBusAckPending)
    {
      g_linkBusAckPending--;
    }

    g_linkBusNacksReceived++;

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (g_debug_prints_enabled)
    {
      Serial.println("NACK");
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
  }
  else if (type.equals(LB_MESSAGE_ESP))
  {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (g_debug_prints_enabled)
    {
      Serial.println(String("Rcvd ESP Msg w/ payload: ") + payload);
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (payload.equals("1")) /* Atmega is asking to receive the next active event */
    {
      if (g_ESP_Comm_State == TX_WAITING_FOR_INSTRUCTIONS)
      {
        g_ESP_Comm_State = TX_SEND_NEXT_EVENT_TO_ATMEGA;
        g_LBOutputBuff->put(LB_MESSAGE_ESP_KEEPALIVE);
      }
      else
      {
        g_onlyUpdateEvent = true;
      }
    }
    else if (payload.equals("X")) /* Atmega is asking ESP to shut down now */
    {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
      if (g_debug_prints_enabled)
      {
        Serial.println(String("Rcvd ESP shutdown: ") + payload);
      }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
      g_ESP_Comm_State = TX_POWER_DOWN_NOW;
    }
  }
  else if (type.equals(LB_MESSAGE_TIME))
  {
    String timeinfo = payload;
    g_timeOfDayFromTx = (unsigned long)payload.toInt();
    unsigned long epoch = g_timeOfDayFromTx;

    if (epoch)
    {
      if (g_justPoweredUp)
      {
        if (g_linkBusAckPending)
        {
          g_linkBusAckPending--; /* This msg serves as an ACK */
        }
        g_justPoweredUp = false;
      }

      if (g_numberOfSocketClients)
      {
        String msg = String(String(SOCK_COMMAND_SYNC_TIME) + "," + timeinfo);
        g_webSocketServer.broadcastTXT(stringObjToConstCharString(&msg), msg.length());
      }
    }
  }
  else if (type.equals(LB_MESSAGE_ERROR_CODE))
  {
    String code = payload;

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (g_debug_prints_enabled)
    {
      Serial.println("err=" + String(code));
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

    if (g_numberOfSocketClients)
    {
      String msg = String(String(SOCK_COMMAND_ERROR) + "," + code);
      g_webSocketServer.broadcastTXT(stringObjToConstCharString(&msg), msg.length());
    }
  }
  else if (type.equals(LB_MESSAGE_STATUS_CODE))
  {
    String code = payload;

    if (g_numberOfSocketClients)
    {
      String msg = String(String(SOCK_COMMAND_STATUS) + "," + code);
      g_webSocketServer.broadcastTXT(stringObjToConstCharString(&msg), msg.length());
    }
  }
  else if (type.equals(LB_MESSAGE_TX_POWER))
  {
    String code = payload;
    int mLocation = code.indexOf('M');

    if (mLocation >= 0)
    {
      code = code.substring(mLocation + 2);
    }

    if (g_numberOfSocketClients)
    {
      String msg = String(String(SOCK_COMMAND_TYPE_PWR) + "," + code);
      g_webSocketServer.broadcastTXT(stringObjToConstCharString(&msg), msg.length());
    }
  }
  else if (type.equals(LB_MESSAGE_TEMP))
  {
#ifdef PROTOTYPE_HARDWARE
    int16_t rawtemp = payload.toInt();
    bool negative =  rawtemp & 0x8000;
    if (negative)
    {
      rawtemp &= 0x7FFF;
    }
    float temp = (rawtemp >> 8) + (0.25 * ((rawtemp & 0x00C0) >> 6)) + 0.05;
    if (negative)
    {
      temp = -temp;
    }

    char dataStr[6];    /* allow for possible negative sign */
    dtostrf(temp, 4, 1, dataStr);
    dataStr[5] = '\0';

    if (g_numberOfSocketClients)
    {
      String msg = String(String(SOCK_COMMAND_TEMPERATURE) + "," + dataStr + "C");

      g_webSocketServer.broadcastTXT(stringObjToConstCharString(&msg), msg.length());
      /*      if (g_debug_prints_enabled)
              {
                Serial.println(msg);
              } */
    }
#else
    if (g_numberOfSocketClients)
    {
      String msg = String(String(SOCK_COMMAND_TEMPERATURE) + "," + payload + "C");

      g_webSocketServer.broadcastTXT(stringObjToConstCharString(&msg), msg.length());
      /*      if (g_debug_prints_enabled)
              {
                Serial.println(msg);
              } */
    }
#endif
  }
  else if (type.equals(LB_MESSAGE_BATTERY))
  {
    int temp = payload.toInt();

    if (g_numberOfSocketClients)
    {
      String msg;

      temp = temp % 10000;
      int v = temp / 10;
      int f = temp % 10;
      msg = String(String(SOCK_COMMAND_BATTERY) + "," + v + "." + f + "V");

      g_webSocketServer.broadcastTXT(stringObjToConstCharString(&msg), msg.length());
    }
  }
  else if (type.equals(LB_MESSAGE_VER))
  {
    if (payload.length() > 1)
    {
      g_atmega_sw_version = payload;
    }
  }
  else if (type.equals(LB_MESSAGE_OSC_CAL))
  {
    int p = payload.toInt();

    if (p <= 255)
    {
      if (!p)
      {
        g_baud_sync_success = true;
      }

      message.trim();
      Serial.println(message); /* send immediately with no ACK required */
    }
  }
}


bool sendEventToATMEGA(String * errorTxt)
{
  int serialIndex = 0;
  bool done = false;
  bool failure = true;
  int role = 0;
  int tx;
  TxDataType* txData = NULL;
  String msgOut;
  int times2try = 5;
  int last = 0;
  LEDPattern ledPattern = RED_BLUE_ALTERNATING;
  int blinkPeriodMillis = 100;

  if (errorTxt)
  {
    *errorTxt = String("");
  }

  g_linkBusAckTimoutOccurred = false;

  /*
       Configure the ATMEGA appropriately for its role in the scheduled event.
  */
  while (!done)
  {
    g_webSocketLocalClient.loop();
    linkbusLoop();
    yield();
    lights->blinkLEDs(blinkPeriodMillis, ledPattern, true);

    switch (serialIndex)
    {
      case 0: /* Prepare ATMEGA to receive event data */
        {
          if (!g_LBOutputBuff->full())
          {
            g_LBOutputBuff->put(LB_MESSAGE_PREP4DATA);
            serialIndex++;
          }
        }
        break;

      case 1: /* send finish time */
        {
          if (!g_LBOutputBuff->full())
          {
            tx = g_activeEvent->getTxSlotIndex();
            role = g_activeEvent->getTxRoleIndex();

            if ((tx >= 0) && (role >= 0))
            {
              txData = g_activeEvent->getTxData(role, tx);

              /* Finish time should be sent first */
              msgOut = String(LB_MESSAGE_STARTFINISH_SET_FINISH + String(convertTimeStringToEpoch(g_activeEvent->getEventFinishDateTime())) + ";");
              g_LBOutputBuff->put(msgOut);
              serialIndex++;
            }
            else
            {
              if (errorTxt)
              {
                *errorTxt = String("Bad role settings");
              }
                
              done = true;
            }
          }
        }
        break;

      case 2: /* Message pattern */
        {
          if (!g_LBOutputBuff->full())
          {
            msgOut = String(LB_MESSAGE_PATTERN_SET + txData->pattern + ";");
            g_LBOutputBuff->put(msgOut);
            serialIndex++;
          }
        }
        break;

      case 3: /* Off time */
        {
          if (!g_LBOutputBuff->full())
          {
            msgOut = String(LB_MESSAGE_TIME_INTERVAL_SET0 + String(txData->offTime) + ";");
            g_LBOutputBuff->put(msgOut);
            serialIndex++;
          }
        }
        break;

      case 4: /* On time */
        {
          if (!g_LBOutputBuff->full())
          {
            msgOut = String(LB_MESSAGE_TIME_INTERVAL_SET1 + String(txData->onTime) + ";");
            g_LBOutputBuff->put(msgOut);
            serialIndex++;
          }
        }
        break;

      case 5: /* Offset time */
        {
          if (!g_LBOutputBuff->full())
          {
            msgOut = String(LB_MESSAGE_TIME_INTERVAL_SETD + String(txData->delayTime) + ";");
            g_LBOutputBuff->put(msgOut);
            serialIndex++;
          }
        }
        break;

      case 6: /* Station ID transmit interval */
        {
          if (!g_LBOutputBuff->full())
          {
            msgOut = String(LB_MESSAGE_TIME_INTERVAL_SETID + String(g_activeEvent->getIDIntervalForRole(role)) + ";");
            g_LBOutputBuff->put(msgOut);
            serialIndex++;
          }
        }
        break;

      case 7: /* Event band */
        {
          if (!g_LBOutputBuff->full())
          {
            String b;

            if (g_activeEvent->getFrequencyForRole(role) <= 4000000L)
            {
              b = "80";
            }
            else
            {
              b = "2";
            }

            msgOut = String(LB_MESSAGE_TX_BAND_SET + b + ";");
            g_LBOutputBuff->put(msgOut);
            serialIndex++;
          }
        }
        break;

      case 8: /* Transmit power for role */
        {
          if (!g_LBOutputBuff->full())
          {
            msgOut = String(LB_MESSAGE_TX_POWER_SET + String(g_activeEvent->getPowerlevelForRole(role)) + ";");
            g_LBOutputBuff->put(msgOut);
            serialIndex++;
          }
        }
        break;

      case 9: /* Modulation format */
        {
          if (!g_LBOutputBuff->full())
          {
            msgOut = String(LB_MESSAGE_TX_MOD_SET + String(g_activeEvent->getEventModulation()) + ";");
            g_LBOutputBuff->put(msgOut);
            serialIndex++;
          }
        }
        break;

      case 10:    /* Frequency for role */
        {
          if (!g_LBOutputBuff->full())
          {
            msgOut = String(LB_MESSAGE_TX_FREQ_SET + String(g_activeEvent->getFrequencyForRole(role)) + ";");
            g_LBOutputBuff->put(msgOut);
            serialIndex++;
          }
        }
        break;

      case 11:    /* Station ID */
        {
          if (!g_LBOutputBuff->full())
          {
            msgOut = String(LB_MESSAGE_CALLSIGN_SET + g_activeEvent->getCallsign() + ";");
            g_LBOutputBuff->put(msgOut);
            serialIndex++;
          }
        }
        break;

      case 12:    /* Message code speed */
        {
          if (!g_LBOutputBuff->full())
          {
            msgOut = String(LB_MESSAGE_CODE_SPEED_SETPAT + String(g_activeEvent->getCodeSpeedForRole(role)) + ";");
            g_LBOutputBuff->put(msgOut);
            serialIndex++;
          }
        }
        break;

      case 13:    /* ID code speed */
        {
          if (!g_LBOutputBuff->full())
          {
            msgOut = String(LB_MESSAGE_CODE_SPEED_SETID + g_activeEvent->getCallsignSpeed() + ";");
            g_LBOutputBuff->put(msgOut);
            serialIndex++;
          }
        }
        break;

      case 14:    /* Start time */
        {
          if (!g_LBOutputBuff->full())
          {
            /* Start time is sent last */
            msgOut = String(LB_MESSAGE_STARTFINISH_SET_START + String(convertTimeStringToEpoch(g_activeEvent->getEventStartDateTime())) + ";");
            g_LBOutputBuff->put(msgOut);
            serialIndex++;
          }
        }
        break;

      case 15:    /* Perm - Have ATMega save everything in EEPROM */
        {
          if (!g_LBOutputBuff->full())
          {
            g_LBOutputBuff->put(LB_MESSAGE_PERM);
            serialIndex++;
          }
        }
        break;

      case 16:
        {
          if (!g_LBOutputBuff->full())
          {
            g_LBOutputBuff->put(LB_MESSAGE_ACTIVATE_EVENT);
            serialIndex++;
            times2try = 10;
          }
        }
        break;

      case 17: /* Wait for confirmation */
        {
          if (times2try)
          {
            if (g_LBOutputBuff->empty() && !g_linkBusAckPending && !g_linkBusAckTimoutOccurred)
            {
              failure = false;
              done = true;
            }
            else
            {
              if ((unsigned long)(millis() - last) > 1000)
              {
                last = millis();
                times2try--;
              }
            }
          }
          else
          {
            if (errorTxt)
            {
              *errorTxt = String("Processor not responding");
            }

            done = true;
            failure = true;
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
            if (g_debug_prints_enabled)
            {
              Serial.println("LB Buffer Timeout");
            }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
          }
        }
        break;

      default:    /* Tell ATMEGA to begin executing the event */
        {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
          if (g_debug_prints_enabled)
          {
            Serial.println("Bad state in event sender");
          }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
          if (g_LBOutputBuff->full())
          {
            if (errorTxt)
            {
              *errorTxt = String("Error: LB buffer full!");
            }

            return (true);
          }

          done = true;
        }
        break;
    }
  }

  return (failure);
}
