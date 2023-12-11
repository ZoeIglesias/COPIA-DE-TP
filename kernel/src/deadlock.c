#include "../include/deadlock.h"

void procesos_en_blocked(){

    while(1){

        sem_wait(&hayPCBsEnBlocked);

        log_info(logger_kernel, "ANALISIS DE DETECCION DE DEADLOCKS");

        analizarDeadlock();
    }
}

void analizarDeadlock(){

    if(!detectar_deadlock()){

            log_info(kernel, "No se genera deadlock entre los procesos bloqueados");
            return;

    }else{
            
        log_info(kernel, "Se detecto deadlock");
            
        consola(FINALIZAR_PROCESO);
            
        analizarDeadlock();

    }

    exit(EXIT_FAILURE);
}


bool detectar_deadlock(){

    //lista auxiliar con las colas de procesos bloqueados
    t_list *colas_procesos_bloqueados_aux = list_create();
    list_add_all(colas_procesos_bloqueados_aux, lista_block); 

    //variables de largo para recorrer las colas bloqueadas y sus procesos adjuntos
    int cant_colas_bloqueadas = list_size(colas_procesos_bloqueados_aux);
    int cant_proc_cola_bloqueada;

    t_list *pcbs_con_retencion = list_create();
    t_list *lista_posible_deadlock = list_create();
    int pid;
    t_pcb *pcb;
    int cantidad_de_elementos = 0;
    t_pcb_bloqueado *pcb_bloqueado_auxiliar;
    t_pcb_bloqueado *pcb_bloqueado = malloc(sizeof(t_pcb_bloqueado *));

    //primer paso: ver si hay retencion (espera ya sabemos que hay, porque sino no estaria bloqueado)
    for(int i = 0; i < cant_colas_bloqueadas ; i++){
        
        //la creo para saber de cual estoy analizando
        t_cola_block *cola_a_analizar = list_get(colas_procesos_bloqueados_aux,i);
        cant_proc_cola_bloqueada = sizeof(cola_a_analizar);

        for(int j = 0; j < cant_proc_cola_bloqueada; j++){
        
            pid = pid_bloqueado(cola_a_analizar);

            pcb = buscarPcb(pid);
            
            cantidad_de_elementos = list_size(pcb->recursos_asignados) + list_size(pcb->tabla_archivos_abiertos);

            if(cantidad_de_elementos > 0){
                
                pcb_bloqueado->pid = pid;
                pcb_bloqueado->elemento_bloqueante = cola_a_analizar->identificador;
                pcb_bloqueado->elementos_asignados = list_create();
                agregar_elementos_asignados(pcb_bloqueado, pcb);
                list_add(pcbs_con_retencion, pcb);
            }

            cantidad_de_elementos = 0;
        }

    }

    int cadena;
    //segundo paso: ver si hay espera circular
    for(int i = 0; i < cant_colas_bloqueadas ; i++){
        
        pcb_bloqueado = list_get(pcbs_con_retencion, i);
    
        for(int j = i+1; j < cant_proc_cola_bloqueada-1; j++){
        
            pcb_bloqueado_auxiliar = list_get(pcbs_con_retencion, j);
            
            //primer caso: dos procesos y dos elementos
            if(list_contains(pcb_bloqueado->elementos_asignados, pcb_bloqueado_auxiliar->elemento_bloqueante)){
                if(list_contains(pcb_bloqueado_auxiliar->elementos_asignados, pcb_bloqueado->elemento_bloqueante)){
                    list_add(pcbs_en_deadlock, &(pcb_bloqueado->pid));
                    list_add(pcbs_en_deadlock, &(pcb_bloqueado_auxiliar->pid));
                }
            }
            else{
                if(list_contains(pcb_bloqueado_auxiliar->elementos_asignados, pcb_bloqueado->elemento_bloqueante)){
                    cadena = cadena_a_continuar_cadena(pcb_bloqueado, lista_posible_deadlock);
                    t_list *pcbs_posible_deadlock;
                    if(cadena == 0){
                        pcbs_posible_deadlock = list_create();
                        list_add(pcbs_posible_deadlock, pcb_bloqueado);
                        list_add(pcbs_posible_deadlock, pcb_bloqueado_auxiliar);
                        list_add(lista_posible_deadlock, pcbs_posible_deadlock);
                    }
                    else{
                        pcbs_posible_deadlock = list_get(lista_posible_deadlock, cadena);
                        list_add(pcbs_posible_deadlock, pcb_bloqueado_auxiliar);
                        free(pcbs_posible_deadlock);
                    }
                }
            }

            
        }

    }

    t_list *pcbs_posible_deadlock_aux;

    for(int w=0;w<list_size(lista_posible_deadlock);w++){

        pcbs_posible_deadlock_aux = list_get(lista_posible_deadlock,w);
        pcb_bloqueado = list_get(pcbs_posible_deadlock_aux,0);
        pcb_bloqueado_auxiliar = list_get(pcbs_posible_deadlock_aux,(list_size(pcbs_posible_deadlock_aux)-1));
        if(list_contains(pcb_bloqueado->elementos_asignados, pcb_bloqueado_auxiliar->elemento_bloqueante)){
            for(int k=0;k<list_size(pcbs_posible_deadlock_aux); k++){
                pcb_bloqueado_auxiliar = list_get(pcbs_posible_deadlock_aux,k);
                list_add(pcbs_en_deadlock,&(pcb_bloqueado_auxiliar->pid));
            }
        }
        libera_listas(pcbs_posible_deadlock_aux);
    }

    sin_repetidos(pcbs_en_deadlock);
    

    int tamanio_pcbs_deadlock = list_size(pcbs_en_deadlock);
    t_pcb_bloqueado *pcb_deadlock;
    char *elementos_en_posicion;
    
    for(int i = 0; i < tamanio_pcbs_deadlock; i++){

        pcb_deadlock = list_get(pcbs_en_deadlock, i);

        elementos_en_posicion = string_new();
        
        for(int j = 0; j < list_size(pcb_deadlock->elementos_asignados); j++){
            string_append(&(elementos_en_posicion), list_get(pcb_deadlock->elementos_asignados, j));
            string_append(&(elementos_en_posicion), ", ");
        }
        
        log_info(logger_kernel, "Deadlock detectado: %d - Recursos en posesión: %s - Recurso requerido: %s", pcb_deadlock->pid, elementos_en_posicion, pcb_deadlock->elemento_bloqueante);

    }

    list_destroy(colas_procesos_bloqueados_aux);
    list_destroy(lista_posible_deadlock);
    libera_listas(pcbs_con_retencion);

    return (list_size(pcbs_en_deadlock) > 0);

}

bool list_contains(t_list* lista, char* nombre_elemento){
    int elementos_lista = list_size(lista);
    char* auxiliar;

    for(int i=0; i<elementos_lista;i++){
        auxiliar = list_get(lista, i);
        if(strcmp(auxiliar, nombre_elemento)==0){
            return true;
        }
    }
    return false;
}

int contar_apariciones(t_list* lista, int pid){
    int elementos_lista = list_size(lista);
    int auxiliar;
    int cantidad=0;

    for(int i=0; i<elementos_lista;i++){
        auxiliar = atoi(list_get(lista, i));
        if(auxiliar == pid){
            cantidad++;
        }
    }
    return cantidad;
}

int cadena_a_continuar_cadena(t_pcb_bloqueado* pcb_bloqueado, t_list* lista){
    t_list* auxiliar;
    int tam_lista;
    t_pcb_bloqueado* aux_pcb_bloqueado;
    for(int i=0; i<list_size(lista);i++){
        auxiliar = list_get(lista, i);
        tam_lista = list_size(auxiliar);
        aux_pcb_bloqueado = list_get(auxiliar, tam_lista-1);
        if(aux_pcb_bloqueado->pid == pcb_bloqueado->pid){
            return i;
        }
    }
    
    exit(EXIT_FAILURE);

}

void sin_repetidos(t_list* lista){
    int cantidad;
    int vueltas = list_size(lista);
    int borrados = 0;
    
    for(int i = (vueltas-1);i >= 0; i--){
        cantidad = contar_apariciones(lista, atoi(list_get(lista, i-borrados)));
        if(cantidad > 1){
            list_remove_element(lista, &i);
        }
    }


}

void agregar_elementos_asignados(t_pcb_bloqueado* pcb_bloqueado, t_pcb* pcb){

    int cant_recursos = list_size(pcb->recursos_asignados);
    int cant_archivos = list_size(pcb->tabla_archivos_abiertos);

    t_recurso* recurso_aux;
    char* nombre_recurso;

    t_archivos_proceso* archivo_aux;
    char* nombre_archivo;

    for(int i=0; i<cant_recursos; i++){
        recurso_aux = list_get(pcb->recursos_asignados,i);
        nombre_recurso = recurso_aux->nombre;
        list_add(pcb_bloqueado->elementos_asignados, nombre_recurso);
    }
    for(int j=0; j<cant_archivos; j++){
        archivo_aux = list_get(pcb->tabla_archivos_abiertos,j);
        nombre_archivo = archivo_aux->fcb->nombre;
        list_add(pcb_bloqueado->elementos_asignados, nombre_archivo);
    }
}

void *liberar_pcb_block(t_pcb_bloqueado* pcb_bloqueado){
    
    free(pcb_bloqueado->elemento_bloqueante);

    list_destroy(pcb_bloqueado->elementos_asignados);

    free(pcb_bloqueado);
    
    return NULL;
}

void libera_listas(t_list *lista_a_liberar){

    for(int i = 0; i < (list_size(lista_a_liberar)); i++){
        
        t_pcb_bloqueado *pcb_bloqueado_aux = list_get(lista_a_liberar, i);
        liberar_pcb_block(pcb_bloqueado_aux);
    }

    list_destroy(lista_a_liberar);

}