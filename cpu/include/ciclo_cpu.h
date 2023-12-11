#ifndef CICLO_CPU_H_
#define CICLO_CPU_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <semaphore.h>
#include <pthread.h>
#include <ctype.h>
#include <stdbool.h>

//NUESTRAS
#include "variables_globales_cpu.h"
#include "../../kernel/include/planificacion.h"//para los semaforos y el t_motivo_desalojo
#include "../../shared/src/registros/registros.h"
#include "leer_configs.h"
#include "../../shared/src/estructuras_compartidas/estructuras_compartidas.h"
#include  "../../kernel/include/recursos.h"
#include "../../cpu/include/conexion_cpu_memoria.h"
#include "conectar_cliente_kernel.h"



//extern sem_t mutexCTX; nose si es necesario al final porque agregamos semafore de finDeCiclo.

pthread_t hilo_interrupciones;
extern int finCiclo;
// ---------------------
// FUNCIONES GENERALES
// ---------------------
void comenzar_ciclo_instruccion(t_contexto_ejecucion* ctx);
t_instruccion *fetch(t_contexto_ejecucion *ctx);
int execute(t_instruccion *instruccion, t_contexto_ejecucion *ctx);
bool decode(t_instruccion *instruccion);
void check_interrupt(t_contexto_ejecucion *ctx);
int traducir_direccion_mmu(int, t_contexto_ejecucion*);
bool es_numero_valido(const char *str);
int numero_pagina(int dir_logica);


#endif