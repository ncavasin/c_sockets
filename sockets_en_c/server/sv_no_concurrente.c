/*
 * sv.h
 *
 *  Created on: Sep 19, 2018
 *      Author: Niccolas Cavasin
 */

// Servidor basado en sockets IPv4

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

#define BACKLOG_MAX 10 		// Limite de conexiones pendientes, aka BACKLOG
#define BUFF_SIZE 2048		// Buffer de 2K
#define MIN_PORT 1024		// Utilizado para controlar que el puerto sea efimero
#define MAX_PORT 65535		// Utilizado para controlar que el puerto sea valido

// Forwardea la query recibida del cliente a la BD.
// Recibe la respuesta a la query y la retorna en un puntero a char
// @param  query [mensaje enviado por el cliente]
// @return       [respuesta de la BD]
int forward_msg(char buf[]){



	// Indico exito
	return 1;
}

// Crea, bindea y pone a escuchar un socket. Recibe un mensaje y lo forwardea a
// quien corresponda. Recibe la respuesta y se la entrega a quien le envio el
// mensaje. Cierra el socket.
// @param  ip_src [ip sobre la que se debe crear el socket]
// @param  port   [puerto sobre el que escuchara el socket]
// @return        [respuesta al mensaje recibido]
int gods_hotline(char ip_rcvd[], char port_rcvd[]){

	// Utilizado para chequear resultados
	int ret_val;

	// File descriptor del socket que escucha sobre el IP:PUERTO indicados x user
	int mastersockfd;

	// File descriptor del socket con el cliente
	int clientfd;

	// Almacena datos del socket que escucha y del que acepta cxs
	struct sockaddr_in mastersock_data, client_data;
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
	unsigned int mastersock_data_len = sizeof(struct sockaddr_in);

	// Almacena la IP recibida del usuario en formato network
	// Sera utilizada en mastersock_data para crear el socket mastersock
	struct in_addr ip_to_use;
	// |========================================================================|
	// | Formato del struct in_addr:											|
	// |------------------------------------------------------------------------|
	// |struct sockaddr_in{														|
	// |	u_int32_t			s_addr;				//direccion IPv4 			|
	// |};																		|
	// |------------------------------------------------------------------------|

	// Almacena en formato long el Puerto indicado por el usuario
	// Sera utilizado en mastersock_data para crear el socket mastersock
	long port_to_use;

	// Buffer utilizado para la comunicacion
	char buf[BUFF_SIZE];

	// Almacenara la longitud de la respuesta enviada por la BD
	int buf_len;

	// Almacenara la IP del socket cliente en formato presentacion
	char client_ip[INET_ADDRSTRLEN];

	// Almacenara el puerto del socket cliente en host byte order
	short unsigned int client_port;

	int yes = 1;

	//-------------------------------------------------------------------------\\

	// Convierto la IP recibida a formato network
	ret_val = inet_pton(AF_INET, &ip_rcvd[0], &ip_to_use);
	if (ret_val <= 0){
		if (ret_val == 0){
			fprintf(stderr, "!!!! Error - La IP no esta en formato presentacion (ddd.ddd.ddd.ddd).\n");
		}else{
			perror("!!!! Error - inet_pton");
			fprintf(stderr, "!!!! Error - En la conversion a formato network.\n");
		}
		exit(EXIT_FAILURE);
	}

	// Convierto el puerto indicado por el usuario a un long integer
	port_to_use = strtol(&port_rcvd[0], NULL, 10);
	if( (port_to_use < (long) MIN_PORT) || (port_to_use > (long) MAX_PORT)){
		fprintf(stderr, "!!!! Error - El puerto indicado no es efimero.\n");
		exit(EXIT_FAILURE);
	}

	system("clear");
	fprintf(stdout, "[Servidor] - Iniciando servidor...\n");

	// Creo el socket que va a escuchar sobre los parametros indicados por usuario
	mastersockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(mastersockfd == -1 ){
		perror("!!!! Error - socket");
		fprintf(stderr, "!!!! Error - Creando mastersocket.\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "[Servidor] - Mastersocket con identificador [%d] creado.\n",
	mastersockfd);


	// Elimino el 2MSL permitiendo la reutilizacion del mismo puerto
	// setstockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
	setsockopt(mastersockfd, SOL_SOCKET, SO_REUSEPORT,&yes, sizeof(yes));


	// Inicializo datos de mastersockfd con los indicados por el usuario
	memset(&mastersock_data, 0, mastersock_data_len);
	mastersock_data.sin_family = AF_INET;
	mastersock_data.sin_port = port_to_use;
	mastersock_data.sin_addr = ip_to_use;


	// Asigno el puerto recibido al socket mastersockfd
	ret_val = bind(mastersockfd, (struct sockaddr *)&mastersock_data, mastersock_data_len);
	if(ret_val == -1){
		perror("!!!! Error - bind");
		fprintf(stderr, "!!!! Error - No se pudo asignar ");
		fprintf(stderr, "el puerto %d a la IPv4 %d.\n", port_to_use, ip_to_use);
		exit(EXIT_FAILURE);
	}
	fprintf(stdout,	"[Servidor] - Mastersocket [%d] es ahora: [%s:%d].\n",
	mastersockfd, 		ip_rcvd, 		port_to_use);


	// Pongo a mastersockfd a escuchar sobre el puerto indicado por el usuario
	ret_val = listen(mastersockfd, BACKLOG_MAX);
	if(ret_val == -1){
		perror("!!!! Error - listen");
		fprintf(stderr, "!!!! Error - Al intentar escuchar sobre el puerto %d.\n",
		port_to_use);
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "[Servidor] - Esperando conexion...\n");


	// Inicializo los datos del socket que aceptara nuevas conexiones
	memset(&client_data, 0, sizeof(client_data));
	// Inicializo el file descriptor del socket que aceptara nuevas conexiones
	clientfd = -1;

	// Acepto conexion entrante
	clientfd = accept(mastersockfd, (struct sockaddr *)&client_data, &mastersock_data_len);


	// Inicializo el buffer que contendra la IP del cliente en formato presentacion
	memset(&client_ip, '\0', sizeof(client_ip));
	// Inicializo el buffer que contendra el puerto del cliente en host byte order
	client_port = -1;

	// Convierto la IP y puerto del cliente a formato presentacion
	if (inet_ntop(client_data.sin_family, &client_data.sin_addr, client_ip, sizeof(client_ip)) == NULL){
		perror("!!!! Error - inet_ntop");
		fprintf(stderr, "!!!! Error - Convirtiendo IP cliente a formato presentacion.\n");
	}
	client_port = ntohs(client_data.sin_port);
	if(client_port == -1){
		perror("!!!! Error - ntohs");
		fprintf(stderr, "!!!! Error - Convirtiendo puerto cliente a host byte order.\n");
	}
	fprintf(stdout, "[Servidor] - Conexion establecida con [%s:%d].\n",
	client_ip, 		client_port);




	// Inicializo el buffer que almacenara el mensaje del cliente
	memset(buf, '\0', BUFF_SIZE);
	// Leo del socket
	ret_val = recv(clientfd, buf, BUFF_SIZE, 0);
	// Verifico que no haya ocurrido un error de lectura
	if(ret_val == -1){
		perror("!!!! Error - recv");
		fprintf(stderr, "!!!! Error - Leyendo en socket cliente [%d].\n", clientfd);
		exit(EXIT_FAILURE);
	}
	// Verifico que el cliente no haya cerrado su canal
	if(ret_val == 0){
		perror("!!!! Error - recv");
		fprintf(stderr, "!!!! Error - El cliente ha finalizado la conexion.\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "[Servidor] <== [%s:%d] - %d bytes: %s.\n",
	client_ip,		client_port, 	ret_val, 	buf);


	// +
	// Forwardeo hacia la BD la query enviada por el cliente y
	// +
	forward_msg(&buf[0]);


	// +
	// Forwardeo hacia el cliente la respuesta enviada por la BD
	// +






	// Cierro el socket con el cliente
	ret_val = close(clientfd);
	if(ret_val == -1){
		perror("!!!! Error - close");
		fprintf(stderr, "!!!! Error - Cerrando socket [%d].\n", clientfd);
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "[Servidor] - Socket cliente [%s:%d] cerrado.\n",
	client_ip,		client_port);



	// Cierro el mastersocket
	ret_val = close(mastersockfd);
	if(ret_val == -1){
		perror("!!!! Error - close");
		fprintf(stderr, "!!!! Error - Cerrando mastersocket [%d].\n", mastersockfd);
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "[Servidor] - Mastersocket cerrado.\n");

	fprintf(stdout, "[Servidor] - Cerrando servidor...\n");

	// Salgo con exito
	return 1;
}


int main(int argc, char *argv[]){

	if(argc < 3){
		fprintf(stderr, "!!!! Error - Missing arguments.\n");
		fprintf(stderr, "Usage: \"./sv <ARGV1> <ARGV2>\"\n");
		fprintf(stderr, "\t<ARGV1> = IPv4 ADDRESS.\n");
		fprintf(stderr, "\t<ARGV2> = EPHIMERAL PORT.\n");

		exit(EXIT_FAILURE);

	}

	gods_hotline(argv[1], argv[2]);

	exit(EXIT_SUCCESS);
}

//
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
