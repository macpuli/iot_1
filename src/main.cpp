#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
//int led_gpio = 15;
int columnas[] = {13,12,14,27,32,33,25,26,15,0,23,22,5,18,19,21};
int filas[] = {17,16,2,4};
int time = 500;

const char* ssid = "INFICobaPu2.4";
const char* password = "P1e2Lu10S2a1";

const char *mqtt_server = "driver.cloudmqtt.com";
const int mqtt_port = 18651;
const char *mqtt_user = "clxxwiek";
const char *mqtt_pass = "PtsiiKX9xhVm";

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;

void recorrido();
void cubo();
void controlColumnas(bool valor);
void controlColumnas2(bool valor);

void recorrido(){
  bool aux = true;
  for(int j = 0; j < 16; j++){
    digitalWrite(columnas[j],HIGH);
    if(aux == true){
      for(int i = 0; i < 4; i++){
  		  digitalWrite(filas[i],HIGH);
      	delay(time);
      	digitalWrite(filas[i],LOW);
  	  }
      aux = false;
    }else{
      for(int i = 3; i >= 0; i--){
  		  digitalWrite(filas[i],HIGH);
      	delay(time);
      	digitalWrite(filas[i],LOW);
  	  }
      aux = true;
    }
    digitalWrite(columnas[j],LOW);
  }
}

void cubo(){ 
  controlColumnas(HIGH);
  for(int i = 0; i < 4; i++){
    digitalWrite(filas[i],HIGH);
  }
  delay(time);
  
  controlColumnas(LOW);
  digitalWrite(filas[0],LOW);
  digitalWrite(filas[3],LOW);
  delay(100);
  
  controlColumnas2(HIGH);
  delay(time);
  controlColumnas2(LOW);
  delay(time);
}

void controlColumnas(bool valor){
  digitalWrite(columnas[0],valor);
  digitalWrite(columnas[1],valor);
  digitalWrite(columnas[2],valor);
  digitalWrite(columnas[3],valor);
  
  digitalWrite(columnas[4],valor);
  digitalWrite(columnas[7],valor);
  digitalWrite(columnas[8],valor);
  digitalWrite(columnas[11],valor);
  
  digitalWrite(columnas[12],valor);
  digitalWrite(columnas[13],valor);
  digitalWrite(columnas[14],valor);
  digitalWrite(columnas[15],valor);
}

void controlColumnas2(bool valor){
  digitalWrite(columnas[5],valor);
  digitalWrite(columnas[6],valor);
  digitalWrite(columnas[9],valor);
  digitalWrite(columnas[10],valor);
}

void callback(char* topic, byte* payload, unsigned int length){
  Serial.print("Mensaje recibido bajo el tópico ->");
  Serial.print(topic);
  Serial.print("\n");

  for (int i=0; i<length; i++){
    Serial.print((char)payload[i]);
  }

  switch ((char)payload[0])
  {
    case '1':
      recorrido();
      break;
    case '2':
      cubo();
      break;
  }
  /*
  if((char)payload[0] == '0'){
    digitalWrite(led_gpio, LOW);
    Serial.println("\n LED apagado");
  }
  else{
    digitalWrite(led_gpio, HIGH);
    Serial.println("\n LED encendido");
  }
  */
  Serial.println();
}

void reconnect(){
  while(!client.connected()){
    Serial.println("Intentando Conexión MQTT");

    String clientId = "iot_1_";
    clientId = clientId + String(random(0xffff),HEX);

    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("Conexión a MQTT exitosa");
      client.publish("salidaMCP", "Primer mensaje");
      client.subscribe("entradaMCP");
    } else {
      Serial.print("Falló la conexión");
      Serial.print(client.state());
      Serial.println("Se intentará de nuevo en 5 segundos");
      delay(5000);
    }
  }
}

void setup_wifi(){
  Serial.println();
  Serial.println("Conectando a...");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Conectando a red ->");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  // put your setup code here, to run once:
  //pinMode(led_gpio, OUTPUT);
  Serial.begin(115200);

  for(int i = 0; i < 16; i++){
    pinMode(columnas[i],OUTPUT);
  }
  
  for(int i = 0; i < 4; i++){
    pinMode(filas[i],OUTPUT);
  }

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(client.connected()==false){
    reconnect();
  }

  client.loop();
  if(millis()-lastMsg > 2000){
    lastMsg = millis();
    value++;
    String mes = "Valor ->" + String(value);
    mes.toCharArray(msg, 50);
    client.publish("MCP", msg);
    Serial.println("Mensaje enviado ->" + String(value));
  }
}