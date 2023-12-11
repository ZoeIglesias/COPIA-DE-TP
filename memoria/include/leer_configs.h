#ifndef LEER_CONFIGS_H_
#define LEER_CONFIGS_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>

//NUESTRAS
#include "variables_globales_memoria.h"

t_config *crear_config(char* config_path);

void destruir_config();

char *puerto_escucha();

char *ip_filesystem();

char *puerto_filesystem();

int tam_memoria();

int tam_pagina();

char* path_instrucciones();

int  retardo_respuesta();

char *algoritmo_reemplazo();



#endif