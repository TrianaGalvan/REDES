#include "checksum.h"

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