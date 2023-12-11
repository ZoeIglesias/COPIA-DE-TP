#include "../include/conexion_kernel_fs.h"    
   
   
void conectar_a_fs(char *ip, char *puerto){
    
    conexion_a_filesystem = crear_conexion(ip, puerto, kernel);
    
    realizar_handshake(3,conexion_a_filesystem);
    //creo un hilo para filesystem modo cliente
    pthread_t conexion_filesystem;
    pthread_create(&conexion_filesystem, NULL,(void*)recibir_mensaje_fs, &conexion_a_filesystem);

    //pthread_join(conexion_filesystem, NULL);
    //liberar_conexion(conexion_a_filesystem);

}


void* recibir_mensaje_fs(){
    return "recibio un mensaje de fs";
}