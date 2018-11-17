/*
 * sv.h
 *
 *  Created on: Sep 19, 2018
 *      Author: nico
 *  En modo grafico utiliza la libreria ncurses (apt-get install libncurses5-dev)
 *  Compilar usando: gcc -o app_cli.o -lncurses app_cli.c
 *  
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <ctype.h>
#include "cli.h"
#include "login.h"

//colores a usar en modo grafico
#define BLACK   0
#define RED     1
#define GREEN   2
#define YELLOW  3
#define BLUE    4
#define MAGENTA 5
#define CYAN    6
#define WHITE   7
#define RESET 	"\x1B[0m"

//macro que limpia la pantalla
#define clrscr system("clear")
#define BD_LEN 12	//tamaño maximo del nombre de la BD

/**Elimina el caracter "\n" del buffer
 * @return     [valor ascii mayuscula de la opcion ingresada]
 */
int flushnl(){
		
	//remueve char \n del buffer
	while( getchar() != '\n'){
		//do nothing
	}
}

/**Devuelve la decision del usuario en un char evitando el \n
 * @return  [description]
 */
int get_opt(void){
	int opt;

	//tomo caracteres del buffer hasta encontrar una letra 
	while(1){
		opt = getchar();
		//si ingreso una letra minuscula...
		if(opt >=97 && opt <= 122)
		{
			//...convierto a mayuscula
			opt = toupper(opt);
			break;
		}
		
		//si ingreso una letra mayuscula
		if(opt >=65 && opt <= 90)
		{
			//la dejo como esta
			break;
		}
	}

	return opt;
}

/**Construye string y lo almacena en mayusculas. Lo devuelve en input
 * @param input 	[puntero a char donde se almacenara el string]
 * @param len       [tamaño maximo del string]
 * @param msg       [mensaje a mostrar]
 * @return			[longitud del string ingresado por el usuario]
 */
int get_string(char input[], int len, char *msg){

	//usado para construir la linea
	char c;

	//utilizado en el for
	int i;

	//imprimo el contenido de msg
	fprintf(stdout, "%s", msg);

	//construyo el string
	for(i = 0; i < len-1; i++){
		
		//obtengo caracter
		c = getchar();

		if(c != EOF || c != '\n' ){

			//salgo del for porque el string finalizo
			break; 
		}
		else{

			//lo paso a mayusculas
			input[i] = toupper(c);
		}
	}
	
	//agrego fin de string
	input[i] = '\0';

	//devuelvo la longitud del string recibido
	return i;
}

/**Solicita credenciales y las valida 
 * @return [1 si son validas, -1 si son invalidas]
 */
int my_login(){
	
	//utilizados para almacenar input
	char *user;
	char *pass;

	//alojo e inicializo un string del tamaño getmaxlen()
	user = (char *)malloc(BD_LEN);
	memset(user, '\0', strlen(user));

	//alojo e inicializo dos strings del tamaño getmaxlen()
	pass  = (char *)malloc(BD_LEN);
	memset(pass, '\0', strlen(pass));

	//solicito usuario
	get_string(user, strlen(user) ,"Ingrese nombre de usuario:\n>");
	flushnl();

	//solicito contraseña
	get_string(pass, strlen(pass) ,"Ingrese contraseña:\n>");
	flushnl();
	
	//verifico credenciales
	if (log_in(user, pass) == -1){

		//indico credenciales invalidas
		return -1; 
	}

	//indico credenciales validas
	return 1;
}

/**Solicita nombre y contraseña y crea nuevo usuario.
 * @return [1 si tuvo exito, -1 si ya existia el nombre]
 */
int my_creuser(){

	//utilizados para almacenar input
	char *user;
	char *pass;
	char *pass2;

	//almaceno el exito de la operacion
	int value;

	//asumo que el usuario se crea con exito
	value = 1;

	//alojo e inicializo un string del tamaño getmaxlen()
	user = malloc( sizeof( getmaxlen() ) );
	memset(user, '\0', strlen(user));
	
	//alojo e inicializo dos strings del tamaño getmaxlen()
	pass  = malloc( sizeof( getmaxlen() ) );
	memset(pass, '\0', strlen(pass));

	pass2 = malloc( sizeof( getmaxlen() ) );
	memset(pass2, '\0', strlen(pass2));


	get_string(user, strlen(user) ,"Ingrese nombre de usuario:\n>");
	flushnl();

	while(1){

		get_string(pass, strlen(pass) ,"Ingrese contraseña:\n>");
		flushnl();
		fprintf(stdout, "Ingreso:%s\n", pass);

		get_string(pass2, strlen(pass2) ,"Ingrese nuevamente la contraseña.\nRecuerde que deben coincidir:\n>");
		flushnl();
		fprintf(stdout, "Ingreso:%s\n", pass2);

		//verifico coincidencia
		if( strcmp(pass, pass2) == 0){
			fprintf(stdout, "Son iguales!\n");
			break;
		}

		clrscr;
		fprintf(stdout, "Error. Las contraseñas no coinciden.\n");
	}

	//si llego hasta aqui las contraseñas coinciden
	//ahora debo verificar si el usuario existe o no (existe = -1, no existe = 1)
	if(creuser(user, pass) == -1){

		//indico que el usuario ya existe
		value = -1;
	}

	//libero la memoria
	free(user);
	free(pass);
	free(pass2);

	//indico usuario creado con exito
	return value;
}

/**Menu principal. Acepta query del usuario o 'Q' para salir
 * 
 */
void main_menu(){

	//almacena el input del usuario
	char *buf;

	//alojo e inicializo el buffer
	buf = (char *) malloc(BD_LEN+getmaxlen());
	memset(buf, '\0', strlen(buf));

	while(1){
		
		//solicito input al usuario
		get_string(buf, strlen(buf), "Ingrese '<DATABASE>:<QUERY>' o 'Q' para salir.\n>");	
		flushnl();
		fprintf(stdout, "Usted ingreso:\n%s.\n", buf);

		//si desea salir, vuelvo al main
		if(buf[0] == 81){

			//salgo del while. el buffer es liberado al final
			break;
		}

		/**
		 * ENVIO LA QUERY ENTERA USANDO EL SOCKET CLIENTE
		 *
		 * ESPERO LA RESPUESTA Y LA MUESTRO
		 *
		 * LE DOY LA OPCION DE VOLVER A EJECUTAR OTRA QUERY O SALIR
		 */
	}

	//libero el buffer
	free(buf);
}


int main(){

	//mantiene el loop activo
	int rpt;

	//almacena la opcion del usuario
	int opt;
	
	//almacena si desea salir
	int ext;

	rpt = 1;
	ext = 0;

	//ejecutar mientras el usuario no desee salir y no haya iniciado sesion
	while(ext == 0 && rpt == 1){	
		clrscr;
		fprintf(stdout,"***Login***\n");
		fprintf(stdout, "L - Iniciar sesion.\n");
		fprintf(stdout, "S - Registrarse.\n");
		fprintf(stdout, "P - Cambiar contraseña.\n");
		fprintf(stdout, "D - Eliminar usuario.\n");
		fprintf(stdout, "Q - Salir del cliente.\n>");

		//capturo su opcion
		opt = get_opt();
		flushnl();

		switch(opt){
			case 76:		//76 = L
				clrscr;

				//no avanzo hasta que ingrese credenciales validas
				while(my_login() == -1){
					clrscr;
					fprintf(stdout, "Error. Credenciales invalidas.\n\n");
					fprintf(stdout, "<<<Presione una tecla para continuar>>>%s\n");
					flushnl();
				}
				clrscr;

				printf("======Sesion iniciada======\n");

				//el usuario ya inicio sesion,
				//ahora rompo el while para que luego llame a main_menu()
				rpt = 0;

				break;
			case 83:		//83 = S
				clrscr;

				if (my_creuser() == -1){
					
					fprintf(stderr, "Error. El nombre de usuario esta en uso.\n");
					fprintf(stdout, "<<<Presione una tecla para continuar>>>%s\n");
					flushnl();
					break;
				}
				
				fprintf(stdout, "Usuario creado.\n");
				fprintf(stdout, "<<<Presione una tecla para continuar>>>%s\n");
				flushnl();
				break;
			case 80:		//80 = P
				clrscr;
				/**
				 * MODULO QUE CAMBIA LA CONTRASEÑA DE UN USUARIO
				 */
				break;
			case 68:		//68 = D
				clrscr;
				/**
				 * 	MODULO QUE ELIMINA UN USUARIO
				 */
				break;
			case 81:		//81 = Q
				clrscr;

				fprintf(stdout, "Seguro que desea salir? (y/n).\n");
				opt = get_opt();
				flushnl();
				if(opt == 89){
					ext = 1;
				}
				break;
			default:
				fprintf(stdout, "Ingrese una opcion valida.\n");
				fprintf(stdout, "<<<Presione una tecla para continuar>>>%s\n");
				flushnl();
				break;
		}
	}
	
	//si el usuario ya inicio sesion y no desea salir
	if(rpt == 0){
		
		main_menu();

	}

	clrscr;
	fprintf(stdout, "Cerrando sesion...\n");

	//salgo con exito
	exit(EXIT_SUCCESS);			
}

/**MODO GRAFICO:
 *===============================================================================
 * KEY_CODES:
 * ------------------------------------------------------------------------------
 * KEY_DOWN 		Down arrow key
 * KEY_UP 			Up arrow key
 * KEY_LEFT 		Left arrow key
 * KEY_RIGHT 		Right arrow key
 * KEY_HOME 		Home key
 * KEY_BACKSPACE 	Backspace
 * KEY_STAB 		Set tab 
 * KEY_ENTER 		Enter or send 
 * ==============================================================================
 * COLORES:
 * ------------------------------------------------------------------------------
 * COLOR_BLACK
 * COLOR_BLUE
 * COLOR_GREEN
 * COLOR_CYAN
 * COLOR_RED
 * COLOR_MAGENTA
 * COLOR_YELLOW
 * COLOR_WHITE 
 *===============================================================================
 * ATRIBUTOS DE OBJETOS chtype:
 * ------------------------------------------------------------------------------
 * A_NORMAL        Normal display (no highlight)
 * A_STANDOUT      Best highlighting mode of the terminal.
 * A_UNDERLINE     Underlining
 * A_REVERSE       Reverse video
 * A_BLINK         Blinking
 * A_DIM           Half bright
 * A_BOLD          Extra bright or bold
 * A_PROTECT       Protected mode
 * A_INVIS         Invisible or blank mode
 * A_ALTCHARSET    Alternate character set
 * A_CHARTEXT      Bit-mask to extract a character
 * COLOR_PAIR(n)   Color-pair number n 
 * ==============================================================================
 * FUNCIONES:
 * ------------------------------------------------------------------------------
 * _____________________________INICIALIZADORAS__________________________________
 * initscrn() 				=	inicializa el modo grafico
 * endwin()					=	finaliza el modo grafico
 * ________________________________COLORES_______________________________________
 * has_colors()				=	indica la consola soporte colores
 * start_color()			=	inicia los colores
 * init_color(col, R, G, B) = 	remapea el color al RGB(0-1000)indicao
 * init_pair(#ID, cl1, cl2) =	asocia un par de colores bground-fground al #ID indicado
 * attron(COLOR_PAIR(#ID))	=	activa el par de colores
 * COLOR_PAIR(#ID)			=	grafica con el par de colores asociado al #ID
 * attroff(COLOR_PAIR(#ID))	= 	desactiva el par de colores
 * chgat()					=	cambia, dsd pos del cursor hsta newline, los atributos y colores de los chars
 * 		'->ejemplo: chgat(-1, A_REVERSE, 0, NULL);
 * mvchgat()				=	combina move() y chgat()
 * ___________________________________INPUT______________________________________
 * keypad(stdscr, TRUE)		=	permite el highlight de input
 * getch()					=	solicita al usuario que ingrese un char (usar con cbreak() )
 * gets()					=	solicita al usuario que ingrese un string
 * scanw()					= 	get formatted input like scanf
 * cbreak()					=	
 * _________________________________OUTPUT_______________________________________
 *						****RECIBEN PRIMERO LA Y, LUEGO LA X***
 * addch()					=	imprime chars con atributos -> addch(ch | A_BOLD | A_UNDERLINE);
 * mvaddch()				= 	combina move() con addch()
 * addstr()					=	imprime un string
 * mvaddstr()				=	combina move() con addstr()
 * printw()					=	imprime formatted output like printf 
 * mvprintw()				=	combina move() con printw()
 * ____________________________POSICIONAMIENTO___________________________________
 * getmaxyx(y,x)			=	obtiene el tamaño de la consola
 * startx(x)				= 	posiciona al cursor en X
 * starty(y)				=	posiciona al cursor en Y
 * getyx(y,x)				= 	obtiene la posicion del cursor
 * move(y,x)				=	posiciona el cursor en xy
 * __________________________________MISC________________________________________
 * refresh()				=	imprime todo en patalla 
 * clear()					=	limpia la pantalla
 */

