#include "../include/conexion_fs_memoria.h"

void conectar_fs_memoria(char *ip_memoria, char *puerto_memoria){

    conexion_memoria = crear_conexion(ip_memoria, puerto_memoria, filesystem);

    realizar_handshake(1,conexion_memoria);

    //liberar_conexion(conexion_memoria);
}