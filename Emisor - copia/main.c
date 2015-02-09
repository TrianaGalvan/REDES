#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "canal.h"
#include "tftp.h"
#include "timer_tftp.h"
#include <errno.h>

#define TIME_OUT 5000
#define TAM_BUF 50

int mandarArchivo(char* nombre_archivo,char direccion);
// Se acabó el tiempo del timer.
extern volatile int timeOut;

int main(int argc, char *argv[])
{
	char nombre[50];
	int tam; 
	int respuestaRecibida;
	char *paquete;
	char bufer[TAM_BUF];
	char dir;

	//verificamos si el numero de argumentos es correcto
	if(argc < 2){
		printf("Parámetros incompletos");
		printf("%s direccion nombre_archivo", argv[0]);
		return 1;
	}
	//obteniendo parametros
	strcpy(nombre,argv[2]);
	dir = argv[1][0];
	
    inicializar();
	
	enviarWRQ(dir,nombre);
	iniciar_timeOut(TIME_OUT);
	while(1){
		tam  = sizeof(bufer);
		rx(bufer,&tam);
		
		if(tam > 0){
			printf("tam = %d  bufer = %s\n",tam,bufer);
			imprimir(bufer,tam);
			respuestaRecibida = 1;
			break;
		}
		else if(timeOut == 1){
			printf("timeout expiró\n");
			break;
		}
	
	}
	terminar();

	return 0;
}


//si regresa 1 se mando bien el archivo si regresa 0 hubo error en la lectura.
int mandarArchivo(char* nomA,char dir){
	char bufer[1000];
	char aux[1002];
	int leidos;
	int contador=1000;
    long bytes = 0;
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
		printf("bytes = %ld\n",bytes);
		bytes += leidos;
		printf("bytes = %ld\n",bytes);
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
       printf("bytes enviados: %ld",contador_tx);

	}//while

    return 0;

}

