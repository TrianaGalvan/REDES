#ifndef	_CANAL_H_
#define	_CANAL_H_

int inicializar(void);
int tx(char paquete[],int tam);
int rx(char paquete[], int *tam);
int imprimir(char paquete[],int tam);
int terminar(void);

#endif
