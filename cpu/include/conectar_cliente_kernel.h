#ifndef CONECTAR_CLIENTE_KERNEL_H_
#define CONECTAR_CLIENTE_KERNEL_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sockets/sockets.h>

//NUESTRAS
#include "variables_globales_cpu.h"
#include "../../shared/src/protocolo_comunicacion/protocolo_comunicacion.h"
#include "ciclo_cpu.h"
#include "../../shared/src/contexto_ejecucion/contexto_ejecucion.h"
#include "../../shared/src/estructuras_compartidas/estructuras_compartidas.h"

// ---------------------
// FUNCIONES GENERALES
// ---------------------
void conectar_cliente_kernel_dispatch(int server_cpu_dispatch);
void conectar_cliente_kernel_interrupt(int server_cpu_interrupt);
void *esperar_contextos(int *socket_cliente_kernel);
void devolver_contexto(t_contexto_ejecucion *ctx,codigo_operacion cod_op);//sin parametros
void devolver_contexto_con_parametro(t_contexto_ejecucion *ctx,codigo_operacion cod_op, char *par1);//1 parametro
void devolver_contexto_con_parametros(t_contexto_ejecucion *ctx,codigo_operacion cod_op, char *par1, char *par2);//2 parametro
void devolver_contexto_con_int(t_contexto_ejecucion *ctx,codigo_operacion cod_op, char *par1, int dir_fisica);
void devolver_contexto_page_fault(t_contexto_ejecucion *ctx,int num_pagina);//1 parametro -> num pagina (int)



#endif