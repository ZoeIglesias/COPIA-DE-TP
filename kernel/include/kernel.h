#ifndef KERNEL_H_
#define KERNEL_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sockets/sockets.h>
#include <configs/configs.h>

//NUESTRAS
#include "conexion_kernel_memoria.h"
#include "conexion_kernel_cpu.h"
#include "conexion_kernel_fs.h"
#include "variables_globales_kernel.h"
#include "leer_configs.h"
#include "pcb.h"
#include "planificacion.h"
#include "recursos.h"
#include "consola.h"


//FUNCIONES
void liberar_programa();


#endif