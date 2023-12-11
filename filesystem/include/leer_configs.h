#ifndef LEER_CONFIGS_H_
#define LEER_CONFIGS_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include "variables_globales_fs.h"

t_config* crear_config(char* config_path);

void destruir_config();

char *ip_memoria();

char *puerto_memoria();

char *puerto_escucha();

char *path_fat();

char *path_bloques();

char *path_fcb();

int cant_bloques_total();

int cant_bloques_swap();

int tam_bloque();

int retardo_acceso_bloque();

int retardo_acceso_fat();



#endif