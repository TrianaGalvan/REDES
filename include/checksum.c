#include "checksum.h"

unsigned short int checkSum(char * trama,int tam){
	unsigned short int *tramaInt = (unsigned short int*)trama;
	unsigned long int suma  = 0;
	int i;
	unsigned short int sumaFinal = 0;
	
	for(i = 0; i < tam/2; i++){
		suma += *tramaInt;
		tramaInt++;
	}
	
	//Eliminando los acarreos y sacandole el complemento  
	sumaFinal = ~((suma&0x0000FFFF) + (suma>>16));
	//printf("Con complemento = %04X\n",sumaFinal);
	
	return sumaFinal;
}
