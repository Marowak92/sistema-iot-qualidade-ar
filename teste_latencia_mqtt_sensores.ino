#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <time.h>

#define DHT_PIN 4
#define DHT_TYPE DHT22
#define MQ135_PIN 34

const char* ssid = "SEU_WIFI";
const char* password = "SUA_SENHA";

const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

const char* topic_dht =
  "mackenzie/iot/projeto_ar/teste_latencia/dht22";

const char* topic_mq =
  "mackenzie/iot/projeto_ar/teste_latencia/mq135";

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHT_PIN, DHT_TYPE);

void conectarWiFi() {
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void conectarMQTT() {
  while (!client.connected()) {
    String clientId = "ESP32-Latencia-";
    clientId += String(random(0xffff), HEX);

    if (!client.connect(clientId.c_str())) {
      delay(2000);
    }
  }
}

String horarioAtual() {
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) {
    return "sem_horario";
  }

  char buffer[20];

  strftime(
    buffer,
    sizeof(buffer),
    "%H:%M:%S",
    &timeinfo
  );

  return String(buffer);
}

void setup() {
  Serial.begin(115200);

  dht.begin();

  conectarWiFi();

  client.setServer(
    mqtt_server,
    mqtt_port
  );

  configTime(
    -3 * 3600,
    0,
    "pool.ntp.org",
    "time.nist.gov"
  );

  delay(3000);

  Serial.println("TESTE DE LATENCIA SENSOR -> MQTT");
}

void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    conectarWiFi();
  }

  if (!client.connected()) {
    conectarMQTT();
  }

  client.loop();

  // ==========================
  // DHT22
  // ==========================

  for (int i = 1; i <= 4; i++) {

    float temperatura =
      dht.readTemperature();

    float umidade =
      dht.readHumidity();

    unsigned long evento =
      millis();

    String hora =
      horarioAtual();

    String payload =
      "{\"teste\":" +
      String(i) +
      ",\"evento_hora\":\"" +
      hora +
      "\",\"evento_epoch_ms\":" +
      String(evento) +
      ",\"temperatura\":" +
      String(temperatura, 2) +
      ",\"umidade\":" +
      String(umidade, 2) +
      "}";

    Serial.print("DHT22 teste ");
    Serial.print(i);
    Serial.print(" - ");
    Serial.println(hora);

    client.publish(
      topic_dht,
      payload.c_str()
    );

    delay(2500);
  }

  // ==========================
  // MQ-135
  // ==========================

  for (int i = 1; i <= 4; i++) {

    int valorMQ =
      analogRead(MQ135_PIN);

    unsigned long evento =
      millis();

    String hora =
      horarioAtual();

    String payload =
      "{\"teste\":" +
      String(i) +
      ",\"evento_hora\":\"" +
      hora +
      "\",\"evento_epoch_ms\":" +
      String(evento) +
      ",\"mq135\":" +
      String(valorMQ) +
      "}";

    Serial.print("MQ-135 teste ");
    Serial.print(i);
    Serial.print(" - ");
    Serial.println(hora);

    client.publish(
      topic_mq,
      payload.c_str()
    );

    delay(2000);
  }

  Serial.println("FIM DOS TESTES");

  while (true) {
    client.loop();
    delay(1000);
  }
}
