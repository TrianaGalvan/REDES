#include <windows.h>
//en el parametro se le pasa de cuanto va a ser el timer 
void iniciar_timeOut(DWORD milisegundos);

//funcion que ejecutara el hilo 
DWORD WINAPI ThreadFunc(void* data);
