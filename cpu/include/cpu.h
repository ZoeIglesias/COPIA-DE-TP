#ifndef CPU_H_
#define CPU_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <pthread.h>
#include <sockets/sockets.h>
#include <configs/configs.h>

//NUESTRAS
#include "conexion_cpu_memoria.h"
#include "variables_globales_cpu.h"
#include "leer_configs.h"
#include "conectar_cliente_kernel.h"

//FUNCIONES
void liberar_programa();


#endif