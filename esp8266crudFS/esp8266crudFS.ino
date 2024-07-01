#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <LittleFS.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x26,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

// Web server on port 80
ESP8266WebServer server(80);

void setup() {
  lcd.init();  
  //lcd.backlight();
  Serial.begin(115200);
  delay(100);
  lcd.setCursor(0,0);
    lcd.print("wifi: Setup-WiFi");
    lcd.setCursor(0,1);
    lcd.print("pass: esp8266pw");
  // Initialize LittleFS
  if (!LittleFS.begin()) {
    Serial.println("Failed to mount file system");
    return;
  }

  // Set up WiFiManager
  WiFiManager wifiManager;

  // If you want to reset the stored WiFi credentials
  // wifiManager.resetSettings();

  // Automatically connect using saved credentials,
  // if not, it starts an access point with the specified name
  if (!wifiManager.autoConnect("Setup-WiFi","esp8266pw")) {
    Serial.println("Failed to connect and hit timeout");
    delay(3000);
    ESP.restart();
    delay(1000);
  }

  // If you get here you have connected to the WiFi
  Serial.println("Connected to WiFi");

  // Print the IP address
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("ip address:");
  lcd.setCursor(0,1);
  lcd.print(WiFi.localIP());
  // Define server routes
  server.on("/", HTTP_GET, []() {
    File file = LittleFS.open("/index.html", "r");
    if (!file) {
      server.send(500, "text/plain", "File not found");
      return;
    }
    server.streamFile(file, "text/html");
    file.close();
  });

  server.on("/style.css", HTTP_GET, []() {
    File file = LittleFS.open("/style.css", "r");
    if (!file) {
      server.send(500, "text/plain", "File not found");
      return;
    }
    server.streamFile(file, "text/css");
    file.close();
  });

  server.on("/script.js", HTTP_GET, []() {
    File file = LittleFS.open("/script.js", "r");
    if (!file) {
      server.send(500, "text/plain", "File not found");
      return;
    }
    server.streamFile(file, "application/javascript");
    file.close();
  });

  server.on("/create", HTTP_POST, handleCreate);
  server.on("/read", HTTP_GET, handleRead);
  server.on("/update", HTTP_POST, handleUpdate);
  server.on("/delete", HTTP_POST, handleDelete);
  server.on("/changeWiFi", HTTP_POST, handleChangeWiFi);

  // Start server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

void handleCreate() {
  if (server.hasArg("name") && server.hasArg("lastname") && server.hasArg("age")) {
    String name = server.arg("name");
    String lastname = server.arg("lastname");
    String age = server.arg("age");

    File file = LittleFS.open("/data.txt", "a");
    if (!file) {
      server.send(500, "text/plain", "File creation failed");
      return;
    }
    file.println(name + "," + lastname + "," + age);
    file.close();
    server.send(200, "text/plain", "Entry created successfully");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("saved");
    delay(1000);
    lcd.clear();
     Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("ip address:");
  lcd.setCursor(0,1);
  lcd.print(WiFi.localIP());
  } else {
    server.send(400, "text/plain", "Bad request");
  }
}

void handleRead() {
  File file = LittleFS.open("/data.txt", "r");
  if (!file) {
    server.send(404, "text/plain", "File not found");
    return;
  }

  String content;
  while (file.available()) {
    content += char(file.read());
  }
  file.close();
  server.send(200, "text/plain", content);
}

void handleUpdate() {
  if (server.hasArg("oldName") && server.hasArg("oldLastname") && server.hasArg("oldAge") &&
      server.hasArg("newName") && server.hasArg("newLastname") && server.hasArg("newAge")) {
    String oldName = server.arg("oldName");
    String oldLastname = server.arg("oldLastname");
    String oldAge = server.arg("oldAge");
    String newName = server.arg("newName");
    String newLastname = server.arg("newLastname");
    String newAge = server.arg("newAge");

    File file = LittleFS.open("/data.txt", "r");
    if (!file) {
      server.send(404, "text/plain", "File not found");
      return;
    }

    String fileContent;
    while (file.available()) {
      fileContent += char(file.read());
    }
    file.close();

    String oldEntry = oldName + "," + oldLastname + "," + oldAge;
    String newEntry = newName + "," + newLastname + "," + newAge;
    int pos = fileContent.indexOf(oldEntry);
    if (pos == -1) {
      server.send(404, "text/plain", "Entry not found");
      return;
    }

    fileContent.replace(oldEntry, newEntry);

    file = LittleFS.open("/data.txt", "w");
    if (!file) {
      server.send(500, "text/plain", "File update failed");
      return;
    }
    file.print(fileContent);
    file.close();
    server.send(200, "text/plain", "Entry updated successfully");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("update success");
    delay(1000);
    lcd.clear();
     Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("ip address:");
  lcd.setCursor(0,1);
  lcd.print(WiFi.localIP());
  } else {
    server.send(400, "text/plain", "Bad request");
  }
}

void handleDelete() {
  if (server.hasArg("name") && server.hasArg("lastname") && server.hasArg("age")) {
    String name = server.arg("name");
    String lastname = server.arg("lastname");
    String age = server.arg("age");

    File file = LittleFS.open("/data.txt", "r");
    if (!file) {
      server.send(404, "text/plain", "File not found");
      return;
    }

    String fileContent;
    while (file.available()) {
      fileContent += char(file.read());
    }
    file.close();

    String entry = name + "," + lastname + "," + age;
    int pos = fileContent.indexOf(entry);
    if (pos == -1) {
      server.send(404, "text/plain", "Entry not found");
      return;
    }

    fileContent.remove(pos, entry.length() + 1); // +1 to remove the newline

    file = LittleFS.open("/data.txt", "w");
    if (!file) {
      server.send(500, "text/plain", "File deletion failed");
      return;
    }
    file.print(fileContent);
    file.close();
    server.send(200, "text/plain", "Entry deleted successfully");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("delete success");
    delay(1000);
    lcd.clear();
     Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("ip address:");
  lcd.setCursor(0,1);
  lcd.print(WiFi.localIP());
  } else {
    server.send(400, "text/plain", "Bad request");
  }
}

void handleChangeWiFi() {
  if (server.hasArg("ssid") && server.hasArg("password")) {
    String ssid = server.arg("ssid");
    String password = server.arg("password");

    WiFi.begin(ssid.c_str(), password.c_str());
    delay(1000);
    ESP.restart();    
  }
}
