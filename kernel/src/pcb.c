#include "../include/pcb.h"   


int pid_global = 1;

void crear_pcb(char *path, int size, int prioridad){
    
    t_pcb *nuevo_pcb = malloc(sizeof(t_pcb));

    nuevo_pcb->contexto_ejecucion = malloc(sizeof(t_contexto_ejecucion)); //asigne espacio al contexto en el pcb

    nuevo_pcb->contexto_ejecucion->pid = pid_global;

    nuevo_pcb->contexto_ejecucion->program_counter = 0;

    nuevo_pcb->contexto_ejecucion->instrucciones = list_create();

    nuevo_pcb->prioridad = prioridad;

    nuevo_pcb->tabla_archivos_abiertos = list_create();

    nuevo_pcb->recursos_asignados = list_create();

    nuevo_pcb->estado = NEW;

    crear_registros(nuevo_pcb);
    
    agregar_a_new(nuevo_pcb);

    agregar_a_procesos_totales(nuevo_pcb);

    pid_global++;

    // ----------------------------------------------------------
    //manejo de memoria: creacion de procesos (envio paquetes con la data de cada proceso a crear)
    // ----------------------------------------------------------

    t_paquete *paquete = crear_paquete(CREAR_PROCESO);
    
    int tam_char = strlen(path)+1;
    
    agregar_a_paquete(paquete,&size,sizeof(int));//TAMANIO DEL PROCESO
    agregar_a_paquete(paquete,&tam_char, sizeof(int));
    agregar_a_paquete(paquete, path,tam_char);
    serializar_pcb(paquete,nuevo_pcb);
    
    enviar_paquete(paquete, conexion_memoria);


    log_info(logger_kernel, "Se crea el proceso %d en NEW", nuevo_pcb->contexto_ejecucion->pid); 

}

void cambiar_estado_pcb(t_pcb *pcb, t_estado estado_nuevo){


    log_info(logger_kernel, "PID: %d - Estado Anterior: %s - Estado Actual: %s", pcb->contexto_ejecucion->pid, estado_a_string(pcb->estado), estado_a_string(estado_nuevo));

    pcb->estado = estado_nuevo;    

}


t_pcb *buscarPcb(int pid_a_buscar)
{
    t_pcb *pcb;
    
    for (int i = 0; i < (list_size(procesosTotales)); i++) {
        pcb = list_get(procesosTotales,i);
        if (pcb->contexto_ejecucion->pid == pid_a_buscar) {
            return pcb;
        }
    }
    return NULL;
}

void actualizar_contexto_pcb(t_pcb *pcb, t_contexto_ejecucion *contexto_actualizado){

    pcb->contexto_ejecucion->instrucciones = contexto_actualizado->instrucciones;

    if(pcb->contexto_ejecucion->pid != contexto_actualizado->pid){
        log_error(kernel, "Error: se sobrescribio el PID del proceso");
    }

    pcb->contexto_ejecucion->registros = contexto_actualizado->registros;

}

t_archivos_proceso* buscar_archivo_abierto(t_list *tabla_archivos_abiertos, char *nombre_archivo_read){
    
    t_archivos_proceso* archivo;

    int archivos_abiertos = sizeof(tabla_archivos_abiertos);
    for(int i = 0; i < archivos_abiertos; i++){
        archivo = list_get(tabla_archivos_abiertos, i);
        if(strcmp(archivo->fcb->nombre, nombre_archivo_read)==0){
            return archivo;
        }
    }

    return NULL;
}


char* estado_a_string(t_estado estado) {
	switch (estado) {
	case NEW:
		return "NEW";
	case READY:
		return "READY";
	case EXEC:
		return "EXEC";
	case BLOCKED:
		return "BLOCK";
	case EXIT:
		return "EXIT";
	default:
		return "ERROR";
	}
}

void liberar_pcb(t_pcb *pcb){
    
    //libera los recursos
    t_recurso* recurso_aux;

    for(int i = 0 ; i< (list_size(pcb->recursos_asignados)) ; i++){
        recurso_aux  = list_get(pcb->recursos_asignados,i);
        aumentar_cantidad_recursos(pcb, recurso_aux);
        free(recurso_aux);
    }

    list_destroy(pcb->recursos_asignados);

    //libera el contexto
	liberar_contexto(pcb->contexto_ejecucion);

    //libera los archivos
    for(int i = 0; i < (list_size(pcb->tabla_archivos_abiertos)); i++){
        t_archivos_proceso *archivo_aux = list_get(pcb->tabla_archivos_abiertos, i);
        cerrar_archivo(pcb, archivo_aux->fcb->nombre);
        free(archivo_aux); //el resto de la liberacion se supone que esta dentro de cerrar_archivo
    }

    list_destroy(pcb->tabla_archivos_abiertos);

	free(pcb);

}