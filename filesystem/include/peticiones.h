#ifndef PETICIONES_H_
#define PETICIONES_H_
#include <math.h>
#include <sockets/sockets.h>
#include "estructuras_principales.h"
#include "leer_configs.h"
#include <commons/string.h>

void iniciar_semaforos();
int abrir_archivo(char* nombreArchivo);
bool existe_fcb(char* nombreArchivo);
void truncar_fcb(char* nombreArchivo, int longitud);
void crear_fcb(char* nombre_archivo);
void agregar_bloques(t_FCB* fcb,int diff_bloques);
void sacar_bloques(t_FCB* fcb,int diff_bloques);
void actualizar_fcb(char* nombreArchivo, int longitud, int bloque_inicial);
int bloque_libre_FAT();
int bloque_libre_SWAP();
int* asignar_bloques_SWAP(int cantidad);
void liberar_bloques_SWAP(int* bloquesAsignados, int cantidad_bloques);
int buscar_bloque_archivo(int bloque_puntero, char* nombre_archivo);
void acceder_bloque_archivo(int bloque_fs, char* nombre_archivo);
int bloque_archivo_FAT(char* nombre_archivo, int posicion);
void escribir_bloque(void* contenido_pagina,int num_bloque);
void* leer_bloque(int num_bloque);

#endif