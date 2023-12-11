#include "../include/conexiones_filesystem.h"

void* conectar_con_kernel(int *socket_kernel){
    codigo_operacion codigo;
    char* nombre_archivo;
    int dir_fisica;
    int tamanio;
    int bloque_puntero;
    int bloque;
    int tam_char;
    //void* contenido_pagina;
    //void* contenido_bloque;
    void* contenido_pagina = (void*)malloc(sizeof(void));
    void* contenido_bloque = (void*)malloc(sizeof(void));
    void* bloque_leido;
    //t_FCB* fcb; no se usaba aparentemente
    t_paquete *paquete;
    int desplazamiento;
    t_buffer *buffer_memoria;

    while(true){
        codigo = recibir_operacion(*socket_kernel);
        t_buffer *buffer = crear_buffer();
        buffer->stream = recibir_buffer(&(buffer->size), *socket_kernel);
        desplazamiento = 0;
        switch(codigo){
            case APERTURA_FS:
                memcpy(&tam_char,buffer->stream + desplazamiento, sizeof(int));
                desplazamiento+=sizeof(int);
                memcpy(&nombre_archivo,buffer->stream + desplazamiento, tam_char);
                desplazamiento+=tam_char;
                tamanio = abrir_archivo(nombre_archivo);
                paquete = crear_paquete(OK);
                //t_paquete* paquete = crear_paquete(OK); probe usarla directo que ya esta creada arriba
                agregar_a_paquete(paquete, &tamanio, sizeof(int));
                enviar_paquete(paquete, *socket_kernel);
                break;

            case LEER_FS:
                memcpy(&tam_char,buffer->stream + desplazamiento, sizeof(int));
                desplazamiento+=sizeof(int);
                memcpy(&nombre_archivo,buffer->stream + desplazamiento, tam_char);
                desplazamiento+=tam_char;
                memcpy(&dir_fisica,buffer->stream + desplazamiento, sizeof(int));
                desplazamiento+=sizeof(int);
                memcpy(&bloque_puntero,buffer->stream + desplazamiento, sizeof(int));
                desplazamiento+=sizeof(int);
                bloque = buscar_bloque_archivo(bloque_puntero, nombre_archivo);
                acceder_bloque_archivo(bloque, nombre_archivo);
                bloque_leido = leer_bloque(bloque);
                //enviar a memoria
                memcpy(contenido_bloque, bloque_leido, tam_bloque());
                paquete = crear_paquete(F_READ);
                agregar_a_paquete(paquete, contenido_bloque, tam_bloque());
                agregar_a_paquete(paquete, &dir_fisica, tam_bloque());
                enviar_paquete(paquete, conexion_memoria);

                codigo = recibir_operacion(conexion_memoria);
                if(codigo == OK){
                    paquete = crear_paquete(OK);
                    enviar_paquete(paquete, *socket_kernel);
                    log_leer_archivo(nombre_archivo, bloque_puntero, dir_fisica);
                }
                break;

            case ESCRIBIR_FS:
                memcpy(&tam_char,buffer->stream + desplazamiento, sizeof(int));
                desplazamiento+=sizeof(int);
                memcpy(&nombre_archivo,buffer->stream + desplazamiento, tam_char);
                desplazamiento+=tam_char;
                memcpy(&dir_fisica,buffer->stream + desplazamiento, sizeof(int));//revisar, deberia ser fisica
                desplazamiento+=sizeof(int);
                memcpy(&bloque_puntero,buffer->stream + desplazamiento, sizeof(int));
                desplazamiento+=sizeof(int);
                bloque = buscar_bloque_archivo(bloque_puntero, nombre_archivo);
                paquete = crear_paquete(F_WRITE);
                agregar_a_paquete(paquete, &dir_fisica, sizeof(int));
                enviar_paquete(paquete, conexion_memoria);

                codigo = recibir_operacion(conexion_memoria);
                if(codigo == OK){
                    buffer_memoria = crear_buffer();
                    buffer_memoria->stream = recibir_buffer(&(buffer_memoria->size), conexion_memoria);
                    int desplazamiento_memoria = 0;

                    memcpy(contenido_pagina,buffer_memoria->stream + desplazamiento_memoria, tam_bloque());
                    desplazamiento_memoria+=tam_bloque();

                    escribir_bloque(contenido_pagina, bloque);
                    paquete = crear_paquete(OK);
                    enviar_paquete(paquete, *socket_kernel);
                    log_escribir_archivo(nombre_archivo, bloque_puntero, dir_fisica);
                    liberar_buffer(buffer_memoria);
                     
                }
                
                break;

            case TRUNCATE_FS:
                memcpy(&tam_char,buffer->stream + desplazamiento, sizeof(int));
                desplazamiento+=sizeof(int);
                memcpy(&nombre_archivo,buffer->stream + desplazamiento, tam_char);
                desplazamiento+=tam_char;
                memcpy(&tamanio,buffer->stream + desplazamiento, sizeof(int));
                desplazamiento+=sizeof(int);
                truncar_fcb(nombre_archivo, tamanio);
                paquete = crear_paquete(OK);
                enviar_paquete(paquete,*socket_kernel);
                break;

            default:
                break;
        }
        liberar_buffer(buffer); 
    }
    free(contenido_pagina);
    free(contenido_bloque);
}


void* conectar_con_memoria(int *socket_memoria){
    codigo_operacion codigo;
    int cantidad_bloques;
    int* bloques_asignados;
    int* bloques_a_liberar;
    //void* contenido_pagina;
    void* contenido_bloque;
    void* contenido_pagina = (void*)malloc(sizeof(void));
    int posicion_swap;
    int bloque;
    int desplazamiento;
    t_paquete *paquete;
    while(true){
        codigo = recibir_operacion(*socket_memoria);
        t_buffer *buffer = crear_buffer();
        buffer->stream = recibir_buffer(&(buffer->size), *socket_memoria);
        desplazamiento = 0;
        switch(codigo){
            case INICIAR_PROCESO_FS:
                memcpy(&cantidad_bloques,buffer->stream + desplazamiento, sizeof(int));
                desplazamiento += sizeof(int);
                
                bloques_asignados = asignar_bloques_SWAP(cantidad_bloques);
                paquete = crear_paquete(BLOQUE_SWAP);
                agregar_a_paquete(paquete,&bloques_asignados, cantidad_bloques * sizeof(int));
                enviar_paquete(paquete,*socket_memoria); 
                break;
            case FINALIZAR_PROCESO_FS:
                memcpy(&cantidad_bloques,buffer->stream + desplazamiento, sizeof(int));
                desplazamiento += sizeof(int);
                memcpy(&bloques_a_liberar,buffer->stream + desplazamiento, cantidad_bloques*sizeof(int));
                desplazamiento += cantidad_bloques*sizeof(int);
                liberar_bloques_SWAP(bloques_a_liberar, cantidad_bloques); //ANDA PARA LA MIERDA REVISAR
                break;
            case ESCRIBIR_SWAP:
                memcpy(contenido_pagina,buffer->stream + desplazamiento, tam_bloque());
                desplazamiento += tam_bloque();
                memcpy(&posicion_swap,buffer->stream + desplazamiento, sizeof(int));
                desplazamiento += sizeof(int);
                bloque = posicion_swap / tam_bloque();

                escribir_bloque(contenido_pagina, bloque);
                
                paquete = crear_paquete(OK);
                enviar_paquete(paquete,*socket_memoria);
                log_acceso_Bloque_SWAP(bloque);
                break;
            case LEER_SWAP:
                memcpy(&posicion_swap,buffer->stream + desplazamiento, sizeof(int));
                desplazamiento += sizeof(int);
                bloque = posicion_swap / tam_bloque();

                contenido_bloque = leer_bloque(bloque);

                paquete = crear_paquete(BLOQUE_SWAP);
                agregar_a_paquete(paquete, contenido_bloque, tam_bloque());
                enviar_paquete(paquete,*socket_memoria);
                log_acceso_Bloque_SWAP(bloque);
                  
                break;
            default:
                break;
        }
        liberar_buffer(buffer);  
    }
}
