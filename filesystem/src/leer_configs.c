#include "../include/leer_configs.h"

t_config* crear_config(char* config_path){
    return config_create(config_path);
}

void destruir_config(){
    config_destroy(config_fs);
}

char *ip_memoria(){
    return config_get_string_value(config_fs, "IP_MEMORIA");
} 

char *puerto_memoria(){
    return config_get_string_value(config_fs, "PUERTO_MEMORIA");
} 

char *puerto_escucha(){
    return config_get_string_value(config_fs, "PUERTO_ESCUCHA");
} 

char *path_fat(){
    return config_get_string_value(config_fs, "PATH_FAT");
}

char *path_bloques(){
    return config_get_string_value(config_fs, "PATH_BLOQUES");
}

char *path_fcb(){
    return config_get_string_value(config_fs, "PATH_FCB");
}

int cant_bloques_total(){
    return config_get_int_value(config_fs, "CANT_BLOQUES_TOTAL");
}

int cant_bloques_swap(){
    return config_get_int_value(config_fs, "CANT_BLOQUES_TOTAL");
}

int tam_bloque(){
    return config_get_int_value(config_fs, "TAM_BLOQUE");
}

int retardo_acceso_bloque(){
    return config_get_int_value(config_fs, "RETARDO_ACCESO_BLOQUE");
}

int retardo_acceso_fat(){
    return config_get_int_value(config_fs, "RETARDO_ACCESO_FAT");
}