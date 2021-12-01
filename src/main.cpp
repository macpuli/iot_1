#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
//int led_gpio = 15;
int columnas[] = {13,12,14,27,32,33,25,26,15,0,23,22,5,18,19,21};
int filas[] = {17,16,2,4};
int tiempo = 500;
char auxPatron = '0';

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

void patron1();
void patron2();
void patron3();
void patron4();
void controlColumnas(bool);
void controlColumnas2(bool);
void despliegaPatron(char);

void patron1(){
  bool aux = true;
  for(int j = 0; j < 16; j++){
    digitalWrite(columnas[j],HIGH);
    if(aux == true){
      for(int i = 0; i < 4; i++){
  		  digitalWrite(filas[i],HIGH);
      	delay(tiempo);
        
      	digitalWrite(filas[i],LOW);
  	  }
      aux = false;
    }else{
      for(int i = 3; i >= 0; i--){
  		  digitalWrite(filas[i],HIGH);
      	delay(tiempo);
      	digitalWrite(filas[i],LOW);
  	  }
      aux = true;
    }
    digitalWrite(columnas[j],LOW);
  }
}

void patron2(){ 
  controlColumnas(HIGH);
  for(int i = 0; i < 4; i++){
    digitalWrite(filas[i],HIGH);
  }
  delay(tiempo);
  
  controlColumnas(LOW);
  digitalWrite(filas[0],LOW);
  digitalWrite(filas[3],LOW);
  delay(tiempo);
  
  controlColumnas2(HIGH);
  delay(tiempo);
  controlColumnas2(LOW);
  delay(tiempo);
}
void patron3(){
  int incremento=3;
  for(int f = 0; f < 2; f++){
   	
  	int mitad1= 7;
  	int mitad2= 8;  
  	digitalWrite(filas[f],HIGH);
    digitalWrite(filas[f+incremento], HIGH);
    
  	for(int i = 0; i < 8; i++){
    
  		digitalWrite(columnas[mitad1],HIGH);
		digitalWrite(columnas[mitad2],HIGH);
    	delay(tiempo);
   		digitalWrite(columnas[mitad1],LOW);
		digitalWrite(columnas[mitad2],LOW);
		mitad1--;
    	mitad2++;
    
  	}
  	digitalWrite(filas[f],LOW); 
    digitalWrite(filas[f+incremento],LOW);
    incremento-=2;
  }
  
}
void patron4(){
  for(int f = 0; f < 4; f++){
  	digitalWrite(filas[f],HIGH);
  }
  for(int i = 0; i < 16; i+=2){
  	digitalWrite(columnas[i],HIGH);
    delay(tiempo);
    digitalWrite(columnas[i],LOW);
  }
  
  for(int i = 15; i > 0; i-=2){
  	digitalWrite(columnas[i],HIGH);
    delay(tiempo);
    digitalWrite(columnas[i],LOW);
  }
  for(int f = 0; f < 4; f++){
  	digitalWrite(filas[f],LOW);
  }
  
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

void despliegaPatron(char patron){
  switch (patron)
  {
    case '1':
      patron1();
      break;
    case '2':
      patron2();
      break;
    case '3':
      patron3();
      break;
    case '4':
      patron4();
      break;  
  }
}

void callback(char* topic, byte* payload, unsigned int length){
  Serial.print("Mensaje recibido bajo el tópico ->");
  Serial.print(topic);
  Serial.print("\n");

  for (int i=0; i<length; i++){
    Serial.print((char)payload[i]);
  }

  auxPatron = payload[0];
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

    String clientId = "equipo10_";
    clientId = clientId + String(random(0xffff),HEX);

    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("Conexión a MQTT exitosa");
      client.publish("salidaEquipo10", "Primer mensaje");
      client.subscribe("patronEquipo10");
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

  if(millis() - lastMsg > 2000){
    lastMsg = millis();
    despliegaPatron(auxPatron);
  }
}