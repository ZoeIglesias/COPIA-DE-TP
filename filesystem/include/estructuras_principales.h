#ifndef ESTRUCTURAS_PRINCIPALES_H_
#define ESTRUCTURAS_PRINCIPALES_H_

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdint.h>
#include <semaphore.h>

//NUESTRAS
#include "leer_configs.h"
#include "variables_globales_fs.h"
#include "sockets/sockets.h"

typedef struct{
    char* NOMBRE_ARCHIVO;
    int TAMANIO_ARCHIVO;
    int BLOQUE_INICIAL;
}t_FCB;

typedef uint32_t t_FAT;

t_list* fcbs;
extern t_FAT* fat;
extern void* bloques;

int levantarFAT();
void iniciarFAT();
void liberarFAT();
void actualizarFAT();
t_FCB* buscarFCB(char* nombreArchivo);
void iniciarBLOQUES();
int levantarBLOQUES();
void actualizarBLOQUES();
void liberarBloques();
void log_crear_archivo(char* nombre);
void log_abrir_archivo(char* nombre);
void log_truncar_archivo(char* nombre, int tamanio);
void log_leer_archivo(char* nombre, int puntero, int direccion);
void log_escribir_archivo(char* nombre, int puntero, int direccion);
void log_acceso_FAT(int nro_entrada, uint32_t valor);
void log_acceso_Bloque_Archivo(char* nombre,int nro_bloque_arch, int nro_bloque_fs);
void log_acceso_Bloque_SWAP(int nro_bloque);

#endif