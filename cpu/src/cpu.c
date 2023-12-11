#include "../include/cpu.h"

char *puerto_escucha_dispatch();
char *puerto_escucha_interrupt();

int main(int argc, char** argv) {   

    logger_cpu = log_create("../cpu/cpu.log","CPU",true,LOG_LEVEL_DEBUG);

    cpu = log_create("../cpu/cpuNoObligatorio.log","CPU-NOOB", true, LOG_LEVEL_DEBUG); //este es el logger para los no obligatorios

    config_cpu = crear_config(argv[1]);
 
    //PRIMERO: creo conexion con Memoria como cliente
    conectar_a_memoria(ip_memoria(),puerto_memoria());

    //SEGUNDO: creo el server de CPU para ambos puertos
    int server_cpu_dispatch = iniciar_servidor(puerto_escucha_dispatch(), cpu);
    int server_cpu_interrupt = iniciar_servidor(puerto_escucha_interrupt(), cpu);
  
    //TERCERO: creo la conexion de KERNEL
    conectar_cliente_kernel_dispatch(server_cpu_dispatch);
    conectar_cliente_kernel_interrupt(server_cpu_interrupt);
    
    //cambiar a futuro
    //esta mal hecho pero funciona asi que ta bien
    sem_t a;
    sem_init(&a, 0, 0);
    sem_wait(&a);

    liberar_programa();

    return 0;
}

void liberar_programa(){
    log_destroy(logger_cpu);
    config_destroy(config_cpu);
}