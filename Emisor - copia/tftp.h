#define BLOCK_SIZE 512

typedef short u_int16_t;
typedef int u_int32_t;

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

struct RRQ{
	u_in	
}typedef RRQ;
