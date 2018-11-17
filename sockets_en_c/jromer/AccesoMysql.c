/*
  Compilacion desde la linea de comandos para crear el progrma objeto AccesoMysql.o
  gcc AccesoMysql.c -Wall -g -c -L/usr/lib/mysql -lmysqlclient 
*/


#include "/home/jromer/Escritorio/ConexionPostgres/mysql/mysql-connector-c-6.1.11-linux-glibc2.12-i686/include/mysql.h"

#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define IPMysql "192.168.1.40"
#define PuertoMysql "3306" 


void funcionMysql(char dabasename[],char query[],char respuesta[])
{
        MYSQL *conn; /* variable de conexión para MySQL */
	MYSQL_RES *res; /* variable que contendra el resultado de la consuta */
	MYSQL_ROW row; /* variable que contendra los campos por cada registro consultado */
	char *server = IPMysql ; /*direccion del servidor 127.0.0.1, localhost o direccion ip */
	char *user = "usuario"; /*usuario para consultar la base de datos */
	char *password = "clave"; /* contraseña para el usuario  */
        char *database = dabasename; /*nombre de la base de datos a consultar */	
        conn = mysql_init(NULL); /*inicializacion a nula la conexión */

	/* conectar a la base de datos */
	if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0))
	{ /* definir los parámetros de la conexión antes establecidos */
		fprintf(stderr, "%s\n", mysql_error(conn)); /* si hay un error definir cual fue dicho error */
		exit(1);
	}

	/* enviar consulta SQL */
//	if (mysql_query(conn, "select * from tabla01"))
        if (mysql_query(conn, query))		
        { /* definicion de la consulta y el origen de la conexion */
		fprintf(stderr, "%s\n", mysql_error(conn));
		exit(1);
	}
        memset(respuesta,0,1024);
        res = mysql_use_result(conn);
	printf("cod\tNombre\n");
	while ((row = mysql_fetch_row(res)) != NULL)
        { 
         /* recorrer la variable res con todos los registros obtenidos para su uso */
	   // printf("%s\t%s\t%s \n", row[0],row[1]); /* la variable row se convierte en un arreglo por el numero de campos que hay en la tabla */
           strcat(respuesta,row[0]);
           strcat(respuesta,"\t");
           strcat(respuesta,row[1]);
           strcat(respuesta,"\n");           
        }
        strcat(respuesta,"\0");                
        
        /* se libera la variable res y se cierra la conexión */
	mysql_free_result(res);
	mysql_close(conn);
}
