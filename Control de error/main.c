#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_CHECKSUM 2

/* run this program using the console pauser or add your own getch, system("pause") or input loop */
short int checkSum(char * trama,int tam);
void imprimirSecciones(char *trama, int tam);
void imprimirBytesTrama(char *trama, int tam);

int main(int argc, char *argv[]) {
	
	// mensaje que se enviará
	char msg[] = "Hola triana andalucia prieto galvan!";
	// Trama en la que se coloca el mensaje
	unsigned char * trama;
	
	/* Inicializar la trama. 
	 * Se pide una trama del tamaño del mensaje y espacio para el checksum (TAM_CHECKSUM bytes).
	 */
	int tamMsg = strlen(msg);
	
	printf("Tamaño Mensaje: %d\n", tamMsg);
	
	trama = (unsigned char*)calloc(tamMsg + TAM_CHECKSUM, sizeof(char));
	
	// Copiar el mensaje a la trama
	memcpy(trama, msg, tamMsg);
	
	// Cálculo de checksum
	short int check = checkSum(trama, tamMsg);
	
	//unsigned char * checkPtr = (unsigned char*)&check;
	
	//unsigned char altaCheck = checkPtr[0];
	//unsigned char bajaCheck = checkPtr[1];
	
	printf("CheckSum calculado: %04X\n", check);
	//printf("CheckSum alto: %02X\n", altaCheck);
	//printf("CheckSum bajo: %02X\n", bajaCheck);
	
	// Colocar el checksum
	//trama[tamMsg] = altaCheck;
	//trama[tamMsg+1] = bajaCheck;
	*((short int*)(trama+tamMsg)) = check;
	
	// Secciones de la trama a verificar
	printf("Secciones de la trama:\n");
	imprimirSecciones(trama, tamMsg+TAM_CHECKSUM);
	
	printf("Bytes de la trama:\n");
	imprimirBytesTrama(trama, tamMsg+TAM_CHECKSUM);
	
	checkSum(trama, tamMsg + TAM_CHECKSUM);
	
	return 0;
}
short int checkSum(char * trama,int tam){
	short int *tramaInt = (short int*)trama;
	int primerByte, segundoByte;
	long int suma  = 0;
	int i;
	short int sumaFinal = 0;
	for(i = 0; i < tam/2; i++){
		suma += *tramaInt;
		tramaInt++;
	}
	//Eliminando los acarreos y sacandole el complemento  
	sumaFinal = ~((suma&0x0000FFFF) + (suma>>16));
	
	return sumaFinal;
}

void imprimirSecciones(char *trama, int tam){
	int i = 0;
	short int *tramaInt = (short int*)trama;
	for(i = 0; i < tam/2; i++){
		printf("%d .- %04X\n",i,*tramaInt);
		tramaInt++;
	}
}

void imprimirBytesTrama(char *trama, int tam){
	int i=0;
	for(i = 0; i < tam; i++){
		if(i%16 == 0){
			printf("\n");
		}
		printf("%02X ", trama[i]);
	}
	printf("\n");
}





