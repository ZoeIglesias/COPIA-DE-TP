#include "../include/conexion_kernel_memoria.h"    
   
   
void conectar_a_memoria(char *ip, char *puerto){
    
    conexion_memoria = crear_conexion(ip, puerto, kernel);
    
    realizar_handshake(3,conexion_memoria);
    
    //creo un hilo para memoria modo cliente
    pthread_t conexion_a_memoria;
    pthread_create(&conexion_a_memoria, NULL,(void*)recibir_mensaje_memoria, &conexion_memoria);

    //pthread_join(conexion_memoria, NULL);
    //liberar_conexion(conexion_a_memoria);

}

void* recibir_mensaje_memoria(){

    return "recibio mensaje de memoria";
    
}