# 04 SMTP-server

En liten e-postserver i SMTP-stil over TCP. Den lytter på localhost, leder en klient gjennom SMTP-samtalen og lagrer den mottatte meldingen som en `.eml`-fil.

Meldingene pakkes inn i en enkel binærprotokoll (EWP): en header med et magisk tall og størrelsen på innholdet, etterfulgt av en struct med fast størrelse for hver kommando eller hvert svar. Protokolldefinisjonene ligger i `include/ewpdef.h`.

## Sesjonsflyt

```
S: 220 <ip> SMTP <server-id> <tidsstempel>
C: HELO <domene>          S: 250 Hello
C: MAIL FROM:<avsender>   S: 250 Sender ok
C: RCPT TO:<mottaker>     S: 250 Recipient ok
C: DATA <filnavn>         S: 354 Ready for message
C: <meldingstekst>        S: 250 Message accepted
C: QUIT                   S: 221 Goodbye
```

## Bruk

```bash
make
./task5 -port 3000 -id SmtpTest
```

## Hva jeg øvde på

* `socket`, `bind`, `listen`, `accept`, `send`, `recv`
* Å tolke en fast binær header og validere størrelser før innholdet leses
* Tilstandshåndtering av protokollen på serversiden og trygge filnavn for lagrede meldinger

## Skjermbilder

![Sesjon](docs/task5_scrnshot.png)
![Valgrind](docs/task5_valgrind.png)
