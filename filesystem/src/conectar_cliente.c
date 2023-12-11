#include "../include/conectar_cliente.h"

void conectar_cliente_memoria(int server_filesystem){
    
    log_info(filesystem, "ENTRE A CONECTAR CLIENTE MEMORIA");
    //NOTIFICO  A MEMORIA
    uint32_t handshake = 1;
    uint32_t result;
    send(conexion_memoria,&handshake, sizeof(uint32_t), 0);
    log_info(filesystem,"Notifico a MEMORIA");
    recv(conexion_memoria, &result, sizeof(uint32_t), MSG_WAITALL);
    log_info(filesystem,"Me notifico MEMORIA");
   
   //ESPERO A QUE SE CONECTE
    cliente_memoria = esperar_cliente(server_filesystem,filesystem);
    
    if(!verificar_cliente(cliente_memoria,filesystem)){
        log_error(filesystem, "Ocurrio un error al reconocer al cliente MEMORIA");
        exit(1);
    }
    pthread_t conexion_filesystem_memoria;
    pthread_create(&conexion_filesystem_memoria, NULL,(void*)conectar_con_memoria, &cliente_memoria);

    //pthread_join(conexion_filesystem, NULL);

}

void conectar_kernel(int server_filesystem){
    
    cliente_kernel_fs = esperar_cliente(server_filesystem,filesystem);
    
    if(!verificar_cliente(cliente_kernel_fs,filesystem)){
        log_error(filesystem, "Ocurrio un error al reconocer al cliente KERNEL");
        exit(1);
    }
    pthread_t conexion_kernel;
    pthread_create(&conexion_kernel, NULL,(void*)conectar_con_kernel, &cliente_kernel_fs);
    
    //pthread_join(conexion_kernel, NULL);

}