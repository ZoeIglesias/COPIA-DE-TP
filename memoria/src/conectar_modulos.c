#include "../include/conectar_modulos.h"

void conectar_cliente_fs(int server_memoria){

    cliente_filesystem = esperar_cliente(server_memoria,memoria);
    
    if(!verificar_cliente(cliente_filesystem,memoria)){
        log_error(memoria, "Ocurrio un error al reconocer al cliente FILE SYSTEM");
        exit(1);
    }
    
    //HILO --> En conexion_memoria_fs.c
    pthread_t hilo_con_filesystem;
    pthread_create(&hilo_con_filesystem, NULL,(void*)conectar_con_filesystem, &cliente_filesystem);
    //pthread_join(hilo_con_filesystem, NULL);

}

void conectar_cliente_cpu(int server_memoria){

    log_info(memoria, "espero a cpu");
    
    cliente_cpu = esperar_cliente(server_memoria,memoria);
    
    log_info(memoria, "Se espero al cliente");


    if(!verificar_cliente(cliente_cpu,memoria)){
        log_error(memoria, "Ocurrio un error al reconocer al cliente CPU");
        exit(1);
    }
    

    log_info(memoria,"conexion exitosa con CPU");

    t_paquete *paquete = crear_paquete(OK);
    int tam = tam_pagina();
    agregar_a_paquete(paquete, &tam, sizeof(int));
    enviar_paquete(paquete,cliente_cpu);
    
    log_info(memoria,"Envio paquete OK a cpu");
    
    //HILO --> En conexion_cpu.c
    pthread_t hilo_conexion_cpu;
    pthread_create(&hilo_conexion_cpu, NULL,(void*)operar_con_cpu, &cliente_cpu);

    //pthread_detach(hilo_conexion_cpu);
    //pthread_join(hilo_conexion_cpu, NULL);

}

void conectar_cliente_kernel(int server_memoria){

    cliente_kernel = esperar_cliente(server_memoria,memoria);

    if(!verificar_cliente(cliente_kernel,memoria)){
        log_error(memoria, "Ocurrio un error al reconocer al cliente KERNEL");
        exit(1);
    }
    
    //HILO --> En conexion_kernel.c
    pthread_t hilo_conexion_kernel;
    pthread_create(&hilo_conexion_kernel, NULL,(void*)conectar_con_kernel, &cliente_kernel);

    //pthread_join(hilo_conexion_kernel, NULL);
}


