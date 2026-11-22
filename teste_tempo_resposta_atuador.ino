#define RELE_PIN 26
#define MONITOR_PIN 27

#define RELE_LIGADO HIGH
#define RELE_DESLIGADO LOW

#define LIMIAR_ADC 2000

unsigned long tempos[4];

void setup() {
  Serial.begin(115200);

  pinMode(RELE_PIN, OUTPUT);
  pinMode(MONITOR_PIN, INPUT);

  digitalWrite(RELE_PIN, RELE_DESLIGADO);

  delay(3000);

  Serial.println();
  Serial.println("====================================");
  Serial.println(" TESTE DE TEMPO DE RESPOSTA ATUADOR");
  Serial.println("====================================");

  for (int teste = 0; teste < 4; teste++) {

    digitalWrite(RELE_PIN, RELE_DESLIGADO);
    delay(3000);

    int adcInicial = analogRead(MONITOR_PIN);

    Serial.println();
    Serial.print("TESTE ");
    Serial.println(teste + 1);

    Serial.print("ADC inicial = ");
    Serial.println(adcInicial);

    if (adcInicial > LIMIAR_ADC) {
      Serial.println("ERRO: atuador ainda energizado.");
      return;
    }

    Serial.println("Enviando comando para ligar...");

    unsigned long inicio = micros();

    digitalWrite(RELE_PIN, RELE_LIGADO);

    while (analogRead(MONITOR_PIN) < LIMIAR_ADC) {
      // aguardando energizacao
    }

    unsigned long fim = micros();

    tempos[teste] = fim - inicio;

    Serial.print("Tempo de resposta = ");
    Serial.print(tempos[teste]);
    Serial.println(" us");

    Serial.println("Cooler ligado.");

    delay(3000);

    digitalWrite(RELE_PIN, RELE_DESLIGADO);

    Serial.println("Cooler desligado.");

    delay(3000);
  }

  unsigned long soma = 0;

  for (int i = 0; i < 4; i++) {
    soma += tempos[i];
  }

  float media = soma / 4.0;

  Serial.println();
  Serial.println("====================================");
  Serial.println(" RESULTADOS");
  Serial.println("====================================");

  for (int i = 0; i < 4; i++) {
    Serial.print("Teste ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(tempos[i]);
    Serial.println(" us");
  }

  Serial.print("MEDIA: ");
  Serial.print(media);
  Serial.println(" us");

  Serial.println("====================================");
}

void loop() {
}
