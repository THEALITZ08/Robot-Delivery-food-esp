#include <esp_now.h>
#include <WiFiClientSecure.h>
#include <WiFi.h>
#include "esp_wifi.h"
#include <AsyncTelegram2.h>
const char* BOT_TOKEN = "YOU_BOT_TOKEN";
const int64_t CHAT_ID = YOU_CHAT_ID;
const char* ssid = "YOU_WIFI";
const char* password = "YOU_PASSWORD";

#define segA 12
#define segB 13
#define segC 14
#define BUZZER 16
bool realTime;
bool activePin;
volatile bool kondisi;
volatile bool robotSampai;
uint16_t currentPin;
unsigned long lastRing = 0;


WiFiClientSecure client;
AsyncTelegram2 bot(client);

QueueHandle_t sendQueue;

uint8_t alamatTujuan[] = {0x68,0X09,0X47,0X2C,0X1E,0X08};

typedef struct hehe {
  char pesan[32];
  int rssi;
} hehe;
hehe dataKirim;
hehe dataTelegram;
hehe dataPin;
void dataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("status pesan: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "TERKIRIM": "GAGAL");
}

void dataRecv(const esp_now_recv_info*info, const uint8_t *incomingData, int len) {
  memcpy(&dataPin, incomingData, sizeof(dataPin));

  if(strcmp(dataPin.pesan, "SAMPAI") == 0) {
    currentPin = random(1000,9999);
    activePin = true;
    robotSampai = true;
    

    
  }
  
}

void cekBuzzer() {
  if(kondisi) {
    ledcWrite(BUZZER, 150);
    lastRing = millis();
    kondisi = false;

  }
  if(lastRing != 0 && millis() - lastRing > 1500) {
    ledcWrite(BUZZER, 0);
  }
}

void espTask(void*pvParameters) {
  hehe dataLokal;
  while(true) {
    if(xQueueReceive(sendQueue, &dataLokal,0) ==pdTRUE){

      esp_now_send(
        alamatTujuan,
        (uint8_t*)&dataLokal,
        sizeof(dataLokal)
      );
    } 
    if(realTime) {
      hehe beacon;
      strcpy(beacon.pesan, "realtime");
      esp_now_send(alamatTujuan, (uint8_t*)&beacon, sizeof(beacon));
    }

    vTaskDelay(pdMS_TO_TICKS(500));
    }
  }


void telegramTask (void*pvParameters) {
  TBMessage message;


  
  while(true) {
     cekBuzzer();

    if(robotSampai) {
      robotSampai = false;
      String pesan = "robot sudah sampai\n.PIN: " + String(currentPin);
      bot.sendTo(CHAT_ID, pesan);
      

      vTaskDelay(pdMS_TO_TICKS(100));

    }

  if(bot.getNewMessage(message)) {

      if(message.chatId != CHAT_ID) {
        Serial.println("Akses ditolak");
        continue;
      }

    
    if(message.text == "HANTAR KE MEJA 7") {
      kondisi = true;
      realTime = true;
      strcpy(dataTelegram.pesan, "MULAI");
      digitalWrite(segA,HIGH);
      digitalWrite(segB,HIGH);
      digitalWrite(segC,HIGH);
      bot.sendMessage(message,  "SIAP!!,KE MEJA 7");
      xQueueSend(sendQueue, &dataTelegram, portMAX_DELAY);
    }
   else if(message.text == "SELESAI") {
    realTime = false;
      strcpy(dataTelegram.pesan,  "PULANG");
      bot.sendMessage(message, "SIAP!!, KEMBALI KE RUMAH!!");
      xQueueSend(sendQueue, &dataTelegram, portMAX_DELAY);
    }
    else if(message.text.startsWith("Ambil: ")) {
      int pin = message.text.substring(7).toInt();

      if(activePin && pin == currentPin) {
        strcpy(dataTelegram.pesan, "BUKA");
        xQueueSend(sendQueue, &dataTelegram,portMAX_DELAY);
        activePin = false;
        bot.sendMessage(message, "PIN benar,pintu terbuka✅");
      } else{
        bot.sendMessage(message, "PIN salah,silahkan coba lagi❌");
      }
    } 

    

    
  }
  vTaskDelay(pdMS_TO_TICKS(50));
}
}



void setup() {
  Serial.begin(115200);
  pinMode(segA,OUTPUT);
  pinMode(segB,OUTPUT);
  pinMode(segC,OUTPUT);
  ledcAttach(BUZZER,500,8);

  sendQueue =  xQueueCreate(15, sizeof(hehe));
  WiFi.begin(ssid,password);
  WiFi.mode(WIFI_STA);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
esp_wifi_set_channel(13,WIFI_SECOND_CHAN_NONE);
  Serial.println();
  Serial.println("WIFI_CONNECT!!!");
  Serial.print("IP: ");
  Serial.print(WiFi.localIP());
  Serial.print(" | Channel: ");
  Serial.println(WiFi.channel());

  client.setInsecure();
  bot.setUpdateTime(800);
  bot.setTelegramToken(BOT_TOKEN);

  if(bot.begin()) {
    Serial.println("bot connected");
  } else{
    Serial.println("bot FAiled");
  }
  Serial.println("Sistem Siap");

  if(esp_now_init() != ESP_OK) {
    Serial.println("gagal init");
    return;
  }

  esp_now_register_send_cb((esp_now_send_cb_t) dataSent);
  esp_now_register_recv_cb((esp_now_recv_cb_t)dataRecv);


  esp_now_peer_info_t teman;
  memcpy(teman.peer_addr,alamatTujuan,6);
  teman.channel =  13;
  teman.encrypt = false;
  teman.ifidx = WIFI_IF_STA;

  if(esp_now_add_peer(&teman) != ESP_OK) {
    Serial.println("gagal menambahkan peer");
  }

  xTaskCreatePinnedToCore(
    telegramTask,
    "telegram",
    10000,
    NULL,
    1,
    NULL,
    0
  );

  xTaskCreatePinnedToCore(
    espTask,
    "hehe",
    10000,
    NULL,
    1,
    NULL,
    1
  );
  

}

void loop() {

  }
