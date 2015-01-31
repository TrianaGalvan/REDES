#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "canal.h"

int mandarArchivo(char* nombre_archivo,char direccion);

int main(int argc, char *argv[])
{

	int tam,leidos,contador=1000;
	FILE* pFile;
	char nombre[50];
	char respuesta;
	char direccion;
	
	//verificamos si el numero de argumentos es correcto
	if(argc < 2){
		printf("Parámetros incompletos");
		printf("%s direccion nombre_archivo",argv[0]);
		return 1;
	}
	inicializar();	
	//obteniendo parametros
	strcpy(nombre,argv[2]);
	direccion = argv[1][0];
	
	respuesta = mandarArchivo(nombre,direccion);
	
	/*tam=sizeof(bufer);
	rx(bufer,&tam);
	imprimir(bufer,tam);*/
	terminar(); 
	    //system("PAUSE");
	    return EXIT_SUCCESS;
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
