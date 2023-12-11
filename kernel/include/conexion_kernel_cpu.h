#ifndef CONEXION_KERNEL_CPU_H_
#define CONEXION_KERNEL_CPU_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


//NUESTRAS
#include "variables_globales_kernel.h"
#include "../../shared/src/protocolo_comunicacion/protocolo_comunicacion.h"
//#include "../../shared/src/contexto_ejecucion/contexto_ejecucion.h"
#include "planificacion.h"
#include "sockets/sockets.h"

#include "../../shared/src/estructuras_compartidas/estructuras_compartidas.h"

//VARIABLES GLOBALES PARA ESTE FILE
extern int conexion_cpu_interrupt;
extern int conexion_cpu_dispatch;

//cree las conexiones de ambos puertos en una misma funcion (a discutir con el team)
void conectar_a_cpu(char *ip, char *puerto_cpu_dispatch, char *puerto_cpu_interrupt);

void* recibir_mensaje_dispatch();

void* recibir_mensaje_interrupt();


#endif