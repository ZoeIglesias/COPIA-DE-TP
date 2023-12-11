#ifndef VARIABLES_GLOBALES_MEMORIA_H_
#define VARIABLES_GLOBALES_MEMORIA_H_
#include <semaphore.h>
//NUESTRAS
#include "sockets/sockets.h"
#include "configs/configs.h"

//SOCKETS ASOCIADOS AL PUERTO DE MEMORIA
extern int cliente_filesystem;
extern int cliente_cpu;
extern int cliente_kernel;

//SOCKETS DE MEMORIA ASOCIADOS A UN PUERTO
extern int conexion_filesystem;

//VARIABLES
extern t_log *logger_memoria; //este log es para los obligatorios
extern t_config *config_memoria;
extern t_log *memoria; //son los no obligatorios

extern t_list * lista_instrucciones_globales;

//SEMAFORO
extern sem_t terminaron_conexiones;

#endif