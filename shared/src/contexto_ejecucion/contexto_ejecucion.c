#include "contexto_ejecucion.h"

//COMUNICACION//
void enviar_contexto(t_contexto_ejecucion *ctx, int socket,codigo_operacion cod_op)
{
    t_paquete *paquete = crear_paquete(cod_op);
    
    serializar_contexto(paquete,ctx);
    
    enviar_paquete(paquete,socket);
}

t_contexto_ejecucion *recibir_contexto(int socket)
{
    t_buffer *buffer = crear_buffer();
    
    buffer->stream = recibir_buffer(&(buffer->size), socket);

    t_contexto_ejecucion *ctx = deserializar_contexto(buffer);
    
    liberar_buffer(buffer);

    return ctx;
}

//LIBERACION//
void liberar_contexto(t_contexto_ejecucion* contexto_ejecucion){

    list_destroy_and_destroy_elements(contexto_ejecucion->instrucciones, (void*) liberar_instrucciones);
    //por ahora libero esto, no se si falten liberar mas cosas
	free(contexto_ejecucion);

}

