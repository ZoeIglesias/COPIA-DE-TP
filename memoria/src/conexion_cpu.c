#include "../include/conexion_cpu.h"

void operar_con_cpu(int *socket_cpu)
{ 
    
    t_buffer *buffer;
    int desplazamiento;
    int dir_fisica;
    int pid;

    while (1)
    {   
        
        int cod_op = recibir_operacion(*socket_cpu);
        buffer = crear_buffer();
        buffer->stream = recibir_buffer(&(buffer->size),*socket_cpu);
        desplazamiento = 0;
        switch (cod_op)
        {
        case SOLICITUD_INSTRUCCION:
           
            enviar_instruccion(socket_cpu, buffer); 
            break;

        case SOLICITUD_MARCO:

            responder_solicitud_de_marco(socket_cpu,buffer);
            break;

        case LEER_VALOR:

            usleep(retardo_respuesta() * 1000);

            memcpy(&pid, buffer->stream + desplazamiento, sizeof(int));
            desplazamiento += sizeof(int);
            
            memcpy(&dir_fisica, buffer->stream + desplazamiento, sizeof(int));
            desplazamiento += sizeof(int);

            uint32_t *valor_leido = (uint32_t *)malloc(sizeof(uint32_t));

            //semaforo para acceso a espacio compartido --> memoria de usuario
            sem_wait(&espacio_usuario);
            memcpy(valor_leido, memoria_usuario + dir_fisica, sizeof(uint32_t));
            log_info(logger_memoria, "PID: %d - Accion: LEER - Direccion fisica: %d", pid, dir_fisica);
            sem_post(&espacio_usuario);
            //semaforo para acceso a espacio compartido --> memoria de usuario

            enviar_mensaje(valor_leido,sizeof(uint32_t), *socket_cpu);

            free(valor_leido);
            break;
        
        case ESCRIBIR:

            usleep(retardo_respuesta() * 1000);

            memcpy(&dir_fisica, buffer->stream + desplazamiento, sizeof(int));
            desplazamiento += sizeof(int);

            memcpy(&pid, buffer->stream + desplazamiento, sizeof(int));
            desplazamiento += sizeof(int);

            uint32_t *registro = malloc(sizeof(uint32_t));
            memcpy(registro, buffer->stream + desplazamiento, sizeof(uint32_t));
            desplazamiento += sizeof(uint32_t);
            
            //semaforo para acceso a espacio compartido --> memoria de usuario
            sem_wait(&espacio_usuario);
            memcpy(memoria_usuario + dir_fisica, registro, sizeof(uint32_t));
            log_info(logger_memoria, "PID: %d - Accion: ESCRIBIR - Direccion fisica: %d", pid, dir_fisica);
            sem_post(&espacio_usuario);
            //semaforo para acceso a espacio compartido --> memoria de usuario

            free(registro);
            enviar_mensaje("OK",3, *socket_cpu);

            break;

        default:

            log_info(memoria, "No se reconocio la operacion solicitada de CPU");
            
            break;
        }
        liberar_buffer(buffer);
    }
    
    
    
}
void enviar_instruccion(int *socket_cpu, t_buffer *buffer) 
{   
    
    int desplazamiento = 0;
    usleep(retardo_respuesta() * 1000);

    uint32_t pc;

    memcpy(&pc, buffer->stream, sizeof(uint32_t)); //Deserilizo el PC
    desplazamiento += sizeof(uint32_t);

    int pid; 
    memcpy(&pid, buffer->stream + desplazamiento, sizeof(int));

    t_list *instrucciones_a_enviar = obtener_instrucciones_proceso(pid);

    if(instrucciones_a_enviar == NULL)
    {
        log_error(memoria,"Error al obtener la lista de instrucciones");
        exit(EXIT_FAILURE);
    }
    
    if(pc < 0 || pc >= (list_size(instrucciones_a_enviar))){
        
        log_error(memoria, "Error: program_counter fuera de rango.");
        exit(EXIT_FAILURE);
    } 
    
    t_instruccion *instruccion_enviar = list_get(instrucciones_a_enviar, pc);
    
    t_paquete *paquete = crear_paquete(INSTRUCCION);
    
    serializar_instruccion(instruccion_enviar, paquete);

    enviar_paquete(paquete, *socket_cpu);

}

t_list *obtener_instrucciones_proceso(int pid_enviado)
{ 
    for(int i=0; i<(list_size(lista_instrucciones_globales));i++)
    {
        t_instrucciones_proceso *instrucciones_asociado_pid = list_get(lista_instrucciones_globales, i);
        if(instrucciones_asociado_pid -> pid == pid_enviado)
        {
            return instrucciones_asociado_pid -> lista_instrucciones;
        }
    }

    return NULL;
}

void leerInstruccionesDesdeArchivo(char *nombre_archivo,int pid_asociado){

    log_info(memoria,"tratanod de leer archivo");
    char* path = string_new();
    string_append_with_format(&path, "%s/%s", path_instrucciones(), nombre_archivo);
    FILE* nuevo_archivo = fopen(path, "r");

    if(nuevo_archivo == NULL)
    {
        log_error(memoria,"error al abrir el archivo");
        exit(EXIT_FAILURE);
    }
    
    int contador_instrucciones = 0;
    fseek(nuevo_archivo,0,SEEK_SET);
    //char ** lista_instrucciones_creada;
    t_instrucciones_proceso *instrucciones_pid = malloc(sizeof(t_instrucciones_proceso));
    instrucciones_pid -> lista_instrucciones = list_create();
    char * linea = malloc(100);
    char* contenido;
    

    while(fgets(linea,100,nuevo_archivo) != NULL)
    {   
        size_t len = strlen(linea);
        t_instruccion * instruccion = malloc(sizeof(t_instruccion));        
        if(len > 0 && linea[len -1] == '\n')
        {
            linea[len - 1] = '\0';
        } 
        contenido = strtok(linea, " ");
        instruccion->operacion = obtener_operacion_cpu(contenido);
        instruccion->cantidad_parametros = get_cant_parametros(instruccion->operacion);
        instruccion->parametros = malloc(sizeof(char*)*(instruccion->cantidad_parametros));
        for(int i=0; i<(instruccion->cantidad_parametros);i++){
            contenido = strtok(NULL, " ");
            instruccion->parametros[i] = malloc(strlen(contenido)+1);
            instruccion->parametros[i] = strdup(contenido);
        }
        
        list_add(instrucciones_pid -> lista_instrucciones, instruccion);
        contador_instrucciones++;
    }

 
    fclose(nuevo_archivo);

    instrucciones_pid -> pid = pid_asociado;

    list_add(lista_instrucciones_globales,instrucciones_pid);

    free(linea);
    free(path);
}


codigo_operacion obtener_operacion_cpu(char *operacion){
    if(strcmp(operacion,"SET") == 0){
        return SET;
    }
    if(strcmp(operacion,"SUM") == 0){
        return SUM;
    }
    if(strcmp(operacion,"SUB") == 0){
        return SUB;
    }
    if(strcmp(operacion,"JNZ") == 0){
        return JNZ;
    }
    if(strcmp(operacion,"SLEEP") == 0){
        return SLEEP;
    }
    if(strcmp(operacion,"WAIT") == 0){
        return WAIT;
    }
    if(strcmp(operacion,"SIGNAL") == 0){
        return SIGNAL;
    }
    if(strcmp(operacion,"MOV_IN") == 0){
        return MOV_IN;
    }
    if(strcmp(operacion,"MOV_OUT") == 0){
        return MOV_OUT;
    }
    if(strcmp(operacion,"F_OPEN") == 0){
        return F_OPEN;
    }
    if(strcmp(operacion,"F_CLOSE") == 0){
        return F_CLOSE;
    }
    if(strcmp(operacion,"F_SEEK") == 0){
        return F_SEEK;
    }
    if(strcmp(operacion,"F_READ") == 0){
        return F_READ;
    }
    if(strcmp(operacion,"F_WRITE") == 0){
        return F_WRITE;
    }
    if(strcmp(operacion,"F_TRUNCATE") == 0){
        return F_TRUNCATE;
    }
    if(strcmp(operacion,"EXIT") == 0){
        return EXIT_OP;
    }else
        return ROMPEROMPEROMPE;
}

int get_cant_parametros(codigo_operacion identificador)
{
    int cant_parametros = 0;
    switch (identificador)
    {
        case EXIT_OP:
             break;

         case WAIT: case SIGNAL: case SLEEP:
             cant_parametros = 1;
             break;
 
        case SET: case MOV_IN: case MOV_OUT: case F_TRUNCATE: case F_SEEK: case JNZ: case SUB: case F_OPEN: case F_READ: case F_WRITE: case SUM: case F_CLOSE:
            cant_parametros = 2;
            break;
     
    }
        
    
    return cant_parametros;
}
void responder_solicitud_de_marco(int socket_cpu, t_buffer *buffer){

    usleep(retardo_respuesta() * 1000);

    int num_pagina;
    int pid;
    int desplazamiento = 0;

    memcpy(&num_pagina, buffer->stream + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);

    memcpy(&pid, buffer->stream + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);

    t_tabla_por_proceso *tabla_proceso = (t_tabla_por_proceso*)malloc(sizeof(t_tabla_por_proceso));

    tabla_proceso->tabla_paginas = list_create();
    tabla_proceso->pid = pid;

    if(num_pagina < 0 || num_pagina >= (list_size(tabla_proceso->tabla_paginas))){
        log_error(memoria, "El numero de pagina se encuentra fuera de los limites");
        send(socket_cpu, &(int){-1}, sizeof(int), 0);
        return;
    }

    t_pagina *pagina = list_get(tabla_proceso->tabla_paginas,num_pagina);

    if(pagina->presencia == 0)
    {
        return;
    }

    int num_marco = pagina -> marco;
    pagina -> ultima_referencia = time(NULL);
    
    log_info(logger_memoria, "PID: %d - Pagina: %d - Marco: %d",pid, num_pagina, num_marco);

    send(socket_cpu, &num_marco, sizeof(int),0);
}
