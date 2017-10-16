# EmbeddedSystems_AssignmentTwo
Alarm Clock made using a quartz crystal and a TFT monitor

### Button is used to:
- Change Modes
    - Idle
    - SetAlarmHour
    - SetAlarmMinute
    - repeat
- Turn off Alarm when alarm is going

### Button2 is used to:
- Change Modes (opposite direction)
    - Idle
    - SetAlarmMinute
    - SetAlarmHour
    - repeat
- Snooze when alarm is going

### Potensiometer is used to:
- Set alarm time when in SetAlarm modes
    - SetHour
    - SetMinute
- `(if implemented)` Choose option for turning off alarm

### Buzzer is used to:
- Play Alarm Melody
- `(if implemented)` Play Awake Melody
- `(if implemented)` Play Shutdown Melody

### Quartz Crystal DS1307 used to:
- Keep track of time
- Keep track of alarm time
- Reset screen every minute

### TFT panel used to:
- Display Date
- Display Time of day (hh:mm)
- Display Alarm Time
- Display Set Alarm Modes
    - Set Hour
    - Set Minute
- Display Alarm
    - Also instructions for turning it off or into snooze
- Display Snoozing
- 