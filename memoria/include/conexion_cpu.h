#ifndef CONEXION_CPU_H_
#define CONEXION_CPU_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//NUESTRAS
#include "sockets/sockets.h"
#include "variables_globales_memoria.h"
#include "estructuras_memoria.h"
#include "leer_configs.h"
#include "../../shared/src/estructuras_compartidas/estructuras_compartidas.h"


//FUNCIONES
void operar_con_cpu(int *socket_cpu);
void enviar_instruccion(int *socket_cpu, t_buffer* buffer);
void responder_solicitud_de_marco(int socket_cpu, t_buffer* buffer);
t_list *obtener_instrucciones_proceso(int pid_enviado);
void leerInstruccionesDesdeArchivo(char *nombre_archivo, int pid_asociado);
codigo_operacion obtener_operacion_cpu(char *operacion);
int get_cant_parametros(codigo_operacion identificador);

#endif