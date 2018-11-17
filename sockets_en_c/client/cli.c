/*
 * cli.h
 *
 *  Created on: Sep 16, 2018
 *      Author: Nicolas Cavasin
 */

/*SOCKETS EN IPv4 - Cliente generico*/

#include <stdlib.h>			// Contiene EXIT_FAILURE y EXIT_SUCCESS
#include <stdio.h>			// Utilizada por la familia fprintf()
#include <unistd.h>			// Utilizada por close()
#include <errno.h>			// Utilizada por perrno()
#include <string.h>			// Utilizada por strlen() y memset()

#include <netinet/in.h>		// Contiene definidos los valores de las constantes
							// INET_ADDRSTRLEN e INET6_ADDRSTRLEN

#include <sys/types.h>		// Utilizadas por funciones asociadas a la creacion
#include <sys/socket.h>		// y comunicacion de sockets.

#include <sys/select.h>		// Utilizada por la funcion select()
#include <arpa/inet.h>		// Utilizada por inet_pton() e inet_ntop()

#define BUFF_SIZE 2048		// Buffer de 2K
#define MIN_PORT 1024		// Utilizado para controlar que el puerto sea efimero
#define MAX_PORT 65535		// Utilizado para controlar que el puerto sea valido

int get_buffsize(){
	return BUFF_SIZE;
}

// Crea, y conecta un socket. Envia el mensaje. Espera la respuesta del servidor
// y la devuelve. Cierra el socket.
// @param  ip_dest   [puntero a char IPv4 con notacion punto-decimal]
// @param  port_dest [numero de puerto a conectarse]
// @param  msg       [puntero a char del mensaje a enviar y a recibir]
// @return           [1 si tuvo exito, -1 si hubo error]
int call_god(char ip_rcvd[], char port_rcvd[], char msg[]){

	// Utilizado para chequear resultados
	int ret_val;

	// File descriptor del socket a crear
	int sockfd;

	// Almacena datos del servidor a conectarse y del cliente creado
	struct sockaddr_in dest_data, src_data;
	// |========================================================================|
	// | Formato del struct sockaddr_in:										|
	// |------------------------------------------------------------------------|
	// |struct sockaddr_in{														|
	// |	short int 			sin_family;		//AF_INET o AF_INET6 o AF_UNSPEC|
	// |	unsigned short int 	sin_port;		//puerto 						|
	// |	struct in_addr		sin_addr;		//direccion IPv4 				|
	// |	unsgined char		sin_zero[8];	//padding compat con sockaddr 	|
	// |};																		|
	// |------------------------------------------------------------------------|

	// Almacena el tamaño de un struct sockaddr_in.
	// Inicializo para utilizarlo en bind y en accept
	unsigned int src_data_len = sizeof(src_data);

	// Almacena en formato network la ip del servidor
	struct in_addr ip_dest;
	// |========================================================================|
	// | Formato del struct in_addr:											|
	// |------------------------------------------------------------------------|
	// |struct sockaddr_in{														|
	// |	u_int32_t			s_addr;				//direccion IPv4 			|
	// |};																		|
	// |------------------------------------------------------------------------|

	// Almacena en formato long el Puerto indicado por el usuario
	// Sera utilizado en mastersock_data para crear el socket mastersock
	long port_dest;

	// Buffer que almacenara la query del user y la respuesta de la BD
	char buf[BUFF_SIZE];

	// Almacenara la longitud de la respuesta enviada por la BD
	int buf_len;

	// Almacenara la IP del socket en formato presentacion
	char ip_src[INET_ADDRSTRLEN];

	// Almacenara el puerto asignado automaticamente por el SO en host byte order
	short unsigned int port_src;

	// Convierto la IP recibida a formato network
	// Syntax: int = inet_pton(AF_INET, *char IP recibida, *addr_in IP network);
	ret_val = inet_pton(AF_INET, &ip_rcvd[0], &ip_dest);
	// Verifico que no haya ocurrido un error en la conversion a formato network
	if (ret_val <= 0){
		if (ret_val == 0){
			fprintf(stderr, "!!!! Error - La IP no esta en formato presentacion (ddd.ddd.ddd.ddd).\n");
		}else{
			perror("!!!! Error - inet_pton");
			fprintf(stderr, "!!!! Error - En la conversion a formato network.\n");
		}
		exit(EXIT_FAILURE);
	}
	//printf("[Cliente] - IPv4 destino ==> %s.\n", ip_rcvd);

	// Convierto el puerto indicado por el usuario a un long integer
	port_dest = strtol(&port_rcvd[0], NULL, 10);
	//printf("[Cliente] - Puerto destino ==> %d.\n", port_dest);

	system("clear");
	fprintf(stdout, "[Cliente] - Iniciando cliente...\n", sockfd);

	// Creo el socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	// Verifico que no haya ocurrido ningun error creando el socket.
	if (sockfd == -1){
		perror("!!!! Error - socket");
		fprintf(stderr, "!!!! Error - Creando socket.\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "[Cliente] - Socket con identificador [%d] creado.\n", sockfd);


	// Inicializo datos del servidor destino con los indicados por el usuario
	memset(&dest_data, 0, sizeof(dest_data));
	dest_data.sin_family = AF_INET;				//socket IPv4
	dest_data.sin_addr = ip_dest;				//IP del servidor
	dest_data.sin_port = port_dest;				//Puerto del servidor


	// Conecto el socket
	ret_val = connect(sockfd, (struct sockaddr*)&dest_data, sizeof(dest_data));
	// Verifico que no haya ocurrido ningun error en la conexion del socket
	if(ret_val == -1){
		perror("!!!! Error - connect");
		fprintf(stderr, "!!!! Error - Conectando con servidor.\n");
		exit(EXIT_FAILURE);
	}


	// Inicializo struct que almacenara datos del socket conectado
	memset(&src_data, 0, sizeof(src_data));
	// Obtengo IP y puerto bindeados automaticamente por el SO
	ret_val = getsockname(sockfd, (struct sockaddr*)&src_data, &src_data_len);
	/// Verifico que no haya ocurrido un error obteniendo IP y puerto
	if(ret_val == -1){
		perror("!!!! Error - getsockname");
		fprintf(stderr, "!!!! Error - Obteniendo IP y puerto del host.\n");

	}
	// Convierto la IP del cliente a formato presentacion para mostrarlo,
	// y verifico que no haya ocurrido un error durante la conversion
	if (inet_ntop(src_data.sin_family, &src_data.sin_addr, ip_src, sizeof(ip_src)) == NULL){
		perror("!!!! Error - inet_ntop");
		fprintf(stderr, "!!!! Error - Convirtiendo IP cliente a formato presentacion.\n");
	}
	// Convierto el puerto del cliente de network byte order a host byte order
	port_src = ntohs(src_data.sin_port);
	if(port_src == -1){
		perror("!!!! Error - ntohs");
		fprintf(stderr, "!!!! Error - Convirtiendo puerto cliente a host byte order.\n");
	}
	fprintf(stdout, "[Cliente] - Socket [%d] es ahora [%s:%d].\n",
	sockfd, 	ip_src, 		port_src);
	fprintf(stdout, "[Cliente] - Se ha conectado con [%s:%d].\n",
	ip_rcvd, 		port_dest);

	// Almaceno la longitud real del mensaje
	buf_len = strlen(msg);

	// Inicializo para utilizar como valor de corte
	ret_val = 0;

	// Me aseguro de enviar por completo el mensaje ingresado por el usuario
	while(ret_val < buf_len){
		/// Utilizado para verificar que no haya ocurrido un error enviando ans
		int temp;

		// Asigno a temp el resultado de send();
		// Acumulo en ret_value la cantidad de bytes enviados por send();
		ret_val += temp = send(sockfd, &msg[ret_val], buf_len-ret_val, 0);
		// Verifico que no haya ocurrido un error enviando ans
		if(temp == -1){
			perror("!!!! Error - send");
			fprintf(stderr, "!!!! Error - Enviando query.\n");
			exit(EXIT_FAILURE);
		}
	}

	fprintf(stdout, "[Cliente] ==> [%s:%d] - %d bytes: %s.\n",
	ip_rcvd,		port_dest,		ret_val,		msg);

	// Inicializo el buffer que almacenara la respuesta
	memset(&msg[0], '\0', BUFF_SIZE);

	// Espero la respuesta
	ret_val = recv(sockfd, &msg[0], sizeof(buf), 0);
	if(ret_val == 0){
		fprintf(stderr, "!!!! Error - El servidor ha finalizado la conexion.\n");
		exit(EXIT_FAILURE);
	}
	if(ret_val == -1){
		perror("!!!! Error - recv");
		fprintf(stderr, "!!!! Error - En la lectura del socket.\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "[Cliente] <== [%s:%d] - %d bytes: %s.\n",
	ip_rcvd, 		port_dest, 		ret_val, 		msg);

	// Cierro el socket
	ret_val = close(sockfd);
	// Verifico que no haya ocurrido un error cerrando el socket sockfd
	if(ret_val == -1){
		perror("!!!! Error - close");
		fprintf(stderr, "!!!! Error - Cerrando el socket [%d].\n", sockfd);
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "[Cliente] - Socket [%s:%d] cerrado.\n", ip_rcvd, port_src);
	fprintf(stdout, "[Cliente] - Cerrando cliente...\n");

	// Salgo con exito
	return 1;
}

int main(int argc, char *argv[]){

	// Verifico que los argumentos sean los correctos
	if(argc < 4){
		fprintf(stdout, "!!!! Error - Missing arguments.\n");
		fprintf(stdout, "Usage: \"./sv <ARGV1> <ARGV2> <ARGV3>\"\n");
		fprintf(stdout, "\t<ARGV1> = IPv4 ADDRESS.\n");
		fprintf(stdout, "\t<ARGV2> = EPHIMERAL PORT.\n");
		fprintf(stdout, "\t<ARGV3> = QUERY AND ANSWER BUFFER.\n");
		exit(EXIT_FAILURE);

	}
	call_god(argv[1], argv[2], argv[3]);

	return 1;
}

//	ESTRUCTURAS DEL KERNEL INVOLUCRADAS EN EL FUNCIONAMIENTO DE SOCKETS
//
//	*****************************************************************
//	la mas importante de todas. es utilizada por el socket descriptor
//	*****************************************************************
//	struct addrinfo
//	{
//		int					ai_flags;			//AI_PASSIVE, AI_CANONNAME, etc.
//		int					ai_family;			//AF_INET, AF_INET6, AF_UNSPEC
//		int					ai_socktype;		//SOCK_STREAM, SOCK_DGRAM
//		int 				ai_protocol;		//usar 0 para cualquiera.
//		size_t				ai_addrlen;			//longitud de la direccion ip
//		struct sockaddr		*ai_addr;			//struct sockaddr_in o sockaddr_in6
//		char				*ai_canonname;		//nombre canonico completo del host
//		struct addrinfo		*ai_next;			//siguiente nodo de la lista
//	};
//
//	****************************************
//	almacena info de la direccion del socket
//	****************************************
//	struct sockaddr
//	{
//		unsigned short 		sa_family;				//familia de direcciones ---> AF_XXXX
//		char				sa_data[14];			//almacena IP + puerto
//	};
//
//	************************************************************
//	almacenan info de la direccion del socket con + facil acceso
//	************************************************************
//		struct sockaddr_in
//		{
//			short int 			sin_family;				//familia de direcciones ---> AF_INET (IPv4)
//			unsigned short int 	sin_port;				//puerto
//			struct in_addr		sin_addr;				//direccion ipv4
//			unsgined char		sin_zero[8];			//padding para matchear con sockaddr
//		};
//
//
//		struct sockaddr_in6
//		{
//			u_int16_t			sin6_family;			//familia de direcciones ---> AF_INET6 (IPv6)
//			u_int16_t		 	sin6_port;				//puerto
//			u_int32_t			sin6_flowinfo;			//IPv6 flow info
//			struc in6_addr		sin6_addr;				//direccion ipv6
//			u_int32_t			sin6_scope_id;			//alcance de la direccion
//		};
//
//	*******************************************************************
//	mantenida por motivos historicos, permite acceder a la direccion IP
//	*******************************************************************
//		struct in_addr
//		{
//			u_int32_t			s_addr;					//direccion IPv4
//		};
//
//
//		struct in_addr6
//		{
//			unsigned char s6_addr[16];					//direccion IPv6
//		};
//
//	****************************************************************************************
//	utilizada por el servidor para aceptar conexiones cuya familia de direcciones no conoce
//	****************************************************************************************
//	struct sockaddr_storage
//	{
//		sa_family_t			ss_family;				//familia de direcciones
//
//		//info de implementacion
//		char				__ss_pad1[_SS_PAD1SIZE];
//		int64_t				__ss_align;
//		char				__ss_pad2[_SS_PAD2SIZE];
//	};
//
