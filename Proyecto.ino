#include <WiFi.h>
#include <WebServer.h>

// Configuración Wi-Fi
const char* ssid = "IoTB";       // Reemplaza con el nombre de tu red Wi-Fi
const char* password = "inventaronelVAR"; // Reemplaza con la contraseña de tu red Wi-Fi

// Declaración del servidor web
WebServer server(80);

// Pines del sensor y del relé
const int soilSensorPin = 34; // Pin analógico para el sensor de humedad
const int pumpPin = 2;        // Pin digital para el relé de la bomba

int soilHumidity = 0;         // Variable para almacenar la lectura del sensor
bool pumpState = false;       // Estado de la bomba

// Función para manejar la página principal
void handleRoot() {
  soilHumidity = analogRead(soilSensorPin);
  float percentage = map(soilHumidity, 0, 4095, 0, 100); // Convertir el valor a porcentaje

  String html = "<!DOCTYPE html>";
  html += "<html>";
  html += "<head><title>Sensor de Humedad</title></head>";
  html += "<body style='text-align:center; font-family:Arial;'>";
  html += "<h1>Lectura de Humedad de Tierra</h1>";
  html += "<p>Humedad: " + String(percentage) + "%</p>";
  html += "<p>Bomba de Agua: ";
  html += pumpState ? "<span style='color:red;'>Encendida</span>" : "<span style='color:green;'>Apagada</span>";
  html += "</p>";
  html += "</body>";
  html += "</html>";

  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);

  // Configuración del pin del relé
  pinMode(pumpPin, OUTPUT);
  digitalWrite(pumpPin, LOW); // Apagar la bomba inicialmente

  // Conexión Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Conectando a Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando...");
  }
  Serial.println("Conexión establecida");
  Serial.print("IP del ESP32: ");
  Serial.println(WiFi.localIP());

  // Configuración del servidor
  server.on("/", handleRoot); // Página principal
  server.begin();
  Serial.println("Servidor web iniciado");
}

void loop() {
  server.handleClient(); // Manejar solicitudes del servidor

  // Leer la humedad del suelo
  soilHumidity = analogRead(soilSensorPin);
  float percentage = map(soilHumidity, 0, 4095, 0, 100); // Convertir a porcentaje

  // Control de la bomba con relé
  if (percentage < 60 && !pumpState) {
    digitalWrite(pumpPin, HIGH); // Encender la bomba
    pumpState = true;
    Serial.println("Bomba encendida");
  } else if (percentage >= 60 && pumpState) {
    digitalWrite(pumpPin, LOW); // Apagar la bomba
    pumpState = false;
    Serial.println("Bomba apagada");
  }

  delay(1000); // Pausa para evitar lecturas excesivas
}
