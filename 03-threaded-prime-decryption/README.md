# 03 Threaded prime decryption

Two POSIX threads work together to find prime numbers in a file and use them as keys to decrypt an XTEA encrypted message. When the right key is found, the plaintext is saved together with a checksum of it.

## How it works

1. Both threads read from the same input file and share a linked list of primes found so far
2. Each prime is tried as an XTEA key on `task4_code.bin`
3. When the decrypted text starts with the expected header, it is saved with a TCP style checksum, and the other thread is told to stop
4. All shared state (file pointer, prime list, found flag) is protected by a mutex, and passed to the threads in a struct instead of globals

## Usage

```bash
make
./task4 task4_primes.txt
```

## What I practised

* `pthread_create`, `pthread_join`, `pthread_mutex_init` / `lock` / `unlock`
* Avoiding race conditions on a shared `FILE *` and a shared list
* Symmetric decryption with XTEA, 8 bytes at a time

## Screenshots

![Output](docs/task4_scrn2.png)
![Valgrind](docs/task4_valgrind.png)
