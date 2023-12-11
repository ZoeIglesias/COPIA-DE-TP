#ifndef VARIABLES_GLOBALES_FS_H_
#define VARIABLES_GLOBALES_FS_H_


//NUESTRAS
#include "sockets/sockets.h"
#include "configs/configs.h"
#include "semaphore.h"

    //VARIABLES
    extern t_log* logger_fs;
    extern t_log* filesystem;
    extern t_config* config_fs;
    
    //SOCKETS
    extern int conexion_memoria;
    extern int cliente_kernel_fs;
    extern int cliente_memoria;
    extern sem_t sem_fat;
    extern sem_t sem_bloques;

#endif