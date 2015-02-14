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

int mandarArchivo(char* nombre_archivo,char direccionOrigen, char direccionDestino);
// Se acabó el tiempo del timer.
extern volatile int timeOut;

int main(int argc, char *argv[])
{
	char nombre[50];
	int tam; 
	int respuestaRecibida;
	char *paquete;
	char bufer[TAM_BUF];
	char dirOrigen;
	char dirDestino;

	//verificamos si el numero de argumentos es correcto
	if(argc < 3){
		printf("Parámetros incompletos");
		printf("%s direccion_origen direccion_destino nombre_archivo", argv[0]);
		return 1;
	}
	//obteniendo nombre del archivo que se va a enviar
	strcpy(nombre,argv[3]);
	
	//direccion origen
	dirOrigen = argv[1][0];
	
	//direccion destino
	dirDestino = argv[2][0];
	
    inicializar();
    
	mandarArchivo(nombre,dirOrigen,dirDestino);
	terminar();
	system("PAUSE");
    return EXIT_SUCCESS;
}


//si regresa 1 se mando bien el archivo si regresa 0 hubo error en la lectura.
int mandarArchivo(char* nomA,char direccionOrigen, char direccionDestino){
	char bufer[BLOCK_SIZE+8];
	char buferArch[BLOCK_SIZE];
	char aux[BLOCK_SIZE];
	int leidos;
	int contador=BLOCK_SIZE;
    long bytes = 0;
	long contador_tx = 0;
	FILE* pFile;
	int tam;
	int contACK = 0;
	int contDATA = 1;
	int contERROR = 1;
	int respuestaRecibida = 0;
	int contadorIntentos = 1;
	int finTransmision = 0;
	int reenviando = 0;
	

	//abrir el archivo
	pFile = fopen(nomA , "rb" );
	
	//Enviar peticion al servidor para escribir 
	enviarWRQ(direccionOrigen,direccionDestino,nomA);
	//Empezar el temporizador
	iniciar_timeOut(TIME_OUT);
	//Esperar por el ack
	while(1){
		tam  = sizeof(bufer);
		rx(bufer,&tam);
		
		if(tam > 0){
			//Verificando las direcciones de origen destino y origen 
			if(bufer[0] == direccionOrigen && bufer[1] == direccionDestino){
				printf("tam = %d \n bufer \n  %s\n",tam,bufer);
				imprimir(bufer,tam);
				respuestaRecibida = 1;
				break;	
			}
			//no coincidieron las direcciones 
			else{
				//enviar un paquete de error y la comunicacion termina
				enviarERROR(5,err_codes[5],direccionOrigen,direccionDestino);
				return 1;
			}
		}
		else if(timeOut == 1){
			if(contadorIntentos != 5){
				//volver a enviar el WRQ
				enviarWRQ(direccionOrigen,direccionDestino,nomA);
			}
			else{
				printf("El receptor no contesta la petición\n");
				fclose(pFile);	
				return 1;
			}
		}
	
	}
	
	//verificamos si se recibio la respuesta 
	if(respuestaRecibida == 1){
		do{

			switch(bufer[2]){
				
				case OPCODE_ACK:
					//verificar que el siguiente paquete corresponda con el ack y que sea de la direccion destino que se espera
					if((bufer[3]+1 == contDATA) && filtroDireccion(direccionOrigen,direccionDestino)){
						//preparar el paquete de data con los datos leidos del archivo 
						leidos = fread(buferArch,1,contador,pFile);
						//verificar fin de transmision 
						if(leidos < 512){
							enviarDATA(contDATA,buferArch,leidos,direccionOrigen,direccionDestino);
							fclose(pFile);	
							return 0;
						}else{
							//Enviar paquete al servidor con los 512 bytes leidos de mi archivo
							enviarDATA(contDATA,buferArch,leidos,direccionOrigen,direccionDestino);					
						}
					
					}else{
						reenviando = 1;
						enviarDATA(contDATA,buferArch,leidos,direccionOrigen,direccionDestino);
					}
					
					break;
			
			}
			
			//Esperar respuesta del servidor 
			iniciar_timeOut(TIME_OUT);
			while(1){
		
				tam = sizeof(bufer);
				rx(bufer,&tam);
				
				if(tam > 0 ){
					printf("tam = %d \n bufer \n  %s\n",tam,bufer);
					
					//verificando direcciones
					if(bufer[0] == direccionOrigen && bufer[1] == direccionDestino){
						if(reenviando == 0){
							contACK++;
							contDATA++;	
							contadorIntentos = 0;
							reenviando = 0;	
							break;		
						}	
					}
					//la direccion no coincidio 
					else{
						//enviar un paquete de error y temrinar conexion 
						enviarERROR(5,err_codes[5],direccionOrigen,direccionDestino);
						return 1;
					}
					
					
				}
				else if(timeOut == 1){
					contadorIntentos++;
					//contar cuantas veces se esta reintentando
					if(contadorIntentos < 3){
						continue;
					}
					else{
						fclose(pFile);	
						//En ninguno de los intentos se logro enviar 
						return 1;
					}
				}
			}
		}while(pFile);
	}
	else{
		fclose(pFile);	
		return 1;
	}
	//Se cierra el archivo
	fclose(pFile);	
	return 0;

}



