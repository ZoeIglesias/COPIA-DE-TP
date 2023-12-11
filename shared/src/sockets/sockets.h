#ifndef SOCKETS_H_
#define SOCKETS_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> 
#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include <readline/readline.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

typedef enum
{
    //CUANDO ESTA TODO PIOLITA
    OK,
    //PAGE_FAULT,
    MENSAJE_FILESYSTEM,
	MENSAJE_MEMORIA,
    CONTEXTO_EJECUCION,
    //INSTRUCCIONES CPU N°16
    SET,
    SUM,
    SUB,
    JNZ,
    SLEEP,
    WAIT,
    SIGNAL,
    MOV_IN,
    MOV_OUT,
    F_OPEN,
    F_CLOSE,
    F_READ,
    F_SEEK,
    F_WRITE,
    F_TRUNCATE,
    EXIT_OP,
    //MEMORIA (msj cpu)
    SOLICITUD_MARCO,
    SOLICITUD_INSTRUCCION,
    ESCRIBIR,
    LEER_VALOR,
    CREAR_PROCESO,
    CARGAR_PAGINA,
    LIBERAR_ESTRUCTURAS,
    INSTRUCCION,
    BLOQUE_SWAP,
    BLOQUEADO,
    EXISTE_ARCHIVO,
    APERTURA_FS,
	CREAR_ARCHIVO_FS, //especificas del file System
    LEER_FS,
    ESCRIBIR_FS,
    TRUNCATE_FS,
    INICIAR_PROCESO_FS,
    FINALIZAR_PROCESO_FS,
    ESCRIBIR_SWAP,
    LEER_SWAP,
    CERRAR_ARCHIVO,
    MENSAJE,
    PAQUETE ,
    ROMPEROMPEROMPE   
    
}codigo_operacion;

typedef enum{
    INICIAR_PROCESO,
    FINALIZAR_PROCESO,
    INICIAR_PLANIFICACION,
    DETENER_PLANIFICACION,
    MULTIPROGRAMACION,
    PROCESO_ESTADO,
    ERROR
}t_mensajes_consola;

typedef enum{
    SUCCESS,
    INVALID_WRITE,
    PAGE_FAULT,
    INVALID_RESOURCE,
    FIN_QUANTUM,
    NUEVA_PRIORIDAD,
    DEADLOCK,
    CONSOLA
}t_msj_error;


typedef enum{
    NEW,
    READY,
    EXIT,
    EXEC,
    BLOCKED
}t_estado;

typedef enum{
    R,//LECTURA
    W //ESCRITURA
}locks;

typedef struct {
    int size;
    void* stream;
} t_buffer;

typedef struct
{
	int tamanio;
	codigo_operacion codigo_operacion;
	t_buffer* buffer;
} t_paquete;



int crear_socket(struct addrinfo* server_info);
void paquete(int);


//cliente
int crear_conexion(char *ip, char* puerto,t_log* logger);
void liberar_conexion(int socket_cliente);

//servidor
int iniciar_servidor(char* puerto, t_log* logger);
int esperar_cliente(int,t_log*);
codigo_operacion recibir_operacion(int);
uint32_t recibir_mensaje(int);
char* recibir_mensaje_char(int);
//void enviar_mensaje(char* mensaje, int socket_cliente);

void* recibir_buffer(int*,int);
//void crear_buffer(t_paquete* paquete);
t_buffer *crear_buffer();
void liberar_buffer(t_buffer *buffer);


void* serializar_paquete(t_paquete* paquete, int bytes); 
t_list* recibir_paquete(int socket_cliente);
t_paquete* crear_paquete(int codigo);
void agregar_a_paquete(t_paquete* paquete, void* valor, int bytes);
void enviar_paquete(t_paquete* paquete, int socket_cliente); 
void enviar_mensaje(void* mensaje,int tamanio, int socket_cliente);
void eliminar_paquete(t_paquete* paquete);


#endif