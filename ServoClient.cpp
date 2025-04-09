#include <WiFi.h>
#include <ESP32Servo.h>

class MG90S {
private:
  Servo servo;
  int pin;

  const int openSpeed = 1200;
  const int closeSpeed = 1800;
  const int neutral = 1500;
  const int time90 = 300;

public:
  MG90S(int servoPin) : pin(servoPin) {}

  void begin() {
    servo.setPeriodHertz(50);
    servo.attach(pin, 1000, 2000);
    stop();
  }

  void open() {
    Serial.println(" Servo: OPEN");
    servo.writeMicroseconds(openSpeed);
    delay(time90);
    stop();
  }

  void close() {
    Serial.println(" Servo: CLOSE");
    servo.writeMicroseconds(closeSpeed);
    delay(time90);
    stop();
  }

  void stop() {
    servo.writeMicroseconds(neutral);
  }
};

const char* ssid = "UCB-IoT";
const char* password = "sistemasyteleco";
const char* host = "192.168.50.120";  
const int port = 8002;               

WiFiClient client;
MG90S myServo(13); 

void connectToWiFi() {
  Serial.print("Conectando a WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("  Conectado a WiFi");
}

void setup() {
  Serial.begin(115200);
  myServo.begin();
  connectToWiFi();
}

void loop() {
  if (!client.connected()) {
    Serial.print(" Conectando al servidor...");
    if (!client.connect(host, port)) {
      Serial.println(" Fallo de conexión.");
      delay(2000);
      return;
    }
    Serial.println(" Conectado al servidor.");
  }

  if (client.available()) {
    String received = client.readStringUntil('\n');
    received.trim();
    Serial.print(" Recibido del servidor: ");
    Serial.println(received);

    if (received == "0") {
      myServo.close();
    } else if (received == "1") {
      myServo.open();
      delay(10); 
      client.print("1\n"); 
      Serial.println(" Enviado: 1");
    }
  }

  delay(100);
}
