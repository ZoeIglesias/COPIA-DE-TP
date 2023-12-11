#ifndef VARIABLES_GLOABLES_KERNEL_H_
#define VARIABLES_GLOABLES_KERNEL_H_


#include <stdio.h>
#include <stdlib.h>
#include <commons/temporal.h>
#include <commons/collections/list.h>

//NUESTRAS
#include "sockets/sockets.h"
#include "configs/configs.h"
#include "estructuras_compartidas/estructuras_compartidas.h"

//sockets
extern int conexion_a_filesystem;
extern int conexion_memoria;
//VARIABLES
extern t_log* logger_kernel;
extern t_log *kernel;
extern t_config* config_kernel;

extern t_temporal* TIEMPO_EN_CPU;

extern t_list* tabla_global_archivos_abiertos;

#endif