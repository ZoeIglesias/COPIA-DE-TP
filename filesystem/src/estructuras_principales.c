#include "../include/estructuras_principales.h"
t_FAT* fat;
void* bloques;

void iniciarFAT(){
    int cantEntradas = cant_bloques_total() - cant_bloques_swap();

    fat = malloc((cantEntradas) * sizeof(uint32_t));

    for (int i = 0; i < (cantEntradas); i++) {
        fat[i] = (uint32_t) 0;
    }

    char* path = path_fat();

    FILE *archivoFAT = fopen(path, "wb");
    if(archivoFAT==NULL){
        return;
    }
    fwrite(fat, sizeof(uint32_t), cantEntradas, archivoFAT);

    log_info(filesystem, "escribe archivo");

    fclose(archivoFAT);
    free(fat);
}

int levantarFAT(){
    char* path = path_fat();
    FILE *archivoFAT = fopen(path, "rb");
    int cantEntradas = cant_bloques_total() - cant_bloques_swap();
    fat = malloc((cantEntradas) * sizeof(uint32_t));

    if(archivoFAT == NULL){
        free(fat);
        return 0;
    }
    else{
        int cantEntradas = cant_bloques_total() - cant_bloques_swap();
        fread(fat, sizeof(uint32_t), cantEntradas, archivoFAT);
    }
    
    fclose(archivoFAT);
    return 1;
}

void actualizarFAT(){
    FILE *archivoFAT = fopen(path_fat(), "wb");
    int cantEntradas = cant_bloques_total() - cant_bloques_swap();
    fwrite(fat, sizeof(uint32_t), cantEntradas, archivoFAT);
    fclose(archivoFAT);
    return;
}


void liberarFAT() {
    free(fat);
}

/*t_FCB* buscarFCB(char* nombreArchivo){
    t_FCB* fcb == NULL;
    t_list_iterator* iteradorFcbs = list_iterator_create(fcbs);   
    int indice = iteradorFcbs->index;

    while(list_iterator_has_next(iteradorFcbs)){
        fcb = list_iterator_next(iteradorFcbs);    
        indice = iteradorFcbs->index;    
        if(strcmp(fcb->NOMBRE_ARCHIVO, nombre_archivo) == 0){
            list_iterator_destroy(iteradorFcbs);
            return fcb;
        }
                    
    }
    list_iterator_destroy(iteradorFcbs);
    
    if(indice != -1 && strcmp(fcb->NOMBRE_ARCHIVO, nombre_archivo) == 0)
        return fcb;
    return NULL;
}*/

t_FCB* buscarFCB(char* nombreArchivo){
    t_FCB* fcb = malloc(sizeof(t_FCB));
    char * extencion = "fcb";
    char *path = string_new();
    string_append_with_format(&path, "%s/%s.%s", path_fcb(), nombreArchivo, extencion);
    t_config *archivoFCB = config_create(path);
    if(archivoFCB == NULL){
        //no se encontro el archivo
        config_destroy(archivoFCB);
        free(fcb);
        return NULL;
    }
    else{
        fcb->NOMBRE_ARCHIVO = config_get_string_value(archivoFCB, "NOMBRE_ARCHIVO");
        fcb->TAMANIO_ARCHIVO =  config_get_int_value(archivoFCB, "TAMANIO_ARCHIVO");
        fcb->BLOQUE_INICIAL =  config_get_int_value(archivoFCB, "BLOQUE_INICIAL");
        config_destroy(archivoFCB);
        list_add(fcbs, fcb);
        free(fcb);
        return fcb;
    }
}

void iniciarBLOQUES(){
    char* path = path_bloques();
    int cant_bloques = cant_bloques_total();
    int cant_bloques_swapp = cant_bloques_swap();
    int tamanio_bloque = tam_bloque();
    bloques = malloc(cant_bloques * tamanio_bloque * sizeof(char));

    for (int i = 0; i < (cant_bloques_swapp); i++) {
        memset(bloques+i*tamanio_bloque*sizeof(char),'\244',sizeof(char));
    }

    FILE *archivoBloques = fopen(path, "wb");
    if(archivoBloques==NULL){
        free(bloques);
        return;
    }
    fwrite(bloques, tamanio_bloque*sizeof(char), cant_bloques, archivoBloques);

    fclose(archivoBloques);
    free(bloques);
}

int levantarBLOQUES(){
    int cant_bloques = cant_bloques_total();
    int tamanio_bloque = tam_bloque();
    bloques = malloc(cant_bloques * tamanio_bloque * sizeof(char));
    char* path = path_bloques();
    FILE *archivoBloques = fopen(path, "rb");

    if(archivoBloques == NULL){
        //no se encontro el archivo
        free(bloques);
        return 0;
    }
    else{
        
        fread(bloques, sizeof(char)*tamanio_bloque, cant_bloques, archivoBloques);
        fclose(archivoBloques);
    }
    return 1;
}
void actualizarBLOQUES(){
    int cant_bloques = cant_bloques_total();

    FILE *archivoBloques = fopen(path_bloques(), "wb");

    if(archivoBloques == NULL){
        //no se encontro el archivo
        fclose(archivoBloques);
    }
    else{
        int tamanio_bloque = tam_bloque();
        fwrite(bloques, tamanio_bloque, cant_bloques, archivoBloques);
        fclose(archivoBloques);
    }
}

//a evualuar el tema del sizeof
void liberarBloques() {

    free(bloques);

}

//int abrir_archivo(char* nombreArchivo, )

void log_crear_archivo(char* nombre){
    log_info(logger_fs, "Crear Archivo: %s",nombre);
}

void log_abrir_archivo(char* nombre){
    log_info(logger_fs, "Abrir Archivo: %s",nombre);
}

void log_truncar_archivo(char* nombre, int tamanio){
    log_info(logger_fs, "Truncar Archivo: %s - Tamaño: %d",nombre,tamanio);
}

void log_leer_archivo(char* nombre, int puntero, int direccion){
    log_info(logger_fs, "Leer Archivo: %s - Puntero: %d - Memoria: %d",nombre,puntero,direccion);
}

void log_escribir_archivo(char* nombre, int puntero, int direccion){
    log_info(logger_fs, "Escribir Archivo: %s - Puntero: %d - Memoria: %d",nombre,puntero,direccion);
}

void log_acceso_FAT(int nro_entrada, uint32_t valor){
    usleep(retardo_acceso_fat()*1000);
    log_info(logger_fs, "Acceso FAT - Entrada: %d - Valor: %d",nro_entrada, valor);
}

void log_acceso_Bloque_Archivo(char* nombre,int nro_bloque_arch, int nro_bloque_fs){
    usleep(retardo_acceso_bloque()*1000);
    log_info(logger_fs, "Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque FS: %d",nombre,nro_bloque_arch,nro_bloque_fs);
}

void log_acceso_Bloque_SWAP(int nro_bloque){
    usleep(retardo_acceso_bloque()*1000);
    log_info(logger_fs, "Acceso SWAP: %d",nro_bloque);
}

