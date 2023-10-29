#include <esp_now.h>
#include <WiFi.h>

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

typedef struct struct_message {
  bool LED_status = false;
} struct_message;

// Create struct_messages
struct_message sentData;
struct_message receivedData;

esp_now_peer_info_t peerInfo;

const int button_pin = 2;
const int LED_pin = 21;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  if(status != ESP_NOW_SEND_SUCCESS){
    Serial.println("Delivery Fail");
  }
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&receivedData, incomingData, sizeof(receivedData));
  Serial.print("LED Status: ");
  Serial.println(receivedData.LED_status);
}

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  pinMode(button_pin,INPUT);
  pinMode(LED_pin,OUTPUT);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}
 
void loop() {
  if (digitalRead(button_pin) == HIGH) {
      delay(1000);
      // Set values to send
      sentData.LED_status = !sentData.LED_status;
      // Send message via ESP-NOW
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &sentData, sizeof(sentData));
      
      if (result != ESP_OK) {
        Serial.println("Error sending the data");
      }
  }
  if (receivedData.LED_status == true){
    digitalWrite(LED_pin,HIGH);
  } else{
    digitalWrite(LED_pin,LOW);
  }
}