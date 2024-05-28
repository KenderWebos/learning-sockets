/* Esta página contiene un programa cliente que puede solicitar un archivo desde el programa servidor de la siguiente
página. El servidor responde
* enviando el archivo completo.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <netdb.h>


#define SERVER_PORT 12345 /* arbitrario, pero el cliente y el servidor deben coincidir */
#define BUF_SIZE 4096 /* tamaño de bloque para transferencia */


int main(int argc, char **argv)
{

int descriptor_socket_cliente; /* descriptor del socket cliente*/
int c, bytes; // variables auxiliares
char buf[BUF_SIZE]; /* búfer para el archivo entrante */
struct sockaddr_in socket_cliente, socket_servidor; /* contiene la configuracion del socket*/

if (argc != 3) printf("Usar: cliente nombre-servidor nombre-archivo \n"); 

descriptor_socket_cliente = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
if (descriptor_socket_cliente < 0) printf("falla en socket \n"); 

//Se prepara el socket de la máquina cliente
socket_cliente.sin_family = AF_INET;
socket_cliente.sin_port = htons(0); 	//Asigna un puerto disponible dela máquina
socket_cliente.sin_addr.s_addr = htonl(INADDR_ANY); //Asigna una IP de la máquina

//Asigna una direccion local al socket
if( bind(descriptor_socket_cliente, (struct sockaddr*)&socket_cliente, sizeof(socket_cliente)) == -1) {
printf("ERROR al configurar el socket del cliente\n");
close(descriptor_socket_cliente);
exit(EXIT_FAILURE);
}

//Se prepara el socket de la máquina servidora
socket_servidor.sin_family = AF_INET;
socket_servidor.sin_addr.s_addr = inet_addr(argv[1]);
socket_servidor.sin_port = htons(SERVER_PORT);

c = connect(descriptor_socket_cliente, (struct sockaddr *) &socket_servidor, sizeof(socket_servidor));
if (c < 0) printf("falla en conexion \n"); 

/* Se ha establecido la conexión. Se envía el nombre del archivo incluyendo el byte 0 al final. */
write(descriptor_socket_cliente, argv[2], strlen(argv[2])+1);

/* Obtiene el archivo y lo escribe en la salida estándar. */
while (1) {
bytes = read(descriptor_socket_cliente, buf, BUF_SIZE); /* lee del socket */
if (bytes <= 0) exit(0); /* verifica el final del archivo */
write(1, buf, bytes); /* escribe en la salida estándar */
}

}


