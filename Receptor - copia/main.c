#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "canal.h"
#include "tftp.h"
#include "timer_tftp.h"
#define TIME_OUT 5000
#define TAM_BUF 50

extern volatile int timeOut;
int contACK = 0;
int recibirArchivo(char direccion);
void copiarArch(char*,int,FILE*);
int filtroDireccion(char dirA,char dirT);

int main(int argc, char *argv[])
{

	int leidos,contador=200;
	char nombre[50];
	char dir,bufer[512];
	int tam;
	//verificamos que el numero de argumentos sea correcto
	if(argc <= 1){
		printf("Parámetros incompletos");
		printf("%s direccion",argv[0]);
		return 1;
	}
	//asignando argumentos
	dir = argv[1][0];
	
	inicializar();
	
//	while(1){
//		 tam = sizeof(bufer);	
//		 
//		 rx(bufer,&tam);			
//
//		if(tam > 0){
//			printf("tam = %d  bufer = %s\n",tam,bufer);
//			imprimir(bufer,tam);
//			break;
//		}	
//	
//	}
//	enviarACK(contACK,dir);
//	
		
	recibirArchivo(dir);

	terminar(); 
    system("PAUSE");
    return EXIT_SUCCESS;
}
int filtroDireccion(char dirA,char dirT){
	//verificamos el campo de direccionamiento
	if(dirA == dirT){
		//retorna 0 si es igual al campo de direccion
		return 1;
	}
	return 0;
}


int recibirArchivo(char direccion){
	char bufer[BLOCK_SIZE+19];
	char buferArch[BLOCK_SIZE];
	int tam=0;
	int abrirArch=0;
	char aux[BLOCK_SIZE];
	signed long bytes=0;
	int opcode;
	int contDATA = 1;
	int contACK = 0;
	int reenviando = 0;
	int contadorIntentos = 0;
	int tipoTrama =0;
	//Apertura del archivo 
	FILE* dFile;
		
    dFile = fopen("archivoRecibido.bmp","wb");
	if(dFile){
		abrirArch = 1;	
	}
	else{
		printf( "Error (NO ABIERTO)\n" );	
	}
	
		
	do{
		//limpiamos el bufer
	
	 	//Esperar respuesta del servidor 
		iniciar_timeOut(TIME_OUT);
		while(1){
			memset(bufer,0,sizeof(char)*BLOCK_SIZE+8);	
			tam = sizeof(bufer);
			rx(bufer,&tam);
			
			if(tam > 0 ){
				printf("tam = %d \n bufer \n  %s\n",tam,bufer);
				if(reenviando == 0 && tipoTrama == OPCODE_ACK){
					contACK++;
					contDATA++;
					contadorIntentos = 0;
					reenviando = 0;	
				}
				break;	
			}
			else if(timeOut == 1){
				contadorIntentos++;
				//contar cuantas veces se esta reintentando
				if(contadorIntentos < 3){
					continue;
				}
				else{
					//En ninguno de los intentos se logro enviar 
					fclose(dFile);	
					return 1;
				}
			}
		}	

		
		switch(bufer[2]){
			case OPCODE_WRQ:
				enviarACK(contACK,direccion);
				tipoTrama = OPCODE_ACK;
				break;
			case OPCODE_DATA:
				//Verificacion de que es el ack que se esta esperando 
				if(contACK == bufer[3]){
					//verificar si es la ultima transmision 
					if(tam < 512){
						//le quitamos el opcode y el numero de bloque 
						memcpy(aux,bufer+5,tam-5);
						copiarArch(aux,tam-5,dFile);
						
						enviarACK(contACK,direccion);
						fclose(dFile);	
						return 	0;
					}
					else{
						//le quitamos el opcode y el numero de bloque 
						memcpy(aux,bufer+5,tam-5);
						copiarArch(aux,tam-5,dFile);
						
						//Enviar siguiente ACK
						enviarACK(contACK,direccion);
							
					}	
				}
				else{
					//reenviando el ack 
					reenviando=1;
					enviarACK(contACK,direccion);
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


