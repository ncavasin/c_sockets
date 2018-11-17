//Cliente en Windows XP
/*
 Cliente mini Telnet de un servidor de comandos Debian
 Hay que agregarle el ingreso de un usuario y contraseña
 para que sea validado por el servidor.
*/
#include <windows.h>
#include <winsock2.h> //Cabecera para los socket windows
#include <stdio.h>
#include <stdlib.h>

void UbicacionDelServidor(struct in_addr);

int main ()
{
    WSADATA wsa; //Variable inicialización
    SOCKET dccliente; //Contiene el descriptor de socket cliente
    struct sockaddr_in remoto; //Contiene los datos de conexion del equipo remoto
    WSAStartup (MAKEWORD(2,0),&wsa); //Inicializamos winsock
    dccliente = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP); // Obtiene el identificador del socket
    if (dccliente == (unsigned)-1 )
     {
          printf ("socket fallo----\n");
          exit(0);
     }
    //Rellenamos la estructura sockaddr_in remoto
    remoto.sin_family = AF_INET; //Dominio de Internet
    remoto.sin_port = htons (6666); // puerto de escucha del servidor
    remoto.sin_addr.s_addr = inet_addr("192.168.0.2"); //direccion IPv4 donde esta corriendo el servidor
    memset(remoto.sin_zero,0,8); //Completamos la estructura con ceros

    //Establecemos conexion
    if (connect(dccliente, (SOCKADDR *)&remoto, sizeof(SOCKADDR)) == -1)
    {
        printf ("Conexion fallo...\n");
        exit(0);
    }
    //--------------------------------------
    printf ("Conexion establecida con exito...Datos del Servidor...\n");
    UbicacionDelServidor(remoto.sin_addr);
    //----------------------------
    printf("Ingrese comando Linux ");
    char bufin[512];
    scanf("%s",bufin);
    if (send(dccliente, bufin,strlen(bufin), 0) == -1)
    {
        printf("send fallo...\n");
        exit(0);
    }
    char buf[4096];
    int nb ;
    if ((nb=recv(dccliente, buf,4095, 0)) == -1)
    {
        printf("recv fallo...\n");
        exit(0);
    }
    else
    {
        buf[nb]='\0';
        printf("%s\n",buf);
    }
    system ("PAUSE");
    return (0);
}

void UbicacionDelServidor(struct in_addr addr )
{
  struct hostent *remoteHost;
//  char *host_name;
  char **pAlias;

  remoteHost = gethostbyaddr((char *)&addr, 4, AF_INET);

  if (remoteHost == NULL)
    printf("Fallo gethostbyaddr\n");
  else
   {
      printf("Nombre Servidor: %s\n", remoteHost->h_name);
      for (pAlias = remoteHost->h_aliases; *pAlias != 0; pAlias++)
       {
            printf("Nombre Alternativo: %s\n", *pAlias);
        }
       printf("Tipo Address: ");
       switch (remoteHost->h_addrtype) {
       case AF_INET:
            printf("AF_INET\n");
            break;
       case AF_INET6:
            printf("AF_INET6\n");
            break;
       case AF_NETBIOS:
            printf("AF_NETBIOS\n");
            break;
       default:
            printf(" %d\n", remoteHost->h_addrtype);
            break;
        }
        printf("Bytes Address: %d\n", remoteHost->h_length);

        if (remoteHost->h_addrtype == AF_INET)
        {
            int i = 0 ;
            while (remoteHost->h_addr_list[i] != 0)
             {
                addr.s_addr = *(u_long *) remoteHost->h_addr_list[i++];
                printf("IPv4 Address #%d: %s\n", i, inet_ntoa(addr));
            }
        }
        else
        if (remoteHost->h_addrtype == AF_INET6) printf("IPv6 address\n");
   }
}
