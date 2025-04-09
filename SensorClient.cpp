#include <WiFi.h>
#include <WiFiClient.h>


const char* ssid = "UCB-IoT";
const char* password = "sistemasyteleco";
const char* serverIP = "192.168.50.120"; 
const uint16_t serverPort = 8001;     

WiFiClient client;

const int trigPin = 23;
const int echoPin = 22;

class ultrasonicsens {
  private:
    int echoPin; 
    int triggerPin;

  public:
    ultrasonicsens(int echoPin, int triggerPin) {
      this->echoPin = echoPin;
      this->triggerPin = triggerPin;
    }

    long readUltrasonicDistance(int triggerPin, int echoPin) {
      pinMode(triggerPin, OUTPUT);
      digitalWrite(triggerPin, LOW);
      delayMicroseconds(2);
      digitalWrite(triggerPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(triggerPin, LOW);       
      pinMode(echoPin, INPUT);
      return pulseIn(echoPin, HIGH, 20000); 
    }

    long getDistance() {
      return 0.01723 * readUltrasonicDistance(triggerPin, echoPin);  
    }
};

ultrasonicsens sensor(echoPin, trigPin);
int lastSent = -1;  
long distance;

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("¡Conectado!");

  while (!client.connect(serverIP, serverPort)) {
    Serial.println("Intentando conectar al servidor...");
    delay(1000);
  }
  Serial.println("¡Conectado al servidor!");
}

void loop() {
  long distance = sensor.getDistance();
  Serial.print("Distancia: ");
  Serial.println(distance);

  int toSend;
  if (distance > 0 && distance < 10) {
    toSend = 1;
  } else {
    toSend = 0;
  }

  if (toSend != lastSent) {  
    client.print(toSend);
    Serial.print("Enviado: ");
    Serial.println(toSend);
    lastSent = toSend;
  }

  delay(200);  
}
