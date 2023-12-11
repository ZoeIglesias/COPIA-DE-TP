#ifndef LEER_CONFIGS_H_
#define LEER_CONFIGS_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include "variables_globales_cpu.h"

t_config* crear_config(char* config_path);

void destruir_config();

char *ip_memoria();

char *puerto_memoria();

//char *puerto_escucha_dispatch();

//char *puerto_escucha_interrupt();


#endif