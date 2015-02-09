#include <windows.h>
#include "timer_tftp.h"

volatile int timeOut;
void iniciar_timeOut(DWORD milisegundos){
	
	DWORD* msThread = (DWORD*)malloc(sizeof(DWORD));
	*msThread = milisegundos;
	
	// Iniciar el hilo para empezar a contar los 2 segundos	.	
	HANDLE thread = CreateThread(NULL, 0, ThreadFunc, (void*)msThread, 0, NULL);
}


DWORD WINAPI ThreadFunc(void* data){	
		DWORD* milisegundos = (DWORD*)data;
		Sleep(*milisegundos);
		// Pedir permisos sobre timeOut;
		timeOut = 1;
		return 0;
}
