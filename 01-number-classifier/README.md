# 01 Tallklassifisering

Leser heltall fra en tekstfil, kjører hvert tall gjennom åtte klassifiseringsfunksjoner og skriver resultatene til en binærfil.

For hvert tall sjekker programmet om det er et Fibonacci-tall, et primtall, et kvadrattall, et kubikktall, et perfekt tall, et overflodstall, et mangeltall og et oddetall. Resultatene lagres i en struct og skrives til `output.bin`, mens en tabell skrives ut i terminalen.

## Bruk

```bash
make
./task2 test_numbers.txt
```

## Hva jeg øvde på

* Å lenke flere `.c`-filer med en makefile og en felles headerfil
* Å lese tekst og skrive en binær post med fast størrelse per tall
* Å rydde opp hver minneallokering og hvert filhåndtak på alle veier ut av programmet

## Skjermbilder

![Utskrift](docs/task2_scrn1.png)
![Valgrind](docs/task2_valgrind.png)
