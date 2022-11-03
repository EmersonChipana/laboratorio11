#include <WiFi.h>  
#include "config.h"
#include <Arduino_JSON.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <stdlib.h>

boolean mod = false;
String estado="0";
int valor=0;

/*
Servo
*/
 #define COUNT_LOW 1638
 #define COUNT_HIGH 7864
 #define TIMER_WIDTH 16
 int i;
 int motores=[];
 int mot1=0;
 int mot2=0;
 int mot3=0;
 int mot4=0;
 int state1=0;
 int state2=0;
 int state3=0;
 int state4=0;
 int valores=[];
 int valorant = 0;
 int contador=  1;

 /*
  * PWMVALUE
  */
#define PWM1_Ch    0    //canales
#define PWM1_Res   8    //resolucion    
#define PWM1_Freq  1000  //frecuencia
#define PWM2_Ch    1
#define PWM2_Res   8
#define PWM2_Freq  1000
#define PWM3_Ch    2
#define PWM3_Res   8
#define PWM3_Freq  1000
int pwm1,pwm2,pwm3;
int PWM1_DutyCycle = 0;


 int ahora1 = 1638;

int ahora2 =1638;
String pwmValue;
 

int color=1;


AsyncWebServer server(80);


                 //Definimos el puerto de comunicaciones
// Variable to store the HTTP request
String header;
 

JSONVar info;
JSONVar serv;


void initWiFi() {
// conectamos al Wi-Fi
  WiFi.begin(ssid, password);
  // Mientras no se conecte, mantenemos un bucle con reintentos sucesivos
  while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      // Esperamos un segundo
      Serial.println("Conectando a la red WiFi..");
    }
  Serial.println();
  Serial.println(WiFi.SSID());
  Serial.print("Direccion IP:\t");
  // Imprimimos la ip que le ha dado nuestro router
  Serial.println(WiFi.localIP());
}
/*
void abrirJson(){
  char json[] = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";

DynamicJsonDocument doc(1024);
deserializeJson(doc, json);

int motSer=doc["mot"][0];
int movi=doc["mov"][0];
const char* sensor = doc["sensor"];
long time          = doc["time"];
double latitude    = doc["data"][0];
double longitude   = doc["data"][1];
}
*/

//add funtion to move servo
void moveServo(int servo, int value){
 
  if(value>valorant){ 
    valorant=valor;
    for (value=COUNT_LOW ; value < COUNT_HIGH ; value=value+100)
   {
      ledcWrite(servo, value);       // sweep servo 1
      delay(20);
   }
   
  } else if (value<valorant){
    valorant=valor;
    for (value=COUNT_HIGH ; value > COUNT_LOW ; value=value-100)
   {
      ledcWrite(servo, value);       // sweep servo 1
      delay(20);
   }
  }
  else if (value==valorant){
    valorant=valor;
    ledcWrite(servo, value);       // sweep servo 1
    delay(20);
  }
}
void setInstruction(int servo, int value){
    int motores[contador]=servo;
    int valores[contador]=value;
    moveServo(motores[contador], valores[contador]=value);
 contador++;

}
 String getinfo(){

  info["dip"] = String(WiFi.localIP());
  info["status"] =  String(WiFi.status());
  info["ssid"] =  String(WiFi.SSID());
  info["rsid"] =  WiFi.RSSI();
  String jsonString = JSON.stringify(info);
  return jsonString;
}

 String getserv(){
  int tama=sizeof(motores[]);
for(int j=0; j<tama;j++){
  if(motores[j]==0){
    state1=valores[0];
    state2=valores[0];
    state3=valores[0];
    state4=valores[0];
  } else
  if(motores[j]==1){
    state1=valores[j];
  } else if(motores[j]==2){
    state2=valores[j];
  } else if(motores[j]==3){
    state3=valores[j];
  }else if(motores[j]==4){
    state4=valores[j];
  }
}

serv["estado"]=String ();
// Lectura de los datos del sensor
  serv["mot1"]   = String(state1);
   // Convirtiendo los datos del ADC a    milivoltios
  serv["mot2"] =  String(state2);
  // Convirtiendo el voltaje al temperatura en °C
  serv["mot3"] = String(state3) ; 
  serv["mot4"]=String(state4) ; 
  String jsonString = JSON.stringify(serv);
  return jsonString; 
}




void setup() {


  Serial.begin(115200); // inicializando el pouerto serial

ledcSetup(1, 50, TIMER_WIDTH); // canal 1, 50 Hz, 16-bit width
   ledcAttachPin(26, 1);   // GPIO 22 asignado al canal 1
   ledcSetup(2, 50, TIMER_WIDTH); // canal 2, 50 Hz, 16-bit width
   ledcAttachPin(25, 2);   // GPIO 19 asignado al canal 2
ledcSetup(3, 50, TIMER_WIDTH); // canal 1, 50 Hz, 16-bit width
   ledcAttachPin(32, 3);   // GPIO 22 asignado al canal 1
   ledcSetup(4, 50, TIMER_WIDTH); // canal 2, 50 Hz, 16-bit width
   ledcAttachPin(33, 4);   // GPIO 19 asignado al canal 2
  


//  DS18B20.begin();      // initializando el sensor DS18B20
  initWiFi();

 // digitalWrite(PinLedR, LOW); digitalWrite(PinLedG, LOW); digitalWrite(PinLedB, LOW);  
  if(!SPIFFS.begin())
     { Serial.println("ha ocurrido un error al montar SPIFFS");
       return; }
  
  //Serial.println(WiFi.localIP());
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
   
    request->send(SPIFFS, "/index.html",String(), false);
    
  });   
  server.on("/MANUAL", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/manual.html",String(), false);
  });   
  
  
  server.on("/AUTOMATICO", HTTP_GET, [](AsyncWebServerRequest *request){

    request->send(SPIFFS, "/automatico.html",String(), false);

  
  }); 
   
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
            request->send(SPIFFS, "/style.css", "text/css");
            });      


   server.on("/INFO", HTTP_GET, [](AsyncWebServerRequest *request){
    String json = getinfo();
    request->send(200, "application/json", json);
    json = String();
  });
server.on("/VALORES", HTTP_GET, [](AsyncWebServerRequest *request){
    String json = getserv();
    request->send(200, "application/json", json);
    json = String();
  }); 
  //ADD HTTP POST HANDLER FOR SERVO
  server.on("/MOTOR1", HTTP_POST, [](AsyncWebServerRequest *request){
    pwmValue = request->arg("valor1");
    valor=pwmValue.toInt();
     setInstruction(1,valor);
    request->redirect("/");    
  }); 
  server.on("/MOTOR2", HTTP_POST, [](AsyncWebServerRequest *request){
    pwmValue = request->arg("valor2");
    valor=pwmValue.toInt();
    setInstruction(2,valor);
    request->redirect("/");    
  }); 
  server.on("/MOTOR3", HTTP_POST, [](AsyncWebServerRequest *request){
    pwmValue = request->arg("valor3");
    valor=pwmValue.toInt();
    setInstruction(3,valor);
    request->redirect("/");    
  }); 
  server.on("/MOTOR4", HTTP_POST, [](AsyncWebServerRequest *request){
    pwmValue = request->arg("valor4");
    valor=pwmValue.toInt();
    setInstruction(4,valor);
   // moveServo(4,valor);
    request->redirect("/");    
  }); 
        
  
  server.begin();

  
}  


void loop() {

  
}
