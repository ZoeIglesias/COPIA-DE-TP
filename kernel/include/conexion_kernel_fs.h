#ifndef CONEXION_KERNEL_FS_H_
#define CONEXION_KERNEL_FS_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


//NUESTRAS
#include "variables_globales_kernel.h"
#include "../../shared/src/protocolo_comunicacion/protocolo_comunicacion.h"
#include "sockets/sockets.h"

//FUNCIONES
void conectar_a_fs(char *ip, char *puerto);

void* recibir_mensaje_fs();


#endif