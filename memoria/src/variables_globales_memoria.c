#include "../include/variables_globales_memoria.h"

t_log *logger_memoria;
t_log *memoria;
t_config *config_memoria;

//SOCKETS ASOCIADOS AL PUERTO DE MEMORIA
int cliente_filesystem;
int cliente_cpu;
int cliente_kernel;
int conexion_filesystem;

//SEMAFORO
sem_t terminaron_conexiones;

t_list * lista_instrucciones_globales;//tiene t_instrucciones_proceso