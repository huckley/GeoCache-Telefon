#/* GeoCache-Telefon
(cc-by-sa) Falk Hackenberger 10.05.2026 
(cc-by-sa) Oliver Mezger 27.12.21 
*/
 
#include <DFRobotDFPlayerMini.h>
#include <WiFi.h>

#define Telefonpin 4        // zum Telefon
#define mp3busypin 10       // zum mp3 player
#define Ziffer_Timeout 200   // nach dieser Zeit kommt kein weiterer Impuls mehr
#define Wahl_Timeout 3000    // nach dieser Zeit wird keine Ziffer mehr gewaehlt

DFRobotDFPlayerMini myMP3;
unsigned char nummerLaenge=0;    // Laenge der gewaehlten Nummer
unsigned char nummer[20];        // gewaehlte Nummer
String textNummer="";
bool intro_played=false;

void setup() {
  randomSeed(analogRead(0));
  WiFi.mode(WIFI_OFF);
  btStop();
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, 20, 21);
  Serial.println();
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!myMP3.begin(Serial1, /*isACK = */true, /*doReset = */true)) {  //Use serial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
  Serial.println(F("DFPlayer Mini online."));
  myMP3.setTimeOut(500); //Set serial communictaion time out 500ms

  Serial.println("start Setting volume to 20");
  myMP3.volume(20); // 0..30
  myMP3.EQ(DFPLAYER_EQ_NORMAL);
  myMP3.outputDevice(DFPLAYER_DEVICE_SD);
  delay(20);
  pinMode(Telefonpin,INPUT_PULLUP);
  pinMode(mp3busypin, INPUT); 
}
typedef enum {ruhe,impuls0,impuls1,naechsteZiffer,NummerAuswerten} wZustandtyp;
wZustandtyp wZustand = ruhe;
unsigned long startzeit;
unsigned long dauer;

void playmp3(int folder,int file){
  bool fertig = false;
  myMP3.playFolder(folder,file);
  delay(100);
  while (!fertig) {
     if (digitalRead(mp3busypin) == HIGH) {
        fertig = true;
    }
    delay(100);
  }
}

void playnumber(String myNummer){
  for (int i = 0; i < myNummer.length(); i++) {
    int zahl=myNummer.substring(i, i+1).toInt();
    Serial.println(zahl);
    playmp3(2,zahl);
    delay(200);
  }
}

void loop() {
  static unsigned char wImpulse = 0; // Impulszaehler
  static unsigned long wMillis;      // Zeitmarken fuer TimeOuts
  static unsigned int wahl=0;        // Nummer als Integer
  bool freizeichen_played=false;
  switch (wZustand){
    case ruhe:                   // auf Impulse warten
      Serial.println("ruhe");
      if (!intro_played){
        playmp3(1,1);
      }
      myMP3.playFolder(1,2);
      delay(100);
      while (!freizeichen_played) {
        if(digitalRead(Telefonpin)){ // Waehlscheibe betaetigt?
          wZustand = impuls0;
          wImpulse = 0;
          nummerLaenge=0;
          myMP3.pause();
          Serial.println("Waehlscheibe betaetigt?");
          delay(20); // Entprellen
          break;
        }
        if (digitalRead(mp3busypin) == HIGH) {
          Serial.println("freizeichen fertig!");
          freizeichen_played = true;
          wZustand = NummerAuswerten;
        }
      }
      break;
    case impuls0:                     // Waehlscheibenkontakt geschlossen
      if(!digitalRead(Telefonpin)){ // Impuls Waehlscheibenkontakt geoeffnet?
        wMillis=millis(); // Zeitmarke setzen
        wImpulse++; // Impuls zaehlen
        wZustand = impuls1;
        Serial.println("impulse0");
        delay(20);  // Entprellen
      }
      break;
    case impuls1:                     // Waehlscheibenkontakt offen
      if(digitalRead(Telefonpin)){ // Kontakt wieder geschlossen?
        Serial.println("impulse1");
        delay(20);  // Entprellen
        if(digitalRead(Telefonpin)){ // Kurze Impulse ignorieren
          wZustand = impuls0;
        }
      }
      if(millis()-wMillis > Ziffer_Timeout){ // kein weiterer Impuls
        Serial.print(wImpulse);
        wMillis=millis(); // Zeitmarke setzen
        wZustand = naechsteZiffer;
        nummer[nummerLaenge++]=wImpulse%10; // 10 Impulse -> Ziffer 0
      }
      break;
    case naechsteZiffer:
      if(digitalRead(Telefonpin)){  // Waehlscheibe betaetigt?
        Serial.println("naechsteZiffer");
        wZustand = impuls0;
        wImpulse = 0;
        delay(20);  // Entprellen
      }
      if(millis()-wMillis > Wahl_Timeout){ // kein weitere Ziffer
        wZustand=NummerAuswerten;
        Serial.print(F("\nGewaehlte Nummer: "));
        wahl=0;
        textNummer="";
        for(unsigned char i=0;i<nummerLaenge;i++){
          Serial.print(nummer[i]);
          wahl=wahl*10 + nummer[i];
          textNummer+= (char)('0'+nummer[i]); // Werte in ASCII-Zeichen umwandeln
        }
        Serial.println();
        Serial.println(textNummer);
      }
      break;
    case NummerAuswerten: // Hier sollte jetzt die Aktion mit der gewaehlten Nummer passieren
      Serial.println("play nummern text ");
      playmp3(1,6);
      Serial.println("play nummern text ");      
      playnumber(textNummer);
      int summe = 0;
      if (textNummer.length() == 4 ) {
        int zahl12 = textNummer.substring(0, 2).toInt();
        int zahl34 = textNummer.substring(2, 4).toInt();
        Serial.println(zahl12);
        Serial.println(zahl34);
        summe = zahl12 + zahl34;
        Serial.println(summe);        
      }
      if(summe==100){
        Serial.println(F("Richtig"));
        playmp3(1,3);
        int code12 = random(51, 100);
        int code34 = 150-code12;
        String code = String(code12) + String(code34);
        playnumber(code);
      } else {
        Serial.println("Falsch");
        playmp3(1,4);
      }
      //wZustand=ruhe;
      playmp3(1,5);
      playmp3(1,7);
      delay(2000);
      playmp3(1,7);
      myMP3.sleep();
      // Sleep forever
      esp_deep_sleep_start();
      break;
  }
}