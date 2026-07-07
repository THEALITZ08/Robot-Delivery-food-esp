#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include "esp_wifi.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SDA_PIN 21
#define SCL_PIN 22
Adafruit_SSD1306 display(SCREEN_WIDTH,SCREEN_HEIGHT,&Wire, -1);

uint8_t alamat[] = {0x88,0X57,0X21,0X23,0X47,0X64};

typedef struct message {
  char pesan[32];
  int rssi;
} message;
message dataTerima;
message dataPin;
message fsmMessage;

volatile bool pin = false;
volatile unsigned long lastPackageTime =0;
volatile const long intervalPackage = 1000;
volatile const long intervalRecovery = 500;
int sensorErrorCount = 0;
unsigned long lastLog = 0;
const long intervalLog = 500;

#define IN1 12
#define IN2 13
#define IN3 14
#define IN4 33
#define ENA 26
#define ENB 15
#define TRIG_PIN 16
#define ECHO_PIN 17
float distance;
float duration;
float kp = 3.72;
float ki = 2.58;
float kd = 1.67;
volatile float jarak;
volatile int globalRssi = -95;
bool dataBaru = false;
QueueHandle_t dataQueue;

int calculate(float distanceNow, float targetSetpoint, int min, int max) {
  static unsigned long lastPid = 0;
  static float integral = 0;
  static float previous = 0;

  unsigned long current = millis();
  float dt = (current - lastPid) / 1000.0;
  if(dt <= 0.0) return 0;

  float error = distanceNow - targetSetpoint;

  if(error <= 0) {
    integral = 0;
    previous = 0;
    lastPid = current;
    return 0;
  }

  integral += (error * dt);
  float derivative = (error - previous) / dt;
  float output = (kp * error) + (ki * integral) + (kd * derivative);

  previous = error;
  lastPid = current;

  int pwmFinal = (int)output;
  if(pwmFinal > max) pwmFinal = max;
  if(pwmFinal < min && max > 0) pwmFinal = min;

  return pwmFinal;

}
void maju( int pwmA, int pwmB) {
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);
  analogWrite(ENA,pwmA);
  analogWrite(ENB,pwmB);
}

void belokKiri( int pwmA, int pwmB) {
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);
  analogWrite(ENA,pwmA);
  analogWrite(ENB,pwmB);

}
void belokKanan( int pwmA, int pwmB) {
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
  analogWrite(ENA,pwmA);
  analogWrite(ENB,pwmB);
}
void stop( int pwmA, int pwmB) {
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,LOW);
  analogWrite(ENA,pwmA);
  analogWrite(ENB,pwmB);
}


void mukaNormal() {
  display.clearDisplay();

  display.fillCircle(40,25,8,WHITE);
  display.fillCircle(88,25,8,WHITE);

  display.drawLine(58,48,70,48,WHITE);
  display.drawLine(56,46,58,48,WHITE);
  display.drawLine(70,48,72,46,WHITE);

  display.display();
}

void mukaSenyum() {
  display.clearDisplay();

  display.drawLine(32,25,48,25,WHITE);
  display.drawLine(80,25,96,25,WHITE);

  display.drawLine(58,48,70,48,WHITE);
  display.drawLine(56,46,58,48,WHITE);
  display.drawLine(70,48,72,46,WHITE);

  display.display();
}

void mukaMarah() {
  display.clearDisplay();

  display.drawLine(28,15,48,22,WHITE);
  display.drawLine(80,22,100,15,WHITE);

  display.drawLine(32,25,48,25,WHITE);
  display.drawLine(80,25,96,25,WHITE);

  display.drawLine(56,50,72,50,WHITE);

  display.display();
}

void mukaNangis() {
  display.clearDisplay();

  display.drawCircle(40,25,10,WHITE);
  display.fillCircle(40,25,3,WHITE);

  display.drawCircle(88,25,10,WHITE);
  display.fillCircle(88,25,3,WHITE);

  display.drawLine(40,35,40,45,WHITE);
  display.drawLine(88,35,88,45,WHITE);

  display.drawLine(56,50,64,46,WHITE);
  display.drawLine(64,46,72,50,WHITE);

  display.display();
}
void mukaDatar() {
  display.clearDisplay();

  display.fillCircle(40,25,4,WHITE);
  display.fillCircle(88,25,4,WHITE);

  display.drawLine(34,16,46,18,WHITE);
  display.drawLine(82,18,94,16,WHITE);

  display.drawLine(50,45,78,45,WHITE);

  display.display();
}
void mukaCemberut() {
  display.clearDisplay();

  display.drawCircle(40,25,10,WHITE);
  display.fillCircle(40,25,3,WHITE);

  display.drawCircle(88,25,10,WHITE);
  display.fillCircle(88,25,3,WHITE);

  display.drawLine(56,48,64,44,WHITE);
  display.drawLine(64,44,72,48,WHITE);

  display.display();
}
void mukaTulus() {
  display.clearDisplay();

  display.drawLine(34,24,40,20,WHITE);
  display.drawLine(40,20,46,24,WHITE);

  display.drawLine(82,24,88,20,WHITE);
  display.drawLine(88,20,94,24,WHITE);

  display.drawCircle(64,42,8, WHITE);
  display.fillRect(56,34,16,8,BLACK);

  display.display();
}
void mukaKaget() {
  display.clearDisplay();

  display.drawCircle(40,25,10,WHITE);
  display.drawCircle(88,25,10,WHITE);

  display.drawCircle(64,50,5,WHITE);

  display.display();
}

void dataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("status pesan: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "TERKIRIM": "GAGAL");
}

void datarecv(const esp_now_recv_info*info, const uint8_t *incomingData, int len ) {
  message dataTerima;
  Serial.println("ADA PAKET");
  memcpy(&dataTerima, incomingData, sizeof(dataTerima));
    lastPackageTime = millis();
    globalRssi = info->rx_ctrl->rssi;
    dataTerima.rssi = globalRssi;
  
  if(strlen(dataTerima.pesan) > 0) {
    BaseType_t xHigherPriorityTaskToken = pdFALSE;
    xQueueSendFromISR(
    dataQueue,
    &dataTerima,
    &xHigherPriorityTaskToken
  );
  if(xHigherPriorityTaskToken == pdTRUE) {
    portYIELD_FROM_ISR();
  }
    }
       }

  
enum sistemIntegrasi {
  IDLE,
  JALAN,
  HALANGAN,
  SAMPAI,
  SELESAI,
  ERROR
};
sistemIntegrasi currentState = IDLE;

enum safeHandling {
  NONE,
  ERROR_SENSOR,
  ERROR_COMMUNICATION
};
safeHandling errorType =NONE;

bool bacaJarak(float &jarak)  {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN,LOW);

  duration = pulseIn(ECHO_PIN, HIGH,30000);
  if(duration == 0) return false;
  distance = duration * 0.034/2;

  if(distance < 2 || distance > 400) return false;

  return true;
  
}

void ultrasonikTask(void *pvParameters) {
  float hasil;
  while(true) {
    if(bacaJarak(hasil)) {
      jarak = hasil;
      sensorErrorCount = 0;
    } else {
      sensorErrorCount++;
    }

    vTaskDelay(pdMS_TO_TICKS(60));

  }
}

void monitoringTask(void* pvParameters) {
  while(true) {
      Serial.print("state saat ini: ");
      Serial.print(currentState);
      Serial.print(" | state error Sekarang: ");
      Serial.print(errorType);
      Serial.print(" | jarak: ");
      Serial.print(jarak);
      Serial.print(" cm | Rssi: ");
      Serial.println(globalRssi);
      vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void handleThreshold() {
  if(globalRssi > -45 && globalRssi < 0) {
        Serial.println("Paket anda sudah sampai!!!");
        currentState = SAMPAI;
      } else if(jarak < 20 && jarak > 0) {
        Serial.println("ada halangan, MENGHINDAR!!!");
        currentState = HALANGAN;
      }
}

void handleCondition() {
if(jarak < 20 && globalRssi > -40 && globalRssi < 0) {
        currentState = SAMPAI;
      }
      else if(jarak >=20 && globalRssi < -40) {
        Serial.println("tidak ada halangan,amann!!");
        currentState = JALAN;
      }

}

void validationPin() {
  if(!pin) {
  strcpy(dataPin.pesan, "SAMPAI");
  esp_now_send(alamat,(uint8_t*)&dataPin, sizeof(dataPin));
  pin = true;
  }
  if(dataBaru && strcmp(fsmMessage.pesan, "BUKA") == 0) {
    currentState = SELESAI;
    dataBaru = false;
  }


}
void fsmTask(void *pvParameters) {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
  pinMode(ENA,OUTPUT);
  pinMode(ENB,OUTPUT);
  int basePwm;
  int pwm2;

  message dataSementara;
  
  while(true) {
    if (xQueueReceive(dataQueue, &fsmMessage,0) == pdTRUE) {
       dataBaru = true;
    }
    
  
    switch(currentState) {
      case IDLE:
      stop(0,0);
      
      mukaDatar();
      if(dataBaru && strcmp(fsmMessage.pesan, "MULAI") == 0) {
        currentState = JALAN;
        dataBaru = false;
      
      }
      break;

      case JALAN:
      mukaSenyum();
      basePwm = calculate(jarak, 20.0,80,165);
      pwm2 = basePwm + 30;
      if(pwm2 > 255) pwm2 = 255;
      maju(basePwm, pwm2);
      handleThreshold();
      if(sensorErrorCount >= 5) {
        errorType = ERROR_SENSOR;
        currentState = ERROR;
      }

      if(millis() - lastPackageTime > intervalPackage) {
        errorType = ERROR_COMMUNICATION;
        currentState = ERROR;
      }
      break;

      case HALANGAN:
      belokKiri(165, 195);
      mukaKaget();
      handleCondition();
      break;

      case SAMPAI:
      stop(0,0);
      mukaTulus();
      validationPin();
      break;
      case SELESAI:

      if(strcmp(fsmMessage.pesan, "PULANG") == 0) {
      mukaDatar();
      stop(0,0);
      vTaskDelay(pdMS_TO_TICKS(1000));
      belokKiri(165,195);
      vTaskDelay(pdMS_TO_TICKS(800));
      maju(basePwm,pwm2);
      vTaskDelay(pdMS_TO_TICKS(1000));
      stop(0,0);
      pin = false;

      

        currentState = IDLE;
      
      }
      break;
      case ERROR:
      stop(0,0);
      mukaNangis();
      switch(errorType) {
        case NONE:
        currentState = JALAN;
        break;

        case ERROR_SENSOR:
        if(sensorErrorCount == 0) {
          errorType = NONE;
          currentState = JALAN;
        }
        break;
        case ERROR_COMMUNICATION:
        if(millis() - lastPackageTime < intervalRecovery) {
          errorType = NONE;
          currentState = JALAN;
        }
        break;

      }
      break;
    }

    dataBaru = false;
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN,SCL_PIN);
  dataQueue = xQueueCreate(15, sizeof(message));
  WiFi.mode(WIFI_STA);
  esp_wifi_set_channel(13, WIFI_SECOND_CHAN_NONE);
  if(esp_now_init() != ESP_OK) {
    Serial.println("gagal init");
    return;
  }
  esp_now_register_send_cb((esp_now_send_cb_t)dataSent);
  esp_now_register_recv_cb((esp_now_recv_cb_t)datarecv);

   esp_now_peer_info_t teman;
  memcpy(teman.peer_addr,alamat,6);
  teman.channel = 13;
  teman.encrypt = false;
  teman.ifidx = WIFI_IF_STA;

  if(esp_now_add_peer(&teman) != ESP_OK) {
    Serial.println("gagal menambahkan peer");
  }
if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
  while(1);
}
display.clearDisplay();
display.display();

  xTaskCreatePinnedToCore(
    fsmTask,
    "fsm",
    7000,
    NULL,
    1,
    NULL,
    1

  );
  xTaskCreatePinnedToCore(
    ultrasonikTask,
    "ultrasonik",
    7000,
    NULL,
    1,
    NULL,
    1
  );

  xTaskCreatePinnedToCore(
    monitoringTask,
    "monitoring",
    3000,
    NULL,
    1,
    NULL,
    1
  );
}

void loop() {
  // put your main code here, to run repeatedly:

}
