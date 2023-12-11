#ifndef ESTRUCTURAS_COMPARTIDAS_H_
#define ESTRUCTURAS_COMPARTIDAS_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <stdint.h>
#include <semaphore.h>
#include "../sockets/sockets.h"
//#include "contexto_ejecucion/contexto_ejecucion.h"

extern sem_t hayInterrupcion;
extern sem_t contextoActualizado;

typedef struct{
   uint32_t AX; 
   uint32_t BX;
   uint32_t CX;
   uint32_t DX;
}t_registros;

typedef struct {
    int pid;
    uint32_t program_counter;
    t_registros *registros;
    t_list *instrucciones;
    int cant_instrucciones;
}t_contexto_ejecucion;

typedef struct
{
    codigo_operacion operacion;//EJ:SET,SUM,SUB
    int cantidad_parametros;//?
    char **parametros;
} t_instruccion;

typedef struct {
    int prioridad;
    t_list* tabla_archivos_abiertos;
    t_contexto_ejecucion* contexto_ejecucion;
    t_estado estado;
    t_list* recursos_asignados; //contiene t_recurso
}t_pcb;

typedef struct {
    int pid;
    t_list *elementos_asignados;
    char * elemento_bloqueante; 
}t_pcb_bloqueado;


typedef struct{
    char *nombre;
    int cantidad;
    int id_recurso_lista;
}t_recurso;

typedef struct{
    t_pcb *pcb;
    int tamanio_proceso_bytes; //cant de bytes
    char *nombre_proceso; //nombre del archivo de pseudocodigo 
    int * bloques_asignados;
}t_proceso_memoria;


typedef struct{
    int marco;
    bool presencia;
    bool modificado;
    int pos_en_swap;
    time_t tiempo_carga;//Para FIFO
    time_t ultima_referencia;//Para LRU
    int PID;
    int indice_pag;
}t_pagina;

typedef struct{
    char* nombre;
    int tamanio;
}t_fcb_kernel;

typedef struct{
    t_fcb_kernel* fcb;
    int puntero_bloque;
    locks tipo_lock;
    t_list* participantes_lock;
}t_archivos_proceso;


typedef struct 
{
    t_list* lista_instrucciones;
    int pid;
    
}t_instrucciones_proceso;


//--------------------------------------
//Funciones para estructuras compartidas
//--------------------------------------
void liberar_pcb(t_pcb *pcb);
void *liberarRecursos(t_recurso *recurso);
void *liberarArchivos(t_archivos_proceso* archivo);

#endif