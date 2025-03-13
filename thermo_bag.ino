//Dependências
#include <ESP8266WiFi.h>
#include <ESP_Mail_Client.h>

//Constantes para uso de WiFi
#define WIFI_SSID "<ssid>"
#define WIFI_PASSWORD "<password>"

//Constantes para envio de email
#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT esp_mail_smtp_port_587
#define AUTHOR_EMAIL "thermobagd@gmail.com"
#define AUTHOR_PASSWORD "prgx ylkd omth qixu"
#define RECIPIENT_EMAIL "<destinatario>"

SMTPSession smtp;
Session_Config config;

//Definição de pino para leitura do sensor
#define pino A0

//Constantes de temperatura mínima e máxima
const float tempMin = 15.0, tempMax = 27.0;

//Entradas e saídas
const int controlLed = 2, ledBlue = 12, ledRed = 5, botao = 4, buzzer = 13;

float temp = 0.0, ultimatemp = 0.0;
int statusBotao, statusRed, statusBlue, envios = 0, interval = 60;
bool limite = false;

//Função para envio de e-mail
void sendEmail(const char* assunto, const char* mensagem) {
  // Serial.println("Envio");
  SMTP_Message message;

  message.sender.name = F("ThermoBag");
  message.sender.email = AUTHOR_EMAIL;
  message.addRecipient(F("Someone"), RECIPIENT_EMAIL);

  message.subject = assunto;
  message.text.content = mensagem;
  MailClient.sendMail(&smtp, &message);

  smtp.connect(&config);

  if (!MailClient.sendMail(&smtp, &message)) {
    // Serial.println("Falha ao enviar o email");
    // Serial.println(smtp.errorReason());
  } else {
    // Serial.println("E-mail enviado com sucesso");
  }
}

void beep(int times){
  for(int i = 0; i < times; i++){
    digitalWrite(buzzer, HIGH);
    delay(100);
    digitalWrite(buzzer, LOW);
    delay(100);
  }
}

void setup() {
  // Serial.begin(9600);

  // Estabelecendo conexão wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
  }

  MailClient.networkReconnect(true);

  //Configurando envio de e-mail
  smtp.debug(1);

  config.server.host_name = SMTP_HOST;
  config.server.port = SMTP_PORT;
  config.login.email = AUTHOR_EMAIL;
  config.login.password = AUTHOR_PASSWORD;

  config.login.user_domain = WiFi.localIP().toString();

  smtp.connect(&config);
  smtp.loginWithPassword(AUTHOR_EMAIL, AUTHOR_PASSWORD);

  //Declarando LED's como dispositivos de saída e botão como dispositivo de entrada
  pinMode(controlLed, OUTPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(ledBlue, OUTPUT);
  pinMode(botao, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
}

void loop() {
  statusBotao = digitalRead(botao);

  //Caso botão tenha sido pressionado, "reseta" leitura e aguarda um tempo até executar a leitura novamente
  if (statusBotao == LOW) {
    limite = false;
    envios = 0;
    digitalWrite(ledBlue, LOW);
    digitalWrite(ledRed, LOW);
    delay(5000);
  }

  //Conversão da leitura analógica em temperatura
  temp = ((analogRead(pino) * (1.0 / 1023)) / 0.01);

  //Caso limite não tenha sido atingido, implementar lógica de blink dos LED's correspondentes
  if (!limite) {
    //Blink é executado apenas se a temperatura estiver a menos de 5 graus do limite
    if (temp > tempMin && temp < tempMin + 5.0) {
      statusBlue = digitalRead(ledBlue);
      digitalWrite(ledBlue, !statusBlue);
      delay(500);
      digitalWrite(ledBlue, statusBlue);

      if (envios < 2 && interval == 0) {
        interval = 60;
        envios++;
        sendEmail("Atenção!", "Temperatura está próximo da mínima");
      }
    } else if (temp < tempMax && temp > tempMax - 5.0) {
      statusRed = digitalRead(ledRed);
      digitalWrite(ledRed, !statusRed);
      delay(500);
      digitalWrite(ledRed, statusRed);

      if (envios < 2 && interval == 0) {
        interval = 60;
        envios++;
        sendEmail("Atenção!", "Temperatura está próximo da máxima");
      }
    }
  }

  //Caso a temperatura tiver alterado, analisar se algum limite foi atingido
  //Se sim, manter LED correspondente aceso constantemente
  if (temp != ultimatemp) {
    ultimatemp = temp;

    if (temp <= tempMin) {
      if (!limite) {
        beep(2);
        sendEmail("Temperatura mínima atingida!", "Infelizmente a temperatura mínima foi atingida pela bolsa");
      }

      digitalWrite(ledBlue, HIGH);
      digitalWrite(ledRed, LOW);
      limite = true;
    } else if (temp >= tempMax) {
      if (!limite) {
        beep(3);
        sendEmail("Temperatura máxima atingida!", "Infelizmente a temperatura máxima foi atingida pela bolsa");
      }

      digitalWrite(ledBlue, LOW);
      digitalWrite(ledRed, HIGH);
      limite = true;
    }

    //Print de controle da temperatura
    // Serial.print(temp);
    // Serial.println(" C");
  }

  interval--;

  //Blink de controle de funcionamento da placa
  digitalWrite(controlLed, LOW);
  delay(500);
  digitalWrite(controlLed, HIGH);
  delay(500);
}