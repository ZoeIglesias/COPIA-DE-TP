#include "../include/kernel.h"

int main(int argc, char** argv) {

    //Se crea el logger
    logger_kernel = log_create("../kernel/kernel.log","KERNEL",1,LOG_LEVEL_DEBUG);

    kernel = log_create("../kernel/kernelNoObligatorio.log","KERNEL-NOOB", true, LOG_LEVEL_DEBUG); //este es el logger para los no obligatorios
    
    //Se crea el config
    config_kernel = crear_config(argv[1]);

    // ----------------------------------------------------------
    // Conexiones creadas con Cpu, Filesystem y Memoria (cliente)
    // ----------------------------------------------------------
    
    inicializar_semaforos();

    //conexion a cpu    
    conectar_a_cpu(ip_cpu(), puerto_cpu_dispatch(), puerto_cpu_interrupt());

    //conexion a filesystem
    conectar_a_fs(ip_filesystem(), puerto_filesystem());

    //conexion a memoria
    conectar_a_memoria(ip_memoria(), puerto_memoria());

    
    // ----------------------------------------------------------
    // Iniciar planificacion largo plazo
    // ----------------------------------------------------------

    crear_lista_recursos();

    inicializar_estructuras();
    
    //inicia los hilos de planificacion
    inicializar_hilos(); 

    //inicializar_colas_bloqueo();
    
    consola();
    
    
    //cambiar a futuro
    //esta mal hecho pero funciona asi que ta bien
    sem_t a;
    sem_init(&a, 0, 0);
    sem_wait(&a);

    liberar_programa();
    liberar_conexion(conexion_cpu_dispatch);
    liberar_conexion(conexion_cpu_interrupt);


    return 1;

}

void liberar_programa(){

    log_destroy(logger_kernel);
    config_destroy(config_kernel);

}

