#include <stdio.h> /* Éste es el código del servidor */
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>


#define SERVER_PORT 12345 /* arbitrario, pero el cliente y el servidor deben coincidir */
#define BUF_SIZE 4096 /* tamaño de bloque para la transferencia */
#define QUEUE_SIZE 10


int main(int argc, char *argv[])
{

socklen_t destino_tam;
int descriptor_socket_servidor, descriptor_socket_cliente; /* descriptores de socket cliente y servidor*/
int b, l, fd, bytes, on = 1;
char buf[BUF_SIZE]; /* búfer para el archivo saliente */
struct sockaddr_in socket_cliente, socket_servidor; /* contiene la configuracion del socket*/

/* Construye la estructura de la dirección para enlazar el socket. */
socket_servidor.sin_family = AF_INET;
socket_servidor.sin_addr.s_addr = htonl(INADDR_ANY);
socket_servidor.sin_port = htons(SERVER_PORT);

/* Apertura pasiva. Espera una conexión. */
descriptor_socket_servidor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); /* crea el socket */
if (descriptor_socket_servidor < 0) printf("falla en socket \n");
//setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on));

b = bind(descriptor_socket_servidor, (struct sockaddr *) &socket_servidor, sizeof(socket_servidor));
if (b < 0) printf("falla en bind \n"); 

l = listen(descriptor_socket_servidor, QUEUE_SIZE); /* especifica el tamaño de la cola */
if (l < 0) printf("falla en listen \n"); 

/* El socket ahora está configurado y enlazado. Espera una conexión y la procesa. */
while (1) {
printf("\n***Servidor ACTIVO escuchando en el puerto: 12345 ...\n");
destino_tam=sizeof(socket_cliente);
descriptor_socket_cliente = accept(descriptor_socket_servidor, (struct sockaddr*)&socket_cliente, &destino_tam); /* se bloquea para la solicitud de conexión */
if (descriptor_socket_cliente < 0) printf("falla en accept \n"); 
printf("***Servidor se conecto con el cliente: %d.\n",socket_cliente.sin_addr.s_addr);

read(descriptor_socket_cliente, buf, BUF_SIZE); /* lee el nombre del archivo desde el socket */
/* Obtiene y regresa el archivo.*/

fd = open(buf, O_RDONLY); /* abre el archivo para regresarlo */
if (fd < 0) printf("falla en open \n");

while (1) {
bytes = read(fd, buf, BUF_SIZE); /* lee del archivo */
if (bytes <= 0) break; /* verifica el final del archivo */
write(descriptor_socket_cliente, buf, bytes); /* escribe bytes en el socket */
}
close(fd); /* cierra el archivo */
close(descriptor_socket_cliente); /* cierra la conexión */
}

}

