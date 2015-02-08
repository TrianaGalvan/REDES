#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "canal.h"
#include "tftp.h"
int contACK = 0;
void recibirArchivo(char direccion);
void copiarArch(char*,int,FILE*);
int filtroDireccion(char dirA,char dirT);

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

int main(int argc, char *argv[])
{

	int leidos,contador=200;
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


