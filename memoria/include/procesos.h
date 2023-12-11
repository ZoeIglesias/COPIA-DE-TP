#ifndef PROCESOS_H_
#define PROCESOS_H_

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> 
#include <commons/collections/list.h>

//NUESTRAS
//#include "conectar_modulos.h"
#include "../../shared/src/estructuras_compartidas/estructuras_compartidas.h"
#include "variables_globales_memoria.h"
#include "conexion_memoria_fs.h"
#include "conexion_cpu.h"




extern t_list *procesos_de_memoria;

//extern int cliente_kernel;
//extern int conexion_filesystem;

void enviar_proceso_fs();
void crear_proceso(t_buffer *t_buffer);
void liberar_estructuras_pcb(t_pcb*);
t_proceso_memoria* obtener_proceso_memoria(int pid_a_buscar);
void finalizar_proceso(int pid);
t_instrucciones_proceso *obtener_struct_instrucciones(int pid_enviado);
#endif