#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "canal.h"
#include "tftp.h"
#include <errno.h>

int mandarArchivo(char* nombre_archivo,char direccion);
void enviarPeticionConexion(char*,char);
char* structToArray(Datagrama*);

int main(int argc, char *argv[])
{

	int tam,leidos,contador=1000;
	FILE* pFile;
	char nombre[50];
	char *paquete;
	char dir;
	int recpuestaConexion,i;
	char prueba[] = "hola triana como estas";	
	//verificamos si el numero de argumentos es correcto
	if(argc < 2){
		printf("Parámetros incompletos");
		printf("%s direccion nombre_archivo",argv[0]);
		return 1;
	}
	
	inicializar();	
	printf("bien");
	//obteniendo parametros
	strcpy(nombre,argv[2]);

	dir = argv[1][0];
	printf("\ndatagrama en el main; %ld",sizeof(Datagrama));
	//pedir memoria para la estructura genérica 
	Datagrama *datagrama = (Datagrama*) calloc(1,sizeof(Datagrama)); 
	//DDireccion(tid)
	datagrama->tid = dir;
	datagrama->formato.opcode = OPCODE_WRQ;
	//bajando una capa (tftp)
	TFTP_FORMATO *wrq = (TFTP_FORMATO*) &(datagrama->formato);
	//Cast a la trama que requiero del tftp
	RRQ_WRQ *wrq_rrq = (RRQ_WRQ*)wrq;
	//Llenando el wrq_rrq
	wrq_rrq->opcode = OPCODE_WRQ;
	wrq_rrq->fileName = (char*)malloc(sizeof(char)*strlen(nombre));
	strcpy(wrq_rrq->fileName,nombre);
	wrq_rrq->mode = (char*)malloc(sizeof(char)*strlen(MODE_NETASCII));
	strcpy(wrq_rrq->mode,MODE_NETASCII);
	printf("bien antes de la funcion");
	//Convertir la estructura en un arreglo 
	paquete = structToArray(datagrama);
	tx(paquete,100);

//	//Enviar petición de conexión al receptor 	
//		enviarPeticionConexion(nombre,direccion);
	
//	respuesta = mandarArchivo(nombre,direccion);
//	
//	/*tam=sizeof(bufer);
//	rx(bufer,&tam);
//	imprimir(bufer,tam);*/
	terminar(); 
	return 0;
}

//pasar mi estructura a un arreglo 
char* structToArray(Datagrama* datagrama){
	//pedir memoria para mi arreglo 
	TFTP_FORMATO *tipoTrama;
	RRQ_WRQ *wrq; 
	int opcode;
	char *array;
	char *prueba;
	printf("\ndatagrama en la funcion; %ld",sizeof(Datagrama));
	array = (char*)calloc(34,sizeof(char));
	prueba = (char*)calloc(1,sizeof(char)*34);	
	printf("\nprueba despues del if= %p\n",array);
	printf("\narray = %p\n",array);
	if(array == NULL){
		perror("Error malloc");
		printf("\nbien despues de calloc\n");
	}
	printf("\narray despues del if= %p\n",array);
	//direccion
	array[0] = datagrama->tid;
	
	//opcode
	opcode = datagrama->formato.opcode;
	array[1] = (char)opcode;
	
	switch(opcode){
		case OPCODE_DATA:
			break;
		case OPCODE_ERR:
			break;
		case OPCODE_RRQ:
			break;
		case OPCODE_WRQ:
			tipoTrama = (TFTP_FORMATO*) &(datagrama->formato);
			wrq = (RRQ_WRQ*) tipoTrama;
			printf("tam = %d",strlen(wrq->fileName));
			memcpy(array+2,wrq->fileName,strlen(wrq->fileName));
			memcpy(array+strlen(wrq->fileName)+2,wrq->mode,strlen(wrq->mode));			
			break;
		case OPCODE_ACK:
			break;
	}
	
	return array;
}

//si rgeresa uno es que se logro la conexión si es 0 fallo la conexion 
void enviarPeticionConexion(char *nombre,char dir){
	char* peticion; 
	int respuesta; 
	
}


//si regresa 1 se mando bien el archivo si regresa 0 hubo error en la lectura.
int mandarArchivo(char* nomA,char dir){
	char bufer[1000],aux[1002];
	int leidos,contador=1000;
	signed long bytes = 0;
	long contador_tx = 0;
	FILE* pFile;

	//abrir el archivo
	pFile = fopen (nomA , "rb" );
	
	//leemos el archivo.
	while(pFile){
		memset(bufer,0,sizeof(char)*1000);
        memset(aux,0,sizeof(char)*1002);
		//leemos del archivo 1000 bytes
       leidos = fread(bufer,1,contador,pFile);        
		printf("leidos = %d\n",leidos);
		printf("bytes = %l\n",bytes);	
		bytes += leidos;
		printf("bytes = %l\n",bytes);	
		memcpy(aux+2,bufer,sizeof(bufer));
		//campo de control
		aux[0]='s';
		//campo de direccion
		aux[1] = dir;
        
		//verificamos si aun quedan datos por leer. 
        if(leidos != contador){
                 //verificamos si hubo algun error 
                  if (ferror (pFile)){
                       printf ("Error al leer el archivo.txt\n");  
					   return 1;         
                  }
                  //verificamos si fue fin de archivo
                  else if(feof(pFile)){
                  		//si fue fin de archivo colocalomos en la primera posicion una f indicando al receptor que se han terminado los datos
                  	   aux[0] = 'f';
                  	   //mandamos los 1000 bytes leidos.
				       contador_tx += tx(aux,leidos+2);
				       
						printf("bytes = %lu\n",bytes);
						printf("tx = %lu\n",contador_tx);
						printf("ultima = %d",leidos);
                       return 0;
                  }     
                        
       }
 
       //mandamos los 1000 bytes leidos.
       contador_tx +=   tx(aux,leidos+2);
       printf("bytes enviados: %d",contador_tx);
      
	}//while	

}
