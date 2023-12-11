#ifndef RECURSOS_H_
#define RECURSOS_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/collections/list.h>

//NUESTRAS
#include "sockets/sockets.h"
//#include "../../shared/src/contexto_ejecucion/contexto_ejecucion.h"
#include "../include/pcb.h"
#include "planificacion.h"
#include "../../shared/src/registros/registros.h" 
#include "leer_configs.h"
#include "variables_globales_kernel.h"

#include "../../shared/src/estructuras_compartidas/estructuras_compartidas.h"

extern t_list *recursos_del_sistema;


// -----------------------------------
//Funciones generales
// -----------------------------------
t_msj_error cumpla_verificaciones(t_pcb *pcb, char *recurso_recibido); //verifica que el recurso exista y haya sido solicitado por el proceso y devuelve mensajes
//bool fue_solicitado(t_pcb *pcb, char *recurso_recibido);
void crear_lista_recursos(); //cuando creo la lista de recursos a cada uno le debo asociar algun valor que vaya aumentando en cada uno para luego cargar la cola de blocked
t_recurso *solicitar_recurso(char *recurso_recibido);//pide el recurso que esta en la lista gneeral que tenga el mismo nombre y lo devuelve
void wait_recurso(t_pcb *pcb, char *recurso_recibido);
void signal_recurso(t_pcb *pcb, char *recurso_recibido);
bool recursos_cargados(t_list* recursos_asignados, char *recurso_recibido);
void eliminar_lista_recursos();
void eliminar_recursos(t_list* recursos_totales);
void liberar_recursos_pcb(t_pcb *pcb);
void disminuir_cantidad_recurso(t_pcb  *pcb, t_recurso *recurso);
void aumentar_cantidad_recursos(t_pcb  *pcb, t_recurso *recurso);


#endif