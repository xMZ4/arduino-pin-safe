# Seifas su PIN kodu

Arduino valdomo seifo prototipas, sukurtas „Tinkercad Circuits“ aplinkoje robotikos namų darbui. PIN įvedamas klaviatūra, o servovariklis imituoja užrakto mechanizmą.

## Veikimas

- Fizinis mygtukas įjungia ir išjungia seifo funkcijas. Paleidus programą seifas yra išjungtas.
- Teisingas PIN atrakina seifą: servovariklis pasisuka į 90° padėtį, įsijungia žalias LED.
- Užrakinto ir įjungto seifo būsenoje šviečia raudonas LED, o servovariklio padėtis yra 0°.
- Įvedami skaitmenys LCD ekrane slepiami žvaigždutėmis.
- Po trijų neteisingų bandymų PIN įvedimas užblokuojamas 30 sekundžių. LCD rodo likusį laiką.
- Pjezo elementas skleidžia paspaudimo, teisingo PIN, klaidos ir blokavimo garsus.
- Programinis išjungimas neištrina neteisingų bandymų ir nenutraukia blokavimo laiko.
- Laikas tikrinamas su `millis()`, o fiziniam mygtukui taikomas 50 ms kontaktų virpėjimo filtravimas.

## Komponentai

- Arduino Uno R3
- 4 × 4 matricinė klaviatūra
- Micro Servo
- LCD 16 × 2 su I2C sąsaja (MCP23008, adresas 0x20)
- Raudonas ir žalias LED
- Du 220 Ω rezistoriai LED srovei riboti
- Pjezo garsinis elementas
- Mygtukas, maketavimo plokštė ir jungiamieji laidai

## Prijungimas

| Komponentas | Arduino kontaktas |
| --- | --- |
| Klaviatūros eilutės pagal `rowPins` tvarką | D9, D8, D7, D6 |
| Klaviatūros stulpeliai pagal `colPins` tvarką | D5, D4, D3, D2 |
| Servovariklio signalas | D10 |
| Raudonas LED per rezistorių | D11 |
| Žalias LED per rezistorių | D12 |
| Pjezo elemento „+“ kontaktas | D13 |
| Įjungimo mygtukas | A0 ir GND (`INPUT_PULLUP`) |
| LCD SDA | A4 |
| LCD SCL | A5 |
| LCD maitinimas | 5V ir GND |

## Valdymas

| Mygtukas | Veiksmas |
| --- | --- |
| Fizinis mygtukas | Įjungti arba išjungti seifą; išjungiant užrakinti |
| 0–9 | Įvesti PIN skaitmenis |
| `#` | Patvirtinti PIN |
| `*` | Ištrinti dabartinę įvestį |
| `D` | Užrakinti atrakintą seifą |

## Programos kodas ir paleidimas

Arduino kodas saugomas faile `seifas/seifas.ino`. Naudojamos bibliotekos: `Keypad`, `Servo` ir `Adafruit_LiquidCrystal`. Teisingas PIN nustatomas kintamajame `correctPin`.

„Tinkercad Circuits“ aplinkoje surinkti schemą pagal nurodytus kontaktus, į teksto režimo kodo redaktorių įkelti programą ir paleisti simuliaciją. Paspausti fizinį mygtuką, įvesti PIN ir patvirtinti su `#`.
