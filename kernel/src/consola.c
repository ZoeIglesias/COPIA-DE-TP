#include "../include/consola.h"


//mensaje : INICIAR_PROCESO sdadasd 23 1
void iniciarProceso(char *mensaje){

    char* path = strtok(mensaje," ");
    //log_info(kernel, "%s", path);

    mensaje+= strlen(path)+1;

    char* size = strtok(mensaje," ");
    //log_info(kernel, "%s", size);

    mensaje+= strlen(size)+1;

    char *prioridad = strtok(mensaje," ");
    //log_info(kernel, "%s", prioridad);

    mensaje+= strlen(prioridad)+1;

    int size_path = atoi(size);

    int prioridade = atoi(prioridad);

    crear_pcb(path, size_path, prioridade);
    

}


void finalizarProcesoPID(char *mensaje){

    char* pid_char = strtok(mensaje, " ");

    int pid = atoi(pid_char);

    t_pcb *pcb_a_finalizar;

    pcb_a_finalizar = buscarPcb(pid);

    //manejo de memoria: eliminacion de procesos (condicion por si estaba en exec)
    t_pcb *pcb_aux = list_get(lista_exec,0);

    if(pcb_a_finalizar->contexto_ejecucion->pid == pcb_aux->contexto_ejecucion->pid)
    {
        enviar_interrupcion(CONSOLA);

        t_contexto_ejecucion *contexto_actualizado = recibir_contexto(conexion_cpu_dispatch);
        
        actualizar_contexto_pcb(pcb_a_finalizar, contexto_actualizado);

    }

    agregar_a_exit(pcb_a_finalizar, CONSOLA);

    sem_post(&finalizarProceso);
    
}

void detenerPlanificacion(){

    sem_wait(&cortoPlazoHablilitado);
    sem_wait(&largoPlazoHabilitado);

    log_info(logger_kernel,"PAUSA DE PLANIFICACION");

}

void iniciarPlanificacion(){
    

    sem_post(&cortoPlazoHablilitado);

    sem_post(&largoPlazoHabilitado);

    log_info(logger_kernel,"INICIO DE PLANIFICACION");
    
}

void modificarGradoMultiprogramacion(char *mensaje) {

    char* gradoMulti = strtok(mensaje, " ");

    int nuevo_grado = atoi(gradoMulti);

    log_info(logger_kernel,"Grado Anterior: %d - Grado Actual: %d ", grado_multiprogramacion(), nuevo_grado); 

    config_set_value(config_kernel,"GRADO_MULTIPROGRAMACION_INI" , gradoMulti); //agregue un &

}


void listarProcesosPorEstado() {
    
    char *lista_pids_new = listar_pids(lista_new);
    log_info(logger_kernel,"Estado: NEW - Procesos:  [%s]", lista_pids_new);

    char *lista_pids_ready = listar_pids(lista_ready);
    log_info(logger_kernel,"Estado: READY - Procesos:  [%s]", lista_pids_ready);

    char *lista_pids_exit = listar_pids(lista_exit);
    log_info(logger_kernel,"Estado: EXIT - Procesos:  [%s]", lista_pids_exit);

    char *lista_pids_exec = listar_pids(lista_exec);
    log_info(logger_kernel,"Estado: EXEC - Procesos:  [%s]", lista_pids_exec);

    char *lista_pids_blocked = listar_pids(lista_block);
    log_info(logger_kernel,"Estado: BLOCKED - Procesos:  [%s]", lista_pids_blocked);
}


void consola(){

    while (1) {

        t_mensajes_consola mensaje_consola;
        char input[50];
        char *mensaje;
        char *puntero = input;
        printf("Menu consola: INICIAR_PROCESO - FINALIZAR_PROCESO - DETENER_PLANIFICACION - INICIAR_PLANIFICACION - MULTIPROGRAMACION - PROCESO_ESTADO \n");
        printf("Ingresar comando: ");
        fgets(input, sizeof(input), stdin);
        log_info(kernel, "%s", input);
        input[strlen(input) - 1] = '\0'; //no estaba entrando en ninguno de los casos por el enter que hacer un barra n
        mensaje = strtok(puntero, " ");

        mensaje_consola = mensaje_a_consola(mensaje);

        switch(mensaje_consola){

            case INICIAR_PROCESO:
                puntero+= strlen(mensaje)+1;
                iniciarProceso(puntero);

                break;


            case FINALIZAR_PROCESO:
                puntero+= strlen(mensaje)+1;
                finalizarProcesoPID(puntero);

                break;

            case INICIAR_PLANIFICACION:

                iniciarPlanificacion();
                return;

                break;

            case DETENER_PLANIFICACION:

                detenerPlanificacion();


                break;

            case MULTIPROGRAMACION:

                puntero+= strlen(mensaje)+1;
                modificarGradoMultiprogramacion(puntero);

                break;

            case PROCESO_ESTADO:

                listarProcesosPorEstado();

                break;

            default:
            
                break;

        }
        
    }
}

t_mensajes_consola mensaje_a_consola(char *mensaje_consola){
    if(strcmp(mensaje_consola,"INICIAR_PROCESO") == 0){
        return INICIAR_PROCESO;
    }
    if(strcmp(mensaje_consola,"FINALIZAR_PROCESO") == 0){
        return FINALIZAR_PROCESO;
    }
    if(strcmp(mensaje_consola,"DETENER_PLANIFICACION") == 0){
        return DETENER_PLANIFICACION;
    }
    if(strcmp(mensaje_consola,"INICIAR_PLANIFICACION") == 0){
        return INICIAR_PLANIFICACION;
    }
    if(strcmp(mensaje_consola,"MULTIPROGRAMACION") == 0){
        return MULTIPROGRAMACION;
    }
    if(strcmp(mensaje_consola,"PROCESO_ESTADO") == 0){
        return PROCESO_ESTADO;
    }else
        return ERROR;
}