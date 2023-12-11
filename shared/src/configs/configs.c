#include "configs.h"


t_config* iniciar_config(char* config_path){

	t_config* nuevo_config =  config_create(config_path);

	if(nuevo_config == NULL){
		printf("No se pudo iniciar la config");
		exit(3);
	}


    return nuevo_config;
}

