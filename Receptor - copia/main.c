#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "canal.h"
#include "tftp.h"
#include "checksum.h"
#include "timer_tftp.h"
#define TIME_OUT 5000
#define TAM_BUF 50

extern volatile int timeOut;
int contACK = 0;
int recibirArchivo(char dirOrigen, char dirDestino);
void copiarArch(char*,int,FILE*);
int filtroDireccion(char dirA,char dirT);

int main(int argc, char *argv[])
{

	int leidos,contador=200;
	char nombre[50];
	char dirO,dirD,bufer[512];
	int tam;
	//verificamos que el numero de argumentos sea correcto
	if(argc <= 2){
		printf("Parámetros incompletos");
		printf("%s direccion",argv[0]);
		return 1;
	}
	//direccion origen 
	dirO = argv[1][0];
	
	//direccion destino 
	dirD = argv[2][0];
	
	
	inicializar();	
		
	recibirArchivo(dirO,dirD);

	terminar(); 
    system("PAUSE");
    return EXIT_SUCCESS;
}



int recibirArchivo(char direccionOrigen, char direccionDestino){
	char bufer[BLOCK_SIZE+22];
	char buferArch[BLOCK_SIZE];
	int tam=0;
	int abrirArch=0;
	char aux[BLOCK_SIZE+2];
	signed long bytes=0;
	int opcode;
	int contDATA = 1;
	int contACK = 0;
	int reenviando = 0;
	int contadorIntentos = 0;
	int tipoTrama =0;
	short int check;
	//int cont_low;
	//Apertura del archivo 
	FILE* dFile;
		
    dFile = fopen("archivoRecibido.bmp","wb");
	if(dFile){
		abrirArch = 1;	
	}
	else{
		printf( "Error (NO ABIERTO)\n" );	
	}
	
	printf("Archivo abierto para escritura\n");
	
		
	do{
		//limpiamos el bufer
	
	 	//Esperar respuesta del servidor 
		iniciar_timeOut(TIME_OUT);
		while(1){
			printf("Esperando respuesta\n");
			memset(bufer,0,sizeof(char)*BLOCK_SIZE+8);	
			tam = sizeof(bufer);
			printf("size of bufer: %d",tam);
			rx(bufer,&tam);
			
			if(tam > 0 ){
				printf("Respuesta recibida\n");
				//verificar las direcciones 
				if(bufer[0] == direccionDestino  && bufer[1] == direccionOrigen){
					printf("Las direcciones coincidieron.\n");
					if(reenviando == 0 && tipoTrama == OPCODE_ACK){
						contACK++;
						contDATA++;
						contadorIntentos = 0;
						reenviando = 0;	
					}
					break;		
				}
				//las direcciones no coincidieron 
				else{
					printf("Las direcciones no coincidieron (enviando error)\n");
					//enviar paquete de error 
					enviarERROR(5,err_codes[5],direccionOrigen,direccionDestino);
					return 1; 
				}
			}
			else if(timeOut == 1){
				printf("Timeout expiro\n");
				contadorIntentos++;
				//contar cuantas veces se esta reintentando
				if(contadorIntentos != 5){
					printf("intento %d\n",contadorIntentos);
					continue;
				}
				else{
					printf("Los intentos se teminaron(terminación de la ejecución)\n");
					//En ninguno de los intentos se logro enviar 
					fclose(dFile);	
					return 1;
				}
			}
		}	

		
		switch(bufer[3]){
			case OPCODE_WRQ:
				printf("Recibiendo WRQ\n");
				printf("Enviar ACK\n");
				enviarACK(contACK,direccionOrigen,direccionDestino);
				
				tipoTrama = OPCODE_ACK;
				break;
			case OPCODE_DATA:
				//Obteniendo parte baja de contACK 
				//cont_low = contACK&0x0011;
				//Verificacion de que es el ack que se esta esperando 
				printf("Recibiendo DATA %d %d\n",bufer[4],bufer[5]);
				//imprimirTrama(aux,520);
				printf("Verificando BlockNum contACK = %d trama = %d %d\n",contACK,bufer[4],bufer[5]);	
				char parte_baja = contACK & 0x00ff;
				char parte_alta = (contACK>>8) & 0x00ff;
				if(parte_baja == bufer[5] && parte_alta == bufer[4]){
				
					printf("Enviar ACK %d\n",contACK);
					printf("Copiando al archivo %d bytes\n",tam-8);
					//verificar si es la ultima transmision 
					if(tam < 512){
						printf("Ultimo paquete recibido\n");
						//le quitamos el opcode y el numero de bloque 
						memcpy(aux,bufer+6,tam-6);
						
						//checksum 
						printf("Verificando el checksum\n");
						//imprimiendo trama antes de verificar check 
						check  = checkSum(aux,514);
						
						printf("check = %04X\n",check);
						printf("check negado = %04X\n",(~check));
						if((~check) == 0 || check == 0){
							printf("Checksum correcto\n");
							printf("Copiando al archivo\n");
							//quitando los ultimos bytes 
							copiarArch(aux,tam-8,dFile);
							enviarACK(contACK,direccionOrigen,direccionDestino);
							fclose(dFile);	
							return 	0;
						}
						else{
							contACK--;
							contDATA--;
							printf("Reenviando ACK %d\n",contACK);
							//reenviando el ack 
							reenviando=1;
							
						}
						
					}
					else{
						//le quitamos el opcode y el numero de bloque 
						memcpy(aux,bufer+6,tam-6);
						
						//checksum 
						printf("Verificando el checksum\n");
						check  = checkSum(aux,514);
						printf("check en el receptor= %04X\n",check);
						printf("check negado = %04X\n",(~check));
						//verificando el checksum 
						printf("Check de 512 = %04X\n",checkSum(aux,512));
						
						if((~check) == 0 || check == 0) {
							printf("Checksum correcto\n");
							printf("Copiando al archivo\n");
							//quitando los ultimos bytes 
							copiarArch(aux,tam-8,dFile);
							//Enviar siguiente ACK
							enviarACK(contACK,direccionOrigen,direccionDestino);
						}else{
							contACK--;
							contDATA--;
							printf("Reenviando ACK %d\n",contACK);
							//reenviando el ack 
							reenviando=1;
						
						}
					}		
				}
				else{
					contACK--;
					contDATA--;
					printf("Reenviando ACK %d\n",contACK);
					//reenviando el ack 
					reenviando=1;
					enviarACK(contACK,direccionOrigen,direccionDestino);
				}
				
				tipoTrama = OPCODE_ACK;
				
				break;
		}				
	}while(dFile);//FIN WHIILE
	
	//Se cierra el archivo
	fclose(dFile);	
}


void copiarArch(char* datos,int leidos,FILE* dFile){
	 
	 fwrite(datos,1,leidos,dFile);	
	 
}


