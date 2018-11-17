/*
  Compilacion desde la linea de comandos para crear el progrma objeto AccesoPostgresql.o

  gcc AccesoPostgresql.c -Wall -g -c -lpq -I/usr/include/postgresql/ 
*/
#include <string.h>
#include <libpq-fe.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define IPPostgresql "192.168.1.10"
#define PuertoPostgresql "5432"


void funcionPostgresql(char bufnomb[],char bufquery[],char respuesta[])
{
   PGconn *conn;
   PGresult *res;
   int i,j;
   conn = PQsetdbLogin(IPPostgresql,PuertoPostgresql,NULL,NULL,bufnomb,"postgres","miclave");
   if (PQstatus(conn) != CONNECTION_BAD)
    {
      printf("funcionPostgresql dice %s %s\n",bufnomb,bufquery);
      res = PQexec(conn, bufquery);
      if (res != NULL && PGRES_TUPLES_OK == PQresultStatus(res))
         {
           for (i = 0 ; i <= PQntuples(res)-1;  i++)
             {
                for (j = 0 ; j < PQnfields(res); j++)
                    {
                      strcat(respuesta,PQgetvalue(res,i,j));
                      strcat(respuesta,"\t");                     
                      //  printf("%s\t",PQgetvalue(res,i,j));
                    }
                strcat(respuesta,"\n");
                // printf("\n");
             }
             strcat(respuesta,"\0");                
             PQclear(res);
         }
    }
    else
    {
      printf("Fallo conn %p\n",conn);
    }
    PQfinish(conn);
}






