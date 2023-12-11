#ifndef PROTOCOLO_COMUNICACION_H_
#define PROTOCOLO_COMUNICACION_H_


#include <stdio.h>
#include <stdlib.h>

//NUESTRAS
#include "sockets/sockets.h"
#include "configs/configs.h"
#include "../../kernel/include/variables_globales_kernel.h"
#include "../estructuras_compartidas/estructuras_compartidas.h"



//FUNCIONES
int verificar_cliente(int socket_cliente, t_log *logger);
void realizar_handshake(int id, int server);
void liberar_instrucciones(t_list* instrucciones);
void liberar_instruccion(t_instruccion *instruccion);


// ------------------------
//  PARA ENVIO DE PAQUETES
// ------------------------
void serializar_instrucciones(t_list *instrucciones, t_paquete *paquete);
void serializar_instruccion(t_instruccion *instruccion, t_paquete *paquete);
void serializar_contexto(t_paquete *paquete,t_contexto_ejecucion *ctx);
void serializar_pcb(t_paquete *paquete, t_pcb *pcb_a_serializar);
void serializar_lista_recursos_asignados(t_paquete *paquete, t_list *recursos_asignados);
void serilizar_recurso(t_paquete *paquete, t_recurso *recurso);
void serializar_archivos_proceso(t_paquete *paquete, t_archivos_proceso *archivos_proceso);
void serializar_fcb(t_paquete *paquete, t_fcb_kernel *fcb_kernel);



t_instruccion *deserializar_instruccion(t_buffer *buffer, int *desplazamiento);
t_instruccion *deserializar_instruccion_solicitada(t_buffer *buffer);
t_registros* deserializar_registros(t_buffer *buffer, int *desplazamiento);
t_contexto_ejecucion *deserializar_contexto(t_buffer *buffer);
t_contexto_ejecucion *deserializar_contexto_con_desplazamiento(t_buffer *buffer, int *desplazamiento);
t_pcb* deserializar_pcb_con_desplazamiento(t_buffer *buffer,int *desplazamiento);
t_recurso* deserializar_recurso(t_buffer* buffer, int* desplazamiento);
t_archivos_proceso *deserializar_archivos_proceso(t_buffer *buffer, int *desplazamiento);
t_fcb_kernel *deserializar_fcb_kernel(t_buffer *buffer, int *desplazamiento);



#endif