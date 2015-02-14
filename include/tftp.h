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

static char err_codes[][33] = {
	"Undef",
	"File not found",
	"Access violation",
	"Disk full or allocation exceeded",
	"Illegal TFTP operation",
	"Unknown transfer ID",
	"File already exists",
	"No such user"
};


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
	char dirOrigen;
	char dirDestino;
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
	int longMsg;
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

//Recibe el numero de bloque y el caracter de direccion irigen y direccion destino
void enviarACK(int,char,char);

//Recibe el numero de bloque y el arreglo que contiene la informacion del archivo y la direccion origen y direccion destino
void enviarDATA(int,char*,int tamInformacion,char,char);

//recibe el error de codigo y el error que sucedio(cadena) y la direccion origen y direccion destino
void enviarERROR(int,char*,char,char);

//recibe el caracter de direccion origen y direccion destino y el nombre del archivo
void enviarRRQ(char ,char,char*);

//recibe el caracter de direccion origen , direccion destino  y el nombre del archivo
void enviarWRQ(char ,char, char*);

//recibe una estructura datagarama que previamente fue creada y recibe el paquete en donde se guardara el array resultante de convertir la estructura 
//Devuelve el numero de btes del array resultante.
int  structToArray(Datagrama*,char**);

//recibe la direccion origen y direcion destino 
//devuelve uno si las direcciones coinciden y 0 si no coinciden 
int filtroDireccion(char dirOrigen,char dirDestino);


void imprimirTrama(char* trama , int tam);

