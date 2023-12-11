#ifndef ESTRUCTURAS_MEMORIA_H_
#define ESTRUCTURAS_MEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <sockets/sockets.h>
#include <configs/configs.h>
#include <semaphore.h>
//NUESTRAS
#include "leer_configs.h"
#include "variables_globales_memoria.h"
#include "procesos.h" 
#include "../../shared/src/estructuras_compartidas/estructuras_compartidas.h"
#include "../../shared/src/protocolo_comunicacion/protocolo_comunicacion.h"

//SEMAFOROS
extern sem_t espacio_usuario;

//ESTRUCTURAS
extern void *memoria_usuario;

typedef struct{
    int pid;
    t_list *tabla_paginas;
}t_tabla_por_proceso;


    
t_list *tablas_por_proceso;//Lista con TODAS las t_paginas
int *vector_marcos;

//INIT
void iniciar_estructuras();
void inicializar_tabla_paginas(t_proceso_memoria *proceso);
int asignar_frame_libre();

//REEMPLAZO
int aplicar_algoritmo_LRU();
int aplicar_algoritmo_FIFO();

//HERRAMIENTAS
t_list *obtener_paginas_en_memoria();
t_pagina *obtener_pagina(int frame);
t_tabla_por_proceso *obtener_tabla_proceso(int pid_asociado);
t_list *obtener_tabla_pagina(int pid);
int obtener_frame(int num_pag, int pid);
int obtener_cant_pags(int tam_proceso);
int get_index(int pid, int frame);
int indice_en_tabla(t_tabla_por_proceso *tabla_proceso);


//LIBERACION
void finalizar_tabla_paginas(t_proceso_memoria *proceso);
void liberar_frames_asignados(t_proceso_memoria *proceso);
void liberar_instruccion(t_instruccion *instruccion);
void liberar_instrucciones_proceso(t_instrucciones_proceso *instrucciones_proceso);
#endif