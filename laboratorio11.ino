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
 motores=[motor1, motor4, motor1];
 valores=[0, 0, 0];
 int valorant = 0;

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
 

int rele = 18;
int rele2 =23;
int rele3= 14;
int ledRojo = 22;
int ledVerde = 21;
int ledAzul = 19;

int color=1;


AsyncWebServer server(80);


                 //Definimos el puerto de comunicaciones
// Variable to store the HTTP request
String header;
 

JSONVar info;
JSONVar temp;
JSONVar luz;





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
 String getinfo(){

  info["dip"] = String(WiFi.localIP());
  info["status"] =  String(WiFi.status());
  info["ssid"] =  String(WiFi.SSID());
  info["rsid"] =  WiFi.RSSI();
  String jsonString = JSON.stringify(info);
  return jsonString;
}

 String getserv(){
// Lectura de los datos del sensor
  datoVal = analogRead(PIN_LM35);
  serv["datoVal"]   = String(datoVal);
   // Convirtiendo los datos del ADC a    milivoltios
  serv["mil"] =  String(datoVal * (ADC_VREF_mV / ADC_RESOLUTION));
  // Convirtiendo el voltaje al temperatura en °C
  serv["tempC"] =  datoVal * factor ; 
  serv["estado"]=estado;
  String jsonString = JSON.stringify(temp);
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
  
  pinMode(rele,OUTPUT);
  pinMode(rele2,OUTPUT);
  pinMode(rele3,OUTPUT);

  pinMode(33,INPUT);

  pinMode(ledRojo,OUTPUT);
pinMode(ledVerde,OUTPUT);
pinMode(ledAzul,OUTPUT);

//  DS18B20.begin();      // initializando el sensor DS18B20
  initWiFi();

 // digitalWrite(PinLedR, LOW); digitalWrite(PinLedG, LOW); digitalWrite(PinLedB, LOW);  
  if(!SPIFFS.begin())
     { Serial.println("ha ocurrido un error al montar SPIFFS");
       return; }
  
  //Serial.println(WiFi.localIP());
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    estado="0";
    request->send(SPIFFS, "/index.html",String(), false);
    
  });   
  server.on("/MANUAL", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/manual.html",String(), false);
  });   

  
  server.on("/AUTOMATICO", HTTP_GET, [](AsyncWebServerRequest *request){
    estado="0";
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


  server.on("/TEMP", HTTP_GET, [](AsyncWebServerRequest *request){
    datoVal=analogRead(33);
    String json = gettemp();
    request->send(200, "application/json", json);
    json = String();
  });
  

  server.on("/SET_POINT", HTTP_POST, [](AsyncWebServerRequest *request){
    pwmValue = request->arg("set_point");
    valor=pwmValue.toInt();
    request->redirect("/CONTROL");    
  });  
  //ADD HTTP POST HANDLER FOR SERVO
  server.on("/MOTOR1", HTTP_POST, [](AsyncWebServerRequest *request){
    pwmValue = request->arg("valor1");
    valor=pwmValue.toInt();
     moveServo(1,pwmValue.toInt());
    request->redirect("/");    
  }); 
  server.on("/MOTOR2", HTTP_POST, [](AsyncWebServerRequest *request){
    pwmValue = request->arg("valor2");
    valor=pwmValue.toInt();
    moveServo(2,pwmValue.toInt());
    request->redirect("/");    
  }); 
  server.on("/MOTOR3", HTTP_POST, [](AsyncWebServerRequest *request){
    pwmValue = request->arg("valor3");
    valor=pwmValue.toInt();
    moveServo(3,pwmValue.toInt());
    request->redirect("/");    
  }); 
  server.on("/MOTOR4", HTTP_POST, [](AsyncWebServerRequest *request){
    pwmValue = request->arg("valor4");
    valor=pwmValue.toInt();
    moveServo(4,pwmValue.toInt());
    request->redirect("/");    
  }); 


  server.on("/TRUE", HTTP_GET, [](AsyncWebServerRequest *request){ 
    mod=true;
    request->send(0);
  });

  server.on("/FALSE", HTTP_GET, [](AsyncWebServerRequest *request){ 
    mod=false;
    request->send(0);
  });
            

  server.on("/MIN", HTTP_POST, [](AsyncWebServerRequest *request){
            String min = request->arg("minutos");
            String seg = request->arg("segundos");
            temporizador(min.toInt(),seg.toInt());
            request->redirect("/HORARIO");  
            });  

        
  
  server.begin();

  
  
}  

void temporizador(int min, int seg){
  for(int i=0; i<min; i++){
    for(int j=0; j<60; j++){
        Serial.print(i);
        Serial.print(":");
        Serial.print(j);
        delay(1000);
    }
  }
}

void loop() {
  datoVal = analogRead(PIN_LM35);
  float datoC=datoVal*factor;
  if(mod==true){
    if (datoC>(valor*1.05)){
      digitalWrite(rele, LOW);    
      digitalWrite(rele2, HIGH);
      digitalWrite(rele3, HIGH);
      estado="1";
    }else if (datoC<(valor*0.95)){
      digitalWrite(rele, HIGH);    
      digitalWrite(rele2,LOW);
      digitalWrite(rele3,LOW);
      estado="2";
    }
  }else {
    estado="0";
  }
  delay (1000);
  
}
