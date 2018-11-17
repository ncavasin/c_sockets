#include <string.h>

#define BD_LEN 12 		//tamaño maximo del nombre de la bd
#define QUERY_LEN 2048	//tamaño maximo de la query

int getquerymaxlen(){
	return QUERY_LEN;
}

int getbdmaxlen(){
	return BD_LEN;
}

/** Recibe puntero a char y parsea para separar nombre de la BD del query.
 * @param input    [puntero al string que ingreso el usuario]
 * @param database [puntero a char que almacenara el nombre de la BD]
 * @param query    [puntero a char que almacenar el query del usuario]
 * @return			-1 si hubo error, 0 si no encontro los ':', 1 si hubo exito
 */
int split(char input[], char database[], char query[]){

	// Almacena la posicion del ':' en el input
	int offset;

	// Almacena el tamaño del input
	int len = strlen(input);

	// Verifico que no sea un string vacio
	if((input[0] == '\0') || (input[0] == ' ')){
		return -1;
	}

	// Busco el caracter : que separa la BD de la query
	offset = 0;
	while(*input++ != ':' && offset <= strlen(input)){
		offset++;
	}
	
	// Verifico si se encontraron los ':'
	// Permite identificar cuando el usuario desea cerrar el Switch
	if(offset == 0){
		return 0;
	}

	// Verifico longitudes de BD y QUERY
	if((offset-1) > BD_LEN) || ((len-(offset+1)) > QUERY_LEN){
		// Salgo con error
		return -1;
	}

	// Inicializo los strings para eliminar la basura
	memset(&database[0], '\0', strlen(database));
	memset(&database[0], '\0', strlen(query));
	
	// Asigno el resultado del parsing
	strncpy(database, input, offset-1 );
	strncpy(query, input, offset+1);

	// Salgo con exito
	return 1;
}

int 

int execute_query(char query[]){

	// Funcion para solicitar el catalogo de todas las BDs
	// Funcion para almacenar el catalogo de todas las BDs en un archivo csv

	// Funcion para determinar a que motor (IP+Port) corresponde la query 
	// Invocar socket cliente con la query como argumento
	// Devolver respuesta a app_cli
}


int main(){




	return 1;


}


