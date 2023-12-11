#ifndef CONECTAR_CLIENTE_H_
#define CONECTAR_CLIENTE_H_

#include <pthread.h>
#include <sockets/sockets.h>
#include <configs/configs.h>
#include <stdint.h>

//NUESTRAS
#include "variables_globales_fs.h"
#include "conexiones_filesystem.h" 
#include "../../shared/src/protocolo_comunicacion/protocolo_comunicacion.h"

//FUNCIONES
void conectar_cliente_memoria(int server_filesystem);
void conectar_kernel(int server_filesystem);



#endif