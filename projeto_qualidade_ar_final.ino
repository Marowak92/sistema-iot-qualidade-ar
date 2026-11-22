#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// =====================================================
// CONFIGURAÇÃO DOS PINOS
// =====================================================

#define DHT_PIN 4
#define DHT_TYPE DHT22

#define MQ135_PIN 34
#define RELE_PIN 26

#define RELE_LIGADO HIGH
#define RELE_DESLIGADO LOW

// =====================================================
// LIMITES DO MQ-135
// =====================================================

// Histerese:
// >= 45 -> liga ventilador
// <= 35 -> desliga ventilador

const int LIMITE_LIGAR = 45;
const int LIMITE_DESLIGAR = 35;

// =====================================================
// WI-FI
// =====================================================

// Preencha com os dados da rede utilizada.
// Não publique sua senha real no GitHub.

const char* ssid = "SEU_WIFI";
const char* password = "SUA_SENHA";

// =====================================================
// MQTT
// =====================================================

const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

const char* topic_temperatura =
  "mackenzie/iot/projeto_ar/temperatura";

const char* topic_umidade =
  "mackenzie/iot/projeto_ar/umidade";

const char* topic_mq135 =
  "mackenzie/iot/projeto_ar/mq135";

const char* topic_ventilador =
  "mackenzie/iot/projeto_ar/ventilador";

// =====================================================
// OBJETOS
// =====================================================

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHT_PIN, DHT_TYPE);

// Estado do ventilador
bool ventiladorLigado = false;

// =====================================================
// CONEXÃO WI-FI
// =====================================================

void conectarWiFi() {

  Serial.print("Conectando ao Wi-Fi");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi conectado.");

  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

// =====================================================
// CONEXÃO MQTT
// =====================================================

void conectarMQTT() {

  while (!client.connected()) {

    Serial.print("Conectando ao broker MQTT...");

    String clientId = "ESP32-QualidadeAr-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {

      Serial.println(" conectado.");

    } else {

      Serial.print(" falhou. Estado = ");
      Serial.println(client.state());

      delay(2000);
    }
  }
}

// =====================================================
// CONTROLE DO VENTILADOR
// =====================================================

void controlarVentilador(int valorMQ135) {

  // Liga quando o valor atingir ou ultrapassar 45
  if (!ventiladorLigado && valorMQ135 >= LIMITE_LIGAR) {

    digitalWrite(RELE_PIN, RELE_LIGADO);

    ventiladorLigado = true;

    Serial.println(">>> VENTILADOR LIGADO");

    client.publish(
      topic_ventilador,
      "LIGADO"
    );
  }

  // Desliga quando o valor cair para 35 ou menos
  else if (ventiladorLigado &&
           valorMQ135 <= LIMITE_DESLIGAR) {

    digitalWrite(RELE_PIN, RELE_DESLIGADO);

    ventiladorLigado = false;

    Serial.println(">>> VENTILADOR DESLIGADO");

    client.publish(
      topic_ventilador,
      "DESLIGADO"
    );
  }
}

// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(115200);

  delay(1000);

  Serial.println();
  Serial.println("====================================");
  Serial.println(" SISTEMA IoT - QUALIDADE DO AR");
  Serial.println("====================================");

  pinMode(RELE_PIN, OUTPUT);

  digitalWrite(
    RELE_PIN,
    RELE_DESLIGADO
  );

  dht.begin();

  conectarWiFi();

  client.setServer(
    mqtt_server,
    mqtt_port
  );

  Serial.println();
  Serial.println("Sistema iniciado.");
}

// =====================================================
// LOOP
// =====================================================

void loop() {

  // Garante conexão Wi-Fi
  if (WiFi.status() != WL_CONNECTED) {
    conectarWiFi();
  }

  // Garante conexão MQTT
  if (!client.connected()) {
    conectarMQTT();
  }

  client.loop();

  // ---------------------------------------------------
  // LEITURA DO DHT22
  // ---------------------------------------------------

  float temperatura =
    dht.readTemperature();

  float umidade =
    dht.readHumidity();

  // ---------------------------------------------------
  // LEITURA DO MQ-135
  // ---------------------------------------------------

  int valorMQ135 =
    analogRead(MQ135_PIN);

  // ---------------------------------------------------
  // SERIAL MONITOR
  // ---------------------------------------------------

  Serial.println();
  Serial.println("-------------------------------");

  if (!isnan(temperatura) &&
      !isnan(umidade)) {

    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.println(" °C");

    Serial.print("Umidade: ");
    Serial.print(umidade);
    Serial.println(" %");

  } else {

    Serial.println(
      "Falha na leitura do DHT22."
    );
  }

  Serial.print("MQ-135 ADC: ");
  Serial.println(valorMQ135);

  // ---------------------------------------------------
  // CONTROLE AUTOMÁTICO
  // ---------------------------------------------------

  controlarVentilador(
    valorMQ135
  );

  Serial.print("Ventilador: ");

  if (ventiladorLigado) {
    Serial.println("LIGADO");
  } else {
    Serial.println("DESLIGADO");
  }

  // ---------------------------------------------------
  // PUBLICAÇÃO MQTT
  // ---------------------------------------------------

  char mensagem[20];

  if (!isnan(temperatura)) {

    dtostrf(
      temperatura,
      1,
      2,
      mensagem
    );

    client.publish(
      topic_temperatura,
      mensagem
    );
  }

  if (!isnan(umidade)) {

    dtostrf(
      umidade,
      1,
      2,
      mensagem
    );

    client.publish(
      topic_umidade,
      mensagem
    );
  }

  snprintf(
    mensagem,
    sizeof(mensagem),
    "%d",
    valorMQ135
  );

  client.publish(
    topic_mq135,
    mensagem
  );

  if (ventiladorLigado) {

    client.publish(
      topic_ventilador,
      "LIGADO"
    );

  } else {

    client.publish(
      topic_ventilador,
      "DESLIGADO"
    );
  }

  delay(2000);
}
