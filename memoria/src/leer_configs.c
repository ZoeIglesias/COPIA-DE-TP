#include "../include/leer_configs.h"

t_config *crear_config(char* config_path){
    return config_create(config_path);
}

void destruir_config(){
    config_destroy(config_memoria);
}

// ------------------------------------------------
//  Datos para el config
// ------------------------------------------------

char *puerto_escucha(){
    return config_get_string_value(config_memoria, "PUERTO_ESCUCHA");
}

char *ip_filesystem(){
    return config_get_string_value(config_memoria, "IP_FILESYSTEM");
}

char *puerto_filesystem(){
    return config_get_string_value(config_memoria, "PUERTO_FILESYSTEM");
} 

int tam_memoria(){
    return config_get_int_value(config_memoria, "TAM_MEMORIA");
}

int tam_pagina(){
    return config_get_int_value(config_memoria, "TAM_PAGINA");
}

char *path_instrucciones(){
     return config_get_string_value(config_memoria, "PATH_INSTRUCCIONES");
}

int  retardo_respuesta(){
       return config_get_int_value(config_memoria, "RETARDO_RESPUESTA");
}

char *algoritmo_reemplazo(){
    return config_get_string_value(config_memoria, "ALGORITMO_REEMPLAZO");
}

