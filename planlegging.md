# Vekkerklokke innlevering:

### inneholder:

- TFT Skjerm
- Buzzer/Høytaler
- Arduino
- Klokke (for systemtid)
- Knapp(er) / pot


### Skjerm skal inneholde:

-	Klokkeslett
-	Klokkeslett for alarm
-	Mulighet for å stille alarm (Ved bruk av pot)
-	Mulighet for å skru av alarm / knapp (pilmot knappen?)
	-	Kan være mer fancy enn trykk på knapp,f.eks. løs et mattestykke eller noe sånt 
	-	(men må ha få knapper å svare med,kanskje en pot)?
	-	Knapp i første omgang 
-	Mulighet for å slumre

### Systemet trenger:

- CurrentTime
- AlarmSetTime
- Buzzer Referance
- Alarm sound/melody
- SnoozeExtensionTime
- State Machine
	- Idle State
	- Alarm State
	- Set Alarm State
	- Snooze State
	- Stop Alarm State


Startet med å se hva graphicstest eksempelet kunne gjøre og hvordan det ble gjort, så jeg kunne forstå og bruke det.

Var også rimelig enkelt å bruke TFT panelet og systemklokken etter dette ble vist i timen. 