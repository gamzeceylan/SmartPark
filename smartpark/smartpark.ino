// kütüphaneler
#include <Servo.h>
#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

//Firebase veritabanı adresi, Token bilgisini ve ağ adresi bilgileri 
#define FIREBASE_HOST "*******" 
#define FIREBASE_AUTH "*******"
#define WIFI_SSID "*******"
#define WIFI_PASSWORD "****"

// tarih almak için
const long utcOffsetInSeconds = 3600;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String months[12]={"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

// kullanılacak değişkenler
int distance1;
int distance2;
int distance3;
int aracNo_S1 = 1; // 1. slota kaç araba girdi?
int aracNo_S2 = 1; // 2. slota kaç araba girdi?
int aracNo_S3 = 1; // 3. slota kaç araba girdi?
int angle=0; // servo motor açı

//int otoparkKapasitesi=15; // atanan otopark kapasitesi
int iceridekiAracSayisi=12; // içeride olduğu varsayılan araç sayısı

FirebaseData veritabanim; 
Servo servo;

void setup() {
  
  Serial.begin(115200);

  // wifi bağlantısı
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Ağ Bağlantısı Oluşturuluyor");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  // zaman başlat
  timeClient.begin(); // saati alma
  timeClient.setTimeOffset(10800); // türkiye GTM +3

  // firebase bağlantısı
  Serial.println();
  Serial.print("IP adresine bağlanıldı: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  //Firebase bağlantısı başlatılıyor
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  //Ağ bağlantısı kesilirse tekrar bağlanmasına izin veriyoruz
  Firebase.reconnectWiFi(true);

  
  // pin baglantiları
  pinMode(D4, OUTPUT); // trigPin -> Output
  pinMode(D3, INPUT); // echoPin -> Input
  pinMode(D2, OUTPUT); 
  pinMode(D1, INPUT); 
  pinMode(D5, INPUT);
  pinMode(D6, OUTPUT);
  servo.attach(D7);

 

}

void loop() {

  timeClient.update(); // zaman güncelleme
  
  // slot kontrolleri
    Slot1();
    Slot2();
    Slot3();
  
  delay(1000);
  Serial.print("iceridekiAracSayisi ");
  Serial.println(iceridekiAracSayisi);

 // Firebase.setInt(veritabanim, "OtoparkKapasite", 15); 
  Firebase.setInt(veritabanim, "IceridekiAracSayisi",iceridekiAracSayisi );

// kapı durumu okunuyor, kosul sağlanırsa açılıyor
  Firebase.getString(veritabanim,"KapiAc");
  String durum = veritabanim.to<String>();
  if(durum=="1" && iceridekiAracSayisi <15){
       ServoHareket();
       Firebase.setString(veritabanim, "KapiAc", "0");
       
   }
}

void Slot1(){
  
  digitalWrite(D4,LOW);
  delayMicroseconds(2);
  digitalWrite(D4,HIGH);
  delayMicroseconds(2);
  digitalWrite(D4,LOW);

  long timedelay1 = pulseIn(D3,HIGH);
  distance1 = timedelay1*0.034/2;

  String number = String(aracNo_S1);
  
  if(distance1<=5){
       Serial.println("Slot1 dolu");
       Firebase.getString(veritabanim,"Slot1/MevcutDurum");
       String durum = veritabanim.to<String>();
       
       if(durum == "1"){}
       else if(durum == "0"){
            Firebase.setString(veritabanim, "Slot1/MevcutDurum", "1");
            Firebase.setString(veritabanim, "Slot1/"+number+"/Giris", timeClient.getFormattedTime());
            iceridekiAracSayisi++;
        }
        delay(15);
    }

  else{
       Serial.println("Slot1 bos");
       Firebase.getString(veritabanim,"Slot1/MevcutDurum");
       String durum = veritabanim.to<String>();
       
        if(durum == "1"){
            Firebase.setString(veritabanim, "Slot1/MevcutDurum", "0");
            Firebase.setString(veritabanim, "Slot1/"+number+"/Cikis", timeClient.getFormattedTime());
            iceridekiAracSayisi--;
          aracNo_S1++;
        }  
        else if(durum == "0"){ }
   }
   delay(15);
}  


void Slot2(){
  digitalWrite(D2,LOW);
  delayMicroseconds(2);
  digitalWrite(D2,HIGH);
  delayMicroseconds(2);
  digitalWrite(D2,LOW);

  long timedelay2 = pulseIn(D1,HIGH);
  distance2 = timedelay2*0.034/2;

   String number = String(aracNo_S2);
   if(distance2<=5){
       Serial.println("Slot2 dolu");
       Firebase.getString(veritabanim,"Slot2/MevcutDurum");
       String durum = veritabanim.to<String>();
       
       if(durum == "1"){} // eğer zaten araç varsa değişiklik yapılmıyor. (Zaman için yapıldı=
       else if(durum == "0"){
            Firebase.setString(veritabanim, "Slot2/MevcutDurum", "1");
            Firebase.setString(veritabanim, "Slot2/"+number+"/Giris", timeClient.getFormattedTime());
            iceridekiAracSayisi++;
        }
        delay(15);
    }

  else{
      Serial.println("Slot2 bos");
      Firebase.getString(veritabanim,"Slot2/MevcutDurum");
      String durum = veritabanim.to<String>();
      
        if(durum == "1"){
            Firebase.setString(veritabanim, "Slot2/MevcutDurum", "0");
            Firebase.setString(veritabanim, "Slot2/"+number+"/Cikis", timeClient.getFormattedTime());
            iceridekiAracSayisi--;
          aracNo_S2++;
        }
        else if(durum == "0"){ } 
   }
   delay(15);
}

void Slot3(){
  
  digitalWrite(D6,LOW);
  delayMicroseconds(2);
  digitalWrite(D6,HIGH);
  delayMicroseconds(2);
  digitalWrite(D6,LOW);

  long timedelay3 = pulseIn(D5,HIGH);
  distance3 = timedelay3*0.034/2;

  String number = String(aracNo_S3);
  
  if(distance3<=5){
       Serial.println("Slot3 dolu");
       Firebase.getString(veritabanim,"Slot3/MevcutDurum");
       String durum = veritabanim.to<String>();
       
       if(durum == "1"){}       
       else if(durum == "0"){      
            Firebase.setString(veritabanim, "Slot3/MevcutDurum", "1");
            Firebase.setString(veritabanim, "Slot3/"+number+"/Giris", timeClient.getFormattedTime());
            iceridekiAracSayisi++;
        }        
        delay(15);
    }

  else{
       Serial.println("Slot3 bos");
       Firebase.getString(veritabanim,"Slot3/MevcutDurum");
       String durum = veritabanim.to<String>();
       
       if(durum == "1"){
            Firebase.setString(veritabanim, "Slot3/MevcutDurum", "0");
            Firebase.setString(veritabanim, "Slot3/"+number+"/Cikis", timeClient.getFormattedTime());
            iceridekiAracSayisi--;
          aracNo_S3++;
       }
       else if(durum == "0"){ }
   }
   delay(15);
  
}


void ServoHareket(){
  
  for(angle = 90; angle < 180; angle += 1)
     {                          
       servo.write(angle);
        delay(5);                       
     } 
     delay(2000);
     for(angle = 180; angle>=90; angle-=1)
     {                                
       servo.write(angle);
       delay(15);                       
     } 
      delay(15);
}



 
