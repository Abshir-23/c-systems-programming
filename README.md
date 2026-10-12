# C Systems Programming on Linux

Five C programs for Linux covering file I/O, data structures, multithreading and network programming. Every program is built with `make`, and every program has been checked with Valgrind with **0 errors and 0 memory leaks**.

| # | Project | Topics |
|---|---------|--------|
| 01 | [Number classifier](01-number-classifier) | Multi-file build with makefile, text and binary file I/O, structs |
| 02 | [Grading system](02-grading-system) | Linked list, dynamic memory, menu-driven CLI, binary save/load |
| 03 | [Threaded prime decryption](03-threaded-prime-decryption) | POSIX threads, mutexes, XTEA decryption, checksums |
| 04 | [SMTP server](04-smtp-server) | TCP sockets, server side protocol state machine, custom binary protocol |
| 05 | [TCP file receiver](05-tcp-file-receiver) | TCP client, out of order packet reassembly, checksums, ACK/NACK |

## Build and run

Requires `gcc`, `make` and Linux (tested on Debian/Ubuntu).

```bash
cd 01-number-classifier
make
./task2 test_numbers.txt
```

Each folder has its own README with usage and screenshots.

## Memory safety

All programs were run with:

```bash
valgrind --leak-check=yes --track-origins=yes ./<program> <args>
```

Result for every program: `All heap blocks were freed -- no leaks are possible` and `ERROR SUMMARY: 0 errors`. Valgrind screenshots are in each project's `docs/` folder.

## Code style

* Hungarian style prefixes (`i` int, `psz` string pointer, `pf` file pointer, `st` struct)
* Headers in `include/`, object files in `obj/`
* No global state in the threaded program, shared resources are passed in a struct

## Third party and provided code

Some helper files were provided as starter code and are marked with a comment at the top of the file: the small number check functions in `01`, the `isPrime`, XTEA (David Wheeler and Roger Needham) and TCP checksum functions in `03`, and the protocol header definitions (`ewpdef.h`) in `04` and `05`. Everything else is my own work.

## Tools

C (C99), GCC, GNU Make, Valgrind, POSIX threads, BSD sockets, Linux
