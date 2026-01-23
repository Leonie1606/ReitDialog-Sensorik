Firmware für Dual‑Waagen‑System (ESP33 IoT + HX711)
Dieses Repository enthält die Firmware für zwei unabhängige Waagenmodule („WaageLinks“ und „WaageRechts“) auf Basis eines ESP33 IoT und eines HX711‑Verstärkers.
Beide Module erfassen Gewichtsdaten, führen Tare und Kalibrierung durch, speichern Einstellungen dauerhaft im Flash und übertragen die Messwerte an die Arduino IoT Cloud.

Funktionen
- Gewichtsmessung über HX711
- Automatische und manuelle Tare
- Kalibrierung mit Referenzgewicht
- Glättung und Mittelwertbildung für stabile Messwerte
- Dauerhafte Speicherung von Offset und Kalibrierfaktor
- Cloud‑Anbindung über die Arduino IoT Cloud (MQTT)
- Serielle Befehle zur Steuerung (Tare, Kalibrierung)
Verwendete Hardware
- ESP33 IoT
- HX711‑Modul
- Wägezelle (je Waage: zwei bei Halbbrücken oder eine Vollbrücke)
- WLAN‑fähiges Endgerät für das Cloud‑Dashboard


Software‑Architektur
1. Systeminitialisierung
   Beim Start lädt das System gespeicherte Werte aus dem Flash, verbindet sich mit der Cloud und initialisiert den HX711.
   Wird ein Erststart erkannt, führt das System automatisch eine Tare durch.
3. Benutzerinteraktion
  Über den Serial Monitor können einfache Befehle ausgeführt werden:
  - t → Tare
  - r → Kalibrierung mit Referenzgewicht
  Das System gibt Rückmeldungen und Hinweise über die serielle Schnittstelle aus.
3. Messlogik
  Die Waage wird mehrfach ausgelesen, um stabile Werte zu erhalten.
  Durch Mittelwertbildung und Glättung werden Störungen reduziert.
  Ein Timeout verhindert Blockieren bei fehlender Sensorbereitschaft.
4. Kalibrier‑Modul
  Nach Eingabe eines bekannten Referenzgewichts berechnet das System automatisch den passenden Kalibrierfaktor.
  Dieser wird gespeichert und bei jedem Start wieder geladen.
5. Offset‑Modul (Tare)
  Die Tare‑Funktion setzt die Waage auf Null und speichert den ermittelten Offset dauerhaft im Flash.
6. Datenbereitstellung
  Die berechneten Gewichtswerte werden kontinuierlich:
  - an die Arduino IoT Cloud übertragen
  - im Serial Monitor ausgegeben
  Die Aktualisierung erfolgt in kurzen Intervallen für nahezu Echtzeit‑Feedback.

Installation & Setup
- Repository klonen
- Benötigte Bibliotheken installieren:
    - HX711
    - ArduinoIoTCloud
    - FlashStorage
- WLAN‑Zugangsdaten im Sketch eintragen
- Thing‑Variablen in der Arduino IoT Cloud anlegen
- Sketch auf den ESP33 IoT hochladen

Bedienung
- Tare: „t“ im Serial Monitor eingeben
- Kalibrierung: „r“ eingeben und Referenzgewicht angeben
- Gewicht wird automatisch alle 200 ms gemessen und übertragen


Autorin:
Leonie Ehrle
Projekt im Rahmen des Bachelorstudiums „Logistik und Digitalisierung“ in der Vorlesung "Digital Capstone Project"



