#ifndef LEER_CONFIGS_H_
#define LEER_CONFIGS_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>

//NUESTRAS
#include "variables_globales_kernel.h"



t_config *crear_config(char* config_path);

void destruir_config();

char *ip_memoria();

char *puerto_memoria();

char *ip_filesystem();

char *puerto_filesystem();

char *ip_cpu();

char *puerto_cpu_dispatch();

char *puerto_cpu_interrupt();

char *algoritmo_planificacion();

int quantum();

char **recursos_config();

char **instancias_recursos_config();

int grado_multiprogramacion();

#endif