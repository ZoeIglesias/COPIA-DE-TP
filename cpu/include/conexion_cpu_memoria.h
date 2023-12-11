#ifndef CONEXION_CPU_MEMORIA_H_
#define CONEXION_CPU_MEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


//NUESTRAS
#include "sockets/sockets.h"
#include "contexto_ejecucion/contexto_ejecucion.h"
#include "variables_globales_cpu.h"
#include "../../shared/src/protocolo_comunicacion/protocolo_comunicacion.h"
#include "../../kernel/include/planificacion.h"//PARA LOS SEMAFOROS DE COMUNICACION CPU-MEMORIA
#include "../../shared/src/estructuras_compartidas/estructuras_compartidas.h"

//VARIABLES GLOBALES PARA ESTE FILE
extern int conexion_memoria;


// ---------------------
// FUNCIONES GENERALES
// ---------------------
void conectar_a_memoria(char *ip,char *puerto);
t_instruccion *solicitar_instruccion_a_memoria(uint32_t program_counter, int pid);
int solicitar_numero_de_marco(int num_pagina, int pid);
char* leer_de_memoria(int dir_fisica, int pid);
void escribir_en_memoria(int dir_fisica,int pid, uint32_t registro);


#endif