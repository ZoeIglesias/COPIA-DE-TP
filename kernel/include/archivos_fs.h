#ifndef ARCHIVO_FS_H_
#define ARCHIVO_FS_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/collections/queue.h>
#include <commons/collections/list.h>

//NUESTRAS
#include "variables_globales_kernel.h"
#include "sockets/sockets.h"
#include "planificacion.h"
#include "deadlock.h"

//---------------------------------------
//Funciones de cada operacion filesystem
//---------------------------------------

//F_OPEN

bool existe_lock(char *nombre_archivo_open);
locks lock(char *nombre_archivo_open); 


//F_TRUNCATE
int truncar_archivo(char *nombre_archivo_read, int tamanio);

//F_SEEK
int actualizar_puntero_archivo(char *nombre_archivo_fseek, int posicion); 
t_archivos_proceso *buscar_archivo_en_tabla_archivos(char *nombre_archivo_fseek);

//cierra el archivo archivo
void cerrar_archivo(t_pcb* pcb, char* nombre_archivo_close);

#endif