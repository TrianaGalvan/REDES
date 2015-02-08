#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "canal.h"
#include "tftp.h"
int contACK = 0;
void recibirArchivo(char direccion);
void copiarArch(char*,int,FILE*);
int filtroDireccion(char dirA,char dirT);
void enviarACK(int,char );
int  structToArray(Datagrama*,char**);
int main(int argc, char *argv[])
{

	int leidos,contador=200;
	FILE* pFile;
	char nombre[50];
	char dir,bufer[500];
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
	
	enviarACK(contACK,dir);
	while(1){
		tam = sizeof(bufer);	
		 rx(bufer,&tam);			

		if(tam > 0){
			printf("tam = %d  bufer = %s\n",tam,bufer);
			imprimir(bufer,tam);
			break;
		}
	}
		
//	recibirArchivo(dir);

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

//pasar mi estructura a un arreglo
int structToArray(Datagrama* datagrama,char** trama){
	//pedir memoria para mi arreglo
	RRQ_WRQ *wrq;
	int numBytes = 0;
	int indice; 
	
	*trama = (char*)calloc(1,sizeof(Datagrama));


	if(*trama == NULL){
		perror("Error malloc");
		printf("\nbien despues de calloc\n");
	}
	printf("\ntrama despues del if= %p\n",trama);
	//direccion
	*trama[0] = datagrama->tid;
	numBytes++;	
	//opcode
	*trama[1] = (char)datagrama->formato.opcode;
	numBytes++;
	
	switch(*trama[1]){
		case OPCODE_DATA:
			break;
		case OPCODE_ERR:
			break;
		case OPCODE_RRQ:
			break;
		case OPCODE_WRQ:
			wrq = (RRQ_WRQ*) &(datagrama->formato);
			indice = 2;	
			memcpy((*trama)+indice,wrq->fileName,strlen(wrq->fileName));
			numBytes += strlen(wrq->fileName);
			
			indice += strlen(wrq->fileName)+1; 
			*trama[indice] = 0;
			numBytes++;
			
			indice += 1;
			memcpy((*trama)+indice, wrq->mode, strlen(wrq->mode));
			numBytes += strlen(wrq->mode);
	
			indice+= strlen(wrq->mode)+1;
			*trama[indice] = 0;
			
			break;
		case OPCODE_ACK:
			break;
	}

	return numBytes;
}


void recibirArchivo(char direccion){
	char bufer[1002];
	int tam=0,abrirArch=0;
	char aux[1000];
	signed long bytes=0;
	int opcode;
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
		memset(bufer,0,sizeof(char)*1002);
		tam=sizeof(bufer);
		rx(bufer,&tam);
		
		//verificar el opcode
		opcode = (int)bufer[0];
		
		switch(opcode){
			case OPCODE_ACK:
				break;
			case OPCODE_DATA:
				break;
			case OPCODE_ERR:
				break;
			case OPCODE_RRQ:
				break; 
			case OPCODE_WRQ:
				
				break;
		}
		if(tam !=0){
			tam += (bufer[0]=='f')? 0 : 14;
			printf("tam-2 = %d\n",tam-2);
			printf("bytes = %l\n",bytes);		
			bytes+= tam-2;
			printf("bytes = %l\n",bytes);	
		}
		//Se verifica si el emisor nos mando tramas
		if(tam !=0 && filtroDireccion(direccion,bufer[1])){
			//limpiamos el arreglo auxiliar
			memset(aux,0,sizeof(char)*1000);
			//verificamos si ya no hay datos por leer
			if(bufer[0] == 'f'){
				printf("ultima = %d\n",tam-2);
			
			}

			//le quitamos el campo de control
			memcpy(aux,bufer+2,tam-2);
			
			copiarArch(aux,tam-2,dFile);	
					
		}
		
	}while(bufer[0] != 'f');//FIN WHIILE
	
	//Se cierra el archivo
	fclose(dFile);	
}


void copiarArch(char* datos,int leidos,FILE* dFile){
	 
	 fwrite(datos,1,leidos,dFile);	
	 
}

void enviarACK(int num,char direccion){
	char* paquete;
	int tam = 0; 
	//pedir memoria para la estructura genérica
	Datagrama *datagrama = (Datagrama*) calloc(1,sizeof(Datagrama));

	//Direccion(tid)
	datagrama->tid = direccion;
	datagrama->formato.opcode = OPCODE_ACK;

	//bajando una capa (tftp)
	ACK *ack = (ACK*) &(datagrama->formato);

	//Llenando el ack
	ack->blockNum = num;
	
	//Convertir la estructura en un arreglo
	tam = structToArray(datagrama,&paquete);
	
	tx(paquete,tam);
	
}
