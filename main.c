#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include "map.c"
#include "list.c"
#include "treemap.c"

typedef struct{
    char * nombre;
    char * fecha;
    int valoracion;
    int precio;
}Game;

const char *get_csv_field(char * tmp, int k){ 
    int open_mark = 0;
    char *ret = (char *)malloc(100 * sizeof(char));
    int ini_i = 0, i = 0;
    int j = 0;
    while (tmp[i + 1] != '\0'){
        if (tmp[i] == '\"'){
            open_mark = 1 - open_mark;
            if (open_mark) ini_i = i + 1;
            i++;
            continue;
        }
        if (open_mark || tmp[i] != ','){
            if (k == j) ret[i - ini_i] = tmp[i];
            i++;
            continue;
        }
        if (tmp[i] == ','){
            if (k == j){
                ret[i - ini_i] = 0;
                return ret;
            }
            j++;
            ini_i = i + 1;
        }
        i++;
    }
    if (k == j){
        ret[i - ini_i] = 0;
        return ret;
    }
    return NULL;
}

int is_equal_str(void *key1, void *key2){
    if (strcmp((char*)key1, (char*)key2) == 0) return 1;
    return 0;
}

int lower_than_str(void *key1, void *key2){
    if (strcmp((char*)key1, (char*)key2) < 0) return 1;
    return 0;
}

int lower_than_int(void *key1, void *key2){
    if (*(int *)key1 < *(int *)key2) return 1;
    return 0;
}

void siuu(char *s){
    while (*s && *s != '\n' && *s != '\r') s++;
    *s= 0;
}

Game * crearJuego(){
    Game * new= (Game *)malloc(sizeof(Game));
    new->nombre= (char *)calloc(100, sizeof(char)); 
    new->fecha= (char *)calloc(100, sizeof(char));
    new->valoracion= 0;
    new->precio= 0;
    return new;
}

void agregarJuego(Map * mapa, TreeMap * valor, TreeMap * prices){
    int * keyval = 0;
    int * keypre = 0;
    Game * new= crearJuego();

    fflush(stdin);
    printf("Ingrese nombre del Juego: \n");
    fgets(new->nombre,100,stdin);
    siuu(new->nombre);
    printf("\n");

    printf("Ingrese fecha de salida: \n");
    fgets(new->fecha,100,stdin);
    siuu(new->fecha);
    printf("\n");

    if (searchMap(mapa, new->nombre) != NULL){
        if (searchMap(mapa, new->fecha) != NULL){
            printf("Este juego ya existe\n");
            free(new);
            return;
        }
    }
    fflush(stdin);
    printf("Ingrese valoracion del juego: \n");
    scanf("%ld",&new->valoracion);
    printf("\n");

    printf("Ingrese precio del juego: \n");
    scanf("%ld",&new->precio);
    printf("\n\n");

    insertMap(mapa, new->nombre, new);
    keyval = &new->valoracion;
    keypre = &new->precio;
    insertMap(mapa, new->nombre, new);
    insertTreeMap(valor, keyval, new);
    insertTreeMap(prices, keypre, new);
}

void importarCSV(Map *mapaNombre, TreeMap * valor, TreeMap * prices){
    char nombreArchivo[100];
    printf("Ingrese nombre del archivo: \n");
    fflush(stdin);
    fgets(nombreArchivo, 100, stdin);
    siuu(nombreArchivo);
    printf("\n");

    FILE * fp= fopen(nombreArchivo, "r");
    if (fp == NULL){
        printf("Archivo no encontrado\n\n");
        return;
    }
    char linea[1024];
    fgets(linea,1024,fp);

    while (fgets(linea, 1023, fp) != NULL){
        int * keyval=0;
        int * keypre=0;
        Game * new= crearJuego();

        strcpy(new->nombre, get_csv_field(linea, 0));
        siuu(new->nombre);
        strcpy(new->fecha, get_csv_field(linea, 1));
        siuu(new->fecha);
        new->valoracion= atoi(get_csv_field(linea, 2));
        new->precio= atoi(get_csv_field(linea, 3));
        keyval= &new->valoracion;
        keypre= &new->precio;
        insertMap(mapaNombre, new->nombre, new);
        insertTreeMap(valor, keyval, new);
        insertTreeMap(prices, keypre, new); 
    }
    printf("Archivo Importado Correctamente\n\n");
    fclose(fp);
}

void mostrarJuego(Game * juego){
    printf("Nombre: %s\nFecha de Salida: %s\n", juego->nombre, juego->fecha);
    printf("Valoracion: %ld\nPrecio: %ld\n\n", juego->valoracion, juego->precio);
}

void mostrarPorPrecioLtH(TreeMap * prices){
    Pair * aux= firstTreeMap(prices);
    if (aux == NULL){
        printf("No hay Juegos");
        return;
    }
    Game * juego= aux->value;
    while (aux != NULL){
        juego= aux->value;
        mostrarJuego(juego);
        aux= nextTreeMap(prices);
    }
    free(aux);
    printf("\n\n");
}

void mostrarPorPrecioHtL(TreeMap * prices){
    Pair * aux= lastTreeMap(prices);
    if (aux == NULL){
        printf("No hay Juegos");
        return;
    }
    Game * juego= aux->value;
    while (aux != NULL){
        juego= aux->value;
        mostrarJuego(juego);
        aux= prevTreeMap(prices);
    }
    free(aux);
    printf("\n\n");
}

void mostrarPorValoracion(int valora, TreeMap * valor){
    Pair * aux= lastTreeMap(valor);
    if (aux == NULL){
        printf("No hay Juegos\n\n");
        return;
    }
    Game * juego= aux->value;
    while (aux != NULL){
        if (!juego->valoracion > valora){
            printf("No hay Juegos de esta valorizacion o mas\n\n");
            return;
        }
        mostrarJuego(juego);
        aux= prevTreeMap(valor);
        juego= aux->value;
    }  
    printf("\n\n");
}

void mostrarPorAnyo(int anyo, TreeMap * valor){
    Pair *aux = firstTreeMap(valor);
    Game *aux1 = aux->value;
    int year, x= 0, inicio= 6, final= 4;
    while (aux1 != NULL){
        char cyear[10];
        strncpy(cyear, aux1->fecha + inicio, final);
        year= atoi(cyear);
        if  (year == anyo){
            mostrarJuego(aux1);
            x++;
        }
        aux= nextTreeMap(valor);
        aux1= aux->value;
    }
    if (!x) printf("No hay Juegos de este ano: %ld\n\n", anyo);
    else printf("\n\n");
}

void BuscarJuego(Map * mapa){
    char key[100];
    printf("Ingrese Nombre del juego a buscar: \n");
    fflush(stdin);
    fgets(key, 100, stdin);
    siuu(key);
    printf("\n\n");

    Game * aux= searchMap(mapa, key);

    if (aux == NULL){
        printf("Juego no encontrado\n\n");
        return;
    }
    mostrarJuego(aux);
    printf("\n\n");
}

void exportarCSV(Map * mapa){
    char nombreArchivo[100];
    printf("Ingrese nombre del archivo: \n");
    fflush(stdin);
    fgets(nombreArchivo, 100, stdin);
    siuu(nombreArchivo);
    printf("\n");

    FILE * fp= fopen(nombreArchivo, "w");
    Game * gameexp;
    gameexp= firstMap(mapa);
    fputs("Nombre,ano de salida,valoracion,precio", fp);
    fputc(',', fp);
    while (gameexp != NULL){
        fputs(gameexp->nombre, fp);
        fputc(',', fp);
        fputs(gameexp->fecha, fp);
        fputc(',', fp);
        fprintf(fp, "%ld", gameexp->valoracion);
        fputc(',', fp);
        fprintf(fp, "%ld", gameexp->precio);
        fputc(',', fp);
        gameexp= nextMap(mapa);
    }
    fclose(fp);
}

int main(){
    Map * mapa= createMap(is_equal_str);
    TreeMap *valor = createTreeMap(lower_than_int);
    TreeMap *prices = createTreeMap(lower_than_int);

    int p= 0, op= -1, anyo, valora;
    while (op != 0){
        printf("\nSeleccione una opcion\n\n");
        while (1){
            printf("1. Importar videojuegos desde un archivo\n");
            printf("2. Agregar videojuego\n");
			printf("3. Mostrar juegos por precio\n");
			printf("4. Filtrar Juegos por Valoracion\n");
			printf("5. Mostrar Juegos del Anyo\n");
			printf("6. Buscar Juego\n");
			printf("7. Exportar base de datos actualizada\n");
			printf("8. Salir\n");
			scanf("%d",&op);
            if (op <= 10 && op >= 1) break;
        }
    switch(op){
			case 1:
                importarCSV(mapa, valor, prices);
                break;
			case 2:
                agregarJuego(mapa, valor, prices);
                break;
			case 3:
                printf("Seleccione una opcion\n");
                printf("1. Mostrar de mayor a menor\n");
                printf("2. Mostrar de menor a mayor\n");
                scanf("%d", &p);
                if (p == 1) mostrarPorPrecioHtL(prices);
                else if (p == 2) mostrarPorPrecioLtH(prices);
                break;
            case 4:
                printf("Ingrese valoracion a buscar: \n");
                scanf("%d", &valora);
                printf("\n\n");
                mostrarPorValoracion(valora, valor);
                printf("SIUUUUUUUUUUUUU: \n");
                printf("SIUUUUUUUUUUUUU: \n");
                printf("SIUUUUUUUUUUUUU: \n");
                printf("SIUUUUUUUUUUUUU: \n");
                printf("SIUUUUUUUUUUUUU: \n");
                break;
			case 5:
                printf("Ingrese anyo a buscar: \n");
                scanf("%d", &anyo);
                printf("\n\n");
                mostrarPorAnyo(anyo, valor);
                break;
			case 6:
                BuscarJuego(mapa);
                break;
			case 7:
                exportarCSV(mapa);
                break;
			case 8:
                exit(-1);
		}
	}
    fflush(stdin);
    getchar();
    return 0;
}