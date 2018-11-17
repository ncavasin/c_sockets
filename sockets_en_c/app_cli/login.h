#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

//tamaños de usuario y contraseña
#define MIN_LEN 4
#define MAX_LEN 25

//ubicacion del archivo que almacena usuarios y contraseñas
#define FILE_PATH "sec.dat"

//estructura utilizada para escribir/leer el archivo "users.dat"
struct log{
	char username[MAX_LEN];
	char password[MAX_LEN];
	int activo; 	// 1 = activo | -1 = inactivo
};

//utilizado para chequear valores
int ret_val;

int getmaxlen(){
	return MAX_LEN;
}

int getminlen(){
	return MIN_LEN;
}

/**Abre un archivo validando errores
 * @return [puntero al archivo]
 */
FILE *my_open(){

	//puntero al archivo
	FILE *fd;

	//abre el archivo en modo binario A y R. Si no existe lo crea. 
	fd = fopen(FILE_PATH, "ab+"); //permiso A y R del usuario
	
	if(fd == NULL){
		perror("open");
	}

	return fd;
}

/**Compara dos strings recibidas en punteros a char
 * @param  s1 [puntero a char de un string]
 * @param  s2 [puntero a char de otro string]
 * @return    [1 si son iguales, -1 si son distintas]
 */
int my_strcmp(char *s1, char *s2){

	//tamaño de string
	int len;

	//valor a retornar
	int value;

	//asumo que las strings van a ser iguales
	value = 1;

	//verifico sus longitudes
	if(strlen(s1) != strlen(s2)){
		
		//indico que son distintas		
		value = -1;

	}
	else{ 	//son iguales
		
		len = strlen(s1);

		//comparo caracter a caracter
		for(int i = 0; i < len; i++){

			if(s1[i] != s2[i]){
				
				//indico que son distintas
				value = -1;
				break;
			}
		}
	}

	return value;
}

/**Verifica la existencia de un usuario en el archivo recibido. Si existe, devuelve su posicion.
 * @param  fd   [puntero FILE al archivo]
 * @param  user [puntero char al nombre de usuario]
 * @return      [1 si existe, -1 si no existe]
 */
int verif_user(FILE *fd, char *user, unsigned long *pos){
	
	//registro para leer el archivo
	struct log r;

	//inicializo
	pos = NULL;

	//verifico que el puntero no sea nulo
	if(fd){

		//me posiciono al principio del archivo 
		rewind(fd);

		//mientras que no llegue a EOF
		while( fread(&r,sizeof(r),1,fd) > 0 ){

			if(r.activo == 1){	//si el registro esta activo

				//verifico coincidencia
				if( strcmp(user, &r.username[0]) == 0){

					//guardo la posicion
					ret_val = fseek(fd, -1 , SEEK_CUR);
					if (ret_val != 0){

						//indico error
						return 0;
					}

					*pos = ftell(fd);

					if (*pos == -1){

						//indoc error
						return 0;
					}

					fprintf(stdout, "Valor de pos: %p\n", pos);

					//indico que existe
					return 1;
				}
			}
		}
	}
	
	//indico que no existe	
	return -1;
}

/**Crea el usuario recibido.
 * @return [1 indica exito, -1 indica usuario existente]
 */
int creuser(char *user, char *pass){

	//file descriptor
	FILE *fd;

	//registro a escribir en el archivo
	struct log r;

	//abre el archivo en modo A y R. Si no existe lo crea. 
	fd = my_open();

	if(fd == NULL){

		//indico que hubo un error
		return 0;
	}

	//verifico existencia
	if(verif_user(fd, user, NULL) == 1){ 	//  1 == existe
				
		//indico que existe
		return -1;
	}

	//inicializo y asigno el registro r
	memset(&r, '\0', sizeof(r));
	strcpy(r.username, user); 
	strcpy(r.password, pass);
	r.activo = 1;

	//me posiciono al final del archivo
	fseek(fd, 0, SEEK_END);

	//escribo el registro
	ret_val = fwrite(&r, sizeof(r), 1, fd);
	if(ret_val <= sizeof(r)){

		fclose(fd);
		//indico que hubo un error
		return 0;
	}

	//cierro el archivo
	fclose(fd);

	//salgo con exito
	return 1;
}

/**Cambia la contraseña del usuario recibido.
 * @param  user    [puntero char del nombre de usuario a actualizar]
 * @param  newpass [puntero a char de la nueva contraseña]
 * @return         [1 si exito, 0 si hubo error, -1 si no existia el usuario]
 */
int updusr(char *user, char *newpass){

	//puntero al archivo
	FILE *fd;

	//almacena la posicion del registro a eliminar
	unsigned long usrpos;

	//utilizado para leer el archivo
	struct log r;

	//abro el archivo
	fd = my_open();

	if (fd == NULL){

		//indico que hubo un error
		return 0;
	}

	//busco el registro
	ret_val = verif_user(fd, user, &usrpos);
	if ( ret_val == 0 ){

		fclose(fd);

		//indico que hubo un error
		return 0;
	}

	if(ret_val == -1){

		fclose(fd);

		//indico que el usuario a actualizar no existe
		return -1;		
	}

	//me posiciono sobre el registro del usuario a actualizar
	rewind(fd);
	fseek(fd, usrpos, SEEK_CUR);

	//leo el registro
	ret_val = fread(&r, sizeof(r), 1, fd);
	if(ret_val < sizeof(r)){

		fclose(fd);

		//indico error
		return 0;
	}
	
	//retrocedo el puntero
	fseek(fd, -1 * sizeof(r), SEEK_CUR);

	//actualizo la contraseña
	strcpy(r.password, newpass);

	//guardo la eliminacion
	ret_val = fwrite(&r, sizeof(1), 1, fd);
	if(ret_val < sizeof(r)){

		fclose(fd);

		//indico error
		return 0;
	}

	fclose(fd);

	//indico que el usuario ha sido eliminado con exito
	return 1;
}

/**Elimina el usuario recibido.
 * @param  user [puntero a char del nombre de usuario a eliminar]
 * @return      [1 si elimino, 0 si hubo error, -1 si no existe el usuario]
 */
int remuser(char *user){

	//puntero al archivo
	FILE *fd;

	//almacena la posicion del registro a eliminar
	unsigned long usrpos;

	//utilizado para leer el archivo
	struct log r;

	//abro el archivo
	fd = my_open();
	if (fd == NULL){

		//indico que hubo un error
		return 0;
	}

	//busco el registro
	ret_val = verif_user(fd, user, &usrpos);
	if ( ret_val == 0 ){

		fclose(fd);

		//indico que hubo un error
		return 0;
	}

	if(ret_val == -1){

		fclose(fd);

		//indico que el usuario a eliminar no existe
		return -1;		
	}

	//me posiciono sobre el registro del usuario a eliminar
	rewind(fd);
	fseek(fd, usrpos, SEEK_CUR);

	//leo el registro
	ret_val = fread(&r, sizeof(r), 1, fd);
	if(ret_val < sizeof(r)){

		fclose(fd);

		//indico error
		return 0;
	}

	//retrocedo el puntero
	fseek(fd, -1, SEEK_CUR);

	//lo doy de baja
	r.activo = -1;

	//guardo la eliminacion
	ret_val = fwrite(&r, sizeof(1), 1, fd);
	if(ret_val < sizeof(r)){

		fclose(fd);

		//indico error
		return 0;
	}

	fclose(fd);

	//indico que el usuario ha sido eliminado con exito
	return 1;
}

/**Solicita credenciales al usuario recibido.
 * @return [1 si son validas,-1 si son invalidas]
 */
int log_in(char *user, char *pass){

	//puntero al archivo
	FILE *fd;

	//utilizado para leer el archivo
	struct log r;

	fd = my_open();
	if (fd == NULL){

		//indico que hubo un error
		return 0;
	}

	//mientras que no llegue a EOF
	while( fread(&r,sizeof(r),1,fd) > 0 ){

		//verifico coincidencia
		if( strcmp(user,&r.username[0]) == 0 && strcmp(pass, &r.password[0]) == 0){

			fclose(fd);

			//indico que las credenciales son validas
			return 1;
		}

	}

	//cierro el archivo
	fclose(fd);

	//indico que las credenciales son invalidas
	return -1;
}

/**Muestra el contenido del archivo 
 *
 */
void show(){

	//puntero al archivo
	FILE *fd;

	//utilizado para leer el archivo
	struct log r;

	fd = my_open();
	if(fd){
			
		//me posiciono al principio del archivo
		rewind(fd);

		//contador de usuarios
		int i = 1;

		while( fread(&r, sizeof(r), 1, fd) > 0){

			fprintf(stdout, "[%d]=======================\n", i);
			fprintf(stdout, "\tNombre de usuario:%s\n", &r.username);
			fprintf(stdout, "\tContraseña:%s\n", &r.password);
			i++;
		}

	}

	//cierro el archivo
	fclose(fd);
}

/*
int main(){


	show();
	return 1;
}
*/