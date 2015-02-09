#include <windows.h>
#include "timer_tftp.h"
#include <stdio.h>
volatile int timeOut;
void iniciar_timeOut(DWORD milisegundos){
	// Iniciar el hilo para empezar a contar los 2 segundos	.	
	HANDLE thread = CreateThread(NULL, 0, ThreadFunc,&milisegundos, 0, NULL);
}


DWORD WINAPI ThreadFunc(void* data){	
		DWORD* milisegundos = (DWORD*)data;
		printf("Hola desde el hilo\n");
		Sleep(*milisegundos);
		// Pedir permisos sobre timeOut;
		timeOut = 1;
		return 0;
}
