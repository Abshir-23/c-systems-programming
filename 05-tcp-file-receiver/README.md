# 05 TCP file receiver

A TCP client that downloads a bitmap image sent as several packets, out of order, with a custom header. It acknowledges each packet, puts them back in the right order by sequence number and writes the complete file to disk.

## How it works

1. Connect to the server given with `-server` and `-port`
2. Read the fixed header first, then allocate and read exactly the payload size
3. Send an ACK (or NACK) back for every packet
4. Stop when a packet with the FIN flag arrives
5. Sort the packets by sequence number and write `task6_received.bmp`

![Received image](docs/received.png)

## Usage

```bash
make
./task6 -server 127.0.0.1 -port 42009
```

## What I practised

* A reliable `recvAll` loop, since TCP can split data across several `recv` calls
* Building TCP style headers with sequence numbers, flags and checksums
* Reassembling out of order data

## Screenshots

![Session](docs/task6_scrnshot.png)
![Valgrind](docs/task6_valgrind.png)
