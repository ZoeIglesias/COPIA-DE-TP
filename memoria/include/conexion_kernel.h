#ifndef CONEXIONES_MEMORIA_H_
#define CONEXIONES_MEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//NUESTRAS
#include "sockets/sockets.h"
#include "variables_globales_memoria.h"
#include "estructuras_memoria.h"
#include "procesos.h"

void conectar_con_kernel(int *socket_kernel);



#endif