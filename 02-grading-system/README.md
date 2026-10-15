# 02 Karaktersystem

Et menystyrt kommandolinjeprogram for å registrere studenter, vurdere oppgaver og regne ut bokstavkarakterer (A til F). Studentene lagres i en enkeltlenket liste på heapen.

## Funksjoner

* Legge til studenter manuelt eller laste inn en liste fra en tekstfil
* Vurdere enkeltoppgaver med skriftlig begrunnelse
* Skrive ut alle studenter, filtrere på karakter eller vise studenter med oppgaver som ikke er vurdert
* Statistikk (antall, snitt, høyeste og laveste poengsum)
* Slette studenter
* Lagre og laste inn hele listen til og fra en binærfil

## Bruk

```bash
make
./task3
```

## Hva jeg øvde på

* Innsetting, søk og sletting i lenket liste, med riktig `free()` av hver node
* Trygg håndtering av input med `fgets` i stedet for `scanf`
* Binær serialisering av structs

## Skjermbilder

![Meny](docs/task3_scrn1.png)
![Valgrind](docs/task3_valgrind.png)
