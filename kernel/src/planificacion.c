#include "../include/planificacion.h"
// ----------
// SEMAFOROS
// ----------
sem_t new_sem;
sem_t ready_sem;
sem_t exec_sem;
sem_t block_sem;
sem_t exit_sem;
sem_t grado_multiprogramacion_sem;
sem_t elemento_exit;
sem_t hayPCBsEnNew;
sem_t hayPCBsEnReady;
sem_t cortoPlazoHablilitado;
sem_t largoPlazoHabilitado;
sem_t hayPCBsEnBlocked;
sem_t solicitud_enviada;
sem_t SeEjecutaProceso;
sem_t nuevoPcbEnReady;
sem_t finalizarProceso;
sem_t contextoActualizado;
sem_t finDeCiclo;
sem_t libera_pcb;

// -----------------------------------
//Funciones generales
// -----------------------------------

int conexion_cpu_dispatch;
int conexion_cpu_interrupt;
int conexion_a_filesystem;

t_list *lista_new;
t_list *lista_ready; 
t_list *lista_exit;
t_list *lista_block;
t_list *procesosTotales;
t_list *lista_exec;
t_pcb *pcb_en_execute;
t_cola_block *archivo_abierto;

bool hayPcbEnExec;
bool hayMayorPrioridad;

t_list *pcbs_en_deadlock;

void inicializar_estructuras(){

    lista_new = list_create();
    lista_ready = list_create();
    lista_exit = list_create();
    lista_block = list_create();
    lista_exec = list_create(); //maximo un pid que puede tener
    //inicializar_colas_bloqueo();
    procesosTotales = list_create();
    tabla_global_archivos_abiertos = list_create();

}

void inicializar_hilos(){

    pthread_t hilo_planificador_largo_plazo, hilo_planificador_corto_plazo, hilo_exit;

    

    pthread_create(&hilo_planificador_largo_plazo, NULL, (void*)planificador_largo_plazo, NULL);
    pthread_create(&hilo_planificador_corto_plazo, NULL, (void*)planificador_corto_plazo, NULL);
    pthread_create(&hilo_exit, NULL, (void*)sacar_de_exit, NULL);
    log_info(kernel, "levanto los hilos");
    
}

void inicializar_colas_bloqueo_de_recusos(){

    lista_block = (t_list *)malloc(sizeof(t_cola_block *));

    //creamos inicialmente una cola por cada recurso que hay en el archivo de config

    int cant_recursos = sizeof(recursos_config()); //a cambiar cuando abramos con archivos con FS

    t_cola_block *recurso_existente = malloc(sizeof(t_cola_block*));

    for(int i = 0; i < cant_recursos; i++){
        recurso_existente->identificador = recursos_config()[i];
        recurso_existente->cola_bloqueados = queue_create();
        recurso_existente->tipo = "recurso";
        list_add(lista_block, recurso_existente);
    }    
    
}

void crear_cola_del_archivo_abierto(t_pcb *pcb, char *nombre_archivo){

    //consultamos si ya existe la cola bloqueada del archivo, si no es asi lo crea y la agrega con las demas de los recursos
    t_cola_block* archivo_abierto = buscar_cola_del_archivo_abierto(nombre_archivo);
    if(archivo_abierto != NULL){

        queue_push(archivo_abierto->cola_bloqueados, &(pcb->contexto_ejecucion->pid));
        return;
    }
    
    archivo_abierto->identificador = nombre_archivo;
    archivo_abierto->cola_bloqueados = queue_create();
    archivo_abierto->tipo = "archivo";
    queue_push(archivo_abierto->cola_bloqueados, &(pcb->contexto_ejecucion->pid));
    list_add(lista_block, archivo_abierto);
}

t_cola_block* buscar_cola_del_archivo_abierto(char *nombre_archivo){

    t_list_iterator* iterador_block = list_iterator_create(lista_block);
    t_cola_block* archivo_abierto;

    //consultamos si ya existe la cola bloqueada del archivo, si no es asi lo crea y la agrega con las demas de los recursos
    while(list_iterator_has_next(iterador_block)){
        archivo_abierto = list_iterator_next(iterador_block);
        if(strcmp(archivo_abierto->tipo, "archivo") == 0){
            if(strcmp(archivo_abierto->identificador, nombre_archivo) == 0){
                list_iterator_destroy(iterador_block);
                return archivo_abierto;
            }
        }
    }
    return NULL;
}

t_cola_block* buscar_cola_del_recurso(char *nombre_recurso){

    t_list_iterator* iterador_block = list_iterator_create(lista_block);
    t_cola_block* cola_recurso;

    //consultamos si ya existe la cola bloqueada del archivo, si no es asi lo crea y la agrega con las demas de los recursos
    while(list_iterator_has_next(iterador_block)){
        cola_recurso = list_iterator_next(iterador_block);
        if(strcmp(cola_recurso->tipo, "recurso") == 0){
            if(strcmp(cola_recurso->identificador, nombre_recurso) == 0){
                list_iterator_destroy(iterador_block);
                return cola_recurso;
            }
        }
    }
    return NULL;
}


t_cola_block* buscar_cola_block(char *tipo_cola, char *nombre){
    if(strcmp(tipo_cola, "recurso") == 0){
        return buscar_cola_del_recurso(nombre);
    }
    if(strcmp(tipo_cola, "archivo") == 0){
        return buscar_cola_del_archivo_abierto(nombre);
    }
    return NULL;
}

void inicializar_semaforos(){
    sem_init(&new_sem, 0, 1);
    sem_init(&ready_sem, 0, 1);
    sem_init(&exec_sem, 0, 1);
    sem_init(&block_sem, 0, 1);
    sem_init(&exit_sem, 0, 1);
    sem_init(&elemento_exit,0,0);
    sem_init(&grado_multiprogramacion_sem, 0, grado_multiprogramacion());
    sem_init(&hayPCBsEnNew, 0, 0);
    sem_init(&hayPCBsEnReady, 0, 0);
    sem_init(&contextoActualizado, 0, 0);
    sem_init(&finalizarProceso,0,0);
    sem_init(&cortoPlazoHablilitado,0,1);
    sem_init(&largoPlazoHabilitado,0,1);
    sem_init(&solicitud_enviada,0,0);
    sem_init(&hayPCBsEnBlocked,0,0);
    sem_init(&SeEjecutaProceso,0,0);
    sem_init(&hayInterrupcion,0,0);
    sem_init(&nuevoPcbEnReady,0,0);
    sem_init(&libera_pcb,0,0);
}

void agregar_a_procesos_totales(t_pcb *nuevo_pcb){
    list_add(procesosTotales, nuevo_pcb);
}

void sacar_de_procesos_totales(t_pcb *pcb_a_sacar){
    list_remove_element(procesosTotales, pcb_a_sacar);
}

// -----------------------------------
//  Funciones generales para los estados
// -----------------------------------

//Estado New
void agregar_a_new(t_pcb* pcb){
    sem_wait(&new_sem);
    list_add(lista_new,pcb);
    sem_post(&new_sem);
    sem_post(&hayPCBsEnNew);
}

t_pcb *sacar_de_new(){
    t_pcb *pcb_a_sacar;
    sem_wait(&new_sem);
    pcb_a_sacar = list_remove(lista_new, 0);
    sem_post(&new_sem);
    return pcb_a_sacar;
}

//Estado Ready
void agregar_a_ready(t_pcb *pcb){
    sem_wait(&ready_sem);
    list_add(lista_ready,pcb);
    char *lista_pids = listar_pids(lista_ready);
    log_info(logger_kernel, "Cola Ready %s : [%s]",algoritmo_planificacion(),lista_pids);
    sem_post(&ready_sem);
    sem_post(&hayPCBsEnReady);
    sem_post(&nuevoPcbEnReady); //este semaforo es para avisarle al algoritmo de prioridades en el hilo de corto plazo
}

t_pcb *sacar_de_ready(){
    t_pcb *pcb_a_sacar = NULL;
    if(!strcmp("FIFO", algoritmo_planificacion())){
        sem_wait(&ready_sem);
        pcb_a_sacar = list_remove(lista_ready, 0);
        sem_post(&ready_sem);
    }

    if(!strcmp("ROUND ROBIN", algoritmo_planificacion())){
        algoritmo_RR(pcb_a_sacar);
    }

    if(!strcmp("PRIORIDADES", algoritmo_planificacion())){
        algoritmo_prioridades(pcb_a_sacar);
    }
    return pcb_a_sacar;
}

void algoritmo_RR(t_pcb *pcb_a_sacar){
    sem_wait(&ready_sem);
    pcb_a_sacar = list_remove(lista_ready, 0);
    sem_post(&ready_sem);
} 

void algoritmo_prioridades(t_pcb *pcb_a_sacar){
    
    int cant_procesos = list_size(lista_ready);
    int mayorPrioridad = 0;
    
    for(int i = 1; i < cant_procesos; i++){
        if(mayor_prioridad_nuevo(list_get(lista_ready, mayorPrioridad),list_get(lista_ready, mayorPrioridad))){
            mayorPrioridad = i;
        }
    }
    sem_wait(&ready_sem);
    pcb_a_sacar = list_remove(lista_ready, mayorPrioridad);
    sem_post(&ready_sem);
}

//Estado Execute 
void agregar_a_execute(t_pcb *pcb){
    sem_wait(&exec_sem);
    pcb_en_execute = pcb;
    hayPcbEnExec = true;
    sem_post(&exec_sem);
}

void sacar_de_execute(){
    sem_wait(&exec_sem);
    pcb_en_execute = NULL;
    hayPcbEnExec = false;
    sem_post(&exec_sem);
}

//Estado Exit
void agregar_a_exit(t_pcb *pcb, t_msj_error motivo){
    
    sem_wait(&exit_sem);

    list_add(lista_exit, pcb);
    
    char *motivo_a_mostrar = mensaje_a_string(motivo);

    pcb->estado = EXIT;

    log_info(logger_kernel, "Finaliza el proceso %d - Motivo: %s", pcb->contexto_ejecucion->pid, motivo_a_mostrar);

    sem_post(&elemento_exit);
    sem_post(&exit_sem); 
}

void *sacar_de_exit(){ //saca de exit los procesos y libera los recursos consumidos
    
    t_pcb *pcb_a_sacar;
    
    while (1)
    {
        sem_wait(&elemento_exit);
        sem_wait(&exit_sem);

        pcb_a_sacar = list_remove(lista_exit, 0);

        t_paquete *paquete = crear_paquete(LIBERAR_ESTRUCTURAS);
        agregar_a_paquete(paquete, &(pcb_a_sacar -> contexto_ejecucion -> pid), sizeof(int));
        enviar_paquete(paquete, conexion_memoria);

        sem_wait(&libera_pcb);

        sacar_de_procesos_totales(pcb_a_sacar);

        liberar_pcb(pcb_a_sacar);

        sem_post(&exit_sem);

        sem_post(&grado_multiprogramacion_sem);
    }
}


//Estado Blocked
void agregar_a_blocked(t_pcb *pcb, char *tipo_cola, char *nombre){
    
    sem_wait(&block_sem);

    t_cola_block *cola_a_agregar = buscar_cola_block(tipo_cola, nombre);

    queue_push(cola_a_agregar->cola_bloqueados, &(pcb->contexto_ejecucion->pid));

    sem_post(&hayPCBsEnBlocked);

    sem_post(&block_sem);
}

t_pcb *sacar_de_blocked(char *tipo_cola, char *nombre){

    sem_wait(&block_sem);
    
    t_cola_block *cola_a_sacar = buscar_cola_block(tipo_cola, nombre);

    int pid = pid_bloqueado(cola_a_sacar);

    queue_pop(cola_a_sacar->cola_bloqueados);

    t_pcb* pcb_a_sacar = buscarPcb(pid);

    sem_post(&block_sem);

    return pcb_a_sacar;
}



// ------------------------------------
//  Implementaciones para Planificacion
// ------------------------------------

void *planificador_largo_plazo(){
    
    while(1){
        
        
        sem_wait(&hayPCBsEnNew);
        sem_wait(&largoPlazoHabilitado);        
        sem_wait(&grado_multiprogramacion_sem);

        t_pcb* pcb = sacar_de_new();

        cambiar_estado_pcb(pcb, READY);

        agregar_a_ready(pcb);

        sem_post(&largoPlazoHabilitado);
    }
    return NULL;
}

void *planificador_corto_plazo(){
   
    hayPcbEnExec = false;

    t_planificador alg_planificacion = planificador();
    
    switch (alg_planificacion)
    {
    case PRIORIDADES:
        pthread_t hilo_prioridades;
        pthread_create(&hilo_prioridades, NULL, (void*)chequear_prioridades, NULL);
        break;
    case ROUND_ROBIN:
        pthread_t hilo_round_robin;
        pthread_create(&hilo_round_robin, NULL, (void *)round_robin, NULL);
        break;
    default:
        break;
    }
    
    while(1){
        if(!hayPcbEnExec){
            log_info(kernel, "entro al while");
            sem_wait(&hayPCBsEnReady);
            log_info(kernel, "hay pcbs en ready");
        }
        sem_wait(&cortoPlazoHablilitado);
        log_info(kernel, "entro al planificador");
        t_pcb* pcb;
        
        t_contexto_ejecucion *contexto_actualizado;

        if(!hayPcbEnExec){
            log_info(kernel, "entro a agregar a execute");
            pcb = sacar_de_ready();
		    cambiar_estado_pcb(pcb, EXEC);
		    agregar_a_execute(pcb);
        }else{
            log_info(kernel, "entro a seguir ejecutando");
            pcb = pcb_en_execute;
        }


        //codigo_operacion operacion_recibida = recibir_operacion(socket_cpu_dispatch);

        //envio el contexto a cpu
        enviar_contexto(pcb->contexto_ejecucion, conexion_cpu_dispatch, CONTEXTO_EJECUCION);

        //iniciar semaforo quantum
        algoritmo_planificador(alg_planificacion);
        
        //crea el buffer
        t_buffer *buffer_cpu = crear_buffer();

        //recibo la operacion de cpu
        codigo_operacion operacion_recibida = recibir_operacion(conexion_cpu_dispatch);

        //recibo el contexto actualizo de cpu y otros parametros
        buffer_cpu->stream = recibir_buffer(&(buffer_cpu->size),conexion_cpu_dispatch);
        int desplazamiento_cpu = 0;

        log_info(kernel,"Recibio bien al contexto de cpu y el temita buffer");

        //deserializo el contexto actualizado
        contexto_actualizado = deserializar_contexto_con_desplazamiento(buffer_cpu, &desplazamiento_cpu);
        
        //En el caso de Rond Robin ddonde se termine el quantum desaloja al proceso y lo vuelve a guardar en ready
        if(alg_planificacion == ROUND_ROBIN){
            sem_post(&contextoActualizado);
        }
        
        //actualizo el contexto de ejecucion del pcb
        actualizar_contexto_pcb(pcb, contexto_actualizado); 

        switch (operacion_recibida){ 
                int tam_char;
                int tam_par1;
                int tam_par2;

            case WAIT: 
            
                memcpy(&tam_char, buffer_cpu->stream + desplazamiento_cpu, sizeof(int));
                desplazamiento_cpu += sizeof(int);
                
                char *recurso_wait = malloc(tam_char + 1);
                memcpy(recurso_wait, buffer_cpu->stream + desplazamiento_cpu, tam_char);
                recurso_wait[tam_char] = '\0';
                desplazamiento_cpu += tam_char;

                wait_recurso(pcb,recurso_wait);
                free(recurso_wait);

                break;

            case SIGNAL:

                memcpy(&tam_char, buffer_cpu->stream + desplazamiento_cpu, sizeof(int));
                desplazamiento_cpu += sizeof(int);

                char *recurso_signal = malloc (tam_char + 1);    
                memcpy(recurso_signal, buffer_cpu->stream + desplazamiento_cpu, tam_char);
                recurso_signal[tam_char] = '\0';
                desplazamiento_cpu += tam_char;

                signal_recurso(pcb, recurso_signal);
                free(recurso_signal);

                break;

            case SLEEP:

                memcpy(&tam_char, buffer_cpu->stream + desplazamiento_cpu, sizeof(int));
                desplazamiento_cpu += sizeof(int);
                
                char *tiempo_bloqueo = malloc(tam_char + 1);
                memcpy(tiempo_bloqueo, buffer_cpu->stream + desplazamiento_cpu, tam_char);
                tiempo_bloqueo[tam_char] = '\0';
                desplazamiento_cpu += tam_char;

                t_sleep *pcb_a_dormir = (t_sleep *)malloc(sizeof(t_sleep));
                pcb_a_dormir->pcb = pcb;
                pcb_a_dormir->tiempo_dormido = atoi(tiempo_bloqueo);

                pthread_t hilo_sleep;
                pthread_create(&hilo_sleep, NULL, (void*)dormir_pcb, pcb_a_dormir);

                log_info(logger_kernel, "PID: %d - Bloqueado por: SLEEP", pcb->contexto_ejecucion->pid);

                //pthread_detach(hilo_sleep);

                free(pcb_a_dormir);
                free(tiempo_bloqueo);
                break;

            case F_OPEN:

                int tamanio_archivo;

                memcpy(&tam_par1, buffer_cpu->stream + desplazamiento_cpu, sizeof(int));
                desplazamiento_cpu += sizeof(int);

                char *nombre_archivo_open = malloc(tam_par1 + 1);
                memcpy(nombre_archivo_open, buffer_cpu->stream + desplazamiento_cpu, tam_par1);
                nombre_archivo_open[tam_par1] = '\0';
                desplazamiento_cpu += tam_par1;

                memcpy(&tam_par2, buffer_cpu->stream + desplazamiento_cpu, sizeof(int));
                desplazamiento_cpu += sizeof(int);

                char *modo_de_apertura = malloc(tam_par2 + 1);
                memcpy(modo_de_apertura, buffer_cpu->stream + desplazamiento_cpu, tam_par2);
                modo_de_apertura[tam_par2] = '\0';
                desplazamiento_cpu += tam_par2;

                locks modo_apertura;
                if(strcmp(modo_de_apertura, "r") == 0){
                    modo_apertura = R;
                }else modo_apertura = W;
                
                t_paquete *paquete = crear_paquete(APERTURA_FS);
                tam_char = sizeof(nombre_archivo_open)+1;
                agregar_a_paquete(paquete, &tam_char, sizeof(int));
                agregar_a_paquete(paquete, &nombre_archivo_open, tam_char);
                enviar_paquete(paquete, conexion_a_filesystem);

                //creo un buffer para fs
                t_buffer *buffer_fs = crear_buffer();

                //recibo la operacion de fs
                if(recibir_operacion(conexion_a_filesystem) == OK){
                    log_info(kernel, "Filesystem envio OK");
                }else{
                    log_error(kernel, "Filesystem no recibio correctamente el archivo a crear/abrir");
                }

                //recibo el buffer de fs
                buffer_fs->stream = recibir_buffer(&(buffer_fs->size),conexion_a_filesystem);
                int desplazamiento_fs = 0;

                //usamos el buffer que me mando fs
                memcpy(&tamanio_archivo, buffer_fs->stream + desplazamiento_fs, sizeof(int));
                desplazamiento_fs += sizeof(int);

                t_archivos_proceso *archivo = malloc(sizeof(t_archivos_proceso));

                archivo->fcb = malloc(sizeof(t_fcb_kernel));

                if(existe_lock(nombre_archivo_open)){//si existe el archivo en tabla de archivos abiertos global
                    if(modo_apertura == R && lock(nombre_archivo_open) == R){//funcion lock devuelve el tipo de lock (R o W) 
                        
                        t_archivos_proceso *archivo_global;
                        archivo->fcb->nombre = nombre_archivo_open;
                        archivo->fcb->tamanio = tamanio_archivo;
                        archivo->puntero_bloque = 0;
                        archivo->tipo_lock = modo_apertura;

                        list_add(pcb->tabla_archivos_abiertos, archivo);
                        archivo_global = buscar_archivo_abierto(tabla_global_archivos_abiertos, nombre_archivo_open);
                        list_add(archivo_global->participantes_lock, &(pcb->contexto_ejecucion->pid));

                    }else{
                        crear_cola_del_archivo_abierto(pcb, nombre_archivo_open); //crea(si no existe) y agrega - en sintesis    
                    }
                    
                }
                else{//si no existe lock

                    //agrego el archivo abierto a la lista del PCB y a la lista GLOBAL ABIERTOS
                    archivo->fcb->nombre = nombre_archivo_open;
                    archivo->fcb->tamanio = tamanio_archivo;
                    archivo->puntero_bloque = 0; //puntero solo para tabla de archivos por proceso

                    //crear nuevo struct para tabla_global_archivos_abiertos
                    archivo->tipo_lock = modo_apertura;
                            
                    list_add(pcb->tabla_archivos_abiertos, archivo);
                    archivo->participantes_lock = list_create();
                    if(modo_apertura == R){
                        list_add(archivo->participantes_lock, &(pcb->contexto_ejecucion->pid));
                    }
                    list_add(tabla_global_archivos_abiertos, archivo);
                }

                log_info(logger_kernel, "PID: %d - Abrir Archivo: %s", pcb->contexto_ejecucion->pid, nombre_archivo_open);
                
                free(nombre_archivo_open);
                free(modo_de_apertura);
                free(archivo->fcb->nombre);
                free(archivo->fcb);
                free(archivo);
                liberar_buffer(buffer_fs);
                break;
    
            case F_CLOSE:

                memcpy(&tam_char, buffer_cpu->stream + desplazamiento_cpu, sizeof(int));
                desplazamiento_cpu += sizeof(int);
                
                char *nombre_archivo_close = malloc(tam_char + 1);
                memcpy(nombre_archivo_close, buffer_cpu->stream + desplazamiento_cpu, tam_char);
                nombre_archivo_close[tam_char] = '\0';
                desplazamiento_cpu += tam_char;

                log_info(logger_kernel, "PID: %d - Cerrar Archivo: %s", pcb->contexto_ejecucion->pid, nombre_archivo_close);

                cerrar_archivo(pcb, nombre_archivo_close);

                //sacar de la tabla de archivos abiertos del pcb
                //LIBERAR FCB KERNEL
                //SACAR DE COLA BLOCK
                //LIBERAR ESPACIO MEMORIA

                break;

            case F_SEEK:

                memcpy(&tam_par1, buffer_cpu->stream + desplazamiento_cpu, sizeof(int));
                desplazamiento_cpu += sizeof(int);

                char *nombre_archivo_fseek = malloc(tam_par1 + 1);
                memcpy(nombre_archivo_fseek, buffer_cpu->stream + desplazamiento_cpu, tam_par1);
                nombre_archivo_fseek[tam_par1] = '\0';
                desplazamiento_cpu += tam_par1;

                memcpy(&tam_par2, buffer_cpu->stream + desplazamiento_cpu, sizeof(int));
                desplazamiento_cpu += sizeof(int);

                char *posicion = malloc(tam_par2 + 1);
                memcpy(posicion, buffer_cpu->stream + desplazamiento_cpu, tam_par2);
                posicion[tam_par2] = '\0';
                desplazamiento_cpu += tam_par2;

                if(actualizar_puntero_archivo(nombre_archivo_fseek, atoi(posicion)) > 0){
                    log_info(logger_kernel, "PID: %d - Actualizar puntero Archivo : %s - Puntero %s",pcb->contexto_ejecucion->pid, nombre_archivo_fseek, posicion); 
                }else{
                    log_error(kernel,"No esta el archivo en la tabla global de archivos abiertos");
                }

                free(nombre_archivo_fseek);
                free(posicion);
                break;

            case F_READ:

                memcpy(&tam_par1, buffer_cpu->stream + desplazamiento_cpu, sizeof(int));
                desplazamiento_cpu += sizeof(int);

                char *nombre_archivo_read = malloc(tam_par1 + 1);
                memcpy(nombre_archivo_read, buffer_cpu->stream + desplazamiento_cpu, tam_par1);//CREO QUE VCA SIN EL & EL PRIMER PARAMETRO VER!!
                nombre_archivo_read[tam_par1] = '\0';
                desplazamiento_cpu += tam_par1;

                int dir_fisica_read;
                memcpy(&dir_fisica_read, buffer_cpu->stream + desplazamiento_cpu, sizeof(int));
                desplazamiento_cpu += sizeof(int);


                t_paquete *paquete_read = crear_paquete(LEER_FS);
                
                t_archivos_proceso* archivo_read = buscar_archivo_abierto(pcb->tabla_archivos_abiertos, nombre_archivo_read);
                int bloque_puntero_rs = archivo_read->puntero_bloque;

                tam_char = sizeof(nombre_archivo_open)+1;
                agregar_a_paquete(paquete, &tam_char, sizeof(int));
                agregar_a_paquete(paquete, &nombre_archivo_read, tam_char);
                agregar_a_paquete(paquete_read, &dir_fisica_read, sizeof(int));
                agregar_a_paquete(paquete_read, &bloque_puntero_rs, sizeof(int));

                enviar_paquete(paquete_read,conexion_a_filesystem);

                log_info(logger_kernel, "PID : %d - Leer Archivo: %s - Puntero %d - Direccion Memoria: %d - Tamaño: %d", pcb->contexto_ejecucion->pid, nombre_archivo_open, bloque_puntero_rs, dir_fisica_read, archivo_read->fcb->tamanio);

                //Permanece en estado bloqueado hasta que el módulo File System informe de la finalización de la operación.
                //codigo_operacion operacion_recibida_read = recibir_operacion(conexion_a_filesystem); 

                if(recibir_operacion(conexion_a_filesystem) == OK){
                    log_info(kernel, "Filesystem envio OK");
                }else{
                    log_error(kernel, "Filesystem no recibio correctamente el paquete read");
                }

                //free(operacion_recibida_read); //A EVALUAR
                //free(&nombre_archivo_read);
                //free(dir_fisica_read);
                break;

            case F_WRITE:

                memcpy(&tam_par1, buffer_cpu->stream + desplazamiento_cpu, sizeof(int));
                desplazamiento_cpu += sizeof(int);

                char *nombre_archivo_write = malloc(tam_par1 + 1);
                memcpy(nombre_archivo_write, buffer_cpu->stream + desplazamiento_cpu, tam_par1);
                nombre_archivo_write[tam_par1] = '\0';
                desplazamiento_cpu += tam_par1;

                memcpy(&tam_par2, buffer_cpu->stream + desplazamiento_cpu, sizeof(int));
                desplazamiento_cpu += sizeof(int);

                int dir_fisica_write;
                memcpy(&dir_fisica_write, buffer_cpu->stream + desplazamiento_cpu, sizeof(int));
                desplazamiento_cpu += sizeof(int);

                if(lock(nombre_archivo_write) == R){
                    
                    cambiar_estado_pcb(pcb, EXIT);
                    agregar_a_exit(pcb,INVALID_WRITE);
                
                    enviar_interrupcion(INVALID_WRITE);//revisar si otro proceso que se va a exit con interrupcion

                    break;
                }

                t_paquete *paquete_write = crear_paquete(ESCRIBIR_FS);
                
                t_archivos_proceso* archivo_wr = buscar_archivo_abierto(pcb->tabla_archivos_abiertos, nombre_archivo_read);
                int bloque_puntero_wr = archivo_wr->puntero_bloque;

                tam_char = sizeof(nombre_archivo_open)+1;
                agregar_a_paquete(paquete, &tam_char, sizeof(int));
                agregar_a_paquete(paquete, &nombre_archivo_write, tam_char);
                agregar_a_paquete(paquete_write, &dir_fisica_write, sizeof(int));
                agregar_a_paquete(paquete_write, &bloque_puntero_wr, sizeof(int));

                enviar_paquete(paquete_write,conexion_a_filesystem);
               
                log_info(logger_kernel, "PID : %d - Escribir Archivo: %s - Puntero %d - Direccion Memoria: %d - Tamaño: %d", pcb->contexto_ejecucion->pid, nombre_archivo_open, bloque_puntero_rs, dir_fisica_read, archivo->fcb->tamanio);


                //Permanece en estado bloqueado hasta que el módulo File System informe de la finalización de la operación.
                //codigo_operacion operacion_recibida_write = recibir_operacion(conexion_a_filesystem); 

                if(recibir_operacion(conexion_a_filesystem) == OK){
                    log_info(kernel, "Filesystem envio OK");
                }else{
                    log_error(kernel, "Filesystem no recibio correctamente el paquete write");
                }

                //free(operacion_recibida_write);
                free(nombre_archivo_write);
                //free(&dir_fisica_write);
                break;

            case F_TRUNCATE:

                memcpy(&tam_par1, buffer_cpu->stream + desplazamiento_cpu, sizeof(int));
                desplazamiento_cpu += sizeof(int);

                char *nombre_archivo_truncate = malloc(tam_par1 + 1);
                memcpy(nombre_archivo_truncate, buffer_cpu->stream + desplazamiento_cpu, tam_par1);
                nombre_archivo_truncate[tam_par1] = '\0';
                desplazamiento_cpu += tam_par1;

                memcpy(&tam_par2, buffer_cpu->stream + desplazamiento_cpu, sizeof(int));
                desplazamiento_cpu += sizeof(int);

                char *tamanio = malloc(tam_par2 + 1);
                memcpy(tamanio, buffer_cpu->stream + desplazamiento_cpu, tam_par2);
                tamanio[tam_par2] = '\0';
                desplazamiento_cpu += tam_par2;

                if(truncar_archivo(nombre_archivo_read, atoi(tamanio)) > 0){
                    log_info(logger_kernel, "PID : %d - Archivo: %s - Tamaño: %s", pcb->contexto_ejecucion->pid, nombre_archivo_truncate, tamanio);
                }else{
                    log_error(kernel,"No se pudo truncar el archivo");
                }

                free(nombre_archivo_truncate);
                free(tamanio);
                break;

            case EXIT_OP:
                
                agregar_a_exit(pcb, SUCCESS);
                sacar_de_execute();
                log_info(logger_kernel, "PID: %d - Estado Anterior: EXEC - Estado Actual: EXIT", pcb->contexto_ejecucion->pid);
                break;

            case PAGE_FAULT:
    
                int num_pag;
                int pid;
    
                memcpy(&num_pag, buffer_cpu->stream + desplazamiento_cpu, sizeof(int));
                desplazamiento_cpu += sizeof(int);
                memcpy(&pid, buffer_cpu->stream + desplazamiento_cpu, sizeof(int));
                desplazamiento_cpu += sizeof(int);
                
                void* parametros = malloc(sizeof(int)*2);
                memcpy(parametros, &num_pag, sizeof(int));
                memcpy(parametros+sizeof(int), &(pcb->contexto_ejecucion->pid), sizeof(int));

                pthread_t hilo_page_fault;
                pthread_create(&hilo_page_fault, NULL, (void*)atender_page_fault, parametros);

                log_info(logger_kernel, "Page Fault PID : %d - PAGINA: %d", pid, num_pag);
            
                //pthread_join(hilo_page_fault,NULL);

                //free(&num_pag);
                free(parametros);
                break;

            default:
                break;
        }
        liberar_buffer(buffer_cpu);
        sem_post(&cortoPlazoHablilitado);

    }

    
    //liberar_buffer(buffer_fs);

}

void *dormir_pcb(t_sleep *pcb_a_dormir){

    sleep(pcb_a_dormir->tiempo_dormido);

    agregar_a_ready(pcb_a_dormir->pcb);

    free(pcb_a_dormir);

    return NULL;

}

void semaforoQuantum(struct timespec *tiempo){
    clock_gettime(CLOCK_REALTIME, tiempo);
    tiempo->tv_nsec  += quantum()*1000000;
}

bool mayor_prioridad_nuevo(t_pcb* pcb, t_pcb* nuevo_pcb){
    return ((nuevo_pcb->prioridad) < (pcb->prioridad));
}

void enviar_interrupcion(t_msj_error motivo){
    send(conexion_cpu_interrupt, &motivo, sizeof(t_msj_error),0);
    
}

char *mensaje_a_string(t_msj_error motivo){
	switch (motivo){
    case SUCCESS:    
	    return "SUCCESS";
    case INVALID_WRITE:
        return "INVALID_WRITE";
    case PAGE_FAULT:
        return "PAGE_FAULT";
    case INVALID_RESOURCE:
        return "INVALID_RESOURCE";
    case FIN_QUANTUM:
        return "FIN_QUANTUM";
    case NUEVA_PRIORIDAD:
        return "NUEVA_PRIORIDAD";
    case DEADLOCK:
        return "DEADLOCK";
    case CONSOLA:
        return "CONSOLA";
    default:
        return "ERROR";
	}
}

void *atender_page_fault(void* parametros){
    int num_pag;
    int pid;

    memcpy(&num_pag, parametros, sizeof(int));
    memcpy(&pid, parametros+sizeof(int), sizeof(int));

    //1.- Mover al proceso al estado Bloqueado. Este estado bloqueado será independiente de todos los
    //      demás ya que solo afecta al proceso y no compromete recursos compartidos.
    
    t_pcb *pcb = buscarPcb(pid);

    sacar_de_execute(pcb);
    cambiar_estado_pcb(pcb, BLOCKED); 


    //2.- Solicitar al módulo memoria que se cargue en memoria principal la página correspondiente, la
    //    misma será obtenida desde el mensaje recibido de la CPU.

    t_paquete *paquete = crear_paquete(CARGAR_PAGINA);
    agregar_a_paquete(paquete, &num_pag, sizeof(int));
    agregar_a_paquete(paquete, &pid, sizeof(int));
    enviar_paquete(paquete,conexion_memoria);

    //3.- Esperar la respuesta del módulo memoria.
    //Permanece en estado bloqueado hasta que el módulo File Memoria informe de la respuesta del modulo

    //codigo_operacion respuesta_ok_mem= recibir_operacion(conexion_a_memoria); 

    if(recibir_operacion(conexion_memoria) == OK){
        log_info(kernel, "Memoria nos envio OK");
    }else{
        log_error(kernel, "Memoria no recibio correctamente la pagina a cargar");
    
    }
    //4.- Al recibir la respuesta del módulo memoria, desbloquear el proceso y colocarlo en la cola de ready.
    
    cambiar_estado_pcb(pcb, READY);
    agregar_a_ready(pcb);

    free(parametros);
    
    return NULL;
}

/*
void *chequear_prioridades(){

    int valor_semaforo;
    while(1){
        sem_wait(&nuevoPcbEnReady);
        sem_getvalue(&nuevoPcbEnReady, &valor_semaforo);
        t_pcb* nuevo_pcb_en_ready = list_get(lista_ready, (list_size(lista_ready) - 1 - valor_semaforo));

        if(mayor_prioridad_nuevo(pcb, nuevo_pcb_en_ready)){
            
            hayMayorPrioridad = true;

        }
    }
}*/

t_planificador planificador(){
    
    char* planificador = algoritmo_planificacion();

    if(strcmp("FIFO", planificador)==0){
        return FIFO;
    }

    if(strcmp("ROUND ROBIN", planificador)==0){
        return ROUND_ROBIN;
    }

    if(strcmp("PRIORIDADES",planificador)==0){
        return PRIORIDADES;        
    }
    return FIFO;
}

void algoritmo_planificador(t_planificador alg_planificacion){
    switch(alg_planificacion){
        case FIFO:
            break;

        case ROUND_ROBIN:
            sem_post(&SeEjecutaProceso);
            break;

        case PRIORIDADES:
            if(hayMayorPrioridad){
                hayMayorPrioridad = false;
                enviar_interrupcion(NUEVA_PRIORIDAD);
                sacar_de_execute();
            }
            break;
            
        default:
            log_error(kernel, "No sabemos que chucha pasó");
            break;
    }
}

void *chequear_prioridades(){

    hayMayorPrioridad = false;

    t_pcb* nuevo_pcb_en_ready;

    while(1){
        sem_wait(&nuevoPcbEnReady);
        
        for(int i = 0; i<list_size(lista_ready); i++){
            nuevo_pcb_en_ready = list_get(lista_ready, i);
            if(mayor_prioridad_nuevo(pcb_en_execute, nuevo_pcb_en_ready)){ 
                hayMayorPrioridad = true;

            }
        }
        
    }
}

void *round_robin(){
    
    while(1){
        sem_wait(&SeEjecutaProceso); 
        
        struct timespec* tiempo = malloc(sizeof(struct timespec)); //variable tiempo para corroborar cuando termina el quantum para RR

        semaforoQuantum(tiempo);

        if(sem_timedwait(&contextoActualizado, tiempo)==-1){

            enviar_interrupcion(FIN_QUANTUM);

            log_info(logger_kernel, "PID: %d - Desalojado por fin de quantum", pcb_en_execute->contexto_ejecucion->pid);

            sacar_de_execute();
        }
        free(tiempo);   
    }
}


char *listar_pids(t_list *lista_a_referenciar){

    char *lista_pids = string_new();

    t_pcb *pcb;

    for(int i=0; i<list_size(lista_a_referenciar)-1;i++){
        pcb = list_get(lista_a_referenciar, i);
        string_append_with_format(&lista_pids, "%d",pcb->contexto_ejecucion->pid);
        string_append(&lista_pids, (", "));
    }

    return (char *)listar_pids;
}

int pid_bloqueado(t_cola_block *cola_a_sacar){

    t_queue *cola_bloqueada = queue_create();
    cola_bloqueada = queue_peek(cola_a_sacar->cola_bloqueados);
    int* pid_bloqueado = (int *)queue_pop(cola_bloqueada);

    return *pid_bloqueado; 
}
