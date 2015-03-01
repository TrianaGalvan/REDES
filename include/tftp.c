#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include "tftp.h"
#include "checksum.h"
#include "canal.h"


//pasar mi estructura a un arreglo
int structToArray(Datagrama* datagrama, char** trama){
	//pedir memoria para mi arreglo
	int numBytes = 0;
	int indice = 0; 
	char block_num_high;
	char block_num_low;
	short int check; 
	
	*trama = (char*)calloc(1,sizeof(Datagrama));
	
	if(*trama == NULL){
		perror("Error malloc");
		printf("\nbien despues de calloc\n");
	}
	
	//direccion	origen	
	memcpy((*trama)+indice,&(datagrama->dirOrigen),1);
	indice++;
	
	//direccion	destino	
	memcpy((*trama)+indice,&(datagrama->dirDestino),1);
	indice++;
	
	numBytes+=2;	
	
	//opcode
	char opcode_high = (datagrama->formato.opcode>>8)&0x00FF;
	char opcode_low = datagrama->formato.opcode & 0x00FF;
	
	memcpy((*trama)+indice, &opcode_high, 1);
	indice++;
	
	memcpy((*trama)+indice, &opcode_low, 1);
	indice ++;
	numBytes+=2;
	
	switch(datagrama->formato.opcode){
		case OPCODE_DATA:{
			DATA *data;
			data = (DATA*) &(datagrama->formato);
			
			*trama = (char*)realloc(*trama,502);
			
			// Copiar block num
			block_num_high = (data->blockNum>>8)&0x00FF;
			block_num_low = data->blockNum & 0x00FF;
			
			memcpy((*trama)+indice, &block_num_high, 1);
			indice++;
			memcpy((*trama)+indice, &block_num_low, 1);
			indice ++;
			numBytes+=2;
			
			// Copiar data
			memcpy((*trama)+indice, data->msg, data->longMsg);
			numBytes += data->longMsg;
			indice += data->longMsg;
			
			//calcular checksum 
			check = checkSum(data->msg,512);
			printf("checksum en struct to array = %04X",check);
			*((short int*)(*trama+indice)) = check;
			numBytes += 2;
			
			//imprimirTrama(*trama,numBytes);
			
			printf("numero de bloque: %d\n",data->blockNum);
			break;
	}
		case OPCODE_ERR:{
			ERROR_TRAMA *error;
			error = (ERROR_TRAMA*) &(datagrama->formato);
			
			//copiar codigo de error 
			memcpy((*trama)+indice, &(error->errorCode), 2);
			indice+=2;
			numBytes += 2;
			
			//copiar mensaje de error
			memcpy((*trama)+indice,&(error->errosMsg),strlen(error->errosMsg));
			numBytes += strlen(error->errosMsg);
			
			break;
		}
		case OPCODE_RRQ:{
			int strlenFileName;
			int strlenMode;
			RRQ *rrq;
			
			rrq = (RRQ*) &(datagrama->formato);
			
			strlenFileName = strlen(rrq->fileName);
			strlenMode = strlen(rrq->mode);
			
			memcpy((*trama)+indice,rrq->fileName,strlenFileName);
			numBytes += strlenFileName+1;
			indice += strlenFileName+1; 	
				
			memcpy((*trama)+indice, rrq->mode,strlenMode);
			numBytes += strlenMode+1;
		
			
			break;
 		}
		case OPCODE_WRQ:{
		
			WRQ *wrq;
			wrq = (WRQ*) &(datagrama->formato);
			
			memcpy((*trama)+indice,wrq->fileName, (sizeof(char)*strlen(wrq->fileName)));
			numBytes += strlen(wrq->fileName)+1;
			indice += strlen(wrq->fileName)+1;
			
			memcpy((*trama)+indice, wrq->mode, (sizeof(char)*strlen(wrq->mode)));
			numBytes += strlen(wrq->mode)+1;
			
			
			break;
		}
		case OPCODE_ACK:{

			ACK *ack = (ACK*)&(datagrama->formato);
			
			// Copiar block num
			block_num_high = (ack->blockNum>>8)&0x00FF;
			block_num_low = ack->blockNum & 0x00FF;
			
			memcpy((*trama)+indice, &block_num_high, 1);
			indice++;
			
			memcpy((*trama)+indice, &block_num_low, 1);
			indice ++;
			numBytes+=2;
			
			printf("numero de bloque: %d\n",ack->blockNum);
			
			break;
		}
	}
	if(datagrama->formato.opcode != OPCODE_DATA){
		imprimirTrama(*trama,numBytes);	
	}
	
	return numBytes;
}

void enviarACK(int num,char direccionOrigen ,char dirDestino){
	char* paquete;
	int tam = 0; 
	//pedir memoria para la estructura genérica
	Datagrama *datagrama = (Datagrama*) calloc(1,sizeof(Datagrama));

	//Direccion(tid)
	datagrama->dirOrigen = direccionOrigen;
	datagrama->dirDestino = dirDestino;
	
	//opcode
	datagrama->formato.opcode = OPCODE_ACK;

	//bajando una capa (tftp)
	ACK *ack = (ACK*) &(datagrama->formato);

	//Llenando el ack
	ack->blockNum = num;
	
	//Convertir la estructura en un arreglo
	tam = structToArray(datagrama, &paquete);
	
	tx(paquete,tam);
	
}

void enviarWRQ(char direccionOrigen , char direccionDestino ,char* nombreArchivo){
	char* paquete;
	//pedir memoria para la estructura genérica
	Datagrama *datagrama = (Datagrama*) calloc(1,sizeof(Datagrama));
	
	int bytesAEnviar;

	//Direccion(tid)
	datagrama->dirOrigen = direccionOrigen;
	datagrama->dirDestino = direccionDestino;
	
	//opcode
	datagrama->formato.opcode = OPCODE_WRQ;

	//bajando una capa (tftp)
	WRQ *wrq = (WRQ*) &(datagrama->formato);

	//Llenando el wrq_rrq
	wrq->fileName = (char*)malloc(sizeof(char)*strlen(nombreArchivo));
	strcpy(wrq->fileName, nombreArchivo);

	wrq->mode = (char*)malloc(sizeof(char)*strlen(MODE_NETASCII));
	strcpy(wrq->mode, MODE_NETASCII);

	//Convertir la estructura en un arreglo
	bytesAEnviar = structToArray(datagrama,&paquete);
	
	tx(paquete,bytesAEnviar);

}

void enviarRRQ(char dirOrigen ,char dirDestino,char* nombreArchivo){
	char* paquete;
	int bytesAEnviar;
	
	//pedir memoria para la estructura genérica
	Datagrama *datagrama = (Datagrama*) calloc(1,sizeof(Datagrama));
	
	//Direccion(tid)
	datagrama->dirOrigen = dirOrigen;
	datagrama->dirDestino = dirDestino;
	
	//opcode
	datagrama->formato.opcode = OPCODE_RRQ;

	//bajando una capa (tftp)
	RRQ *rrq = (RRQ*) &(datagrama->formato);

	//Llenando el wrq_rrq
	rrq->fileName = (char*)malloc(sizeof(char)*strlen(nombreArchivo));
	strcpy(rrq->fileName, nombreArchivo);

	rrq->mode = (char*)malloc(sizeof(char)*strlen(MODE_NETASCII));
	strcpy(rrq->mode, MODE_NETASCII);

	//Convertir la estructura en un arreglo
	bytesAEnviar = structToArray(datagrama,&paquete);
	
	tx(paquete,bytesAEnviar);

}

void enviarERROR(int errorCode,char* error,char dirOrigen , char dirDestino){
	char* paquete;
	int bytesAEnviar;
	
	//pedir memoria para la estructura genérica
	Datagrama *datagrama = (Datagrama*) calloc(1,sizeof(Datagrama));
		
	//Direccion(tid)
	datagrama->dirOrigen = dirOrigen;
	datagrama->dirDestino = dirDestino;
	
	//opcode
	datagrama->formato.opcode = OPCODE_ERR;
	
	//bajando una capa (tftp)
	ERROR_TRAMA *err = (ERROR_TRAMA*) &(datagrama->formato);
	
	//copiando codigo de error
	err->errorCode = errorCode;
	
	//copiando el mensaje de error
    err->errosMsg = (char*) malloc(sizeof(char)*strlen(error));
	strcpy(err->errosMsg,error);
	
	err->relleno = 0;
	
	//Convertir la estructura en un arreglo
	bytesAEnviar = structToArray(datagrama,&paquete);
	
	tx(paquete,bytesAEnviar);

}

void enviarDATA(int numB, char* informacion, int tamInformacion, char dirOrigen,char dirDestino){
	char* paquete;
	int bytesAEnviar;
	int sizeP;
	
	//pedir memoria para la estructura genérica
	Datagrama *datagrama = (Datagrama*) calloc(1,sizeof(Datagrama));
		
	//Direccion(tid)
	datagrama->dirOrigen = dirOrigen;
	datagrama->dirDestino = dirDestino;
	
	//opcode
	datagrama->formato.opcode = OPCODE_DATA;
	
	//bajando una capa (tftp)
	DATA *datos = (DATA*) &(datagrama->formato);
	
	//numero de bloque 
	datos->blockNum = numB;
	
	//copiando la informacion 
	size_t tamAlloc = sizeof(char)*tamInformacion;
	datos->msg = (char*)malloc(tamAlloc);
	memcpy(datos->msg, informacion, tamAlloc);
	
	// Copiar el tamaño de la información
	datos->longMsg = tamInformacion;
	
	//Convertir la estructura en un arreglo
	bytesAEnviar = structToArray(datagrama, &paquete);
	sizeP = strlen(paquete);

	
	tx(paquete, bytesAEnviar);
	

}

int filtroDireccion(char dirOrigen,char dirDestino){
	//verificamos el campo de direccionamiento
	if(dirOrigen  == dirDestino){
		//retorna 0 si es igual al campo de direccion
		return 1;
	}
	return 0;
}

void imprimirTrama(char* trama , int tam){
	int i;
	printf("bytes a imprimir: %d\n",tam);
	for(i = 0; i < tam ; i++){
		if(i % 16 == 0){
			printf("\n");
		}
		printf("%02X ",trama[i]);
	}
	printf("\n");
}
