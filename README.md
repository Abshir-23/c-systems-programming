# Systemprogrammering i C på Linux

Fem C-programmer for Linux som dekker fil-I/O, datastrukturer, flertråding og nettverksprogrammering. Alle programmene bygges med `make`, og alle er sjekket med Valgrind med **0 feil og 0 minnelekkasjer**.

| # | Prosjekt | Temaer |
|---|---------|--------|
| 01 | [Tallklassifisering](01-number-classifier) | Bygg med flere filer og makefile, fil-I/O med tekst og binærfiler, structs |
| 02 | [Karaktersystem](02-grading-system) | Lenket liste, dynamisk minne, menystyrt kommandolinjeprogram, lagring og innlasting i binærfil |
| 03 | [Trådbasert primtallsdekryptering](03-threaded-prime-decryption) | POSIX-tråder, mutexer, XTEA-dekryptering, sjekksummer |
| 04 | [SMTP-server](04-smtp-server) | TCP-sockets, tilstandsmaskin for protokollen på serversiden, egen binærprotokoll |
| 05 | [TCP-filmottaker](05-tcp-file-receiver) | TCP-klient, sammensetting av pakker i feil rekkefølge, sjekksummer, ACK/NACK |

## Bygge og kjøre

Krever `gcc`, `make` og Linux (testet på Debian/Ubuntu).

```bash
cd 01-number-classifier
make
./task2 test_numbers.txt
```

Hver mappe har sin egen README med bruk og skjermbilder.

## Minnesikkerhet

Alle programmene ble kjørt med:

```bash
valgrind --leak-check=yes --track-origins=yes ./<program> <argumenter>
```

Resultat for alle programmene: `All heap blocks were freed -- no leaks are possible` og `ERROR SUMMARY: 0 errors`. Skjermbilder fra Valgrind ligger i `docs/`-mappen i hvert prosjekt.

## Kodestil

* Ungarsk notasjon som prefiks (`i` int, `psz` strengpeker, `pf` filpeker, `st` struct)
* Headerfiler i `include/`, objektfiler i `obj/`
* Ingen global tilstand i det trådbaserte programmet, delte ressurser sendes med i en struct

## Kode fra andre og utlevert kode

Noen hjelpefiler ble utlevert som startkode og er merket med en kommentar øverst i filen: de små tallsjekkfunksjonene i `01`, funksjonene `isPrime`, XTEA (David Wheeler og Roger Needham) og TCP-sjekksum i `03`, og definisjonene av protokollheaderen (`ewpdef.h`) i `04` og `05`. Alt annet er mitt eget arbeid.

## Verktøy

C (C99), GCC, GNU Make, Valgrind, POSIX-tråder, BSD-sockets, Linux
