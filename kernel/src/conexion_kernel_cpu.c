#include "../include/conexion_kernel_cpu.h"    
   
int conexion_cpu_interrupt;
int conexion_cpu_dispatch;

void conectar_a_cpu(char *ip, char *puerto_cpu_dispatch,char *puerto_cpu_interrupt){
    
    //CONEXION DISPATCH
    conexion_cpu_dispatch = crear_conexion(ip, puerto_cpu_dispatch, kernel);
    
    realizar_handshake(3,conexion_cpu_dispatch);

    //CONEXION INTERRUPT
    conexion_cpu_interrupt = crear_conexion(ip, puerto_cpu_interrupt, kernel);
   
    realizar_handshake(3,conexion_cpu_interrupt);

}

