/* GeoCache-Telefon V1.0 (cc-by-sa) Oliver Mezger 27.12.21 */
 
//#include <SoftwareSerial.h>
//#include <DFPlayerMini_Fast.h>
 
#define Telefonpin 4        // zum Telefon
#define Ziffer_Timeout 200   // nach dieser Zeit kommt kein weiterer Impuls mehr
#define Wahl_Timeout 2000    // nach dieser Zeit wird keine Ziffer mehr gewaehlt
 
 
//SoftwareSerial mySerial(20, 21); // RX, TX
//DFPlayerMini_Fast myMP3;
unsigned char nummerLaenge=0;    // Laenge der gewaehlten Nummer
unsigned char nummer[20];        // gewaehlte Nummer
String textNummer="";
String loesung="123123"; // richtige Nummer
 
void setup() {
  Serial.begin(115200);
  //mySerial.begin(9600);
  //myMP3.begin(mySerial);
  Serial.println("start");
  Serial.println(F("Setting volume to 20"));
  //myMP3.volume(20); // 0..30
  delay(20);
  pinMode(Telefonpin,INPUT_PULLUP);
}
typedef enum {ruhe,impuls0,impuls1,naechsteZiffer,NummerAuswerten} wZustandtyp;
wZustandtyp wZustand = ruhe;
unsigned  long startzeit;
unsigned  long dauer;
void loop() {
  static unsigned char wImpulse = 0; // Impulszaehler
  static unsigned long wMillis;      // Zeitmarken fuer TimeOuts
  static unsigned int wahl=0;        // Nummer als Integer
  switch (wZustand){
    case ruhe:                   // auf Impulse warten
      if(digitalRead(Telefonpin)){ // Waehlscheibe betaetigt?
        wZustand = impuls0;
        wImpulse = 0;
        nummerLaenge=0;
        Serial.println("Waehlscheibe betaetigt?");
        //myMP3.pause();
        delay(20); // Entprellen
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
      if(textNummer==loesung){
        Serial.println(F("Richtig"));
        //myMP3.playFromMP3Folder(2); // spiele Stueck
      } else {
        Serial.println(F("Falsch"));
        //myMP3.playFromMP3Folder(1); // spiele Stueck
      }
      wZustand=ruhe;
      break;
  }
}