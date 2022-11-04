#include <WiFi.h>  
#include "config.h"
#include <Arduino_JSON.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>


boolean mod = false;
String estado="0";
int valor1 =0;
int valor2 =0;
int valor3 =0;
int valor4 =0;
/*
Servo
*/
 #define COUNT_LOW 1638
 #define COUNT_HIGH 7864
 #define TIMER_WIDTH 16
 int i;
 int valorant = 0;
 int contador=  1;

String pwmValue;
 

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

 String getinfo(){

  info["dip"] = String(WiFi.localIP());
  info["status"] =  String(WiFi.status());
  info["ssid"] =  String(WiFi.SSID());
  info["rsid"] =  WiFi.RSSI();
  String jsonString = JSON.stringify(info);
  return jsonString;
}
String getservo(){
serv["estado"]=String ();
  serv["mot1"]   = String(valor1);
  serv["mot2"] =  String(valor2);
  serv["mot3"] = String(valor3) ; 
  serv["mot4"]=String(valor4) ; 
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
   server.on("/VALUES", HTTP_GET, [](AsyncWebServerRequest *request){
    String json = getservo();
    request->send(200, "application/json", json);
    json = String();
  });   

  //ADD HTTP POST HANDLER FOR SERVO
  server.on("/MOTOR1", HTTP_POST, [](AsyncWebServerRequest *request){
    pwmValue = request->arg("valor1");
    valor=pwmValue.toInt();
     moveServo(1,valor1);
    // moveServo(1,valor);
    
  }); 
  server.on("/MOTOR2", HTTP_POST, [](AsyncWebServerRequest *request){
    pwmValue = request->arg("valor2");
    valor=pwmValue.toInt();
    moveServo(2,valor2);

  }); 
  server.on("/MOTOR3", HTTP_POST, [](AsyncWebServerRequest *request){
    pwmValue = request->arg("valor3");
    valor=pwmValue.toInt();
    moveServo(3,valor3);
   
  }); 
  server.on("/MOTOR4", HTTP_POST, [](AsyncWebServerRequest *request){
    pwmValue = request->arg("valor4");
    valor=pwmValue.toInt();
    moveServo(4,valor4);
   // moveServo(4,valor);   
  }); 
        
  
  server.begin();

  
}  


void loop() {

}
