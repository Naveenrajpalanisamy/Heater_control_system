#include <Arduino.h>
#include "DHT.h"

#define DHTPIN 15
#define DHTTYPE DHT22

#define HEATER_PIN 5
#define LED_PIN    4
#define BUZZER_PIN 2

// Temperature Thresholds
#define LOWER_THRESHOLD 30.0
#define UPPER_THRESHOLD 40.0
#define OVERHEAT_THRESHOLD 45.0

DHT dht(DHTPIN, DHTTYPE);

// State Definitions
enum HeaterState {
  IDLE,
  HEATING,
  STABILIZING,
  TARGET_REACHED,
  OVERHEAT
};

HeaterState currentState = IDLE;

// Mutex for shared state access
SemaphoreHandle_t stateMutex;

// Function Prototypes
void TemperatureReadTask(void *pvParameters);
void HeaterControlTask(void *pvParameters);
void SerialLoggerTask(void *pvParameters);
void VisualIndicatorTask(void *pvParameters);

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(HEATER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  stateMutex = xSemaphoreCreateMutex();

  // FreeRTOS Tasks
  xTaskCreate(TemperatureReadTask, "TempRead", 2048, NULL, 1, NULL);
  xTaskCreate(HeaterControlTask, "HeaterCtrl", 2048, NULL, 1, NULL);
  xTaskCreate(SerialLoggerTask, "SerialLog", 2048, NULL, 1, NULL);
  xTaskCreate(VisualIndicatorTask, "VisualInd", 2048, NULL, 1, NULL);

}

void loop() {
  // FreeRTOS handles tasks
}

void TemperatureReadTask(void *pvParameters) {
  float temp;
  while (1) {
    temp = dht.readTemperature();
    if (isnan(temp)) {
      Serial.print(temp);
      Serial.println(" *C");
    } else {
      xSemaphoreTake(stateMutex, portMAX_DELAY);
      if (temp < LOWER_THRESHOLD) {
        currentState = HEATING;
      } else if (temp >= LOWER_THRESHOLD && temp < UPPER_THRESHOLD) {
        currentState = STABILIZING;
      } else if (temp >= UPPER_THRESHOLD && temp < OVERHEAT_THRESHOLD) {
        currentState = TARGET_REACHED;
      } else if (temp >= OVERHEAT_THRESHOLD) {
        currentState = OVERHEAT;
      }
      xSemaphoreGive(stateMutex);
    }
    vTaskDelay(pdMS_TO_TICKS(500)); // Read every 500ms
  }
}

void HeaterControlTask(void *pvParameters) {
  while (1) {
    xSemaphoreTake(stateMutex, portMAX_DELAY);
    switch (currentState) {
      case HEATING:
        digitalWrite(HEATER_PIN, HIGH);
        break;
      case STABILIZING:
      case TARGET_REACHED:
      case OVERHEAT:
        digitalWrite(HEATER_PIN, LOW);
        break;
      default:
        digitalWrite(HEATER_PIN, LOW);
        break;
    }
    xSemaphoreGive(stateMutex);
    vTaskDelay(pdMS_TO_TICKS(500)); // Check control every 500ms
  }
}

void SerialLoggerTask(void *pvParameters) {
  const char* stateNames[] = {"IDLE", "HEATING", "STABILIZING", "TARGET_REACHED", "OVERHEAT"};
  float currentTemp;
  while (1) {
    xSemaphoreTake(stateMutex, portMAX_DELAY);
    currentTemp = dht.readTemperature();

    Serial.print("Current State: ");
    Serial.print(stateNames[currentState]);
    Serial.print(" | Temperature: ");

    if (isnan(currentTemp)) {
      Serial.println("Initializing");
    } else {
      Serial.print(currentTemp);
      Serial.println(" *C");
    }

    xSemaphoreGive(stateMutex);
    vTaskDelay(pdMS_TO_TICKS(1000)); // Log every 1s
  }
}


void VisualIndicatorTask(void *pvParameters) {
  float currentTemp;
  float excessTemp;
  int toneFreq;

  while (1) {
    xSemaphoreTake(stateMutex, portMAX_DELAY);
    currentTemp = dht.readTemperature();

    if (currentState == OVERHEAT) {
      excessTemp = currentTemp - UPPER_THRESHOLD;
      if (excessTemp < 0) excessTemp = 0;  // no negative excessTemp

      toneFreq = 1000 + (excessTemp * 200);

      // Clamp frequency between 1000 Hz and 5000 Hz
      if (toneFreq < 1000) toneFreq = 1000;
      if (toneFreq > 5000) toneFreq = 5000;
      tone(BUZZER_PIN, toneFreq);
      digitalWrite(LED_PIN, HIGH);
    } else if (currentState == TARGET_REACHED) {
      noTone(BUZZER_PIN); // Turn OFF buzzer
      digitalWrite(LED_PIN, HIGH);
    } else {
      noTone(BUZZER_PIN);  
      digitalWrite(LED_PIN, LOW);
    }

    xSemaphoreGive(stateMutex);
    vTaskDelay(pdMS_TO_TICKS(500)); 
  }
}
