#include "../include/filesystem.h"


//t_log* logger_fs;
//t_config* config_fs;

int main(int argc, char** argv) {
    
    logger_fs = log_create("../filesystem/filesystem.log","FILESYSTEM",true,LOG_LEVEL_DEBUG);

    filesystem = log_create("../filesystem/filesystemNoObligatorio.log","FILESYSTEM-NOOB", true, LOG_LEVEL_DEBUG); //este es el logger para los no obligatorios

    config_fs = crear_config(argv[1]);
    iniciar_semaforos();
    while(!levantarFAT()){
        iniciarFAT();
    }//SI ROMPE FS COMENTA ESTO
    //levantarBLOQUES(); SE EMPIEZA A PROBAR CON LAS PRUEBAS
    while(!levantarBLOQUES()){
        iniciarBLOQUES();
    }
   
    //1.) Creo conexion con el Memoria como cliente
    conectar_fs_memoria(ip_memoria(),puerto_memoria());

    //2.) Creo el server de filesystem
    int server_filesystem = iniciar_servidor(puerto_escucha(), filesystem);
    log_info(filesystem, "INICIE SV FS");
    //3.) Espero que se conecten los modulos MEMORIA y KERNEL respectivamente al server.
    conectar_cliente_memoria(server_filesystem);
    conectar_kernel(server_filesystem);

    //cambiar a futuro
    //esta mal hecho pero funciona asi que ta bien
    sem_t a;
    sem_init(&a, 0, 0);
    sem_wait(&a);


    //liberar_programa();
    
    return 0;
}

void liberar_programa(){
    log_destroy(logger_fs);
    config_destroy(config_fs);
}