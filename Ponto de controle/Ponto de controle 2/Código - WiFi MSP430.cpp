#define DEBUG 0
int connectionId = -1;

void SendCommand(String command, const int timeout, boolean debug)
{
  // Envio dos comandos AT para o modulo
  String response = "";
  Serial.print(command);
  unsigned long time = millis();
  while ((time + timeout) > millis())
  {
    while (Serial.available())
    {      
      response += (char)Serial.read();
    }
  }
  if (debug)
  {
    Serial.print(response);
  }
}

void setup()
{
  Serial.begin(19200);
  delay(3000);
  //Serial.println("Inicio");
   Serial.begin(19200);
    //esp8266.println("Olá serial");
  SendCommand("AT+RST\r\n", 2000, DEBUG);
  delay(1000);
  SendCommand("AT+CIOBAUD=19200\r\n", 2000, DEBUG);
       // Serial.println("Serial setado");
  delay(1000);
  SendCommand("AT+CWJAP=\"VIVO-0100\",\"5760500100\"\r\n", 2000, DEBUG);
  delay(1000);
  SendCommand("AT+CWMODE=1\r\n", 1000, DEBUG);
  // Mostra o endereco IP
  delay(300);
  SendCommand("AT+CIFSR\r\n", 1000, DEBUG);
  // Configura para multiplas conexoes
  SendCommand("AT+CIPMUX=1\r\n", 1000, DEBUG);
  // Inicia o web server na porta 80
  SendCommand("AT+CIPSERVER=1,8090\r\n", 1000, DEBUG);
}

void loop()
{
 if (Serial.available())
  {
    if (Serial.find("+IPD,"))
    {
      connectionId = Serial.read() + 1;

      String data = Serial.readStringUntil(':'); // dump, ignorar

      String command;
      String value;
      String aux;
      String RawData = Serial.readStringUntil('\0');
      for (int i = 0; i < RawData.length(); i++)
      {
        if (RawData[i] == ';')
        {
          command = aux.substring(0, 2);
          value = aux.substring(3, aux.length());
          aux = "";
          String response = "Command: ";
          response += command;
          response += "\r\nValue: ";
          response += value;

          //CallCommand(command, value);
          Serial.print("Comando: ");
          Serial.println(command);
          Serial.print("Valor: ");
          Serial.println(value);
        }
        else if (RawData[i] == '\0')
        {
          break;
        }
        else
          aux += RawData[i];
      }
    }
  }
}
