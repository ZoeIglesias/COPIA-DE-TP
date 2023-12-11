#ifndef CONEXION_FS_MEMORIA_H_
#define CONEXION_FS_MEMORIA_H_

#include <sockets/sockets.h>

//NUESTRAS
#include "variables_globales_fs.h"
#include "../../shared/src/protocolo_comunicacion/protocolo_comunicacion.h"

//FUNCIONES
void conectar_fs_memoria(char *ip_memoria, char *puerto_memoria);



#endif