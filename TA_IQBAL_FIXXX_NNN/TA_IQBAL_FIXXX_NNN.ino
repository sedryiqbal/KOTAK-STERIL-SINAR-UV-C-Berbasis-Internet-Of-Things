#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
#include <UniversalTelegramBot.h>   // Universal Telegram Bot Library written by Brian Lough: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <ArduinoJson.h>
#include "SoftwareSerial.h"
SoftwareSerial serial(D7, A0);
String data;
char c;


// Replace with your network credentials
const char* ssid = "12345";
const char* pass = "namaakuu";

// Initialize Telegram BOT
#define BOTtoken "5572376712:AAEJs55miW52bTni-LPzfhj0u9-crer3b6w"  // your Bot Token (Get from Botfather)

// Use @myidbot to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
#define CHAT_ID "1307893988"

#ifdef ESP8266
X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Checks for new messages every 1 second.
int botRequestDelay = 10;
unsigned long lastTimeBotRan;


const int ledPin =  D5;  // the number of the LED pin
const int kunci =  D6;  // the number of the LED pin
const int buzer = D8;
int UV = 0;

int minute = 0;
int m, s = 0;


void setup() {
  Serial.begin(9600);
  serial.begin(9600);
  lcd.init();
  lcd.backlight();
  pinMode(ledPin, OUTPUT);
  pinMode(kunci, OUTPUT);
  pinMode(buzer, OUTPUT);
  digitalWrite(ledPin, HIGH);
  digitalWrite(kunci, LOW);
  digitalWrite(buzer, LOW);
#ifdef ESP8266
  configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
#endif

  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
#ifdef ESP32
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
#endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
    lcd.setCursor(1, 0);
    lcd.print("Connect WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("WELCOME");
  lcd.setCursor(1, 1);
  lcd.print("Kotak Steril");
  delay(4000);
  lcd.clear();
}


void terima() {
  //put your main code here, to run repeatedly:
  while (serial.available() > 0) {
    delay(10);
    c = serial.read();
    data += c;
  }
  if (data.length() > 0) {
    lcd.setCursor(0, 1);
    lcd.print("Sinar UV : ");
    lcd.print(data);
    Serial.println(data);
    data = "";

  }
}

void loop() {
  getnewmessage();
  pengaturanUV();
  lcd.setCursor(0, 0);
  lcd.print("Silakan Masukan");
  lcd.setCursor(0, 1);
  lcd.print("Untuk Sterilisasi");
}


void getnewmessage() {
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      //      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}

// Handle what happens when you receive new messages
void handleNewMessages(int numNewMessages) {
  //  Serial.println("handleNewMessages");
  //  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {

    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID) {
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    // Print the received message
    String text = bot.messages[i].text;
    //    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Welcome, " + from_name + ",\n";
      welcome += "KOTAK STERIL UV-C 2022 \n\n";
      welcome += "Jurusan Teknik Komputer \n";
      welcome += "Prodi D3-Teknik Komputer \n";
      welcome += "Politeknik Negeri Sriwijaya \n";
      bot.sendMessage(chat_id, welcome, "");
    }

    else if (text == "/ON") {
      UV = 1;
      bot.sendMessage(chat_id, "Sistem MULAI!", "");
    }

    else if (text == "/OFF") {
      UV = 0;
      bot.sendMessage(chat_id, "Sistem BERHENTI!", "");
    }
  }
}


void pengaturanUV() {
  //  terima();
  if (UV == 1) {
    lcd.clear();
    m = 5;
    s = 0;
    lcd.setCursor(0, 0);
    lcd.print("Mulai Proses Sterilisasi");
    delay(500);

    for (m; m >= 0; m--) {
      for (s; s >= 0; s--) {
        digitalWrite(ledPin, LOW);
        digitalWrite(kunci, HIGH);
        lcd.clear();
        terima();
        //        lcd.setCursor(0, 0);
        //        lcd.print("Sinar UV : ");
        //        lcd.print(data);
        lcd.setCursor(0, 0);
        lcd.print("Timer : ");
        lcd.print(m);
        lcd.print(" : ");
        lcd.print(s);
        delay(1000);
      }
      s = 59;
    }
    digitalWrite(ledPin, HIGH);
    digitalWrite(kunci, LOW);

timeStop:
    delay(1000);
    digitalWrite(buzer, HIGH);
    //    lcd.clear();
    //    lcd.setCursor(0, 1);
    //    lcd.print("Strilisasi Selesai");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Waktu Selesai");
    terima();
    delay(1200);
    bot.sendMessage(CHAT_ID, "PROSES STERILISASI SELESAI", "");
    delay(1000);
    UV = 0;
  }

  if (UV == 0) {
    digitalWrite(buzer, LOW);
    digitalWrite(ledPin, HIGH);
    digitalWrite(kunci, HIGH);
  }
}
