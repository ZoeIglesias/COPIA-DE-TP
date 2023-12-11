#ifndef PCB_H_
#define PCB_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <commons/collections/list.h>

//NUESTRAS
#include "variables_globales_kernel.h"
#include "../../shared/src/sockets/sockets.h"
#include "planificacion.h"
#include "../../shared/src/estructuras_compartidas/estructuras_compartidas.h"
#include "recursos.h"



extern int pid_global; //valor del pid que ira aumentando en cada pcb que se cree
extern int conexion_a_memoria;

// -----------------------------------
//Funciones generales
// -----------------------------------

void crear_pcb(char *path,int size, int prioridad);
void cambiar_estado_pcb(t_pcb *pcb, t_estado estado_nuevo);
void asignar_instrucciones_pcb(); //no se puede hacer hasta que veamos como recibimos el nuevo contexto de cpu y modifiquemos los datos de pcb, capas esto este incluso dentro de cpu 
void actualizar_contexto_pcb(t_pcb *pcb, t_contexto_ejecucion *contexto_actualizado);
char* estado_a_string(t_estado estado);
t_archivos_proceso* buscar_archivo_abierto(t_list *tabla_archivos_abiertos, char *nombre_archivo_read);
t_pcb *buscarPcb(int pid_a_buscar);
void liberar_pcb(t_pcb *pcb);
#endif