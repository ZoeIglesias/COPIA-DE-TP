#include "../include/archivos_fs.h"


//-------------------------------
//Funciones de Manejo Filesystem
//-------------------------------

//F_OPEN

bool existe_lock(char *nombre_archivo_open){

    t_archivos_proceso *archivo = buscar_archivo_en_tabla_archivos(nombre_archivo_open);

    return archivo != NULL;
}

locks lock(char *nombre_archivo_open){
    
    t_archivos_proceso *archivo = buscar_archivo_en_tabla_archivos(nombre_archivo_open);

    return archivo->tipo_lock;
}

//F_SEEK
int actualizar_puntero_archivo(char *nombre_archivo_fseek, int posicion){
    
    t_archivos_proceso *archivo = buscar_archivo_en_tabla_archivos(nombre_archivo_fseek);

    if(archivo == NULL){
        return -1;
    }else{
        archivo->puntero_bloque = posicion;
        return 1;
    }
}

t_archivos_proceso *buscar_archivo_en_tabla_archivos(char *nombre_archivo_fseek){
    
    for(int i = 0; i < list_size(tabla_global_archivos_abiertos); i++){

        t_archivos_proceso *archivo = list_get(tabla_global_archivos_abiertos, i);

        if(strcmp(nombre_archivo_fseek, archivo->fcb->nombre) == 0){
            return archivo;
        }
        
    }
    return NULL;
}

//F_TRUNCATE
int truncar_archivo(char *nombre_archivo_read, int tamanio){

    t_paquete *paquete = crear_paquete(TRUNCATE_FS);
    //enviar paquete a fs con estos datos para que trunque el archivo
    int tam_char = sizeof(nombre_archivo_read)+1;
    agregar_a_paquete(paquete, &tam_char, sizeof(int));
    agregar_a_paquete(paquete, &nombre_archivo_read, tam_char);
    agregar_a_paquete(paquete,&tamanio, sizeof(int));
    enviar_paquete(paquete,conexion_a_filesystem);

    //IMPLEMENTAR EL BLOQUEO PROCESO

    if(recibir_operacion(conexion_a_filesystem) != OK){
        return -1;
    }else{
        return 1;
    }
}

void cerrar_archivo(t_pcb* pcb, char* nombre_archivo_close){
    t_archivos_proceso* archivo = buscar_archivo_abierto(pcb->tabla_archivos_abiertos, nombre_archivo_close);
    list_remove_element(pcb->tabla_archivos_abiertos, archivo);
    t_archivos_proceso* archivo_global = buscar_archivo_abierto(tabla_global_archivos_abiertos, nombre_archivo_close);
    if(archivo->tipo_lock == R){
        list_remove_element(archivo_global->participantes_lock, &(pcb->contexto_ejecucion->pid));
    }
    if(list_size(archivo_global->participantes_lock) == 0){
        list_remove_element(tabla_global_archivos_abiertos, archivo_global);
        free(archivo_global->fcb);
        list_destroy(archivo_global->participantes_lock);
        free(archivo_global);

        t_cola_block* archivo_abierto = buscar_cola_del_archivo_abierto(nombre_archivo_close);
        while(!queue_is_empty(archivo_abierto->cola_bloqueados)){
            int pid = pid_bloqueado(archivo_abierto);
                agregar_a_ready(buscarPcb(pid));
        }

        queue_destroy(archivo_abierto->cola_bloqueados);
        free(archivo_abierto);
    }
    free(archivo->fcb->nombre);
    free(archivo->fcb);
    free(archivo);
}