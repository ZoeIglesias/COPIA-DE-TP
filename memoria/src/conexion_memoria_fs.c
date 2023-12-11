#include "../include/conexion_memoria_fs.h"

void conectar_memoria_a_fs(char *ip_filesystem, char *puerto_filesystem){
    esperar_server_fs(cliente_filesystem);
    
    conexion_filesystem = crear_conexion(ip_filesystem, puerto_filesystem, memoria);
    
    realizar_handshake(0,conexion_filesystem);
    sem_post(&terminaron_conexiones);
    //liberar_conexion(conexion_a_memoria);
}

void esperar_server_fs(int cliente_filesystem){

    log_info(memoria,"Espero que se levante el FS");
    uint32_t result;
    recv(cliente_filesystem,&result,sizeof(uint32_t),MSG_WAITALL);
    
    log_info(memoria, "hice el recv");
    if(result == 1){
        uint32_t resultOk = 0;
        send(cliente_filesystem, &resultOk, sizeof(uint32_t), 0);
       log_info(memoria, "hice el send");
    }
    else{
        uint32_t resultError = -1;
        send(cliente_filesystem, &resultError, sizeof(uint32_t), 0);
        log_info(memoria, "hice el send");      
    }
}

void conectar_con_filesystem(int *socket_cliente_fs){ //HILO
    sem_wait(&terminaron_conexiones);
    t_paquete* paquete;
    t_pagina *pagina;
    int desplazamiento;
    void* contenido_bloque = malloc(tam_pagina()); //falta asignar memoria si lo que queremos hacer es memcpy
    void* contenido_pagina = malloc(tam_pagina()); //falta asignar memoria si lo que queremos hacer es memcpy
    int dir_fisica;
    int frame;
   

    while(1)
    {
        codigo_operacion cod_op = recibir_operacion(*socket_cliente_fs);
        t_buffer *buffer = crear_buffer();
        buffer -> stream = recibir_buffer(&(buffer->size), *socket_cliente_fs);
        desplazamiento = 0;
        switch (cod_op)
        {
        case F_READ://memoria escribe

            usleep(retardo_respuesta() * 1000);
            memcpy(contenido_bloque, buffer -> stream + desplazamiento, tam_pagina());
            desplazamiento += tam_pagina();
            memcpy(&dir_fisica, buffer -> stream + desplazamiento , sizeof(int));
            desplazamiento+=sizeof(int);
            
            sem_wait(&espacio_usuario);
            memcpy(memoria_usuario + dir_fisica, contenido_bloque, tam_pagina());
            sem_post(&espacio_usuario);
            frame = floor(dir_fisica / tam_pagina());
            pagina = obtener_pagina(frame);
            log_info(logger_memoria, "PID: %d - Accion: ESCRIBIR - Direccion fisica: %d", pagina -> PID, dir_fisica);

            paquete = crear_paquete(OK);
            enviar_paquete(paquete, *socket_cliente_fs);
            break;

        case F_WRITE://yo leo

            usleep(retardo_respuesta() * 1000);
            memcpy(&dir_fisica, buffer -> stream + desplazamiento , sizeof(int));
            desplazamiento += sizeof(int);
            
            sem_wait(&espacio_usuario);
            memcpy(contenido_pagina, memoria_usuario + dir_fisica, tam_pagina());
            frame = floor(dir_fisica / tam_pagina());
            pagina = obtener_pagina(frame);
            log_info(logger_memoria, "PID: %d - Accion: LEER - Direccion fisica: %d", pagina -> PID, dir_fisica);
            sem_post(&espacio_usuario);
 
            paquete = crear_paquete(OK);
            agregar_a_paquete(paquete, contenido_pagina, tam_pagina());
            enviar_paquete(paquete, *socket_cliente_fs);
            break;

        default:
            log_error(memoria, "No se reconocio la operacion solicitada de FS");
            break;
        }
        liberar_buffer(buffer);
        
    }
  free(contenido_bloque);
  free(contenido_pagina);
}

int* solicitar_SWAP(t_proceso_memoria *proceso)
{   
    int cant_paginas = obtener_cant_pags(proceso->tamanio_proceso_bytes);

    t_paquete *paquete = crear_paquete(INICIAR_PROCESO_FS);

    agregar_a_paquete(paquete, &cant_paginas, sizeof(int));

    enviar_paquete(paquete, conexion_filesystem);

    codigo_operacion cod_op = recibir_operacion(conexion_filesystem);

    if(cod_op != BLOQUE_SWAP)
    {
        log_error(memoria,"Error en la operacion con FS");
    }
    //RECIBO BLOQUES
    t_buffer *buffer = crear_buffer();
    buffer->stream = recibir_buffer(&(buffer->size), conexion_filesystem);

    int *bloques_asignados = malloc(cant_paginas * sizeof(int));
    memcpy(bloques_asignados, buffer->stream, cant_paginas * sizeof(int));
    liberar_buffer(buffer);
    
    log_info(memoria, "%d" ,bloques_asignados[1]);
    log_info(memoria, "%d" ,bloques_asignados[2]);

    proceso->bloques_asignados = bloques_asignados;

    return bloques_asignados;
}

void liberar_SWAP(t_proceso_memoria *proceso)
{   
    int cant_paginas = obtener_cant_pags(proceso->tamanio_proceso_bytes);

    t_paquete *paquete = crear_paquete(FINALIZAR_PROCESO_FS);

    agregar_a_paquete(paquete, &cant_paginas, sizeof(int));

    int *bloques_asignados;

    t_list *tabla_de_pagina = obtener_tabla_pagina(proceso->pcb->contexto_ejecucion->pid);

    t_pagina *pagina;

    int bloque_asignado;

    for(int i = 0; i<cant_paginas; i++){
        pagina = list_get(tabla_de_pagina, i);
        bloque_asignado = (pagina->pos_en_swap)/tam_pagina();
        bloques_asignados[i] = bloque_asignado;
    }


    agregar_a_paquete(paquete, &bloques_asignados, cant_paginas*sizeof(int));

    enviar_paquete(paquete, conexion_filesystem);
}


void solicitar_pagina(int num_pagina, int pid)
{
    
    t_list *tabla_paginas = obtener_tabla_pagina(pid);
   
    t_pagina* pagina_in = list_get(tabla_paginas,num_pagina);

    t_proceso_memoria* proceso = obtener_proceso_memoria(pid); 

    int frame = asignar_frame_libre();

    //SI NO HAY ESPACIO DEBO CORRER ALGUNO DE LOS DOS ALGORTIMOS.
    if(frame == -1)
    {   
        t_pagina* pagina_out;
        if(strcmp(algoritmo_reemplazo(), "FIFO") == 0)
        {
            frame = aplicar_algoritmo_FIFO();
        }
        else 
            {
                frame = aplicar_algoritmo_LRU();
            }
        
        pagina_out = obtener_pagina(frame);

        log_info(logger_memoria, "REEMPLAZO - Marco: %d - Page Out: %d - %d - Page In: %d - %d", frame, pagina_out ->PID, get_index(pagina_out -> PID, frame), pagina_in->PID, num_pagina);
        if(pagina_out -> modificado)
        {
            //ESCRIBIR EN SWAP
            t_paquete *paquete = crear_paquete(ESCRIBIR_SWAP);
            void* contenido_pagina = malloc(tam_pagina());
            memcpy(contenido_pagina, memoria_usuario + tam_pagina() * pagina_out->marco, tam_pagina());
            agregar_a_paquete(paquete, &contenido_pagina, tam_pagina());
            agregar_a_paquete(paquete, &pagina_out -> pos_en_swap, sizeof(int));
            enviar_paquete(paquete, conexion_filesystem);
            free(contenido_pagina);
            
            //codigo_operacion codigo = recibir_operacion(conexion_filesystem);//recibir_operacion - INSTRUCCION OK 

            if(recibir_operacion(conexion_filesystem) == OK){
                log_info(memoria, "Filesystem nos envio OK");
            }else{
                log_error(memoria, "Filesystem no recibio bien la directiva de escribir en swap");
            }
            
            pagina_out -> modificado = 0;
            log_info(logger_memoria, "SWAP OUT - PID: %d - Marco: %d - Page Out: %d-%d", pid, frame, pagina_out -> PID, num_pagina);
        }

        pagina_out -> presencia = 0;
  
    }

    
    pagina_in -> marco = frame;
    pagina_in -> presencia = true;
    pagina_in -> modificado = false;
    pagina_in -> pos_en_swap = proceso->bloques_asignados[num_pagina] * tam_pagina();
    pagina_in -> tiempo_carga = time(NULL); 
    pagina_in -> ultima_referencia = time(NULL);
    pagina_in -> PID = pid;

    //PEDIR EL BLOQUE A FS
    t_paquete *paquete = crear_paquete(LEER_SWAP);
    agregar_a_paquete(paquete, &(pagina_in->pos_en_swap), sizeof(int));
    enviar_paquete(paquete, conexion_filesystem);

    

    if(recibir_operacion(conexion_filesystem) != BLOQUE_SWAP)
    {
        log_error(memoria,"Error en la operacion");
    }

    void* contenido_bloque = malloc(tam_pagina()); 

    t_buffer *buffer = crear_buffer();
    buffer -> stream = recibir_buffer(&(buffer->size), conexion_filesystem);

    memcpy(contenido_bloque, buffer -> stream, tam_pagina());
    //ME QUEDO ESPERANDO EL BLOQUE DE FS

    //guardar bloque swap en memoria en el frame
    sem_wait(&espacio_usuario);
    memcpy(memoria_usuario + frame *tam_pagina(),contenido_bloque,tam_pagina());
    sem_post(&espacio_usuario);
    free(contenido_bloque);//noc xd
    log_info(logger_memoria,"SWAP IN -  PID: %d - Marco: %d - Page In: %d-%d", pid, frame, pid, num_pagina);
}

