#include "../include/conexion_cpu_memoria.h"
//   INITS     //
int conexion_memoria;

//  END INITS  //

void conectar_a_memoria(char *ip_memoria,char *puerto_memoria)
{
    conexion_memoria = crear_conexion(ip_memoria, puerto_memoria, cpu);
    
    //REALIZO HANDSHAKE
    realizar_handshake(2,conexion_memoria);

    log_info(cpu,"conexion con memoria realizada");

    //codigo_operacion cod = recibir_operacion(conexion_memoria);

    if(recibir_operacion(conexion_memoria) == OK){
       log_info(cpu, "Memoria nos envio OK");
    }else{
       log_error(cpu, "Cpu no recibio correctamente el tamanio de la pagina");
    }

    t_buffer *buffer = crear_buffer();
    buffer -> stream = recibir_buffer(&(buffer->size),conexion_memoria);

    log_info(cpu,"recibio operacion de memoria");

    memcpy(&tam_pag, buffer->stream, sizeof(int));
    
    liberar_buffer(buffer);
    //liberar_conexion(conexion_memoria);
}

t_instruccion *solicitar_instruccion_a_memoria(uint32_t program_counter, int pid)
{   
    //Solicito instruccion
    t_paquete *paquete = crear_paquete(SOLICITUD_INSTRUCCION);

    agregar_a_paquete(paquete, &program_counter, sizeof(uint32_t));
    agregar_a_paquete(paquete, &pid, sizeof(int));

    enviar_paquete(paquete, conexion_memoria);

    //codigo_operacion cod_op = recibir_operacion(conexion_memoria);
    t_buffer *buffer = crear_buffer();

    if(recibir_operacion(conexion_memoria) == INSTRUCCION){
       log_info(cpu, "Memoria nos envio la instruccion");
    }else{
        log_error(cpu, "Ocurrio un error al recibir la instruccion");
    }
    
    buffer->stream = recibir_buffer(&(buffer->size), conexion_memoria);

    t_instruccion *instruccion = deserializar_instruccion_solicitada(buffer);
    
    liberar_buffer(buffer);

    return instruccion;
}


int solicitar_numero_de_marco(int num_pagina, int pid)
{
    t_paquete *paquete = crear_paquete(SOLICITUD_MARCO);

    agregar_a_paquete(paquete, &num_pagina, sizeof(int));
    agregar_a_paquete(paquete, &pid, sizeof(int));

    enviar_paquete(paquete,conexion_memoria);

    int num_marco;

    if(recv(conexion_memoria, &num_marco, sizeof(int), MSG_WAITALL) <= 0)
    {
        log_error(cpu, "Ocurrio un error al recibir el numero de marco");
        return -1;
    }

   
    return num_marco;
}

char* leer_de_memoria(int dir_fisica, int pid)
{
    t_paquete *paquete = crear_paquete(LEER_VALOR);

    agregar_a_paquete(paquete,&dir_fisica,sizeof(int));

    agregar_a_paquete(paquete,&pid, sizeof(int));

    enviar_paquete(paquete,conexion_memoria);

    uint32_t valor_leido = recibir_mensaje(conexion_memoria);

    char* valor_cadena = (char*)malloc(12 * sizeof(char));

    snprintf(valor_cadena, 12, "%u", valor_leido);
 
    return valor_cadena;

}

void escribir_en_memoria(int dir_fisica,int pid, uint32_t registro)
{
    t_paquete *paquete = crear_paquete(ESCRIBIR);

    agregar_a_paquete(paquete, &dir_fisica, sizeof(int));

    agregar_a_paquete(paquete, &pid, sizeof(int));

    agregar_a_paquete(paquete, &registro, sizeof(uint32_t));

    enviar_paquete(paquete,conexion_memoria);

    //recibir_operacion(conexion_memoria);

    char *mensaje = recibir_mensaje_char(conexion_memoria);

    if(strcmp(mensaje, "OK") != 0)
    {
        log_error(cpu, "Error al escribir en memoria");
    }

    free(mensaje);
}

