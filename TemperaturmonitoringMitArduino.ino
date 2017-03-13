/**
 * Temperaturmonitoring mit Arduino
 * 
 * benötigte Bauteile:
 * 
 * 1x DS18B20
 * 1x HD44780 1602 LCD
 * 1x PCF8574 LCD I2C
 * 1x ENC28J60 Ethernet Module und
 * 1x Arduino
 * 
 * by Dominik Halley
 * 
 * Benötigte Bibliotheken aus den Includes ableiten. 
 * 
 * Zukünftige Ergänzungen:
 * -> IP-Adresse auf dem LCD anzeigen
 * -> IP über DHCP empfangen
 */

 // Includes für ENC28J60
 #include <EtherCard.h>

 // Includes für DS18B20
 #include <OneWire.h>
 #include <DallasTemperature.h>

 // Includes für LCD
 #include <LCD.h>
 #include <LiquidCrystal_I2C.h>

 // Includes für Serial
 #include <Wire.h>
 #include <SoftwareSerial.h>

 // Temperatursensor liegt an Pin D9
 #definde ONE_WIRE_BUS 9

 // Festlegen der MAC & IP-Adresse
 static byte mymac[] = { 0x74, 0x69, 0x69, 0x2D, 0x30, 0x31 };
 static byte myip[] = { 192, 168, 2, 254 };

 // Ethernetbuffer deklarieren
 byte Ethernet::buffer{500];
 BufferFiller bFill;

 // OneWire wurde in der Variable ONE_WIRE_BUS definiert
 OneWire oneWire(ONE_WIRE_BUS);
 DallasTemperature sensors(&oneWire);

 // LCD-Pins definieren 0x27 = Standartadresse
 LiquidCrystal_I2C lcd (0x27, 2, 1, 0, 4, 5, 6, 7);

 // Ethernetmodul definieren
 #define rxPin 10
 #define txPin 11

 // Serielle Verbindung zum Ethernetmodul
 SoftwareSerial btSerial(rxPin, txPin);
 String btData;

 void setup() {
  
    // Serielle Schnittstelle initialisieren
    Serial.begin(9600);
    Serial.println("Arduino Temperaturmonitoring Webserver"),
    // Temperaturauswertung beginnen. getTempCByIndex(x) = Temperatur in °C, x steht für die Reihenfolge der Sensoren, erster Sensor = 0
    sensors.begin();
    Serial.println("Temperatur: ");
    Serial.println(sensors.getTempCByIndex(0));

    // Netzwerkverbindung aufbauen
    if (ether.begin(sizeof Ethernet::buffer, mymac) == 0)
      Serial.println( "Konnte nicht auf den Ethernetcontroller zugreifen!! ");
      
    ether.staticSetup(myip);

    // LCD ansprechen
    lcd.begin (16,2); // für 16 x 2 LCD modul, 20,4 für 20 x 4 LCD modul
    lcd.setBacklightPin(3,POSITIVE);
    lcd.setBacklight(HIGH);
 }


 static word webseite() {
      bFill = ether.tcpOffset();

      // Webseite aufbauen
      bFill.emit_p(PSTR(
      "HTTP/1.0 200 OK\r\n"
      "Content-Type: text/html\r\n"
      "Pragma: no-cache\r\n"
      "\r\n"
      "<title>Temperatur</title>"
      "<h1>Raumtemperatur: $T\n</h1><br><br><h3>"),sensors.getTempCByIndex(0)); // $T = Platzhalter für den Temperaturwert

      return bFill.position();
 }

 void loop() {
      word len = ether.packetReceive();
      word pos = ether.packetLoop(len);

      if (pos)
        ether.httpServerReply(webseite()); // Überprüfen ob TCP-Daten gültig sind.

      // Temperaturwert auf der seriellen Konsole ausgeben
      Serial.println("Temperatur: ");
      Serial.println(sensors.getTempCByIndex(0));

      // Temperaturwert auf dem LCD ausgeben
      lcd.home();
      lcd.print("Temp: ");
      lcd.setCursor(6,0);
      lcd.print(sensors.getTempCByIndex(0));
 }

