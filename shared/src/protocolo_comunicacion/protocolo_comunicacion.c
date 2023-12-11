#include "protocolo_comunicacion.h"

//HANDSHAKE
int verificar_cliente(int socket_cliente, t_log *logger){

    switch (recibir_operacion(socket_cliente))
    {
    case 0: 
        log_info(logger,"Se reconocio al cliente MEMORIA");
        break;
    case 1:
        log_info(logger,"Se reconocio al cliente FILE SYSTEM");
        break;
    case 2:
        log_info(logger,"Se reconocio al cliente CPU");
        break;
    case 3:
        log_info(logger,"Se reconocio al cliente KERNEL");
        break;
    default:
        //printf("No se pudo reconocer al CLIENTE :%d", socket_cliente);
        return 0;
    }

    return 1;

}

void realizar_handshake(int numero, int server){

    int *handshake = malloc(sizeof(int));
    *handshake = numero;
    send(server,handshake, sizeof(int), 0);
    free(handshake);


}
// -----------------
//  SERIALIZACION
// -----------------
void serializar_contexto(t_paquete *paquete,t_contexto_ejecucion *ctx)
{
    agregar_a_paquete(paquete, &(ctx->pid), sizeof(int));
    
    agregar_a_paquete(paquete, &(ctx->program_counter), sizeof(uint32_t));
    
    serializar_instrucciones(ctx->instrucciones, paquete);

    //AGREGO REGISTROS
    agregar_a_paquete(paquete,&(ctx->registros->AX), sizeof(uint32_t));
    agregar_a_paquete(paquete,&(ctx->registros->BX), sizeof(uint32_t));
    agregar_a_paquete(paquete,&(ctx->registros->CX), sizeof(uint32_t));
    agregar_a_paquete(paquete,&(ctx->registros->DX), sizeof(uint32_t));


}
void serializar_instrucciones(t_list *instrucciones, t_paquete *paquete)
{
	int cant_instrucciones = list_size(instrucciones);
    agregar_a_paquete(paquete, &cant_instrucciones, sizeof(int));

	for (int i = 0; i < cant_instrucciones; i++)
	{
		t_instruccion *instruccion = list_get(instrucciones, i);
		serializar_instruccion(instruccion, paquete);
	}
}
void serializar_instruccion(t_instruccion *instruccion, t_paquete *paquete)
{
	int tamanio_parametro;

	agregar_a_paquete(paquete, &(instruccion->operacion), sizeof(codigo_operacion));

	agregar_a_paquete(paquete, &(instruccion->cantidad_parametros), sizeof(int));

	for (int i = 0; i < instruccion->cantidad_parametros; i++)
	{
		// Agrego el tamanio del parametro porque es un char*
		tamanio_parametro = strlen(instruccion->parametros[i]) + 1;// /'0'
		agregar_a_paquete(paquete, &tamanio_parametro, sizeof(int));

		// Agrego el parametro
		agregar_a_paquete(paquete, instruccion->parametros[i], tamanio_parametro);
	}
}
//SERIALIZAR PCB
void serializar_pcb(t_paquete *paquete, t_pcb *pcb_a_serializar)
{
    agregar_a_paquete(paquete, &(pcb_a_serializar->prioridad), sizeof(int));

    // Serializar la lista tabla_archivos_abiertos
    int cantidad_archivos = list_size(pcb_a_serializar->tabla_archivos_abiertos);
    agregar_a_paquete(paquete, &cantidad_archivos, sizeof(int));

    for (int i = 0; i < cantidad_archivos; i++)
    {
        t_archivos_proceso *archivo_actual = list_get(pcb_a_serializar->tabla_archivos_abiertos, i);
        serializar_archivos_proceso(paquete, archivo_actual);
    }

    serializar_contexto(paquete, pcb_a_serializar->contexto_ejecucion);
    agregar_a_paquete(paquete, &(pcb_a_serializar->estado), sizeof(t_estado));
    serializar_lista_recursos_asignados(paquete, pcb_a_serializar->recursos_asignados);
}


void serializar_archivos_proceso(t_paquete *paquete, t_archivos_proceso *archivos_proceso)
{
    serializar_fcb(paquete, archivos_proceso -> fcb);
    agregar_a_paquete(paquete,&(archivos_proceso -> puntero_bloque), sizeof(int));
    agregar_a_paquete(paquete,&(archivos_proceso -> tipo_lock), sizeof(locks));

}
void serializar_fcb(t_paquete *paquete, t_fcb_kernel *fcb_kernel)
{
    int tamanio_nombre = strlen(fcb_kernel->nombre)+1;
    agregar_a_paquete(paquete,&tamanio_nombre, sizeof(int));
    agregar_a_paquete(paquete, fcb_kernel -> nombre, tamanio_nombre);
    agregar_a_paquete(paquete, &(fcb_kernel -> tamanio), sizeof(int));
}
//
void serializar_lista_recursos_asignados(t_paquete *paquete, t_list *recursos_asignados)
{
    int cant_recursos = list_size(recursos_asignados);

    agregar_a_paquete(paquete,&cant_recursos, sizeof(int));

    for(int i = 0; i < cant_recursos; i++)
    {
        t_recurso *recurso = list_get(recursos_asignados,i);
        serilizar_recurso(paquete, recurso);
    }
}

void serilizar_recurso(t_paquete *paquete, t_recurso *recurso)
{
    int tam_nombre = strlen(recurso -> nombre)+1;
    agregar_a_paquete(paquete, &tam_nombre, sizeof(int));
    agregar_a_paquete(paquete, recurso->nombre, tam_nombre);
    agregar_a_paquete(paquete, &(recurso->cantidad), sizeof(int));
    agregar_a_paquete(paquete, &(recurso->id_recurso_lista), sizeof(int));

}
// -------------------
//  DESERIALIZACION
// -------------------
t_contexto_ejecucion *deserializar_contexto(t_buffer *buffer)
{   
    int desplazamiento = 0;
    //Asigno memoria dinamica para el contexto
    t_contexto_ejecucion *ctx = malloc(sizeof(t_contexto_ejecucion));  //Se libera en liberar_contexto(*ctx)

    //Copio en el contexto los campos y actualizo el desplazamiento
    memcpy(&(ctx->pid), buffer->stream + desplazamiento, sizeof(ctx->pid));
    desplazamiento += sizeof(ctx->pid);
    
    memcpy(&(ctx->program_counter), buffer->stream + desplazamiento, sizeof(ctx->program_counter));
    desplazamiento += sizeof(ctx->program_counter);
    
    memcpy(&(ctx->cant_instrucciones), buffer->stream + desplazamiento, sizeof(int));//VER PARA AGREGAR AL CTX
	desplazamiento += sizeof(int);

	// Deserializo Instrucciones
	ctx->instrucciones = list_create();
    for (int i = 0; i <  ctx->cant_instrucciones; i++)
    {
        t_instruccion *instruccion = deserializar_instruccion(buffer, &desplazamiento);
        list_add(ctx->instrucciones, instruccion);
    }
    
    //Deserializo registros
    ctx->registros = deserializar_registros(buffer, &desplazamiento);
    
    return ctx;
}
t_contexto_ejecucion *deserializar_contexto_con_desplazamiento(t_buffer *buffer, int *desplazamiento)
{   
    //Asigno memoria dinamica para el contexto
    t_contexto_ejecucion *ctx = malloc(sizeof(t_contexto_ejecucion));  

    //Copio en el contexto los campos y actualizo el desplazamiento
    memcpy(&(ctx->pid), buffer->stream + *desplazamiento, sizeof(int));
    *desplazamiento += sizeof(int);
    
    memcpy(&(ctx->program_counter), buffer->stream + *desplazamiento, sizeof(uint32_t));
    *desplazamiento += sizeof(uint32_t);
    
    memcpy(&(ctx->cant_instrucciones), buffer->stream + *desplazamiento, sizeof(int));//VER PARA AGREGAR AL CTX
	*desplazamiento += sizeof(int);

	// Deserializo Instrucciones
	ctx->instrucciones = list_create();
    for (int i = 0; i <  ctx->cant_instrucciones; i++)
    {
        t_instruccion *instruccion = deserializar_instruccion(buffer, desplazamiento);
        list_add(ctx->instrucciones, instruccion);
    }
    
    //Deserializo registros
    ctx->registros = deserializar_registros(buffer, desplazamiento);
    
    return ctx;
}
t_registros *deserializar_registros(t_buffer *buffer, int *desplazamiento)
{
    t_registros* registros = malloc(sizeof(t_registros));

    memcpy(&(registros->AX), buffer->stream + *desplazamiento,sizeof(uint32_t ));
    *desplazamiento += sizeof(uint32_t );

    memcpy(&(registros->BX), buffer->stream + *desplazamiento,sizeof(uint32_t ));
    *desplazamiento += sizeof(uint32_t );

    memcpy(&(registros->CX), buffer->stream + *desplazamiento,sizeof(uint32_t ));
    *desplazamiento += sizeof(uint32_t );
    
    memcpy(&(registros->DX), buffer->stream + *desplazamiento,sizeof(uint32_t ));
    *desplazamiento += sizeof(uint32_t );

    return registros;
}
t_instruccion *deserializar_instruccion(t_buffer *buffer, int *desplazamiento)
{

    t_instruccion *instruccion_deserializada = malloc(sizeof(t_instruccion));

    memcpy(&(instruccion_deserializada->operacion), buffer->stream + *desplazamiento, sizeof(codigo_operacion));
    *desplazamiento += sizeof(codigo_operacion);

    memcpy(&(instruccion_deserializada->cantidad_parametros), buffer->stream + *desplazamiento, sizeof(int));
    *desplazamiento += sizeof(int);

    instruccion_deserializada->parametros = malloc(instruccion_deserializada->cantidad_parametros * sizeof(char*));
    for (int i = 0; i < instruccion_deserializada->cantidad_parametros; i++)
    {
        int len;
        memcpy(&len, buffer->stream + *desplazamiento, sizeof(int));
        *desplazamiento += sizeof(int);
    
        instruccion_deserializada->parametros[i] = malloc(len); 
        memcpy(instruccion_deserializada->parametros[i], buffer->stream + *desplazamiento, len);
        *desplazamiento += len;
    }

    return instruccion_deserializada;
}

t_instruccion *deserializar_instruccion_solicitada(t_buffer *buffer)
{   
    int desplazamiento = 0;

    t_instruccion *instruccion_deserializada = malloc(sizeof(t_instruccion));

    memcpy(&(instruccion_deserializada->operacion), buffer->stream + desplazamiento, sizeof(codigo_operacion));
    desplazamiento += sizeof(codigo_operacion);

    memcpy(&(instruccion_deserializada->cantidad_parametros), buffer->stream + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);

 
    instruccion_deserializada->parametros = malloc(instruccion_deserializada->cantidad_parametros * sizeof(char*));
    for (int i = 0; i < instruccion_deserializada->cantidad_parametros; i++)
    {
        int len;
        memcpy(&len, buffer->stream + desplazamiento, sizeof(int));
        desplazamiento += sizeof(int);
        
        instruccion_deserializada->parametros[i] = malloc(len); 
        memcpy(instruccion_deserializada->parametros[i], buffer->stream + desplazamiento, len);
        desplazamiento += len;
    }

    return instruccion_deserializada;
}

t_pcb* deserializar_pcb_con_desplazamiento(t_buffer* buffer, int* desplazamiento)
{
    t_pcb* pcb = malloc(sizeof(t_pcb));
    
    // Deserializar prioridad
    memcpy(&(pcb->prioridad), buffer->stream + *desplazamiento, sizeof(int));
    *desplazamiento += sizeof(int);

    // Deserializar tabla_archivos_abiertos
    int cantidad_archivos ;
    memcpy(&cantidad_archivos, buffer->stream + *desplazamiento, sizeof(int));
    *desplazamiento += sizeof(int);

    pcb->tabla_archivos_abiertos = list_create();

    for (int i = 0; i < cantidad_archivos; i++)
    {
        t_archivos_proceso* archivo_proceso = deserializar_archivos_proceso(buffer, desplazamiento);
        list_add(pcb->tabla_archivos_abiertos, archivo_proceso);
    }

    // Deserializar contexto_ejecucion
    pcb->contexto_ejecucion = deserializar_contexto_con_desplazamiento(buffer, desplazamiento);

    // Deserializar estado
    memcpy(&(pcb->estado), buffer->stream + *desplazamiento, sizeof(t_estado));
    *desplazamiento += sizeof(t_estado);

    // Deserializar recursos_asignados
    int cantidad_recursos;
    memcpy(&cantidad_recursos, buffer->stream + *desplazamiento, sizeof(int));
    *desplazamiento += sizeof(int);

    pcb->recursos_asignados = list_create();

    for (int i = 0; i < cantidad_recursos; i++)
    {
        t_recurso* recurso = deserializar_recurso(buffer, desplazamiento);
        list_add(pcb->recursos_asignados, recurso);
    }

    return pcb;
}

t_archivos_proceso* deserializar_archivos_proceso(t_buffer* buffer, int* desplazamiento)
{
    t_archivos_proceso* archivos_proceso = malloc(sizeof(t_archivos_proceso));

    // Deserializar fcb
    archivos_proceso->fcb = deserializar_fcb_kernel(buffer, desplazamiento);

    // Deserializar puntero_bloque
    memcpy(&(archivos_proceso->puntero_bloque), buffer->stream + *desplazamiento, sizeof(int));
    *desplazamiento += sizeof(int);

    // Deserializar tipo_lock
    memcpy(&(archivos_proceso->tipo_lock), buffer->stream + *desplazamiento, sizeof(locks));
    *desplazamiento += sizeof(locks);

    // No es necesario deserializar participantes_lock ya que siempre es NULL
    
    return archivos_proceso;
}

t_fcb_kernel* deserializar_fcb_kernel(t_buffer* buffer, int* desplazamiento)
{
    t_fcb_kernel* fcb_kernel = malloc(sizeof(t_fcb_kernel));

    // Deserializar tamanio_nombre
    int tamanio_nombre;
    memcpy(&tamanio_nombre, buffer->stream + *desplazamiento, sizeof(int));
    *desplazamiento += sizeof(int);

    // Deserializar nombre
    fcb_kernel->nombre = malloc(tamanio_nombre);
    memcpy(fcb_kernel->nombre, buffer->stream + *desplazamiento, tamanio_nombre);
    *desplazamiento += tamanio_nombre;

    // Deserializar tamanio
    memcpy(&(fcb_kernel->tamanio), buffer->stream + *desplazamiento, sizeof(int));
    *desplazamiento += sizeof(int);

    return fcb_kernel;
}

t_recurso* deserializar_recurso(t_buffer* buffer, int* desplazamiento)
{
    t_recurso* recurso = malloc(sizeof(t_recurso));

    int longitud_nombre;
    memcpy(&longitud_nombre, buffer->stream + *desplazamiento, sizeof(int));
    *desplazamiento += sizeof(int);

    recurso->nombre = malloc(longitud_nombre); 
    memcpy(&(recurso->nombre), buffer->stream + *desplazamiento, longitud_nombre);
    *desplazamiento += longitud_nombre;

    memcpy(&(recurso->cantidad), buffer->stream + *desplazamiento, sizeof(int));
    *desplazamiento += sizeof(int);

    memcpy(&(recurso->id_recurso_lista), buffer->stream + *desplazamiento, sizeof(int));
    *desplazamiento += sizeof(int);

    return recurso;
}


//LIBERACION
void liberar_instrucciones(t_list* instrucciones){

    for(int i = 0; i < list_size(instrucciones) - 1; i++){
        liberar_instruccion(list_get(instrucciones, i));
    }
    list_destroy(instrucciones);
}

void liberar_instruccion(t_instruccion *instruccion)
{
    for (int i = 0; i < instruccion->cantidad_parametros; i++)
    {
        free(instruccion->parametros[i]);
    }
    free(instruccion->parametros);
    free(instruccion);
}

