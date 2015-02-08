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

//Paquete  RRQ
struct RRQ{
	u_int16_t opcode;
	char *fileName;
	char* mode;
}typedef RRQ;

//Paquete  WRQ
struct WRQ{
	u_int16_t opcode;
	char *fileName;
	char* mode;
}typedef WRQ;

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

//Recibe el numero de bloque y el caracter de direccion
void enviarACK(int,char);

//Recibe el numero de bloque y el arreglo que contiene la informacion del archivo 
void enviarDATA(int,char*);

//recibe el error de codigo y el error que sucedio(cadena) 
void enviarERROR(int,char*);

//recibe el caracter de direccion y el nombre del archivo 
void enviarRRQ(char ,char*);

//recibe el caracter de direccion y el nombre del archivo 
void enviarWRQ(char ,char*);

//recibe una estructura datagarama que previamente fue creada y recibe el paquete en donde se guardara el array resultante de convertir la estructura 
//Devuelve el numero de btes del array resultante.
int  structToArray(Datagrama*,char**);
