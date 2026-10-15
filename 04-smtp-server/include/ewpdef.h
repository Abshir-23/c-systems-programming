/* Utlevert startfil, ikke skrevet av meg. */
#ifndef EWP_SMTP_PROTOCOL_DEFINED
#define EWP_SMTP_PROTOCOL_DEFINED

#define EWP_SMTP_PROTOCOL_MAGIC "EWP"
struct EWP_SMTP_PROTOCOL_SIZEHEADER {
   char acMagicNumber[3]; /* = EWP */
   char acDataSize[4]; /* = sizeof(structure below), as ASCII, always base 10, ignore leading 0 */
   char acDelimeter[1]; /* = | */
}; 

struct EWP_SMTP_PROTOCOL_SERVERACCEPT {
   struct EWP_SMTP_PROTOCOL_SIZEHEADER stHead; /* Size is 0064 */
   char acStatusCode[3]; /* Set to 220 if server is ready */
   char acHardSpace[1]; /* Set to a space (ascii 0x20) */
   char acFormattedString[51]; /* Set to SERVERIP PROTOCOL SERVERNAME TIMESTAMP */
                               /* For instance; 127.0.0.1 SMTP SmtpTest 27 okt 2020, 12:42:42 */
   char acHardZero[1]; /* Set to binary 0, ensures the received data is a zero-terminated string */
}; 

struct EWP_SMTP_PROTOCOL_CLIENTHELO {
   struct EWP_SMTP_PROTOCOL_SIZEHEADER stHead; /* Size is 0064 */
   char acCommand[4]; /* Set to HELO */
   char acHardSpace[1]; /* Set to a space (ascii 0x20) */
   char acFormattedString[50]; /* Set to USERNAME.CLIENTIP */
                               /* For instance; bengt.127.0.0.1 */
   char acHardZero[1]; /* Set to binary 0, ensures the received data is a zero-terminated string */
}; 

struct EWP_SMTP_PROTOCOL_SERVERHELO {
   struct EWP_SMTP_PROTOCOL_SIZEHEADER stHead; /* Size is 0064 */
   char acStatusCode[3]; /* Set to 250 if HELO received successfully */
   char acHardSpace[1]; /* Set to a space (ascii 0x20) */
   char acFormattedString[51]; /* Set to CLIENTIP GREATING */
                               /* For instance; 127.0.0.1 Hello bengt */
   char acHardZero[1]; /* Set to binary 0, ensures the received data is a zero-terminated string */
}; 

#define EWP_SMTP_PROTOCOL_SERVERREPLY_OK "250"
#define EWP_SMTP_PROTOCOL_SERVERREPLY_CLOSED "221"
#define EWP_SMTP_PROTOCOL_SERVERREPLY_READY "354"
struct EWP_SMTP_PROTOCOL_SERVERREPLY {
   struct EWP_SMTP_PROTOCOL_SIZEHEADER stHead; /* Size is 0064 */
   char acStatusCode[3]; /* Set to 250 if command is supported */
                         /* As a response to a DATA command field should be 354 if server is ready, */
                         /*  or for instance 501 if filename not valid or acceptable */
                         /* Set to 221 if the server is closing */
   char acHardSpace[1]; /* Set to a space (ascii 0x20) */
   char acFormattedString[51]; /* Set to TEXTMESSAGE */
                               /* For instance; Sender address ok | Ready for message */
   char acHardZero[1]; /* Set to binary 0, ensures the received data is a zero-terminated string */
}; 

struct EWP_SMTP_PROTOCOL_MAILFROM {
   struct EWP_SMTP_PROTOCOL_SIZEHEADER stHead; /* Size is 0064 */
   char acCommand[10]; /* Set to MAIL FROM: */
   char acHardSpace[1]; /* Set to a space (ascii 0x20) */
   char acFormattedString[44]; /* Set to EMAILADDRESS */
                               /* For instance; <bengt@test.com> */
   char acHardZero[1]; /* Set to binary 0, ensures the received data is a zero-terminated string */
}; 

struct EWP_SMTP_PROTOCOL_RCPTTO {
   struct EWP_SMTP_PROTOCOL_SIZEHEADER stHead; /* Size is 0064 */
   char acCommand[8]; /* Set to RCPT TO: */
   char acHardSpace[1]; /* Set to a space (ascii 0x20) */
   char acFormattedString[46]; /* Set to EMAILADDRESS */
                               /* For instance; <user@example.com> */
   char acHardZero[1]; /* Set to binary 0, ensures the received data is a zero-terminated string */
}; 

struct EWP_SMTP_PROTOCOL_CLIENTDATACMD {
   struct EWP_SMTP_PROTOCOL_SIZEHEADER stHead; /* Size is 0064 */
   char acCommand[4]; /* Set to DATA: */
   char acHardSpace[1]; /* Set to a space (ascii 0x20) */
   char acFormattedString[50]; /* Set to FILENAME*/
                               /* For instance; myfile.eml */
   char acHardZero[1]; /* Set to binary 0, ensures the received data is a zero-terminated string */
}; 

struct EWP_SMTP_PROTOCOL_CLIENTDATAFILE {
   struct EWP_SMTP_PROTOCOL_SIZEHEADER stHead; /* Size is dynamic */
   char acFileContent[1]; /* Dynamic size array, will be stHead.acDataSize - 1 bytes, results in max size 9998 bytes*/
   /* char acHardZero[1]; */ /* Ensure a zero-termination character is part of data, all though not part of this struct */
}; 

struct EWP_SMTP_PROTOCOL_CLOSECOMMAND {
   struct EWP_SMTP_PROTOCOL_SIZEHEADER stHead; 
   char acCommand[4]; /* Set to QUIT */
   char acFormattedString[51]; /* Reserved for future use */
   char acHardZero[1]; /* Set to binary 0, ensures the received data is a zero-terminated string */
}; 

#endif

