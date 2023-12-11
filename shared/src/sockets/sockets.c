#include "sockets.h"

struct addrinfo *addrinfo_servidor(char *ip, char *puerto){
	
	struct addrinfo hints;
	struct addrinfo *servinfo;
	
	memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
	
	getaddrinfo(ip, puerto, &hints, &servinfo);

	return servinfo;
}

int crear_socket(struct addrinfo* servinfo){

    int socket_nuevo = socket(servinfo->ai_family, 
        servinfo->ai_socktype,
        servinfo->ai_protocol);


    return socket_nuevo;
}

int crear_conexion(char *ip, char* puerto, t_log* logger)
{
	struct addrinfo *servinfo = addrinfo_servidor(ip, puerto);

	//creamos el socket
    int socket_cliente = crear_socket(servinfo);

	setsockopt(socket_cliente, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
	
    //conectamos el socket 
	//cuando no se crea la conexion
    if(connect(socket_cliente,servinfo->ai_addr,servinfo->ai_addrlen) == -1){
        log_error(logger,"No fue exitosa la conexion con el cliente %d.", socket_cliente);
        abort();
    }

    log_info(logger,"Conexion Exitosa");
    
    //liberar memoria en servidor
    freeaddrinfo(servinfo);
   
    return socket_cliente;
}


int iniciar_servidor(char* puerto, t_log* logger)
{	
	int socket_servidor;

	struct addrinfo *servinfo = addrinfo_servidor(NULL, puerto);
	
	socket_servidor = crear_socket(servinfo);
	
	if (socket_servidor == -1) {
        perror("Error al crear el socket:");
        abort();
    }
	
	//nos permite controlar el comportamiento del socket para las conexiones
	setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

	bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);


	if (listen(socket_servidor, SOMAXCONN) == 0){
		log_info(logger, "Listo para escuchar al cliente");
	}else{
		log_error(logger,"Ocurrio un error para escuchar nuevos clientes");
		abort();
	}

	freeaddrinfo(servinfo);	
	
    return socket_servidor;
}


int esperar_cliente(int socket_servidor, t_log* logger)
{
	int socket_cliente;

	socket_cliente = accept(socket_servidor, NULL, NULL);

	if (socket_cliente != -1){
		log_info(logger, "Se conecto un cliente!");
	}else{
		log_error(logger,"Ocurrio un error para conectar un cliente :(");
		abort();
	}
	
	//te retorna el socket del cliente que se logro conectar
	return socket_cliente;
}

codigo_operacion recibir_operacion(int socket_cliente)
{
	codigo_operacion cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(codigo_operacion), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}


uint32_t recibir_mensaje(int socket_cliente)
{
	int size;
	uint32_t valor_leido;
	void* buffer = recibir_buffer(&size, socket_cliente);
	memcpy(&valor_leido, buffer, sizeof(uint32_t));
	free(buffer);
	return valor_leido;
}
char* recibir_mensaje_char(int socket_cliente)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	return buffer;
}
/*void enviar_mensaje(char* mensaje, int socket_cliente)
{
    t_paquete* paquete = malloc(sizeof(t_paquete));


    paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(mensaje) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}*/
void enviar_mensaje(void* mensaje,int tamanio, int socket_cliente)
{
	t_paquete *paquete = crear_paquete(MENSAJE);
	agregar_a_paquete(paquete,mensaje,tamanio);
	enviar_paquete(paquete,socket_cliente);

}

t_list* recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}


void* serializar_paquete(t_paquete* paquete, int bytes)
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}

t_paquete* crear_paquete(int codigo)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = codigo;
	
	paquete->buffer = crear_buffer();
	
	return paquete;
}

void enviar_paquete(t_paquete* paquete, int socket_cliente)
{
	int bytes = paquete->buffer->size + 2*sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}

void eliminar_paquete(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void agregar_a_paquete(t_paquete* paquete, void* valor, int bytes)
{
	t_buffer *buffer = paquete->buffer;
	buffer->stream = realloc(buffer->stream, buffer->size + bytes);
	memcpy(buffer->stream + buffer->size, valor, bytes);
	buffer->size += bytes;
}

void liberar_buffer(t_buffer *buffer)
{
	free(buffer->stream);
	free(buffer);
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}

/*void crear_buffer(t_paquete* paquete)
{
    
    paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
	
}*/

t_buffer *crear_buffer()
{
	t_buffer *buffer = malloc(sizeof(t_buffer));
	
	assert(buffer != NULL);

	buffer->size = 0;
	
	buffer->stream = NULL;
	
	return buffer;
}

void* recibir_buffer(int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	
	buffer = malloc(*size);
	
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

