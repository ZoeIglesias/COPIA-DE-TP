#ifndef CONTEXTO_EJECUCION_H_
#define CONTEXTO_EJECUCION_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <commons/collections/list.h>

//NUESTRAS
#include "../estructuras_compartidas/estructuras_compartidas.h"
#include "../registros/registros.h"
#include "../sockets/sockets.h"
#include "../protocolo_comunicacion/protocolo_comunicacion.h"



// -----------------------------------
//Funciones generales
// -----------------------------------
void enviar_contexto(t_contexto_ejecucion *ctx, int socket,codigo_operacion cod_op);
t_contexto_ejecucion *recibir_contexto(int socket);
void liberar_contexto(t_contexto_ejecucion* contexto_ejecucion);


#endif 