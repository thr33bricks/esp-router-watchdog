
// Jordan

#include <ESP8266WiFi.h> 

#define DEBUG

#define RELAY_PIN 5
#define MAX_COUNT 10
//#define RES_TIME 7200000 // in ms, 2*60*60*1000 = 7200000 (2 hours)
#define WAIT_CON 300000  // in ms, 5*60*1000 = 300000 (5 min)
const unsigned long RES_TIME = 7200000; // in ms, 2*60*60*1000 = 7200000 (2 hours)

const char* ssid = "Peshu";      // Wifi name
const char* password = "70000000"; // Wifi pass
const char* host = "google.com"; // website check internet connection

int checkCounter = 0;
unsigned long timeOfRestart = RES_TIME;

void setup() {
  #ifdef DEBUG
  Serial.begin(115200);
  #endif

  pinMode(RELAY_PIN, OUTPUT);
}

void loop() {
  unsigned long currTime = millis();
  if(currTime-timeOfRestart >= RES_TIME){
    timeOfRestart = currTime + RES_TIME;

    mainLogic();
  }
  else{
    debug("Time remaining: ");
    debug((int)((RES_TIME - (currTime-timeOfRestart))/1000));
    debug(" sec\n");
  }
}

void mainLogic(){
  bool conn = checkInternet();
  delay(WAIT_CON);
  
  if(conn)
    checkCounter = 0;
  else{
    checkCounter++;

    if(checkCounter == MAX_COUNT){
      checkCounter = 0;
      ResRouter();
      //delay(RES_TIME);
      timeOfRestart = millis();
    }
  }

  debug("Internet: ");
  debug(conn);
  debug("\n");
  debug("Counter: ");
  debug(checkCounter);
  debug("\n");
}

void debug(bool val){
  #ifdef DEBUG
  Serial.print(val);
  #endif
}

void debug(int val){
  #ifdef DEBUG
  Serial.print(val);
  #endif
}

void debug(const char *val){
  #ifdef DEBUG
  Serial.print(val);
  #endif
}

void ResRouter(){
  debug("Router restarting...\n");
  
  digitalWrite(RELAY_PIN, 1);
  delay(10000);
  digitalWrite(RELAY_PIN, 0);
  
  debug("Router restarted!\n");
}

bool checkInternet(){
  bool wifi = false;
  bool internet = false;
  
  WiFi.begin(ssid, password);
  debug("Connecting to WiFi");

  // Wait for wifi connection
  for(uint8_t i=0; i<10; ++i){
    if(WiFi.status() == WL_CONNECTED){
      wifi = true;
      break;
    }
    debug(".");
    delay(1000);
  }

  // Check google.com
  if(wifi)
    internet = checkInternetConnection();

  WiFi.disconnect();
  delay(5000);
  debug("Disconnected from WiFi!\n");
  
  return internet;
}

bool checkInternetConnection() {
  WiFiClient client;
  client.setTimeout(5000);
  
  debug("Connecting to ");
  debug(host);
  debug("...\n");
  
  if (client.connect(host, 80)) {
    debug("Connected to the internet!\n");
    client.stop();
    return true;
  } else {
    debug("Failed to connect to the internet!\n");
    return false;
  }
}
