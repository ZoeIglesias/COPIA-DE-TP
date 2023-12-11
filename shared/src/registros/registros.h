#ifndef REGISTROS_H_
#define REGISTROS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

//NUESTRAS
#include "../estructuras_compartidas/estructuras_compartidas.h"
#include "../sockets/sockets.h"

// ---------------------
// FUNCIONES GENERALES
// ---------------------

uint32_t *obtener_registro(const char *nombre, t_registros *registros);
void asignar_a_registro(char* registro, char* valor, t_registros *registros);
void operar_registros(codigo_operacion operacion, char* registro_destino, char* registro_origen, t_registros *registros);
void crear_registros(t_pcb* pcb);

#endif