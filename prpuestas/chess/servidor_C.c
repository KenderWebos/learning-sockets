/*SERVIDOR*/
/********/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define MAX_CONN 100     //Nº máximo de conexiones en espera
#define MAX_TAM_MENSAJE 512 //Numero de caracteres maximo del mensaje

int descriptor_socket_servidor, descriptor_socket_cliente;

/**********************************************************/
/* función catch que captura una interrupción             */
/**********************************************************/
void catch(int sig)
{
	printf("***Señal: %d atrapada!\n", sig);
  printf("***Cerrando servicio ...\n");
  close(descriptor_socket_cliente);
  close(descriptor_socket_servidor);
  printf("***Servicio cerrado.\n");
  exit(EXIT_SUCCESS);
}


/**********************************************************/
/* función MAIN                                           */
/* Orden Parametros: Puerto                               */
/**********************************************************/

int main(int argc, char *argv[]) {
  socklen_t destino_tam;
  struct sockaddr_in socket_servidor, socket_cliente;
  char mensaje_entrada[MAX_TAM_MENSAJE], mensaje_salida[MAX_TAM_MENSAJE];

  if (argc != 2) {
    printf("\n\nEl número de parámetros es incorrecto\n");
    printf("Use: %s <puerto>\n\n",argv[0]);
    exit(EXIT_FAILURE);
  }

  // Creamos el socket del servidor
	descriptor_socket_servidor = socket(AF_INET,SOCK_STREAM,0);
	if (descriptor_socket_servidor == -1) {
    printf("ERROR: El socket del servidor no se ha creado correctamente!\n");
    exit(EXIT_FAILURE);
  }

  //Se prepara la dirección de la máquina servidora
  socket_servidor.sin_family = AF_INET;
  socket_servidor.sin_port = htons(atoi(argv[1]));
  socket_servidor.sin_addr.s_addr = htonl(INADDR_ANY);

  //Asigna una direccion local al socket
	if( bind(descriptor_socket_servidor, (struct sockaddr*)&socket_servidor, sizeof(socket_servidor)) == -1) {
    printf("ERROR al configurar el socket del servidor\n");
    close(descriptor_socket_servidor);
    exit(EXIT_FAILURE);
  }

  //Espera al establecimiento de alguna conexión de multiples usuarios
  if( listen(descriptor_socket_servidor, MAX_CONN) == -1) {
    printf("ERROR al establecer la escucha de N conecciones en el servidor\n");
    close(descriptor_socket_servidor);
    exit(EXIT_FAILURE);
  }

  signal(SIGINT, &catch);
  while(1){
    printf("\n***Servidor ACTIVO escuchando en el puerto: %s ...\n",argv[1]);
    //Establece una conexión
    destino_tam=sizeof(socket_cliente);
    descriptor_socket_cliente = accept(descriptor_socket_servidor, (struct sockaddr*)&socket_cliente, &destino_tam);
    if(descriptor_socket_cliente == -1) {
      printf("ERROR al establecer la conexion del servidor con el cliente\n");
      close(descriptor_socket_servidor);
      exit(EXIT_FAILURE);
    }
    printf("***Servidor se conecto con el cliente: %d.\n",socket_cliente.sin_addr.s_addr);
    do {
      //Recibe el mensaje del cliente
      if (recv(descriptor_socket_cliente, mensaje_entrada, sizeof(mensaje_entrada), 0) == -1) {
        perror("Error en recv");
        close(descriptor_socket_cliente);
        close(descriptor_socket_servidor);
        exit(EXIT_SUCCESS);
      } else
			   printf("<<Client envía >>: %s\n", mensaje_entrada);

      //Envia el mensaje al cliente
		  sprintf(mensaje_salida, "El mensaje recibido fue --- %s ---.",mensaje_entrada);
		  if (send(descriptor_socket_cliente, strcat(mensaje_salida,"\0"), strlen(mensaje_salida)+1, 0) == -1) {
        perror("Error en send");
        close(descriptor_socket_cliente);
        close(descriptor_socket_servidor);
        exit(EXIT_SUCCESS);
      } else
        printf("<<Server replica>>: %s\n", mensaje_salida);
    }while(strcmp(mensaje_entrada,"terminar();") != 0);

    //Cierra la conexión con el cliente actual
    printf("***Cerrando conexion con cliente ...\n");
    close(descriptor_socket_cliente);
    printf("***conexion cerrada.\n");
  }
  //Cierra el servidor
  printf("***Cerrando servicio ...\n");
  close(descriptor_socket_servidor);
  printf("***Servicio cerrado.\n");
  exit(EXIT_SUCCESS);
}