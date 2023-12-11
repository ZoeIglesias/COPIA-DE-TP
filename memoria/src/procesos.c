#include "../include/procesos.h"   

t_list *procesos_de_memoria;

void crear_proceso(t_buffer *buffer)
{   
    int desplazamiento = 0;
    
    int size;
    memcpy(&size, buffer->stream + desplazamiento , sizeof(int));
    desplazamiento += sizeof(int);

    int tam_char;
    memcpy(&tam_char, buffer->stream + desplazamiento , sizeof(int));
    desplazamiento += sizeof(int);

    char  *nombre_archivo = malloc(tam_char);//LIBERAR
    memcpy(nombre_archivo, buffer->stream + desplazamiento , tam_char);
    desplazamiento += tam_char;
  
    log_info(memoria, "Se logro recibir los tres parametros");
    t_pcb *nuevo_pcb = deserializar_pcb_con_desplazamiento(buffer,&desplazamiento);
    log_info(memoria, "se logro deserializar");


    t_proceso_memoria *nuevo_proceso = malloc(sizeof(t_proceso_memoria));//LIBERAR

    nuevo_proceso->pcb = nuevo_pcb;
    
    nuevo_proceso->tamanio_proceso_bytes = size;
    
    nuevo_proceso->nombre_proceso = strdup(nombre_archivo);
    //nuevo_proceso->nombre_proceso = nombre_archivo;

    inicializar_tabla_paginas(nuevo_proceso);//aca se piden bloques swap

    list_add(procesos_de_memoria, nuevo_proceso);
    
    log_info(memoria, "Se leen las instrucciones de archivo");
    leerInstruccionesDesdeArchivo(nuevo_proceso->nombre_proceso, nuevo_pcb -> contexto_ejecucion -> pid);
    //free(nombre_archivo);

    log_info(memoria, "se termino de leer las isntrucciones del archivo");

}

void finalizar_proceso(int pid)
{
    t_proceso_memoria *proceso_a_liberar = obtener_proceso_memoria(pid);

    finalizar_tabla_paginas(proceso_a_liberar);

    free(proceso_a_liberar -> nombre_proceso);

    bool result = list_remove_element(procesos_de_memoria, proceso_a_liberar);

    if(!result)
    {
        log_error(memoria, "No se encontró el t_proceso_memoria en la lista global");
    }
    
    t_instrucciones_proceso *instrucciones_asociadas_a_pid = obtener_struct_instrucciones(pid);///HABALR CON CARO--> SEMAFORO
    
    liberar_instrucciones_proceso(instrucciones_asociadas_a_pid); //LIBERO t_instrucciones_proceso y t_instruccion
    
    free(proceso_a_liberar);//LIBERO t_proceso_memoria
}

t_proceso_memoria* obtener_proceso_memoria(int pid_a_buscar)
{
    t_proceso_memoria* proceso_memoria;

    for(int i=0; i< (list_size(procesos_de_memoria)); i++){
        proceso_memoria = list_get(procesos_de_memoria, i);
        if(proceso_memoria->pcb->contexto_ejecucion->pid == pid_a_buscar)
        {
            return proceso_memoria;
        }
    }
    return NULL;
}

t_instrucciones_proceso *obtener_struct_instrucciones(int pid_enviado)
{

    for(int i=0; i< (list_size(lista_instrucciones_globales));i++)
    {
        t_instrucciones_proceso *instrucciones_proceso = list_get(lista_instrucciones_globales,i);
        if(instrucciones_proceso -> pid == pid_enviado)
        {
            return instrucciones_proceso;
        }
    }

    return NULL;
}



