#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include "tftp.h"
#include "canal.h"


//pasar mi estructura a un arreglo
int structToArray(Datagrama* datagrama, char** trama){
	//pedir memoria para mi arreglo
	int numBytes = 0;
	int indice = 0; 
	
	*trama = (char*)calloc(1,sizeof(Datagrama));


	if(*trama == NULL){
		perror("Error malloc");
		printf("\nbien despues de calloc\n");
	}
	
	printf("\ntrama despues del if= %p\n",trama);
	
	//direccion
	*trama[indice] = datagrama->tid;
	indice++;
	numBytes++;	
	
	//opcode
	memcpy((*trama)+indice, &(datagrama->formato.opcode), 2);
	indice += 2;
	numBytes+=2;
	
	switch(datagrama->formato.opcode){
		case OPCODE_DATA:
			DATA *data;
			data = (DATA*) &(datagrama->formato);
			
			// Copiar block-num.
			memcpy((*trama)+indice, &(data->blockNum), sizeof(char)*2);
			indice += 2;
			numBytes += 2;
			
			// Copiar data
			memcpy((*trama)+indice, data->msg, data->longMsg);
			numBytes += data->longMsg;
			
			break;
		case OPCODE_ERR:
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
		case OPCODE_RRQ:
			int strlenFileName;
			int strlenMode;
			RRQ *rrq;
			
			rrq = (RRQ*) &(datagrama->formato);
			
			strlenFileName = strlen(rrq->fileName);
			strlenMode = strlen(rrq->mode);
			
			memcpy((*trama)+indice,rrq->fileName,strlenFileName);
			numBytes += strlenFileName;
			indice += strlenFileName; 	
		
			*trama[indice] = 0;
			numBytes++;
			indice += 1;
			
			memcpy((*trama)+indice, rrq->mode,strlenMode);
			numBytes += strlenMode;
			indice+= strlenMode;
			
			*trama[indice] = 0;
			
			break;
		case OPCODE_WRQ:
			WRQ *wrq;
			wrq = (WRQ*) &(datagrama->formato);

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
			ACK *ack = (ACK*)&(datagrama->formato);
			
			// Copiar block num
			memcpy(((*trama)+indice), &(ack->blockNum), sizeof(char)*2);
			numBytes += 2;
			
			break;
	}

	return numBytes;
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
	tam = structToArray(datagrama, &paquete);
	
	tx(paquete,tam);
	
}

void enviarWRQ(char direccion ,char* nombreArchivo){
	char* paquete;
	//pedir memoria para la estructura genérica
	Datagrama *datagrama = (Datagrama*) calloc(1,sizeof(Datagrama));
	int bytesAEnviar;

	//Direccion(tid)
	datagrama->tid = direccion;
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

void enviarRRQ(char direccion ,char* nombreArchivo){
	char* paquete;
	int bytesAEnviar;
	
	//pedir memoria para la estructura genérica
	Datagrama *datagrama = (Datagrama*) calloc(1,sizeof(Datagrama));
	
	//Direccion(tid)
	datagrama->tid = direccion;
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

void enviarERROR(int errorCode,char* error,char dir){
	char* paquete;
	int bytesAEnviar;
	
	//pedir memoria para la estructura genérica
	Datagrama *datagrama = (Datagrama*) calloc(1,sizeof(Datagrama));
		
	//Direccion(tid)
	datagrama->tid = dir;
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

void enviarDATA(int numB, char* informacion, int tamInformacion, char dir){
	char* paquete;
	int bytesAEnviar;
	
	//pedir memoria para la estructura genérica
	Datagrama *datagrama = (Datagrama*) calloc(1,sizeof(Datagrama));
		
	//Direccion(tid)
	datagrama->tid = dir;
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
	
	tx(paquete, bytesAEnviar);

}
