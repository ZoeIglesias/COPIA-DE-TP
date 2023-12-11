#include "../include/variables_globales_fs.h"


t_log* logger_fs;
t_log* filesystem;
t_config* config_fs;

//SOCKETS
int cliente_kernel_fs;
int cliente_memoria;
int conexion_memoria;
sem_t sem_bloques;
sem_t sem_fat;