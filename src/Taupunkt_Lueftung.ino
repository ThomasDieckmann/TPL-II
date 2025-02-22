/* 

Original bei Heise
Upgrade by Thomas Dieckmann ;-)


Entwicklungs-Plattform: Arduino IDE 1.8.19 / Linux

Pinbelegung Arduino Nano:
=========================

D0   RX                   D11  opt. MOSI
D1   TX                   D12  opt. MISO
D2   Timer_Knopf          D13  opt. SCK/LED f. Serial ACK
D3                        A0   Randomizer (frei)
D4   DHT außen            A1   
D5   DHT innen            A2   Poti
D6   Relais1              A3
D7   Relais2              A4   I2C SDA
D8   MODE: Timer/Auto     A5   I2C SCL
D9   MODE: Timer&Sensor   A6
D10  opt. CS              A7



HELP: https://jsonformatter.org/#

"Innen":{"Temperature":7.86,"Humidity":40.86,"DewPoint":-4.60},"Aussen":{"Temperature":15.86,"Humidity":45.86,"DewPoint":4.23},"Timer":{"Preset":300,"Remain":0},"Actuators":{"Relay1":"OFF","Relay2":"OFF"},"Mode":"Combi2","TempUnit":"C","TimeUnit":"seconds","Version":"1.5.7"}

==>
=== test:     
*/

#define PRG_VERSION               "2.0.5"
#define EEPROM_ID_WERT            1     // sozusagen eine Sub-Version um zu prüfen, ob Settings aus dem EEPROM gelesen werden sollen
#define EEPROM_BANK               0     // Speicher-Seite, damit die Daten woanders liegen können um den EEPORM-Speicher zu schonen
#define EEPROM_BANKGROESSE        16    // definiert die Größe der Speicherbank (derzeit 16)

// l10n stuff
#define DE 1
#define EN 2
#define LANGUAGE DE

/* Commands:
 *  B   press button
 *  0   timer = zero
 *  J   send JSON
 *  P   ping
 *  RR  reset 328P chip
 */
/*
   TODO/Changelog
   ==============

   Todo:
   - optimizing Display outputs (obvious after starting, with lange page refresh interval)
   - password-option, block setup (Pfeil zurück Seite 2 Zeile 1 Symbol 15)
   - Bugs: A/H command don't work properly

   
   V2.0.5 v. 17.01.2024:
   ---------------------
   - remove password routine, i'll decide later, if it'll be needed, when it's ready...

   V2.0.4 v. 17.01.2024:
   ---------------------
   - Bugs: Offset/settings for inner temp / outer temp

   V2.0.3 v. 17.01.2024:
   ---------------------
   - some changes for Config/commands

   V2.0.2 v. 16.01.2024:
   ---------------------
   - accept commands and use switch-structure... BOJPR
   
   V2.0.1 v. 16.01.2024:
   ---------------------
   - accept O via Serial as reset Timer to zero
   - accept R via Serial as Reset, has to be repeated

   V2.0.0 v. 16.01.2024:
   ---------------------
   - accept B via Serial as BUTTON
   
   V1.5.8 v. 13.01.2024:
   ---------------------
   - interprete any serial input from 8266 as Trigger

   V1.5.7 v. 12.01.2024:
   ---------------------
   - simplified JSON output

   V1.5.6 v. 03.10.2023:
   ---------------------
   - bug: temperature calibration

   V1.5.5 v. 01.10.2023:
   ---------------------
   - möglicher Fehler bei Temperatur-Kalibrierung => zunächst bei Dummy-Sensor-Werten
     Fix-Temperatur und dann Suche nach Codestellen, wo es haken könnte
     => vermutlich durch Groß/Kleinschreibung gelöst
   
   V1.5.4 v. 25.11.2022:
   ---------------------
   - l10n: DE & EN for german and english

   V1.5.3 v. 24.11.2022:
   ---------------------
   - EEPROM-BANK Größe verwenden, damit später vergrößert und das Passwort
     abgespeichert werden kann

   V1.5.2 v. 23.11.2022:
   ---------------------
   - Zusammenfassung von Textausgaben mit Sonderzeichen (oktal) spart Speicherplatz
   - Funktion für Grad-Celsius-Anzeige besser genutzt, spart auch Speicher
   - Ersatz ß gegen ss in Übergabestring für Tasmota Serial2MQTT-Bridge,
     JSON wird nun sauber erkannt
   - einfache serielle Debugging Ausgaben nur noch, wenn kein JSON ausgegeben wird
   - Vorbereitung größere EEPROM-BANK für Passwort etc.

   V1.5.1 v. 16.11.2022:
   ---------------------
   - Auslagerung ja/nein-Funktion, die wird häufiger benötigt

   V1.5.0 v. 11.11.2022:
   ---------------------
   - Einbau Speicherroutine unter Verwendung verschiedener Speicherblöcke um EEPROM zu schonen

   V1.4.9 v. 11.11.2022:
   ---------------------
   - Ausgabe sämtlicher Einstellungsmöglichkeiten => Menue
   - Speichern erst einmal deaktiviert, Speicherroutine muss noch codiert werden

   V1.4.8 v. 10.11.2022:
   ---------------------
   - Setup-Routine für Hysterese, Differenz, Sensor-Korrektur-Werte
     derzeit realisiert: Schalt-Differenz und Hysterese
                         Speichern im EEPROM

   V1.4.7 v. 09.11.2022:
   ---------------------
   - Bugfix: Feuchte-Update bei jedem Durchlauf auf Seite 1, alt-Wert wurde nicht gesetzt

   V1.4.6 v. 09.11.2022:
   ---------------------
   - Ausgabe Sensoren-Check von 1+2 auf Innen/Außen geändert
   - 4. Betriebsart = Kombi-2, Also erst Timer, dann Auto
   - Bei Änderung der Betriebsart wird Seite2 angezeigt
   - Bugfix: nach Wechsel auf Seite 1 wird bsp. 1° bzw. 1% zu viel angezeigt
     (Vorsicht bei ++ Verwendung! a = b++; ist nicht immer gewollt!)
   
   V1.4.5 v. 27.08.2022:
   ---------------------
   - Korrektur der Sekundenanzeige (cast: long bei JSON-String)   ok, beide Teile des Produktes gecastet... dann passt's.
   
   V1.4.4 v. 26.08.2022:
   ---------------------
   - optionale serielle Ausgabe als JSON-String per #define       ok, per define JSON_OUTPUT

   V1.4.3 v. 26.08.2022:
   ---------------------
   - Dummy-Sensoren einzeln per #DEFINE
   
   V1.4.2 v. 25.08.2022:
   ---------------------
   - Komplizierten Wechsel-Murks der Animation wegmachen           ok, umgestellt auf Zähler und index
   - Eigenen Timer dafür einbauen, wie beim Seitenwechsel          ok
   - Zugriff per char(symbol_zaehler) (0 bis MAXSYMBOLS)           ok, siehe oben
     dann braucht es rest_laufzeit_fuenftel oder wasauchimmer
     nicht mehr.
   - elegantere Lösung mit den #ifdefs finden vielleicht kann      ok, #if a == b...
     das nach dem #ifdef mit einfachen ifs machen?
   - ein paar Animationen hinzufügen                               ok
*/

// Dieser Code benötigt zwingend die folgenden Libraries:
#include "DHT.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <avr/wdt.h>
#include <EEPROM.h>


#if LANGUAGE == DE
   #include "l10n/constants_de.h"
#elif LANGUAGE == EN
   #include "l10n/constants_en.h"
#endif




// #define DISPLAYTEST                          // Testen des LCD Character-Set
// #define DEBUG                                // eventuell bleibt etwas Test-Code übrig
#define WRITE_EEPROM                            // Aktiviere EEPROM Schreib-Routine
//#define DUMMYSENSOR_I                           // wenn man keine Sensoren hat, hilft der Zufall
//#define DUMMYSENSOR_A                           // wenn man keine Sensoren hat, hilft der Zufall
#define SYMBOLSET                  3            // verschiedene Symbolsets möglich (für Animation)
// ***************  Arduino-Pin-Definitionen (siehe oben)  **************
#define TIMER_KNOPF_PIN            2            // Eingang (interner) Pull-Up für Auslösung Timer
#define DHTPIN_AUSSEN              4            // Datenleitung für den DHT-Sensor 2 (außen)
#define DHTPIN_INNEN               5            // Datenleitung für den DHT-Sensor 1 (innen)
#define RELAIS1_PIN                6            // Anschluss des Lüfter-Relais #1
#define RELAIS2_PIN                7            // Anschluss des Lüfter-Relais #2
#define TIMER_AUTO_PIN             8            // Schalter Timer/Auto
#define KOMBI_PIN                  9            // Timer und Sensor kombiniert (interner Pullup)
#define POTI_PIN                   A2           // Pin für das Timer-Poti die 10bit werden umgerechnet auf 1-25
#define ACK_PIN                    13           // Quittierungs-Pin
//  *****************  für bessere Wartbarkeit des Codes  ***************
#define FORCE                      true         // erzwingen der Ausgabe wg. Wechsel der Seite
#define RELAIS_EIN                 LOW          // Das Relais wird geschaltet, wenn der Eingang auf TTL LOW liegt
#define RELAIS_AUS                 HIGH
#define MODE_TIMER                 LOW          // wenn LOW, dann ist der Lüfter aus oder vom Timer gesteuert
#define MODE_KOMBI                 LOW          // wenn LOW, dann kombiniere Timer & Sensor-Betrieb
#define DHTTYPE_INNEN              DHT22        // DHT 22 innen
#define DHTTYPE_AUSSEN             DHT22        // DHT 22 außen
#define SENSOR_INTERVALL           3            // in Sekunden, Pausen zwischen den Auslesezyklen für die Sensoren
#define SEITEN_INTERVALL           7            // in Sekunden, Wartezeit auf Seitenwechsel
// *************  Korrekturwerte der einzelnen Sensorwerte  *************
#define KORREKTUR_TEMP_INNEN      -1            // Korrekturwert Innensensor Temperatur
#define KORREKTUR_TEMP_AUSSEN     -1            // Korrekturwert Außensensor Temperatur
#define KORREKTUR_FEUCHTE_INNEN    0            // Korrekturwert Innensensor Luftfeuchtigkeit
#define KORREKTUR_FEUCHTE_AUSSEN   0            // Korrekturwert Außensensor Luftfeuchtigkeit
// *******************  weitere wichtige Konstanten  ********************
#define SCHALT_MIN                 2.5          // minimaler Taupunktunterschied, bei dem das Relais schaltet
#define HYSTERESE                  1.0          // Abstand von Ein- und Ausschaltpunkt
#define TEMP_INNEN_MIN            10.0          // Minimale Innentemperatur, bei der die Lüftung aktiviert wird
#define TEMP_AUSSEN_MIN          -10.0          // Minimale Außentemperatur, bei der die Lüftung aktiviert wird
#define MAX_MILLIS                 4294967295   // millis() liefert den Datentyp unsigned long, also 32 bit
#define MAX_EINSTELLWERTE          9            // Anzahl der Werte, die angesehen und verändert werden können
// **************************  Betriebsarten  ****************************
#define BETRIEBSART_AUTO           0
#define BETRIEBSART_TIMER          1
#define BETRIEBSART_KOMBI_1        2
#define BETRIEBSART_KOMBI_2        3
// ******************  Optionen für Eingaberoutine  **********************                    
#define OPTION_GRAD                0
#define OPTION_FEUCHTE             1
#define OPTION_SEKUNDEN            2
#define MAX_PASSWORT_LAENGE        6
// ******************  alles nur für den JSON OUTPUT  ********************
#define JSON_OUTPUT                             // muss nur definiert sein. Sendet die Daten als JSON-String


// allgemeine Daten
float Feuchte_innen                =  0,           // Innen-Luftfeuchtigkeit, voerher H1
      Temperatur_innen             =  0,           // Innen-Tmperatur, vorher T1
      Feuchte_aussen               =  0,           // Außen-Luftfeuchtigkeit, vorher H2
      Temperatur_aussen            =  0,           // Außen-Temperatur, vorher T2
      Taupunkt_innen               =  0,           // Innen-Taupunkt
      Taupunkt_aussen              =  0,           // Außen-Taupunkt
      Feuchte_innen_alt            =  0,           // Innen-Luftfeuchtigkeit ALT
      Temperatur_innen_alt         =  0,           // Innen-Temperatur ALT
      Feuchte_aussen_alt           =  0,           // Außen-Luftfeuchtigkeit ALT
      Temperatur_aussen_alt        =  0,           // Außen-Temperatur ALT
      Taupunkt_innen_alt           =  0,           // Innen-Taupunkt ALT
      Taupunkt_aussen_alt          =  0,           // Außen-Taupunkt ALT
      DeltaTP                      =  0,           // Differenz der Taupunkte
      DeltaTP_alt                  =  0;           // Differenz der Taupunkte ALT

// beeinflusst das Schaltverhalten und Intervalle
float schalt_min                   =  SCHALT_MIN,  // minimale Differenz der Taupunkte für Aktivieren der Lüftung
      hysterese                    =  HYSTERESE,
      korrektur_temp_innen         =  KORREKTUR_TEMP_INNEN,
      korrektur_temp_aussen        =  KORREKTUR_TEMP_AUSSEN,
      korrektur_Feuchte_innen      =  KORREKTUR_FEUCHTE_INNEN,
      korrektur_Feuchte_aussen     =  KORREKTUR_FEUCHTE_AUSSEN,
      temp_innen_min               =  TEMP_INNEN_MIN,
      temp_aussen_min              =  TEMP_AUSSEN_MIN;
      
byte  sensor_intervall_byte        =  SENSOR_INTERVALL,
      seiten_intervall_byte        =  SEITEN_INTERVALL;
     
/*    // Vorbereitung um in einer der nächsten Versionen das Flackern bei der Setup-Anzeige zu deaktivieren
      schalt_min_alt               =  -666.0,
      hysterese_alt                =  -666.0,
      korrektur_temp_innen_alt     =  -666.0,
      korrektur_temp_aussen_alt    =  -666.0,
      korrektur_Feuchte_innen_alt  =  -666.0,
      korrektur_Feuchte_aussen_alt =  -666.0,
      temp_innen_min_alt           =  -666.0,
      temp_aussen_min_alt          =  -666.0; */

// weitere Variablen
byte  poti_wert                    =  0,          // Auslesewert POTI_PIN mit Mapping 1-25
      seite                        =  1,          // Display kann verschiedene Seiten anzeigen: Messwerte, Lüfter-Status etc.
      seite_alt                    = -9,          // Speicher, um Seitenwechsel zu prüfen, nur dann wird neu angezeigt
      symbol_zeiger                =  0,          // Welches Symbol wird gerade für Timer-Lüfter-Tätigkeit dargestellt
      betriebsart                  =  0,          // 0 = Auto, 1 = Timer, 2 = Kombi-1, 3 = Kombi-2
      betriebsart_alt              =  0,          // Für Umschaltung der Betriebsart => Seite2 aktivieren
      menue_eintrag                =  0,          // für die Setup-Routine
      menue_eintrag_alt            =  0,          // für die Setup-Routine (Veränderung)
      ja_nein_poti,                               // Auslesewert bei JaNein Frage
      ja_nein_poti_alt;                           // Auslesewert bei JaNein Frage

      
bool  rel                          =  RELAIS_AUS,
      rel_alt                      =  RELAIS_EIN,
      fehler                       =  true,
      setup_verlassen              =  false,
      stringComplete               =  false,
      zeichen_gelesen              =  false,
      erstes_R_gelesen             =  false;

char  inChar;

// Definitionen für Timer-Funktion, nach 49 Komma irgendwas Tagen springt millis zurück auf 0.
// Nirgendwo die 42, wie langweilig ;-)
// CAVE:
// Es gibt verschiedene Timer im Programm: a) Lüfter-Laufzeit
//                                         b) Programm-Timer für verschiedene Funktionen
unsigned long seitentimer_start          = millis(),  // Startwert für den Timer1
              timer_sensors              = millis(),  // Startwert für Sensor-Messung
              timer_symbol               = millis(),  // Startwert für Symbol
              timer_luefter_start_millis = millis(),  // Startwert für Lüfterbetrieb im Timer-Modus
              rest_laufzeit_sekunden     = 0;         // Restlaufzeit des Lüfter-Timers
unsigned int  timer_minuten              = 0,         // Minutenwert für Timer, Einstellwert via Poti
              timer_minuten_start        = 0,         // der Wert wird bei Knopfdruck im Timer-Modus gesetzt
              timer_minuten_alt          = 0;         // Minutenwert für Timer_ALT

DHT dht_innen(DHTPIN_INNEN, DHTTYPE_INNEN);     // Der Innensensor wird ab jetzt mit dht_innen angesprochen
DHT dht_aussen(DHTPIN_AUSSEN, DHTTYPE_AUSSEN);  // Der Außensensor wird ab jetzt mit dht_aussen angesprochen

LiquidCrystal_I2C lcd(0x27,20,4);               // LCD: I2C-Addresse und Displaygröße setzen



// Initialisierung, Tests etc.
void setup() {
  
   pinMode(TIMER_KNOPF_PIN,  INPUT_PULLUP);   // Damit wird der Timer aktiviert
   pinMode(TIMER_AUTO_PIN,   INPUT_PULLUP);   // Betriebsmodus Timer/Auto
   pinMode(KOMBI_PIN,        INPUT_PULLUP);   // Betriebsmodus Timer & Sensor
   pinMode(RELAIS1_PIN,      OUTPUT);         // Relaispin als Output definieren
   pinMode(RELAIS2_PIN,      OUTPUT);         // Relaispin als Output definieren
   pinMode(ACK_PIN,          OUTPUT);         // Quittierungs-Pin als Output definieren
   digitalWrite(RELAIS1_PIN, RELAIS_AUS);     // Relais ausschalten
   digitalWrite(RELAIS2_PIN, RELAIS_AUS);     // Relais ausschalten

   for (int i=0;i<10;i++) {
      digitalWrite(ACK_PIN, HIGH);
      delay(30);
      digitalWrite(ACK_PIN,  LOW);
      delay(40);
   }
  
   Serial.begin(115200);                   // Serielle Ausgabe, falls noch kein LCD angeschlossen ist
   //inputString.reserve(100);               // 100 Bytes Input buffer

   #ifndef JSON_OUTPUT
      Serial.println(F(TXT_CONST_TEST_SENSORS));
      Serial.print(F(TXT_CONST_EEPROM_SIZE));
      Serial.println(EEPROM.length());
      //Serial.println(EEPROM.read(0));
   #endif

   lcd.init(); lcd.backlight();

   #if SYMBOLSET == 1   // vielleicht geht das anders?
      byte segment0[8] = {B01110,B10111,B10111,B10101,B10001,B10001,B01110,B00000};
      byte segment1[8] = {B01110,B10001,B10001,B10101,B10111,B10111,B01110,B00000};
      byte segment2[8] = {B01110,B10001,B10001,B10101,B11101,B11101,B01110,B00000};
      byte segment3[8] = {B01110,B11101,B11101,B10101,B10001,B10001,B01110,B00000};
      #define MAX_SYMBOLE         4   // Es sind die Symbole 0-3 vorhanden, bis zu 8 sind möglich
      #define SYMBOL_INTERVALL  100   // Alle 100ms ein neues Symbol
      
   #elif SYMBOLSET == 2
      byte segment0[8] = {B00000,B00000,B00000,B00100,B00000,B00000,B00000,B00000};
      byte segment1[8] = {B00000,B00000,B01110,B01110,B01110,B00000,B00000,B00000};
      byte segment2[8] = {B00000,B11111,B11111,B11111,B11111,B11111,B00000,B00000};
      byte segment3[8] = {B00000,B00000,B01110,B01110,B01110,B00000,B00000,B00000};
      #define MAX_SYMBOLE         4   // Es sind die Symbole 0-3 vorhanden
      #define SYMBOL_INTERVALL  100   // Alle 100ms ein neues Symbol

   #elif SYMBOLSET == 3
      byte segment0[8] = {B00000,B00100,B00100,B00100,B00100,B00100,B00000,B00000};
      byte segment1[8] = {B00000,B00001,B00010,B00100,B01000,B10000,B00000,B00000};
      byte segment2[8] = {B00000,B00000,B00000,B11111,B00000,B00000,B00000,B00000};
      byte segment3[8] = {B00000,B10000,B01000,B00100,B00010,B00001,B00000,B00000};
      #define MAX_SYMBOLE         4   // Es sind die Symbole 0-3 vorhanden
      #define SYMBOL_INTERVALL   80   // Alle N ms ein neues Symbol

   #elif SYMBOLSET == 4
      byte segment0[8] = {B00000,B00100,B00100,B11111,B00100,B00100,B00000,B00000};
      byte segment1[8] = {B00000,B00010,B11010,B00100,B01011,B01000,B00000,B00000};
      byte segment2[8] = {B00000,B10001,B01010,B00100,B01010,B10001,B00000,B00000};
      byte segment3[8] = {B00000,B01000,B01011,B00100,B11010,B00010,B00000,B00000};
      #define MAX_SYMBOLE         4   // Es sind die Symbole 0-3 vorhanden
      #define SYMBOL_INTERVALL   80   // Alle 100ms ein neues Symbol

   #elif SYMBOLSET == 5
      byte segment0[8] = {B00000,B11000,B11000,B00000,B00000,B00000,B00000,B00000};
      byte segment1[8] = {B00000,B00011,B00011,B00000,B00000,B00000,B00000,B00000};
      byte segment2[8] = {B00000,B00000,B00000,B00000,B00011,B00011,B00000,B00000};
      byte segment3[8] = {B00000,B00000,B00000,B00000,B11000,B11000,B00000,B00000};
      #define MAX_SYMBOLE         4   // Es sind die Symbole 0-3 vorhanden
      #define SYMBOL_INTERVALL   80   // Alle 100ms ein neues Symbol

   #elif SYMBOLSET == 6
      byte segment0[8] = {B00000,
                          B00000,
                          B00000,
                          B00000,
                          B00000,
                          B00000,
                          B00000,
                          B00000};

      byte segment1[8] = {B00000,
                          B00000,
                          B00000,
                          B00000,
                          B00000,
                          B00000,
                          B00000,
                          B00000};

      byte segment2[8] = {B00000,
                          B00000,
                          B00000,
                          B00000,
                          B00000,
                          B00000,
                          B00000,
                          B00000};

      byte segment3[8] = {B00000,
                          B00000,
                          B00000,
                          B00000,
                          B00000,
                          B00000,
                          B00000,
                          B00000};
      #define MAX_SYMBOLE         4   // Es sind die Symbole 0-3 vorhanden
      #define SYMBOL_INTERVALL   70   // Alle 100ms ein neues Symbol

   #endif

   // definiere Symbole für Animation
   #ifdef SYMBOLSET
      lcd.createChar(0, segment0);
      lcd.createChar(1, segment1);
      lcd.createChar(2, segment2);
      lcd.createChar(3, segment3);   
   #endif
   
   #ifdef DISPLAYTEST
      display_test();
   #endif

   anzeige_programm_version();
   anzeige_eeprom_status();
   werte_anzeige_und_einstellungen();
   lcd.clear();
   initialisiere_sensoren();
   lese_sensoren();    //test für bessere ausgebe
   lcd.clear();
   pruefe_sensoren();  //test für bessere ausgebe
   lcd.clear();

   seitentimer_start = millis();
}


void loop() {

   wdt_reset();  // Watchdog zurücksetzen

   // Sensoren nur alle x Sekunden lesen
   if (zeit_differenz(timer_sensors) > long(sensor_intervall_byte) * 1000) {
      lese_sensoren();
      pruefe_sensoren();
      reboot_bei_fehler();
      errechne_beide_taupunkte();
      timer_sensors=millis();
      serial_ausgabe_werte();
   }

   // alle paar Sekunden wird die Seite umgeschaltet
   if (zeit_differenz(seitentimer_start) > long(seiten_intervall_byte) * 1000) {
      seite_alt=seite;
      rel_alt = !rel;
      seite++;
      timer_minuten_alt = 0; // damit bei Wechsel auf Seite 2 auch der Wert aktualisiert wird
      seitentimer_start = millis();  // Interner Timer = Start
      #ifdef DEBUG
         if (seite > 2) seite = 2; // DEBUGGING only
      #else
         if (seite > 2) seite = 1;
      #endif
   }

   // alle paar MilliSekunden wird das Symbol umgeschaltet
   if (zeit_differenz(timer_symbol) > SYMBOL_INTERVALL) {
      symbol_zeiger++;
      if (symbol_zeiger > MAX_SYMBOLE - 1) symbol_zeiger = 0;
      timer_symbol = millis();  // Interner Timer = Start
   }

  
   if (seite_alt != seite) {
      display_ausgabe_maske();
      display_ausgabe_werte(FORCE);
      seite_alt=seite;
   } else {
      display_ausgabe_werte(false);
   }

   interrupt_delay_routine(100); // Zeit um das Display zu lesen

   // wenn man am Timer-Knopf dreht, möchte man nicht erst auf Wechsel der Seite warten
   if (timer_minuten != timer_minuten_alt) {
      seite = 2;
      seitentimer_start = millis();  // Interner Timer = Start      
   }

   relais_schalten();

   // bei Timer-Betrieb Rotations-Symbol anzeigen
   lcd.setCursor(19,2);
   if (rest_laufzeit_sekunden > 0) lcd.write(char(symbol_zeiger)); else lcd.write(' ');

}


// Display-Test um beispielsweise Zeichen zu finden, die nicht ASCII Norm sind
// z.B. Grad-Zeichen, Umlaute usw.
void display_test() {
   lcd.clear();
   lcd.setCursor(0,0); for (int i=33; i<53; i++)   lcd.write(char(i)); 
   lcd.setCursor(0,1); for (int i=53; i<73; i++)   lcd.write(char(i));
   lcd.setCursor(0,2); for (int i=73; i<93; i++)   lcd.write(char(i));   
   lcd.setCursor(0,3); for (int i=93; i<113; i++)  lcd.write(char(i));
   while (!knopf_gedrueckt()) delay (100);
   delay(400);
   lcd.setCursor(0,0); for (int i=113; i<133; i++) lcd.write(char(i)); 
   lcd.setCursor(0,1); for (int i=133; i<153; i++) lcd.write(char(i));
   lcd.setCursor(0,2); for (int i=153; i<173; i++) lcd.write(char(i));   
   lcd.setCursor(0,3); for (int i=173; i<193; i++) lcd.write(char(i));
   while (!knopf_gedrueckt()) delay (100);
   delay(400);
   lcd.setCursor(0,0); for (int i=193; i<213; i++) lcd.write(char(i)); 
   lcd.setCursor(0,1); for (int i=213; i<233; i++) lcd.write(char(i));
   lcd.setCursor(0,2); for (int i=233; i<253; i++) lcd.write(char(i));   
   lcd.setCursor(0,3); for (int i=173; i<193; i++) lcd.write(char(127));
   while (!knopf_gedrueckt()) delay (100);
   delay(400);
   lcd.clear();  
}


void anzeige_programm_version() {
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print(F(TXT_CONST_DEWPOINT_VENT));
   lcd.setCursor(0,1);
   lcd.print(F(TXT_CONST_UNDERLINE_PRG));
   lcd.setCursor(3,2);
   lcd.print(F("Version: "));
   lcd.print(F(PRG_VERSION));
   lcd.setCursor(5,3);
   lcd.print(F(__DATE__));
   #ifndef DEBUG
      delay(2500);
   #endif
}


void anzeige_eeprom_status() {
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print(F("INFO:"));
   lcd.setCursor(0,1);
   lcd.print(F(TXT_CONST_EEPROM_SETTINGS));
   lcd.setCursor(0,2);

   if (EEPROM.read(EEPROM_BANK * EEPROM_BANKGROESSE) != EEPROM_ID_WERT) {
      lcd.print(F(TXT_CONST_SETTINGS_N_FOUND));
      lcd.setCursor(0,3);
      lcd.print(F(TXT_CONST_LOAD_DEFAULTS));
   } else {
      // Offset, der vor dem Speichern addiert wurde, wird nun abgezogen, halbiert wegen Reverse-Rechnung
      // Da die EEPROM-Zellen nur jeweils ein Byte speichern, wird da immer wieder umgerechnet.
      lcd.print(F(TXT_CONST_LOADING_SETTINGS));
      schalt_min               = float(EEPROM.read( 1 + EEPROM_BANK * EEPROM_BANKGROESSE)) / 2.0;
      hysterese                = float(EEPROM.read( 2 + EEPROM_BANK * EEPROM_BANKGROESSE)) / 2.0;
      korrektur_temp_innen     = float(EEPROM.read( 3 + EEPROM_BANK * EEPROM_BANKGROESSE)) / 2.0 - 10.0;
      korrektur_temp_aussen    = float(EEPROM.read( 4 + EEPROM_BANK * EEPROM_BANKGROESSE)) / 2.0 - 10.0;
      korrektur_Feuchte_innen  = float(EEPROM.read( 5 + EEPROM_BANK * EEPROM_BANKGROESSE)) / 2.0 - 15.0;
      korrektur_Feuchte_aussen = float(EEPROM.read( 6 + EEPROM_BANK * EEPROM_BANKGROESSE)) / 2.0 - 15.0;
      temp_innen_min           = float(EEPROM.read( 7 + EEPROM_BANK * EEPROM_BANKGROESSE)) / 2.0 - 15.0;
      temp_aussen_min          = float(EEPROM.read( 8 + EEPROM_BANK * EEPROM_BANKGROESSE)) / 2.0 - 15.0;
      seiten_intervall_byte    = float(EEPROM.read( 9 + EEPROM_BANK * EEPROM_BANKGROESSE)) / 2.0 - 15.0;
      sensor_intervall_byte    = float(EEPROM.read(10 + EEPROM_BANK * EEPROM_BANKGROESSE)) / 2.0 - 15.0;
      lcd.setCursor(0,3);
      lcd.print(F("OK."));
   }
   if (knopf_gedrueckt()) {
      delay(750);
   } else {
      delay(3000);
   }
}


// Setup-Routine, Anschauen und Werte setzen
// CAVE! Routine muss vor Aktivierung des Watchdog aufgerufen werden.
void werte_anzeige_und_einstellungen() {
   if (knopf_gedrueckt()) {
      lcd.clear();
      lcd.setCursor(0,0);
      // diese 3 Zeilen sind für Passwort-Eingabe vorgesehen
      //lcd.print(F(TXT_CONST_PASSWORD));
      //while (knopf_gedrueckt()) delay(50);
      //eingabe_passwort(10,0);
      
      setup_verlassen = false;
      menue_eintrag_alt = 255;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(F(TXT_CONST_MENUE));
      lcd.setCursor(0,1);
      lcd.print(F(TXT_CONST_UNDERLINE_MENUE));
      while (knopf_gedrueckt()) delay(50);

      while (!setup_verlassen) {
         menue_eintrag = map(analogRead(POTI_PIN),0,1023,0,11);
         if (menue_eintrag != menue_eintrag_alt) {
            menue_eintrag_alt = menue_eintrag;
            lcd.setCursor(0,2);
            lcd.print(F("                    "));
            lcd.setCursor(0,3);
            lcd.print(F("                    "));
            anzeige_eintrag();
         }
         delay(50);
         if (knopf_gedrueckt()) {
            while (knopf_gedrueckt()) delay(50);
            waehle_aktion();
            menue_eintrag_alt = 255; // damit das Menü neu ausgegeben wird
         }
      }
      delay(250);
      while (knopf_gedrueckt()) delay(50);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(F(TXT_CONST_SAVE_VALUES1));
      lcd.setCursor(0,1);
      lcd.print(F(TXT_CONST_SAVE_VALUES2));
      if ( eingabe_ja_nein(11,1) ) {
         lcd.setCursor(0,2);
         lcd.print(F(TXT_CONST_SAVE_TO_EEPROM));
         // Speicherroutine
         // hier wird das Setup in das EEPROM geschrieben

         #ifdef WRITE_EEPROM
            EEPROM.update( 0 + EEPROM_BANK * EEPROM_BANKGROESSE, EEPROM_ID_WERT);
            EEPROM.update( 1 + EEPROM_BANK * EEPROM_BANKGROESSE, byte(schalt_min * 2.0));
            EEPROM.update( 2 + EEPROM_BANK * EEPROM_BANKGROESSE, byte(hysterese  * 2.0));
            EEPROM.update( 3 + EEPROM_BANK * EEPROM_BANKGROESSE, byte(korrektur_temp_innen     * 2.0) + 20);
            EEPROM.update( 4 + EEPROM_BANK * EEPROM_BANKGROESSE, byte(korrektur_temp_aussen    * 2.0) + 20);
            EEPROM.update( 5 + EEPROM_BANK * EEPROM_BANKGROESSE, byte(korrektur_Feuchte_innen  * 2.0) + 30);
            EEPROM.update( 6 + EEPROM_BANK * EEPROM_BANKGROESSE, byte(korrektur_Feuchte_aussen * 2.0) + 30);
            EEPROM.update( 7 + EEPROM_BANK * EEPROM_BANKGROESSE, byte(temp_innen_min           * 2.0) + 30);
            EEPROM.update( 8 + EEPROM_BANK * EEPROM_BANKGROESSE, byte(temp_aussen_min          * 2.0) + 30);
            EEPROM.update( 9 + EEPROM_BANK * EEPROM_BANKGROESSE, byte(seiten_intervall_byte    * 2.0) + 30);
            EEPROM.update(10 + EEPROM_BANK * EEPROM_BANKGROESSE, byte(sensor_intervall_byte    * 2.0) + 30);
            lcd.print(F(TXT_CONST_CONFIG_SAVED));
         #endif
         delay(2000);
      }
   }
}


// Ja-Nein Abfrage
bool eingabe_ja_nein(byte xpos, byte ypos) {
      lcd.setCursor(xpos,ypos);
      ja_nein_poti_alt = 255;
      while(!knopf_gedrueckt()) {
         ja_nein_poti = map(analogRead(POTI_PIN),0,1023,0,10);
         if (ja_nein_poti != ja_nein_poti_alt) {
            ja_nein_poti_alt = ja_nein_poti;
            lcd.setCursor(xpos,ypos);
            if ((ja_nein_poti % 2) == 1) {
               lcd.print(F(TXT_CONST_ANSWER_YES));
            } else {
               lcd.print(F(TXT_CONST_ANSWER_NO));
            }
         }
         delay(50);
      }
      return ((ja_nein_poti % 2) == 1);
}


// Eingabe Passwort
// 97 - 122 und back=127 cursor=94   enter=62
void eingabe_passwort(byte xpos, byte ypos) {
   byte eingabe_position         =  0,
        eingabe_zeichencode      =  0,
        eingabe_zeichencode_alt  =  255;
   bool ende_der_eingabe         =  false;
   
   while (!ende_der_eingabe) {
 //   while (eingabe_position < MAX_PASSWORT_LAENGE) {
      while (!knopf_gedrueckt()) {
         lcd.setCursor(xpos + eingabe_position,ypos);
         eingabe_zeichencode = byte(map(analogRead(POTI_PIN),0,1023,95,123));
         if (eingabe_zeichencode < 97) eingabe_zeichencode = 62;
         if (eingabe_zeichencode > 122) eingabe_zeichencode = 127;
         if (eingabe_zeichencode != eingabe_zeichencode_alt) {
            lcd.setCursor(xpos + eingabe_position,ypos);
            lcd.write(char(eingabe_zeichencode));
            lcd.setCursor(xpos + eingabe_position,ypos+1);
            lcd.write(char(94));
            eingabe_zeichencode_alt = eingabe_zeichencode;
         }
         delay(50);
      }
      lcd.setCursor(xpos + eingabe_position,ypos+1);
      lcd.write(char(32));
      if (eingabe_zeichencode != 127) {
         eingabe_position++;
      } else {
         lcd.setCursor(xpos + eingabe_position,ypos);
         lcd.write(' ');
         eingabe_position--;
         if (eingabe_position == 0) delay(50);
      }
      if (eingabe_position == MAX_PASSWORT_LAENGE) ende_der_eingabe = true;
      if (eingabe_zeichencode == 62) ende_der_eingabe = true;
      eingabe_zeichencode_alt = 255;
      while (knopf_gedrueckt()) delay(100);
   }
   delay(1000);
}


// Menue-Eintrag anzeigen (abhängig von Poti-Wert)
void anzeige_eintrag() {
   lcd.setCursor(0,2);        
   switch (menue_eintrag) {
      case 0:
         lcd.print(F("EXIT"));
         break;
      case 1:
         lcd.print(F(TXT_CONST_THRESHOLD));
         lcd.setCursor(0,3);
         lcd_ausgabe_grad_celsius(schalt_min);
         break;
      case 2:
         lcd.print(F(TXT_CONST_HYSTERESIS));
         lcd.setCursor(0,3);
         lcd_ausgabe_grad_celsius(hysterese);
         break;
      case 3:
         lcd.print(F(TXT_CONST_CORR_TEMP_INSIDE));
         lcd.setCursor(0,3);
         lcd_ausgabe_grad_celsius(korrektur_temp_innen);
         break;
      case 4:
         lcd.print(F(TXT_CONST_CORR_TEMP_OUTSIDE));
         lcd.setCursor(0,3);
         lcd_ausgabe_grad_celsius(korrektur_temp_aussen);
         break;
      case 5:
         lcd.print(F(TXT_CONST_CORR_HUMID_INSIDE));
         lcd.setCursor(0,3);
         lcd.print(korrektur_Feuchte_innen);
         lcd.print(F("%"));
         break;
      case 6:
         lcd.print(F(TXT_CONST_CORR_HUMID_OUTSIDE));
         lcd.setCursor(0,3);
         lcd.print(korrektur_Feuchte_aussen);
         lcd.print(F("%"));
         break;
      case 7:
         lcd.print(F(TXT_CONST_MIN_TEMP_INSIDE));
         lcd.setCursor(0,3);
         lcd_ausgabe_grad_celsius(temp_innen_min);
         break;
      case 8:
         lcd.print(F(TXT_CONST_MIN_TEMP_OUTSIDE));
         lcd.setCursor(0,3);
         lcd_ausgabe_grad_celsius(temp_aussen_min);
         break;
      case 9:
         lcd.print(F(TXT_CONST_PAGE_REFRESH));
         lcd.setCursor(0,3);
         lcd.print(seiten_intervall_byte);
         lcd.print(TXT_CONST_SECONDS);
         break;
      case 10:
         lcd.print(F(TXT_CONST_SENSOR_REFRESH));
         lcd.setCursor(0,3);
         lcd.print(sensor_intervall_byte);
         lcd.print(F(TXT_CONST_SECONDS));
         break;
      case 11:
         lcd.print(F("EXIT"));
         break;
   } 
}


// Menue-Eintrag ändern (abhängig von Poti-Wert)
void waehle_aktion() {
   lcd.setCursor(0,2);        
   switch (menue_eintrag) {
      case 0:
         setup_verlassen = true;
         break;
      case 1:
         schalt_min = einstellen_wert(1,14,0,OPTION_GRAD);
         break;
      case 2:
         hysterese = einstellen_wert(1,8,0,OPTION_GRAD);
         break;
      case 3:
         // hier kann es auch negatve Werte geben.... tricky
         // korrektur_temp_innen = einstellen_wert(0,40,-10.0,OPTION_GRAD); // -10.0 bis 10.0 in 0.5° Schritten
         korrektur_temp_innen = einstellen_wert(0,51,-2.50,OPTION_GRAD);
         break;
      case 4:
         // hier kann es auch negatve Werte geben.... tricky
         // korrektur_temp_aussen = einstellen_wert(0,40,-10.0,OPTION_GRAD); // -10.0 bis 10.0 in 0.5° Schritten
         korrektur_temp_aussen = einstellen_wert(0,51,-2.50,OPTION_GRAD);
         break;
      case 5:
         korrektur_Feuchte_innen = einstellen_wert(0,60,-15.0,OPTION_FEUCHTE);
         break;
      case 6:
         korrektur_Feuchte_aussen = einstellen_wert(0,60,-15.0,OPTION_FEUCHTE);
         break;
      case 7:
         temp_innen_min = einstellen_wert(0,100,10.0,OPTION_GRAD); // 10° bis 20°
         break;
      case 8:
         temp_aussen_min = einstellen_wert(0,250,-10.0,OPTION_GRAD); // -10° bis 15°
         break;
      case 9:
         seiten_intervall_byte = einstellen_wert(2,30,0.0,OPTION_SEKUNDEN);
         break;
      case 10:
         sensor_intervall_byte = einstellen_wert(1,30,0.0,OPTION_SEKUNDEN);
         break;
      case 11:
         setup_verlassen = true;
         break;
   }   
}


// Augabe Leerzeiche + °C auf dem Display
void lcd_ausgabe_grad_celsius (float grad_wert) {
   lcd.print(grad_wert);
   lcd.print(F("\337C"));   // °C
}


// Einstellung von Korrekturwerten und Intervall-Zeiten
float einstellen_wert(int minimalwert, int maximalwert, float offset_wert, byte datentyp) {
   float regler_wert, regler_wert_alt = -1.0;
   delay(100);
   while (!knopf_gedrueckt()) {
      if (datentyp == OPTION_GRAD) {
         // regler_wert = float(map(analogRead(POTI_PIN),0,1023,minimalwert,maximalwert)) / 2.0 + offset_wert; // alt
         regler_wert = float(map(analogRead(POTI_PIN),0,1023,minimalwert,maximalwert)) / 10.0 + offset_wert;
         if (regler_wert != regler_wert_alt) {
            regler_wert_alt = regler_wert;
            lcd.setCursor(10,3);
            lcd.print(F("          "));
            lcd.setCursor(10,3);
            lcd_ausgabe_grad_celsius(regler_wert);
         }
      } else if (datentyp == OPTION_FEUCHTE) {
         regler_wert = float(map(analogRead(POTI_PIN),0,1023,minimalwert,maximalwert)) / 2.0 + offset_wert;                
         if (regler_wert != regler_wert_alt) {
            regler_wert_alt = regler_wert;
            lcd.setCursor(10,3);
            lcd.print(F("          "));
            lcd.setCursor(10,3);
            lcd.print(regler_wert);
            lcd.write('%');
         }
      } else if (datentyp == OPTION_SEKUNDEN) {
         regler_wert = float(map(analogRead(POTI_PIN),0,1023,minimalwert,maximalwert)) + offset_wert;        
         if (regler_wert != regler_wert_alt) {
            regler_wert_alt = regler_wert;
            lcd.setCursor(0,3);
            lcd.print(F("               "));
            lcd.setCursor(0,3);
            lcd.print(int(regler_wert),DEC);
            lcd.print(F(TXT_CONST_SECONDS));
         }
      }
      delay(50);
   }
   while (knopf_gedrueckt()) delay(50);
   return(regler_wert);
}



// Initialisierung und Test der Sensoren
void initialisiere_sensoren() {
   lcd.clear();  
   lcd.setCursor(2,0);
   lcd.print(F(TXT_CONST_TEST_SENSORS)); 
   delay(500);
   wdt_enable(WDTO_8S); // Watchdog timer auf 8 Sekunden stellen, aber erst nach dem Display-Test und dem Menue!

   #ifndef DUMMYSENSOR_A
      dht_aussen.begin(); // Sensor starten
   #endif
   
   #ifndef DUMMYSENSOR_I
      dht_innen.begin(); // Sensor starten
   #endif
}


// Errechne Minuten anhand Poti-Wert
// Grund: Bei dem Poti gibt es nur eine begrenzte Anzahl gut einstellbarer
// Positionen. Und die Intervalle dürfen ja auch zunehmend größer werden.
// Niemand benötigt einen Timer-Wert von 2 Stunden und 14 Minuten.
void errechne_timer_minuten() {
   poti_wert=map(analogRead(POTI_PIN),0,1023,1,21); // die Werte 0 bis 1023 werden auf 1-21 umgerechnet

   if (poti_wert < 4) {
      timer_minuten = poti_wert * 5;            //    5    10    15                ==> Minuten
   } else if (poti_wert < 7) {
      timer_minuten = (poti_wert - 3) * 30;     //   30    60    90                ==> Minuten
   } else if (poti_wert < 10) {
      timer_minuten = (poti_wert - 5) * 60;     //  120   180   240                ==> Stunden
   } else if (poti_wert < 13) {
      timer_minuten = (poti_wert - 7) * 120;    //  360   480   600                ==> Stunden
   } else if (poti_wert < 16) {
      timer_minuten = (poti_wert - 12) * 720;   //  720  1440  2160                ==> Stunden
   } else if (poti_wert < 22) {
      timer_minuten = (poti_wert - 14) * 1440;  // 2880  4320  5760  7200  8640    ==> Tage
   } else {
      timer_minuten = 0;
   }
   
   // Ausgabe TIMER erfolgt nur auf Seite 2
   if (seite == 2) {
      if (timer_minuten != timer_minuten_alt)
      {
         timer_minuten_alt = timer_minuten;
         lcd.setCursor(7,2);
         lcd.print(F("             "));
         lcd.setCursor(7,2);
         if (timer_minuten < 91) {
            lcd.print(timer_minuten);
            lcd.print(F(TXT_CONST_MINUTES));
         } else if (timer_minuten  < 2161) {
            lcd.print((timer_minuten / 60));
            lcd.print(F(TXT_CONST_HOURS));
         } else {
            lcd.print((timer_minuten / 1440));
            lcd.print(F(TXT_CONST_DAYS));
         }
         #ifdef DEBUG
            lcd.setCursor(18,2);
            lcd.print(F("  "));
            lcd.setCursor(18,2);
            lcd.print(poti_wert);
         #endif
      }
   }
}

  
// Sensoren auslesen
void lese_sensoren() {
   #ifndef DUMMYSENSOR_I
      Feuchte_innen = dht_innen.readHumidity()       + korrektur_Feuchte_innen;  // Innenluftfeuchtigkeit auslesen und unter „Feuchte_innen“ speichern
      Temperatur_innen = dht_innen.readTemperature() + korrektur_temp_innen;     // Innentemperatur auslesen und unter „Temperatur_innen“ speichern
   #else
      randomSeed(analogRead(0));
      Feuchte_innen = random(2000,9000) / 100.00;
      randomSeed(analogRead(0));
      Temperatur_innen = random(-1100,2500) / 100.00;

      //Feuchte_innen = 44.0; // war für Fehlerdiagnose (++ Fehler:a = b++; ==> nicht gut!)
      //Temperatur_innen = 10.0 + korrektur_temp_innen; // Fix-Werte für Fehlersuche
   #endif

   #ifndef DUMMYSENSOR_A
      Feuchte_aussen = dht_aussen.readHumidity()       + korrektur_Feuchte_aussen;  // Außenluftfeuchtigkeit auslesen und unter „Feuchte_aussen“ speichern
      Temperatur_aussen = dht_aussen.readTemperature() + korrektur_temp_aussen;     // Außentemperatur auslesen und unter „Temperatur_aussen“ speichern
   #else
      randomSeed(analogRead(0));
      Feuchte_aussen = random(2000,8500) / 100.00;
      randomSeed(analogRead(0));
      Temperatur_aussen = random(-1500,4500) / 100.00;

      //Feuchte_aussen = 40; // war für Fehlerdiagnose
      //Temperatur_aussen = 10.0 + korrektur_temp_aussen; // Fix-Werte für Fehlersuche
   #endif

   #if defined(DUMMYSENSOR_A) || defined(DUMMYSENSOR_I)
      while (Feuchte_innen == Feuchte_aussen) {
         randomSeed(analogRead(0));
         #ifdef DUMMYSENSOR_A
            Feuchte_aussen = random(2000,8500) / 100.00;
         #else
            Feuchte_innen  = random(2000,9000) / 100.00;
         #endif
      }
      while (Temperatur_innen == Temperatur_aussen) {
         randomSeed(analogRead(0));
         #ifdef DUMMYSENSOR_A
            Temperatur_aussen = random(1500,3500) / 100.00;
         #else
            Temperatur_innen  = random(1000,3000) / 100.00;
         #endif
      }
   #endif
}

// Prüfen, ob gültige Werte von den Sensoren kommen
void pruefe_sensoren() {
   if (fehler == true)
   {
      fehler = false; 
      lcd.setCursor(0,1);
      lcd.print(F(TXT_CONST_SENSOR_INSIDE));
      if (isnan(Feuchte_innen) || isnan(Temperatur_innen) || Feuchte_innen > 100.0 || Feuchte_innen < 1.0 || Temperatur_innen < -40.0 || Temperatur_innen > 80.0 ) {
         #ifndef JSON_OUTPUT
            Serial.println(F(TXT_CONST_ERR_SENSOR_INSIDE));
         #endif
         lcd.print(F(TXT_CONST_ERROR));
         fehler = true;
      } else {
         lcd.print(F(TXT_CONST_OK));
      }     
      interrupt_delay_routine(1000);  // Zeit um das Display zu lesen
  
      lcd.setCursor(0,2);
      lcd.print(F(TXT_CONST_SENSOR_OUTSIDE));
      if (isnan(Feuchte_aussen) || isnan(Temperatur_aussen) || Feuchte_aussen > 100.0 || Feuchte_aussen < 1.0 || Temperatur_aussen < -40.0 || Temperatur_aussen  > 80.0) {
         #ifndef JSON_OUTPUT
            Serial.println(F(TXT_CONST_ERR_SENSOR_OUTSIDE));
         #endif
         lcd.print(F(TXT_CONST_ERROR));
         fehler = true;
      } else {
         lcd.print(F(TXT_CONST_OK));
      }
      interrupt_delay_routine(1000);  // Zeit um das Display zu lesen
   }
   if (isnan(Feuchte_innen) || isnan(Temperatur_innen) || isnan(Feuchte_aussen) || isnan(Temperatur_aussen)) fehler = true;
}


void reboot_bei_fehler() {
   if (fehler == true) {
      digitalWrite(RELAIS1_PIN, RELAIS_AUS); // Relais ausschalten 
      digitalWrite(RELAIS2_PIN, RELAIS_AUS); // Relais ausschalten 
      lcd.setCursor(0,2);
      lcd.print(F(TXT_CONST_CPU_RESTART));
      while (1);  // Endlosschleife um das Display zu lesen und die CPU durch den Watchdog neu zu starten
   }  
}

// Werteausgabe auf Serial Monitor
// wenn JSON_OUTPUT definiert ist, sendet der Arduino die Daten als JSON-String
// für die Übergabe an eine Tasmota MCU. Andernfalls einfacher Serieller Output.
/*
void serial_ausgabe_werte() {
   #ifndef JSON_OUTPUT
      Serial.print(F(TXT_CONST_SENSOR_INSIDE));
      Serial.print(F(TXT_CONST_SERIAL_HUMIDITY));
      Serial.print(Feuchte_innen);                     
      Serial.print(F(TXT_CONST_SERIAL_TEMP));
      Serial.print(Temperatur_innen);
      Serial.print(F("°C  "));
      Serial.print(F(TXT_CONST_SERIAL_DEWPOINT));
      Serial.print(Taupunkt_innen);
      Serial.println(F("°C  "));
      Serial.print(F(TXT_CONST_SERIAL_SENSOR_OUT));
      Serial.print(F(TXT_CONST_SERIAL_HUMIDITY));
      Serial.print(Feuchte_aussen);
      Serial.print(F(TXT_CONST_SERIAL_TEMP));
      Serial.print(Temperatur_aussen);
      Serial.print(F("°C "));
      Serial.print(F(TXT_CONST_SERIAL_DEWPOINT));
      Serial.print(Taupunkt_aussen);
      Serial.println(F("°C  "));
   #else
       Serial.print(F("{\"dewpoint_ventilation\":{\"mode\":\""));
       if (betriebsart == BETRIEBSART_KOMBI_1) {
          Serial.print(F("combi1"));
       } else if (betriebsart == BETRIEBSART_KOMBI_2) {
          Serial.print(F("combi2"));
       } else if (betriebsart == BETRIEBSART_TIMER) {
          Serial.print(F("timer"));
       } else if (betriebsart == BETRIEBSART_AUTO) {
          Serial.print(F("auto"));
       } else {
          Serial.print(F("-"));
       }
       Serial.print(F("\",\"sensors\":[{\"name\":\""));
       Serial.print(F(TXT_CONST_SENSOR_NAME_1));
       Serial.print(F("\",\"temp\":\""));
       Serial.print(Temperatur_innen);
       Serial.print(F("\",\"unit\":\""));
       Serial.print(F(TXT_CONST_TEMP_UNIT));
       Serial.print(F("\",\"humid\":\""));
       Serial.print(Feuchte_innen);
       Serial.print(F("\",\"dewpoint\":\""));
       Serial.print(Taupunkt_innen);
       Serial.print(F("\"},{\"name\":\""));
       Serial.print(F(TXT_CONST_SENSOR_NAME_2));
       Serial.print(F("\",\"temp\":\""));
       Serial.print(Temperatur_aussen);
       Serial.print(F("\",\"unit\":\""));
       Serial.print(F(TXT_CONST_TEMP_UNIT));
       Serial.print(F("\",\"humid\":\""));
       Serial.print(Feuchte_aussen);
       Serial.print(F("\",\"dewpoint\":\""));
       Serial.print(Taupunkt_aussen);
       Serial.print(F("\"}],\"timer\":[{\"setup\":"));
       Serial.print(long(timer_minuten) * (long)60);
       Serial.print(F(",\"remain\":"));
       Serial.print(rest_laufzeit_sekunden);
       Serial.print(F(",\"unit\":\"seconds\"}],\"actuator\":[{\"relay1\":\""));
       if (luefter_aktivieren()) Serial.print(F("ON")); else Serial.print(F("OFF"));
       Serial.print(F("\",\"relay2\":\""));
       if (luefter_aktivieren()) Serial.print(F("ON")); else Serial.print(F("OFF"));
       Serial.println(F("\"}]}}"));       
   #endif
} */



void serial_ausgabe_werte() {
   #ifndef JSON_OUTPUT
      Serial.print(F(TXT_CONST_SENSOR_INSIDE));
      Serial.print(F(TXT_CONST_SERIAL_HUMIDITY));
      Serial.print(Feuchte_innen);                     
      Serial.print(F(TXT_CONST_SERIAL_TEMP));
      Serial.print(Temperatur_innen);
      Serial.print(F("°C  "));
      Serial.print(F(TXT_CONST_SERIAL_DEWPOINT));
      Serial.print(Taupunkt_innen);
      Serial.println(F("°C  "));
      Serial.print(F(TXT_CONST_SERIAL_SENSOR_OUT));
      Serial.print(F(TXT_CONST_SERIAL_HUMIDITY));
      Serial.print(Feuchte_aussen);
      Serial.print(F(TXT_CONST_SERIAL_TEMP));
      Serial.print(Temperatur_aussen);
      Serial.print(F("°C "));
      Serial.print(F(TXT_CONST_SERIAL_DEWPOINT));
      Serial.print(Taupunkt_aussen);
      Serial.println(F("°C  "));
   #else
       Serial.print(F("{\""));
       Serial.print(F(TXT_CONST_SENSOR_NAME_1));   // Innen
       Serial.print(F("\":{\"Temperature\":"));
       Serial.print(Temperatur_innen);
       Serial.print(F(",\"Humidity\":"));
       Serial.print(Feuchte_innen);
       Serial.print(F(",\"DewPoint\":"));
       Serial.print(Taupunkt_innen);
       Serial.print(F("},\""));
       Serial.print(F(TXT_CONST_SENSOR_NAME_2));   // Aussen
       Serial.print(F("\":{\"Temperature\":"));
       Serial.print(Temperatur_aussen);
       Serial.print(F(",\"Humidity\":"));
       Serial.print(Feuchte_aussen);
       Serial.print(F(",\"DewPoint\":"));
       Serial.print(Taupunkt_aussen);
       Serial.print(F("},\"Timer\":{\"Preset\":"));
       Serial.print(long(timer_minuten) * (long)60);
       Serial.print(F(",\"Remain\":"));
       Serial.print(rest_laufzeit_sekunden);
       Serial.print(F("},\"Actuators\":{\"Relay1\":\""));
       if (luefter_aktivieren()) Serial.print(F("ON")); else Serial.print(F("OFF"));
       Serial.print(F("\",\"Relay2\":\""));
       if (luefter_aktivieren()) Serial.print(F("ON")); else Serial.print(F("OFF"));
       Serial.print(F("\"},\"Mode\":\""));      
       if (betriebsart == BETRIEBSART_KOMBI_1) {
          Serial.print(F("Combi1"));
       } else if (betriebsart == BETRIEBSART_KOMBI_2) {
          Serial.print(F("Combi2"));
       } else if (betriebsart == BETRIEBSART_TIMER) {
          Serial.print(F("Timer"));
       } else if (betriebsart == BETRIEBSART_AUTO) {
          Serial.print(F("Auto"));
       } else {
          Serial.print(F("-"));
       }
       Serial.print(F("\",\"TempUnit\":\""));
       Serial.print(F(TXT_CONST_TEMP_UNIT));
       Serial.print(F("\",\"TimeUnit\":\"seconds\",\"Version\":\""));
       Serial.print(F(PRG_VERSION));
       Serial.println(F("\"}"));
       
   #endif
}




// die Funktion gibt die konstanten Teile aus
void display_ausgabe_maske() {
   if (seite == 1) {
      lcd.setCursor(0,0);
      lcd.print(F(TXT_CONST_MASK_SENSOR_IN));
      lcd.setCursor(0,1);
      lcd.print(F(TXT_CONST_MASK_SENSOR_OUT));
      lcd.setCursor(0,2);
      lcd.print(F(TXT_CONST_MASK_DEWPT_IN));
      lcd.setCursor(0,3);
      lcd.print(F(TXT_CONST_MASK_DEWPT_OUT));
   } else {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(F(TXT_CONST_MASK_AIRING));  
      lcd.setCursor(0,1);
      lcd.print(F(TXT_CONST_MASK_DELTA_DPT));
      lcd.setCursor(0,2);
      lcd.print(F(TXT_CONST_MASK_TIMER));
   }
}


// Ausgabe der Restlaufzeit
void ausgabe_restlaufzeit() {
   int tage_rest = 0,
       stunden_rest = 0,
       minuten_rest = 0,
       sekunden_rest = 0;
   unsigned long  rest_berechnung_sekunden = rest_laufzeit_sekunden;
  
   tage_rest = rest_berechnung_sekunden / long(86400);
   rest_berechnung_sekunden = rest_berechnung_sekunden - tage_rest * long(86400);
   stunden_rest = rest_berechnung_sekunden / long(3600);
   rest_berechnung_sekunden = rest_berechnung_sekunden - stunden_rest * long(3600);
   minuten_rest = rest_berechnung_sekunden / long(60);
   rest_berechnung_sekunden = rest_berechnung_sekunden - minuten_rest * long(60);
   sekunden_rest = rest_berechnung_sekunden;
   
   lcd.setCursor(7,3);
   lcd.print(tage_rest);
   lcd.write(char(165));
   if (stunden_rest < long(10)) lcd.write('0');
   lcd.print(stunden_rest);
   lcd.write(':');
   if (minuten_rest < long(10)) lcd.write('0');
   lcd.print(minuten_rest);
   lcd.write(':');
   if (sekunden_rest < long(10)) lcd.write('0');
   lcd.print(sekunden_rest);
}


// Werteausgabe auf dem I2C-Display
void display_ausgabe_werte(bool force) {
   if (force == true) {
      Temperatur_innen_alt = Temperatur_innen + 1.0;
      Temperatur_aussen_alt = Temperatur_aussen + 1.0;
      Feuchte_innen_alt = Feuchte_innen + 1.0;
      Feuchte_aussen_alt = Feuchte_aussen + 1.0;
      Taupunkt_innen_alt = Taupunkt_innen + 1.0;
      Taupunkt_aussen_alt = Taupunkt_aussen + 1.0;
      rel_alt = !rel;
      DeltaTP_alt = DeltaTP + 1.0;
      timer_minuten_alt = timer_minuten++;
   }
   if (seite == 1) {
      if (Temperatur_innen != Temperatur_innen_alt) {
         lcd.setCursor(4,0);
         lcd.print(F("        "));
         lcd.setCursor(4,0);
         lcd_ausgabe_grad_celsius (Temperatur_innen);
         Temperatur_innen_alt = Temperatur_innen;
      }
      if (Feuchte_innen != Feuchte_innen_alt) {
         lcd.setCursor(13,0);
         lcd.print(F("      "));
         lcd.setCursor(13,0);
         lcd.print(Feuchte_innen);
         lcd.write(('%'));
         Feuchte_innen_alt = Feuchte_innen;
      }
      if (Temperatur_aussen != Temperatur_aussen_alt) {
         lcd.setCursor(4,1);
         lcd.print(F("        "));
         lcd.setCursor(4,1);
         lcd_ausgabe_grad_celsius(Temperatur_aussen);
         Temperatur_aussen_alt = Temperatur_aussen;
      }
      if (Feuchte_aussen != Feuchte_aussen_alt) {
         lcd.setCursor(13,1);
         lcd.print(F("      "));
         lcd.setCursor(13,1);
         lcd.print(Feuchte_aussen);
         lcd.write(('%'));
         Feuchte_aussen_alt = Feuchte_aussen;
      }

      if (Taupunkt_innen != Taupunkt_innen_alt) {
         lcd.setCursor(11,2);
         lcd.print(F("        "));
         lcd.setCursor(11,2);
         lcd_ausgabe_grad_celsius(Taupunkt_innen);
         Taupunkt_innen_alt = Taupunkt_innen;
      }
      if (Taupunkt_aussen != Taupunkt_aussen_alt) {
         lcd.setCursor(11,3);
         lcd.print(F("        "));
         lcd.setCursor(11,3);
         lcd_ausgabe_grad_celsius(Taupunkt_aussen);
         Taupunkt_aussen_alt = Taupunkt_aussen;
      }      
   } else { // seite == 2
      if (DeltaTP != DeltaTP_alt) {
         lcd.setCursor(11,1);
         lcd.print(F("        "));
         lcd.setCursor(11,1);
         lcd_ausgabe_grad_celsius(DeltaTP);
         DeltaTP_alt = DeltaTP;
      }
      if (timer_minuten != timer_minuten_alt) {
         timer_minuten_alt = timer_minuten;
         lcd.setCursor(7,2);
         lcd.print(F("             "));
         lcd.setCursor(7,2);
         if (timer_minuten < 91) {
            lcd.print(timer_minuten);
            lcd.print(F(TXT_CONST_MINUTES));
         } else if (timer_minuten  < 2161) {
            lcd.print((timer_minuten / 60));
            lcd.print(F(TXT_CONST_HOURS));
         } else {
            lcd.print((timer_minuten / 1440));
            lcd.print(F(TXT_CONST_DAYS));
         }
      }
      if ((betriebsart != BETRIEBSART_AUTO) && (rest_laufzeit_sekunden > 0)) {
         lcd.setCursor(0,3);
         lcd.print(F(" Rest:"));
         ausgabe_restlaufzeit();
      } else {
         lcd.setCursor(0,3);
         lcd.print(F("                    "));        
      }
   }
}


// Taupunkt-Berechnung
float taupunkt(float t, float r)
{  
   float a, b;
  
   if (t >= 0) {
     a = 7.5;
     b = 237.3;
   } else if (t < 0) {
     a = 7.6;
     b = 240.7;
   }
   // Sättigungsdampfdruck in hPa
   float sdd = 6.1078 * pow(10, (a*t)/(b+t));
   // Dampfdruck in hPa
   float dd = sdd * (r/100);
   // v-Parameter
   float v = log10(dd/6.1078);
   // Taupunkttemperatur (°C)
   float tt = (b*v) / (a-v);
   return { tt };  
}


//**** Taupunkte errechnen********
void errechne_beide_taupunkte() {
   Taupunkt_innen  = taupunkt(Temperatur_innen,  Feuchte_innen);
   Taupunkt_aussen = taupunkt(Temperatur_aussen, Feuchte_aussen);
}


// prüfen, ob der Lüfter aktiviert werden soll, je nach Betriebsart
bool luefter_aktivieren() {
   DeltaTP = Taupunkt_innen - Taupunkt_aussen;
   if (DeltaTP > (schalt_min + hysterese))   rel = true;
   if (DeltaTP < (schalt_min))               rel = false;
   if (Temperatur_innen < TEMP_INNEN_MIN )   rel = false;
   if (Temperatur_aussen < TEMP_AUSSEN_MIN ) rel = false;

   switch (betriebsart) {
      case BETRIEBSART_KOMBI_1:
         if ((rel == true) && (rest_laufzeit_sekunden > 0)) return true; else return false;
         break;
      case BETRIEBSART_KOMBI_2:
         if ((rel == true) || (rest_laufzeit_sekunden > 0)) return true; else return false;
         break;
      case BETRIEBSART_TIMER:
         if (rest_laufzeit_sekunden > 0) return true; else return false;
         break;
      case BETRIEBSART_AUTO:
         if ((rel == true) || knopf_gedrueckt()) return true; else return false;
         break;
      default:
         digitalWrite(RELAIS1_PIN, RELAIS_AUS); // Relais ausschalten 
         digitalWrite(RELAIS2_PIN, RELAIS_AUS); // Relais ausschalten 
         lcd.clear();
         lcd.setCursor(0,1);
         lcd.print(F(TXT_CONST_MODE_ERROR));
         delay(2000);
         reboot_bei_fehler();
         while (1);
         return false;
   }
}


// ggfs. Relais ein- oder ausschalten
void relais_schalten() {
   if (luefter_aktivieren()) {
      digitalWrite(RELAIS1_PIN, RELAIS_EIN); // Relais einschalten
      digitalWrite(RELAIS2_PIN, RELAIS_EIN); // Relais einschalten
   } else {                             
      digitalWrite(RELAIS1_PIN, RELAIS_AUS); // Relais ausschalten
      digitalWrite(RELAIS2_PIN, RELAIS_AUS); // Relais ausschalten
   }
}



// Errechne Zeitdifferenz unter Berücksichtungung Rücksetzung der millis() Funktion
unsigned long zeit_differenz(unsigned long start)
{
   if (start <= millis())
   {
      return (millis() - start);
   } else {
      // zwischendurch war millis() auf 0
      return (millis() + (MAX_MILLIS - start));
   }
}


// ist der Taster gedrückt?
bool knopf_gedrueckt() {
   if (digitalRead(TIMER_KNOPF_PIN) == LOW) {
      return true;
   } else {
      return false;
   }
}


// Ist der Modus-Schalter auf AUTO
bool mode_select_timer() {
   if (digitalRead(TIMER_AUTO_PIN) == MODE_TIMER) return true; else return false;
}


// Ist der Kombi-Schalter geschlossen? (=LOW)
bool mode_select_kombi() {
    if (digitalRead(KOMBI_PIN) == MODE_KOMBI) return true; else return false;
}


// diese Dinge werden in Warteschleifen abgearbeitet um Taster abzufragen usw.
void interrupt_routine() {
   unsigned long zeit_vergangen_millis = 0;
   errechne_timer_minuten();

   if (knopf_gedrueckt() || zeichen_gelesen) {
      if (betriebsart != BETRIEBSART_AUTO) { // Bei TIMER oder KOMBI 1&2
         timer_luefter_start_millis = millis();  // ab hier wird's eklig mit dem Herunterrechnen, denn das muss irgendwie
                                             // in die rest_laufzeit_sekunden übernommen werden...
         timer_minuten_start = timer_minuten;
         // rest_laufzeit_sekunden wird hier initial gesetzt und später neu berechnet
         rest_laufzeit_sekunden = long(timer_minuten_start) * long(60) - zeit_vergangen_millis / long(1000);
      }
      zeichen_gelesen = false;
      lcd.setCursor(19,3);
      lcd.write(char(165));
   } else {
      lcd.setCursor(19,3);
      lcd.write(char(32));
   }

   betriebsart = digitalRead(TIMER_AUTO_PIN) + digitalRead(KOMBI_PIN) * 2;
   #ifdef DEBUG
      lcd.setCursor(19,1);
      lcd.print(betriebsart);
   #endif
   // beim Umschalten auf Seite 2 wechseln
   if (betriebsart != betriebsart_alt) {
      seite = 2;
      betriebsart_alt = betriebsart;
      timer_minuten_alt = 0; // damit bei Wechsel auf Seite 2 auch der Wert aktualisiert wird
      seitentimer_start = millis();  // Interner Timer = Start
   }


   if (betriebsart != BETRIEBSART_AUTO) {
      if (rest_laufzeit_sekunden > 0) {
         //errechne vergangene Sekunden seit Start
         if (millis() > timer_luefter_start_millis) {  // kein Overflow, also noch keine 49 Tage vergangen
            zeit_vergangen_millis = millis() - timer_luefter_start_millis;
         } else {
            zeit_vergangen_millis = millis() + (MAX_MILLIS - timer_luefter_start_millis);
         }
         rest_laufzeit_sekunden = long(timer_minuten_start) * long(60) - zeit_vergangen_millis / long(1000);
      }
   } else {
      rest_laufzeit_sekunden = 0;  // Timer zurücksetzen, wenn Automatik-Modus
   }
   if (seite == 2) {
      #ifdef DEBUG
         lcd.setCursor(0,3);
         if (mode_select_kombi() || mode_select_timer()) lcd.write(char(165)); else lcd.write(' ');
         //if (mode_select_timer()) lcd.write(char(165)); else lcd.write(' ');
         lcd.write(char(48 + betriebsart));
      #endif
      lcd.setCursor(14,0);
      switch (betriebsart) {
         case BETRIEBSART_KOMBI_1:  lcd.print(F(TXT_CONST_MODE_COMBI1)); break;
         case BETRIEBSART_KOMBI_2:  lcd.print(F(TXT_CONST_MODE_COMBI2)); break;
         case BETRIEBSART_TIMER:    lcd.print(F(TXT_CONST_MODE_TIMER)); break;
         case BETRIEBSART_AUTO:     lcd.print(F(TXT_CONST_MODE_AUTO)); break;
      }
      lcd.setCursor(9,0);
      if (luefter_aktivieren()) lcd.print(F(TXT_CONST_ON)); else lcd.print(F(TXT_CONST_OFF));
   } else {
      lcd.setCursor(19,0);
      if (luefter_aktivieren()) lcd.write(char(165)); else lcd.write(char(32));
   }
}


// delay-Ersatz, mit Dingen, die in der Wartezeit getan werden müssen
void interrupt_delay_routine(unsigned long dauer) {
   unsigned long startzeit=millis();
   while ( zeit_differenz(startzeit) < dauer) {
       interrupt_routine();
       // Watchdog (jede Sekunde) zurücksetzen
       wdt_reset();
   }
}

// Einlesen von Kommandos über den seriellen Eingang
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    inChar = (char)Serial.read();
    switch (inChar) {
       case 'B': zeichen_gelesen = true; break;
       case 'H': zeichen_gelesen = true; rest_laufzeit_sekunden = 3600; break;
       case 'A': if (rest_laufzeit_sekunden != 0) rest_laufzeit_sekunden = rest_laufzeit_sekunden + 3600;; break;
       case 'O': rest_laufzeit_sekunden = 0; break;
       case 'J': serial_ausgabe_werte(); delay(500);break;
       case 'P': Serial.print(F("PONG"));; delay(500);break;
       case 'R': if (erstes_R_gelesen == true) {
                    Serial.print(F("Resetting 328P"));
                    delay(1000);
                    asm volatile ("  jmp 0");
                 } else {
                    erstes_R_gelesen = true;
                 }
                 break;
       default:  erstes_R_gelesen = false;
    }
    digitalWrite(LED_BUILTIN,HIGH);
    digitalWrite(LED_BUILTIN,LOW);
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}


void software_Reset() // Startet das Programm neu, nicht aber die Sensoren oder das LCD 
{
   asm volatile ("  jmp 0");
}
