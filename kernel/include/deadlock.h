#ifndef DEADLOCK_H_
#define DEADLOCK_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>


//NUESTRAS
#include "../include/pcb.h"
//#include "kernel.h"
#include "sockets/sockets.h"
#include "../../shared/src/contexto_ejecucion/contexto_ejecucion.h"
#include "recursos.h"
#include "../../shared/src/estructuras_compartidas/estructuras_compartidas.h"
#include "archivos_fs.h"
#include "planificacion.h"



void procesos_en_blocked();
void analizarDeadlock();
bool detectar_deadlock();//a medida que se agregen colas a la lista block, se debe chequear que no haya deadlock
bool list_contains(t_list* lista, char* nombre_elemento);
int contar_apariciones(t_list* lista, int pid);
int cadena_a_continuar_cadena(t_pcb_bloqueado* pcb_bloqueado, t_list* lista);
void sin_repetidos(t_list* lista);
void agregar_elementos_asignados(t_pcb_bloqueado* pcb_bloqueado, t_pcb* pcb);
void *liberar_pcb_block(t_pcb_bloqueado* pcb_bloqueado);
void libera_listas(t_list *lista_a_liberar);




#endif