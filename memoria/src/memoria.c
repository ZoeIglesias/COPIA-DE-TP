#include "../include/memoria.h"


sem_t crearEstructuras;
sem_t liberarEstructuras;

int main(int argc, char** argv){
    
    //Se crean los loggers
    logger_memoria = log_create("../memoria/memoria.log","MEMORIA",true,LOG_LEVEL_DEBUG);

    memoria = log_create("../memoria/memoriaNoObligatorio.log","MEMORIA-NOOB", true, LOG_LEVEL_DEBUG); //este es el logger para los no obligatorios
    
    //Se crea el config
    config_memoria = crear_config(argv[1]);

    //Inicio estructuras 
    iniciar_estructuras();
    //Creo el server de Memoria
    int server_memoria = iniciar_servidor(puerto_escucha(), memoria);

    // ------------------------------------
    // Essem_wait(&)pero que se conecte el Filesystem 
    // ------------------------------------
    
    conectar_cliente_fs(server_memoria);
    
    // ----------------------------------------------------------
    // Conexion creada con Filesystem (cliente)
    // ----------------------------------------------------------
    conectar_memoria_a_fs(ip_filesystem(),puerto_filesystem());
    
    // ----------------------------------------------------------
    // Conexiones creada con Cpu y Kernel (servidor)
    // ----------------------------------------------------------
    log_info(memoria, "LLEGA ACA");
    conectar_cliente_cpu(server_memoria);
    
    conectar_cliente_kernel(server_memoria);

    //cambiar a futuro
    //esta mal hecho pero funciona asi que ta bien
    sem_t a;
    sem_init(&a, 0, 0);
    sem_wait(&a);
    
    
    //FALTA EL SEMAFORO QUE IMPIDA QUE ROMPA TODO Y FRENar
    //liberar_instrucciones(instrucciones); NO SE DE DONDE SALE
    //liberar_programa();
    return 0;
}


void liberar_programa(){
    log_destroy(logger_memoria);
    config_destroy(config_memoria);
    //CLIENTES
    liberar_conexion(cliente_kernel);
    liberar_conexion(cliente_cpu);
    liberar_conexion(cliente_filesystem);
    //A SERVIDOR
    liberar_conexion(conexion_filesystem);

}
