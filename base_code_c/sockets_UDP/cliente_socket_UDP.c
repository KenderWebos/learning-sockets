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
#include <netdb.h>


#define MAX_TAM_MENSAJE 50 //Numero de caracteres maximo del mensaje

char mensaje[MAX_TAM_MENSAJE];

/**********************************************************/
/* función catch que captura una interrupción             */
/**********************************************************/
void catch(int sig)
{
	strcpy(mensaje,"terminar();");
}

/**********************************************************/
/* función MAIN                                           */
/* Orden Parametros: IP destino, puerto                   */
/**********************************************************/
int main(int argc, char *argv[]) {
	int descriptor_socket_origen;
	struct sockaddr_in socket_origen, socket_destino;
    socklen_t destino_tam;
	char respuesta[MAX_TAM_MENSAJE]; mensaje[MAX_TAM_MENSAJE];
    int recibidos, enviados; //bytes recibidos y enviados


 	if (argc != 3) {
    printf("\n\nEl número de parámetros es incorrecto\n");
    printf("Use: %s <IP_servidor> <puerto>\n\n",argv[0]);
    exit(EXIT_FAILURE);
  }

	// Creamos el socket del cliente
	descriptor_socket_origen = socket(AF_INET,SOCK_DGRAM,0);
	if (descriptor_socket_origen == -1) {
    printf("ERROR: El socket del cliente no se ha creado correctamente!\n");
    exit(EXIT_FAILURE);
  }

  //Se prepara la dirección de la máquina cliente
  socket_origen.sin_family = AF_INET;
	socket_origen.sin_port = htons(0); 								//Asigna un puerto disponible dela máquina
	socket_origen.sin_addr.s_addr = htonl(INADDR_ANY); //Asigna una IP de la máquina

	//Asigna una direccion local al socket
	if( bind(descriptor_socket_origen, (struct sockaddr*)&socket_origen, sizeof(socket_origen)) == -1) {
    printf("ERROR al unir el socket a la dirección de la máquina cliente\n");
    close(descriptor_socket_origen);
    exit(EXIT_FAILURE);
  }

  //Se prepara la dirección de la máquina servidora
	socket_destino.sin_family = AF_INET;
	socket_destino.sin_addr.s_addr = inet_addr(argv[1]);
	socket_destino.sin_port = htons(atoi(argv[2]));



 signal(SIGINT, &catch);

  do {

    //Envía el mensaje
    printf("<<Client>>: ");
    scanf("%s",mensaje);
    enviados = sendto(descriptor_socket_origen, mensaje, MAX_TAM_MENSAJE, 0, (struct sockaddr*)&socket_destino, sizeof(socket_destino));

    if(enviados<0) {
      printf("ERROR en sento() \n");
      close(descriptor_socket_origen);
      exit(EXIT_FAILURE);
    } else {
      printf("***Cliente envía: %s a: %s en puerto: %s \n",mensaje, argv[1], argv[2]);
    }

	  //Recibe la respuesta
    destino_tam=sizeof(socket_destino);
    recibidos = recvfrom(descriptor_socket_origen, respuesta, sizeof(respuesta), 0, (struct sockaddr*)&socket_destino, &destino_tam);
    if(recibidos<0) {
      printf("ERROR en recvfrom() \n");
      close(descriptor_socket_origen);
      exit(EXIT_FAILURE);
    } else {
      printf("<<Server>>: %s\n", respuesta);
    }

    //Se cierra la conexión (socket)
    if(strcmp(mensaje,"terminar();") == 0) {
      printf("***Cliente terminó conección con servidor.\n");
      close(descriptor_socket_origen);
      exit(EXIT_FAILURE);
    }

  } while(1);
	//Se cierra la conexión (socket)
  printf("\nCliente termina.\n");
	close(descriptor_socket_origen);
  exit(EXIT_SUCCESS);
}