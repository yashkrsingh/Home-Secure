#include "DHT.h"
#include<SoftwareSerial.h>

#define SSID        "XT1068 4711"
#define PASS        "mypassword" 
#define DEST_IP     "192.168.43.186"
#define TIMEOUT     1000 
#define CONTINUE    false
#define HALT        true
#define DHTPIN 6
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial mySerial(11,12); //RX TX
char VALUE = 48;

const int buzzer = 7;
const int gas = 0;
const int motion = 2;
const int ledPin1 = 8;
const int ledPin2 = 9;
const int INOUT = 10;
//const int button = 13;
//int buttonState;
//int count = 0, debounce = 0;
boolean security = false;
float cel,hum;
int lpg,fire=0,intrusion=0;


void errorHalt(String msg)
{
  Serial.println(msg);
  Serial.println("HALT");
  while(true){};
}

boolean echoValue(String keyword)
{
  byte current_char   = 0;
  byte keyword_length = keyword.length();
  
  long deadline = millis() + TIMEOUT;
  while(millis() < deadline)
  {
    if (mySerial.available())
    {
      char ch = mySerial.read();
      Serial.write(ch);
      if (current_char == (keyword_length-1)){
          VALUE = ch;
          return true; 
      }
      if (ch == keyword[current_char])
        if (++current_char == keyword_length)
        { 
          Serial.println(ch);
          return true;
        }
    }
  }
  return false; 
}

boolean echoFind(String keyword)
{
  byte current_char   = 0;
  byte keyword_length = keyword.length();
  long deadline = millis() + TIMEOUT;
  while(millis() < deadline)
  {
    if (mySerial.available())
    {
      char ch = mySerial.read();
      Serial.write(ch);
      if (ch == keyword[current_char])
        if (++current_char == keyword_length)
        {
          Serial.println();
          return true;
        }
    }
  }
  return false; 
}

void echoFlush()
{
  while(mySerial.available()) 
  Serial.write(mySerial.read());
}
  
void echoSkip()
{
  echoFind("\n");        
  echoFind("\n");        
  echoFind("\n");        
}

boolean echoCommand(String cmd, String ack, boolean halt_on_fail)
{
  mySerial.println(cmd);
  #ifdef ECHO_COMMANDS
    Serial.print("--"); Serial.println(cmd);
  #endif
  
  if (ack == "")
    echoSkip();
  else
    if (!echoFind(ack))          
      if (halt_on_fail)
        errorHalt(cmd+" failed");
      else
        return false;            
  return true;                   
}

boolean connectWiFi()
{
  String cmd = "AT+CWJAP=\""; cmd += SSID; cmd += "\",\""; cmd += PASS; cmd += "\"";
  if (echoCommand(cmd, "OK", CONTINUE)) 
  {
    Serial.println("Connected to WiFi.");
    return true;
  }
  else
  {
    Serial.println("Connection to WiFi failed.");
    return false;
  }
}

// ******** SETUP ********
void setup()  
{
  Serial.begin(9600);         
  mySerial.begin(9600);        
  pinMode(motion, INPUT);
  //pinMode(button, INPUT); 
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(INOUT, OUTPUT);
  pinMode(INOUT, OUTPUT);
  pinMode(buzzer, OUTPUT);
  
  mySerial.setTimeout(TIMEOUT);
  Serial.println("ESP8266 Demo");
  delay(2000);  
  Serial.println("Module is ready.");
  echoCommand("AT+GMR", "OK", CONTINUE);  
  echoCommand("AT+CWMODE?","OK", CONTINUE);
  echoCommand("AT+CWMODE=1", "", HALT);    
  echoCommand("AT+CIPMUX=1", "", HALT);    

  boolean connection_established = false;
  for(int i=0;i<5;i++)
  {
    if(connectWiFi())
    {
      connection_established = true;
      break;
    }
  }
  if (!connection_established) errorHalt("Connection failed");
  delay(5000);
  echoCommand("AT+CIFSR", "", HALT);        
}

// ******** LOOP ********
void loop() 
{
  /*
  buttonState = digitalRead(button);
  if(buttonState== HIGH && debounce == 0) {
    debounce++;
    count++;
  }
  else if(buttonState== LOW){
    debounce=0;
  }
  */
  fire = 0;
  intrusion = 0;
  hum = dht.readHumidity();
  cel = dht.readTemperature();
  lpg = analogRead(gas);
  if(analogRead(gas) > 500 || cel > 75.0){
    if(!security){
      Serial.println("Fire Alert!!");
      fire = 1;
      analogWrite(buzzer, 20);    
      delay(500);         
      analogWrite(buzzer, 0);      
      delay(100);
    }
    else{
      Serial.println("Fire Alert!!");
      fire = 1;
    }
  }
  if(!security){
    digitalWrite(INOUT, HIGH);
    digitalWrite(ledPin2, LOW);
    int x = map(hum, 0, 100, 30, 16);
    analogWrite(ledPin1, x);
  }
  else if(security){
    digitalWrite(INOUT, LOW);
    digitalWrite(ledPin1, LOW);
    digitalWrite(ledPin2, LOW);
    if(digitalRead(motion) == HIGH){
      digitalWrite(ledPin2, HIGH);
      Serial.println("Intrusion Detected!!");
      intrusion = 1;
    }
    else{
      digitalWrite(ledPin2, LOW);
      intrusion = 0;
    }
  }
  sendData();
  getToggle();
  appInput();
}

void sendData(){
  String cmd = "AT+CIPSTART=0,\"TCP\",\""; cmd += DEST_IP; cmd += "\",5000";
  if (!echoCommand(cmd, "OK", CONTINUE)) return;
  delay(1000); 
  if (!echoCommand("AT+CIPSTATUS", "OK", CONTINUE)) return;
  cmd = "GET";
  cmd+= " /get/sensorData?temp="+String(cel)+"&humid="+String(hum)+"&lpg="+String(lpg)+"&fire="+String(fire)+"&intrusion="+String(intrusion)+"\r\n";
  cmd+= "HTTP/1.1\r\nHost:192.168.43.186\r\n\r\n";

  if (!echoCommand("AT+CIPSEND=0,"+String(cmd.length()), ">", CONTINUE))
  {
     echoCommand("AT+CIPCLOSE", "", CONTINUE);
     Serial.println("Connection timeout.");
  }
  echoCommand(cmd, "OK", CONTINUE);  
  int flag = 0;
  while(true){
    while (mySerial.available()){
      Serial.write(mySerial.read());
      if(echoFind("Unlink")){
        Serial.println("Break Here");
        flag=1;
        break;
      }
    }
    if(flag==1){
      Serial.println("Broken");
      break;
    }
  }
}

void getToggle(){
  String cmd = "AT+CIPSTART=0,\"TCP\",\""; cmd += DEST_IP; cmd += "\",5000";
      if (!echoCommand(cmd, "OK", CONTINUE)) return;
      delay(1000);
      if (!echoCommand("AT+CIPSTATUS", "OK", CONTINUE)) return;
      cmd = "GET";
      cmd+= " /get/securityToggle\r\n";
      cmd+= "HTTP/1.1\r\nHost:192.168.43.186\r\n\r\n";
      if (!echoCommand("AT+CIPSEND=0,"+String(cmd.length()), ">", CONTINUE))
      {
        echoCommand("AT+CIPCLOSE", "", CONTINUE);
        Serial.println("Connection timeout.");
      }
      echoCommand(cmd, "OK", CONTINUE); 
      int flag = 0;
      while(true){
        while (mySerial.available()){
        Serial.write(mySerial.read());
          if(echoValue("IPD,0,1: ")){
            Serial.println("Value Found");
            flag=1;
            break;
          }
        }
      if(flag==1){
        Serial.println("Broken");
        break;
      }
    }
}

void appInput(){
  int input;
  if(Serial.available())
    input = Serial.read();
  if(input == 48 || VALUE == '0' || count%2 == 0)
    security = false;
  if(input == 49 || VALUE == '1' || count%2 == 1) 
    security = true;
}
