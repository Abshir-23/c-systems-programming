/* Provided starter file, not written by me. */
#ifndef EWP_TCP_PROTOCOL_DEFINED
#define EWP_TCP_PROTOCOL_DEFINED


/* TCP over TCP has no additional handshake, */
/* Sequence Number starts at 0, */
/* Window field replaced by a SizeOfPacket for this packet, */
/* only ACK (packet accepted), NACK (resend packet) and FIN (last packet) flag is used, */
/* Use URG value to mean NACK instead - so set 0x20 if request to resend by setting NACK flag */
/* Data is of dynamic size, and is usSizeOfPacket bytes large */
#pragma pack(1) 
struct EWP_TCP_PROTOCOL_TCP { 
   unsigned short usSourcePort; 
   unsigned short usDestinationPort; 
   unsigned int uiSequenceNumber; 
   unsigned int uiAckNumber; 
   unsigned char ucDataOffset : 4; 
   unsigned char ucReserved : 4; 
   unsigned char ucFlags; 
   unsigned short usSizeOfPacket; 
   unsigned short usChecksum; 
   unsigned short usUnused; 
   unsigned char Data[1]; 
}; 
#pragma pack() 

#endif

