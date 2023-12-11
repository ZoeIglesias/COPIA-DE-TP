#ifndef CONECTAR_MODULOS_H_
#define CONECTAR_MODULOS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


//NUESTRAS
#include "conexion_cpu.h"
#include "conexion_memoria_fs.h"
#include "procesos.h"
#include "conexion_kernel.h"
#include "variables_globales_memoria.h"
#include "../../shared/src/protocolo_comunicacion/protocolo_comunicacion.h"
#include "leer_configs.h"
#include "sockets/sockets.h"

//FUNCIONES
void conectar_cliente_fs(int server);
void conectar_cliente_cpu(int server);
void conectar_cliente_kernel(int server);


#endif