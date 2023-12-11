#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//NUESTRAS
#include "conectar_modulos.h"
#include "variables_globales_memoria.h"
#include "conexion_memoria_fs.h"
#include "leer_configs.h"
#include "../../kernel/include/planificacion.h" //revisar
#include "sockets/sockets.h"
#include "configs/configs.h"
#include "procesos.h"

//FUNCIONES
void liberar_programa();

//extern int cliente_kernel;



#endif