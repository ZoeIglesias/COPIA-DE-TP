#ifndef CONEXION_KERNEL_MEMORIA_H_
#define CONEXION_KERNEL_MEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <sockets/sockets.h>

//NUESTRAS
#include "variables_globales_kernel.h"
#include "../../shared/src/protocolo_comunicacion/protocolo_comunicacion.h"


void conectar_a_memoria(char *ip, char *puerto);

void* recibir_mensaje_memoria();


#endif