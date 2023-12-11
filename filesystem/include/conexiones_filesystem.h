#ifndef CONEXIONES_MEMORIA_H_
#define CONEXIONES_MEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "sockets/sockets.h"
#include "peticiones.h"
#include "estructuras_principales.h"

//int* sockets_kernel
void* conectar_con_kernel();
void* conectar_con_memoria();


#endif