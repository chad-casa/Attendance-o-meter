/*
 *Attendance-o-meter 
 *MMWave PEOPLE COUNTER - USING DISTANCE CHANGE AND DETECTION ZONE
 * IGNORES STATIC OBSERVATIONS
 */

#define RadarSerial Serial1

// ============ INCLUDES ============
#include <SPI.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include "arduino_secrets.h"
#include <utility/wifi_drv.h>
#include <Wire.h>
#include "Waveshare_LCD1602_RGB.h"

// ============ OBJECTS ============
Waveshare_LCD1602_RGB lcd(16, 2);
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// ============ CREDENTIALS ============
const char* ssid          = SECRET_SSID;
const char* password      = SECRET_PASS;
const char* mqtt_username = SECRET_MQTTUSER;
const char* mqtt_password = SECRET_MQTTPASS;
const char* mqtt_server   = "mqtt.cetools.org";
const int mqtt_port       = 1884;

String visitorId = "6";
String mqtt_topic_base = "student/CASA0014/visitors/" + visitorId;

// ============ DETECTION ZONES ============
const int CLOSE_ZONE = 150;    // < 150 cm = CLOSE
const int FAR_ZONE = 300;      // > 300 cm = FAR
const int MIN_TRAVEL = 100;    // Must move at least 100 cm

// ============ MOVEMENT DETECTION ============
const int DISTANCE_CHANGE_THRESHOLD = 45;  // 45cm change = movement detected
const int STABLE_READINGS_TO_STOP = 3;     // 3 similar readings = person stopped

// ============ TIMING ============
const unsigned long CROSSING_TIMEOUT = 4000;
const unsigned long COOLDOWN = 2000;
const unsigned long READY_DELAY = 800;

// ============ STATE ============
int entries = 0;
int exits = 0;
int inRoom = 0;

bool tracking = false;
bool ready = true;
unsigned long trackStart = 0;
unsigned long lastCount = 0;
unsigned long lastNone = 0;

int startDist = 0;
int minDist = 9999;
int maxDist = 0;
int lastDist = 0;
bool sawClose = false;
bool sawFar = false;

// Movement detection by distance change
int prevDist = 0;
int stableCount = 0;
int distanceChanges = 0;

// Sensor buffer
byte buffer[64];
int bufIdx = 0;

// ============ SETUP ============
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  RadarSerial.begin(9600);
  
  Serial.println("\n=== PEOPLE COUNTER v3.3 ===");
  Serial.println("DISTANCE CHANGE DETECTION");
  Serial.println("CLOSE: < 150 cm");
  Serial.println("FAR:   > 300 cm");
  Serial.println("Movement: 45 cm change");
  Serial.println("Commands: r=reset, s=status\n");

  WiFiDrv::pinMode(25, OUTPUT);
  WiFiDrv::pinMode(26, OUTPUT);
  WiFiDrv::pinMode(27, OUTPUT);

  lcd.init();
  lcd.setRGB(255, 255, 255);
  lcd.send_string("Starting...");
  
  connectWiFi();
  
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setBufferSize(512);
  connectMQTT();
  
  updateLCD();
  Serial.println("Ready!\n");
}

// ============ MAIN LOOP ============
void loop() {
  if (WiFi.status() != WL_CONNECTED) connectWiFi();
  if (!mqttClient.connected()) connectMQTT();
  mqttClient.loop();
  
  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'r' || c == 'R') {
      entries = exits = inRoom = 0;
      ready = true;
      tracking = false;
      Serial.println("*** RESET ***");
      updateLCD();
      publish("reset");
    }
    if (c == 's' || c == 'S') {
      Serial.print("Entries: "); Serial.println(entries);
      Serial.print("Exits: "); Serial.println(exits);
      Serial.print("In Room: "); Serial.println(inRoom);
    }
  }
  
  readSensor();
  
  if (tracking && millis() - trackStart > CROSSING_TIMEOUT) {
    Serial.println("TIMEOUT");
    evaluate();
    resetTracking();
  }
}

// ============ SENSOR ============
void readSensor() {
  while (RadarSerial.available()) {
    byte b = RadarSerial.read();
    if (bufIdx < 64) buffer[bufIdx++] = b;
    else bufIdx = 0;
    
    if (bufIdx >= 4 &&
        buffer[bufIdx-4] == 0xF8 && buffer[bufIdx-3] == 0xF7 &&
        buffer[bufIdx-2] == 0xF6 && buffer[bufIdx-1] == 0xF5) {
      
      for (int i = 0; i < bufIdx - 8; i++) {
        if (buffer[i] == 0xF4 && buffer[i+1] == 0xF3 &&
            buffer[i+2] == 0xF2 && buffer[i+3] == 0xF1) {
          processFrame(buffer + i, bufIdx - i);
          break;
        }
      }
      bufIdx = 0;
    }
  }
}

void processFrame(byte* frame, int len) {
  if (len < 17 || frame[6] != 0x02 || frame[7] != 0xAA) return;
  
  byte status = frame[8];
  int moveDist = frame[9] | (frame[10] << 8);
  int staticDist = frame[12] | (frame[13] << 8);
  
  // Get distance (prefer movement distance if available)
  int dist = 0;
  String statusStr = "";
  
  if (status == 0x00) {
    statusStr = "NONE";
    dist = 0;
  } else if (status == 0x01) {
    statusStr = "MOVE";
    dist = moveDist;
  } else if (status == 0x02) {
    statusStr = "STATIC";
    dist = staticDist;
  } else if (status == 0x03) {
    statusStr = "BOTH";
    dist = moveDist > 0 ? moveDist : staticDist;
  }
  
  // Determine zone
  String zone = "MID";
  if (dist > 0 && dist < CLOSE_ZONE) zone = "CLOSE";
  else if (dist > FAR_ZONE) zone = "FAR";
  
  // Detect REAL movement by distance change (ignore sensor status)
  bool realMovement = false;
  if (prevDist > 0 && dist > 0) {
    int change = abs(dist - prevDist);
    if (change > DISTANCE_CHANGE_THRESHOLD) {
      realMovement = true;
      stableCount = 0;
    } else {
      stableCount++;
    }
  }
  
  // Print status every 500ms
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 500) {
    lastPrint = millis();
    Serial.print(statusStr);
    Serial.print(" ");
    Serial.print(dist);
    Serial.print("cm [");
    Serial.print(zone);
    Serial.print("] ");
    
    if (tracking) {
      Serial.print("TRACKING chg:");
      Serial.print(distanceChanges);
    } else {
      Serial.print(ready ? "READY" : "WAIT");
    }
    
    Serial.print(" | In:");
    Serial.print(entries);
    Serial.print(" Out:");
    Serial.print(exits);
    Serial.print(" Room:");
    Serial.println(inRoom);
  }
  
  // === NO TARGET ===
  if (status == 0x00) {
    if (lastNone == 0) lastNone = millis();
    
    if (!ready && millis() - lastNone > READY_DELAY) {
      ready = true;
      Serial.println(">> READY");
    }
    
    if (tracking) {
      Serial.println(">> TARGET LOST");
      evaluate();
      resetTracking();
    }
    
    prevDist = 0;
    return;
  }
  
  // === TARGET PRESENT ===
  lastNone = 0;
  if (dist < 10) return;
  
  // Start tracking when we see significant distance change
  if (!tracking && ready && prevDist > 0) {
    int change = abs(dist - prevDist);
    if (change > DISTANCE_CHANGE_THRESHOLD) {
      tracking = true;
      ready = false;
      trackStart = millis();
      startDist = prevDist;  // Use previous as start point
      minDist = min(prevDist, dist);
      maxDist = max(prevDist, dist);
      sawClose = (dist < CLOSE_ZONE) || (prevDist < CLOSE_ZONE);
      sawFar = (dist > FAR_ZONE) || (prevDist > FAR_ZONE);
      distanceChanges = 1;
      stableCount = 0;
      Serial.print(">> TRACKING: ");
      Serial.print(prevDist);
      Serial.print(" -> ");
      Serial.print(dist);
      Serial.println("cm");
    }
  }
  
  // Update tracking
  if (tracking) {
    if (realMovement) {
      distanceChanges++;
    }
    
    if (dist < minDist) minDist = dist;
    if (dist > maxDist) maxDist = dist;
    if (dist < CLOSE_ZONE) sawClose = true;
    if (dist > FAR_ZONE) sawFar = true;
    lastDist = dist;
    
    // Cancel if person stopped moving (many stable readings)
    if (stableCount > STABLE_READINGS_TO_STOP) {
      Serial.println(">> STOPPED MOVING");
      evaluate();
      resetTracking();
    }
  }
  
  prevDist = dist;
}

// ============ EVALUATE ============
void evaluate() {
  int travel = maxDist - minDist;
  
  Serial.println("--- EVALUATION ---");
  Serial.print("Start: "); Serial.println(startDist);
  Serial.print("End: "); Serial.println(lastDist);
  Serial.print("Min: "); Serial.println(minDist);
  Serial.print("Max: "); Serial.println(maxDist);
  Serial.print("Travel: "); Serial.println(travel);
  Serial.print("Distance changes: "); Serial.println(distanceChanges);
  Serial.print("Saw CLOSE: "); Serial.println(sawClose ? "YES" : "NO");
  Serial.print("Saw FAR: "); Serial.println(sawFar ? "YES" : "NO");
  
  // Must have multiple distance changes (real walking)
  if (distanceChanges < 2) {
    Serial.println("IGNORED: Not enough distance changes");
    return;
  }
  
  // Must travel enough
  if (travel < MIN_TRAVEL) {
    Serial.println("IGNORED: Not enough travel");
    return;
  }
  
  // Must cross both zones
  if (!sawClose || !sawFar) {
    Serial.println("IGNORED: Didn't cross both zones");
    return;
  }
  
  // Cooldown
  if (millis() - lastCount < COOLDOWN) {
    Serial.println("IGNORED: Cooldown");
    return;
  }
  
  // Determine direction by comparing start and end distances
  bool isEntry = (lastDist < startDist);  // Moved closer = entry
  bool isExit = (lastDist > startDist);   // Moved away = exit
  
  if (isEntry) {
    entries++;
    inRoom = entries - exits;
    if (inRoom < 0) inRoom = 0;
    lastCount = millis();
    Serial.println("==> ENTRY!");
    updateLCD();
    publish("entry");
  } else if (isExit) {
    exits++;
    inRoom = entries - exits;
    if (inRoom < 0) inRoom = 0;
    lastCount = millis();
    Serial.println("==> EXIT!");
    updateLCD();
    publish("exit");
  }
}

void resetTracking() {
  tracking = false;
  ready = true;
  minDist = 9999;
  maxDist = 0;
  sawClose = false;
  sawFar = false;
  startDist = 0;
  lastDist = 0;
  lastNone = 0;
  distanceChanges = 0;
  stableCount = 0;
}

// ============ LCD ============
void updateLCD() {
  if (inRoom < 0) inRoom = 0;
  
  char line1[17], line2[17];
  sprintf(line1, "In:%-3d Out:%-3d", entries, exits);
  sprintf(line2, "Room: %-3d", inRoom);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.send_string(line1);
  lcd.setCursor(0, 1);
  lcd.send_string(line2);
  
  if (inRoom > 5) lcd.setRGB(255, 0, 0);
  else if (inRoom > 0) lcd.setRGB(0, 255, 0);
  else lcd.setRGB(0, 100, 255);
}

// ============ WIFI ============
void connectWiFi() {
  WiFiDrv::digitalWrite(26, HIGH);
  Serial.print("WiFi...");
  WiFi.begin(ssid, password);
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 30) {
    delay(500);
    Serial.print(".");
    tries++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("OK");
    WiFiDrv::digitalWrite(25, HIGH);
    WiFiDrv::digitalWrite(26, LOW);
  } else {
    Serial.println("FAILED");
  }
}

// ============ MQTT ============
void connectMQTT() {
  Serial.print("MQTT...");
  String clientId = "Counter-" + String(random(1000));
  if (mqttClient.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
    Serial.println("OK");
  } else {
    Serial.println("FAILED");
  }
}

void publish(String event) {
  String payload = "{\"event\":\"" + event + "\",";
  payload += "\"entries\":" + String(entries) + ",";
  payload += "\"exits\":" + String(exits) + ",";
  payload += "\"occupancy\":" + String(inRoom) + "}";
  
  String topic = mqtt_topic_base + "/count";
  mqttClient.publish(topic.c_str(), payload.c_str());
  Serial.print("MQTT: ");
  Serial.println(payload);
}