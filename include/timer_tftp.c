#include <windows.h>
#include "timer_tftp.h"
#include <stdio.h>
#include <stdlib.h>
volatile int timeOut;
void iniciar_timeOut(DWORD milisegundos){
	
	unsigned int* ms = (unsigned int*)malloc(sizeof(unsigned int));
	
	if(ms == NULL){
		perror("timeout malloc");
	}
	*ms = milisegundos;
	
	// Iniciar el hilo para empezar a contar los 2 segundos	.	
	HANDLE thread = CreateThread(NULL, 0, ThreadFunc, (void*)ms, 0, NULL);
}


DWORD WINAPI ThreadFunc(void* data){
		printf("milisegundos hilo %ud:\n", *((unsigned int*)data));
		DWORD* milisegundos = (DWORD*)data;
		Sleep(*milisegundos);
		// Pedir permisos sobre timeOut;
		timeOut = 1;
		return 0;
}
