#define BLOCK_SIZE 512

typedef short u_int16_t;
typedef int u_int32_t;
typedef char u_int8_t;

//opcodes 
#define OPCODE_RRQ   1
#define OPCODE_WRQ   2
#define OPCODE_DATA  3
#define OPCODE_ACK   4
#define OPCODE_ERR   5

//mode
#define MODE_NETASCII "netascii"
#define MODE_OCTET    "octet"
#define MODE_MAIL     "mail"

/* Timeout in seconds */
#define TFTP_TIMEOUT 2

//estructura genérica de formato de tftp 
struct TFTP_FORMATO{
	u_int16_t opcode;
	char data[30]; 	
} typedef TFTP_FORMATO;

//Estructura de la trmaa padre a enviar 
struct Datagrama{
	char tid; 
	TFTP_FORMATO formato;	
}typedef Datagrama;

//Paquete  RRQ_WRQ
struct RRQ_WRQ{
	u_int16_t opcode;
	char *fileName;
	char* mode;
}typedef RRQ_WRQ;

//paquete data 
struct DATA{
	u_int16_t opcode;
	u_int16_t blockNum;
	char* msg;
} typedef DATA;

//Paquete ACK 
struct ACK{
	u_int16_t opcode;
	u_int16_t blockNum;	
}typedef ACK;

//Paquete de error
struct ERROR_TRAMA{
	u_int16_t opcode;
	u_int16_t errorCode;
	char* errosMsg;
	u_int8_t relleno;
}typedef ERROR_TRAMA;
