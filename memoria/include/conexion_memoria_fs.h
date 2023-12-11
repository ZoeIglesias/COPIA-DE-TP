#ifndef CONEXION_MEMORIA_FS_H_
#define CONEXION_MEMORIA_FS_H_


#include <math.h>
//NUESTRAS
#include "variables_globales_memoria.h"
#include "../../shared/src/protocolo_comunicacion/protocolo_comunicacion.h"
#include "leer_configs.h"
#include "estructuras_memoria.h"
#include "../../shared/src/estructuras_compartidas/estructuras_compartidas.h"


//FUNCIONES
void esperar_server_fs(int cliente_filesystem);
void conectar_memoria_a_fs(char *ip_filesystem, char *puerto_filesystem);
void conectar_con_filesystem(int *socket_fs);
int * solicitar_SWAP(t_proceso_memoria *proceso);
void liberar_SWAP(t_proceso_memoria *proceso);
void solicitar_pagina(int num_pagina, int pid);


#endif