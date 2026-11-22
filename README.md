# Sistema IoT para Monitoramento da Qualidade do Ar

Projeto desenvolvido para a disciplina **Objetos Inteligentes Conectados**, utilizando Internet das Coisas (IoT) para monitoramento da qualidade do ar em ambientes internos e acionamento automático de ventilação.

O sistema utiliza um **ESP32**, sensores **DHT22** e **MQ-135**, um módulo relé e um cooler de 5 V. Os dados coletados são enviados por Wi-Fi utilizando o protocolo **MQTT**.

## Objetivo

Desenvolver um protótipo IoT de baixo custo capaz de:

- monitorar temperatura e umidade do ambiente;
- monitorar variações indicativas da qualidade do ar por meio do MQ-135;
- publicar os dados em uma plataforma MQTT;
- acionar automaticamente um sistema de ventilação;
- desligar o ventilador quando os níveis retornarem a uma faixa adequada.

O projeto está relacionado ao **Objetivo de Desenvolvimento Sustentável 3 – Saúde e Bem-Estar**, especialmente à meta 3.9.

## Componentes utilizados

- NodeMCU ESP-32S V1.1 – 38 pinos
- Sensor DHT22 / AM2302
- Sensor MQ-135
- Módulo relé de 1 canal
- Cooler 5 V
- Fonte externa 5 V / 2 A
- Protoboard
- Resistores de 10 kΩ e 20 kΩ
- Jumpers
- Cabos de conexão

## Conexões principais

| Componente | Conexão ESP32 |
|---|---|
| DHT22 – DATA | GPIO 4 |
| MQ-135 – saída analógica | GPIO 34 |
| Relé – IN | GPIO 26 |
| Monitoramento experimental do atuador | GPIO 27 |

O MQ-135 utiliza um divisor resistivo para adequar a tensão da saída analógica ao nível suportado pelo ESP32.

O GPIO 27 foi utilizado apenas durante os ensaios de tempo de resposta do atuador.

## Funcionamento

O ESP32 realiza continuamente a leitura dos sensores.

O DHT22 fornece:

- temperatura;
- umidade relativa do ar.

O MQ-135 fornece um valor analógico utilizado como indicador experimental da qualidade do ar.

Neste projeto, os valores do MQ-135 são tratados como valores de ADC e **não como concentração calibrada em ppm**.

Foi implementada uma lógica de histerese para evitar acionamentos repetitivos do relé:

- **MQ-135 ≥ 45:** ventilador ligado;
- **MQ-135 ≤ 35:** ventilador desligado;
- entre 35 e 45, o estado anterior é mantido.

## Comunicação MQTT

O protocolo utilizado para comunicação entre o ESP32 e a plataforma de monitoramento é o **MQTT**.

### Broker

`broker.hivemq.com`

### Porta

`1883`

### QoS

`0`

### Tópicos

```text
mackenzie/iot/projeto_ar/temperatura
mackenzie/iot/projeto_ar/umidade
mackenzie/iot/projeto_ar/mq135
mackenzie/iot/projeto_ar/ventilador
```

Para os ensaios de latência também foram utilizados:

```text
mackenzie/iot/projeto_ar/teste_latencia/dht22
mackenzie/iot/projeto_ar/teste_latencia/mq135
```

Os dados foram monitorados utilizando o **MQTTX Desktop**.

## Bibliotecas utilizadas

O projeto foi desenvolvido na Arduino IDE utilizando, entre outras:

- WiFi
- PubSubClient
- DHT Sensor Library
- Adafruit Unified Sensor

A biblioteca PubSubClient foi utilizada para a comunicação MQTT.

## Resultados dos testes

Foram realizadas quatro medições para cada sensor e para o atuador.

### DHT22 → MQTT

| Teste | Tempo |
|---|---:|
| 1 | 526 ms |
| 2 | 513 ms |
| 3 | 508 ms |
| 4 | 519 ms |
| **Média** | **516,5 ms** |

### MQ-135 → MQTT

| Teste | Tempo |
|---|---:|
| 1 | 504 ms |
| 2 | 517 ms |
| 3 | 517 ms |
| 4 | 518 ms |
| **Média** | **514,0 ms** |

### Acionamento do atuador

O tempo do atuador foi medido entre o envio do comando de acionamento pelo ESP32 e a detecção da energização do cooler após o fechamento do relé.

| Teste | Tempo |
|---|---:|
| 1 | 4,212 ms |
| 2 | 4,167 ms |
| 3 | 4,167 ms |
| 4 | 4,127 ms |
| **Média** | **4,168 ms** |

O ensaio mede a **energização elétrica do atuador**, e não o instante mecânico exato em que as pás do cooler começam a se movimentar.

## Estrutura do projeto

O código principal executa as seguintes etapas:

1. inicialização do ESP32;
2. conexão à rede Wi-Fi;
3. conexão ao broker MQTT;
4. leitura do DHT22;
5. leitura analógica do MQ-135;
6. publicação dos dados nos tópicos MQTT;
7. aplicação da lógica de histerese;
8. acionamento ou desligamento do relé;
9. publicação do estado do ventilador;
10. repetição do ciclo.

## Como reproduzir

1. Instalar a Arduino IDE.
2. Adicionar o suporte para placas ESP32.
3. Instalar as bibliotecas necessárias.
4. Montar o circuito conforme o esquema do projeto.
5. Configurar no código o nome e a senha da rede Wi-Fi.
6. Utilizar o broker MQTT indicado ou configurar outro broker.
7. Compilar e enviar o programa para o ESP32.
8. Abrir o MQTTX.
9. Assinar o tópico:

```text
mackenzie/iot/projeto_ar/#
```

10. Acompanhar as leituras e o estado do ventilador em tempo real.

## Observações

O MQ-135 utilizado no projeto não foi submetido a calibração laboratorial para determinação da concentração absoluta de gases.

Portanto, os valores são utilizados como **indicadores relativos da variação da qualidade do ar**, adequados para demonstração do funcionamento do protótipo.

O broker HiveMQ utilizado é público. Por isso, os tópicos empregados neste projeto não devem ser utilizados para transmissão de informações sensíveis.

## Autor

**David Pessoa**

Projeto acadêmico - 2026.
