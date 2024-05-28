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

int descriptor_socket_servidor;

/**********************************************************/
/* función catch que captura una interrupción             */
/**********************************************************/
void catch(int sig)
{
	printf("***Señal: %d atrapada!\n", sig);
  printf("***Cerrando servicio ...\n");
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
  int recibidos, enviados; //bytes recibidos

  if (argc != 2) {
    printf("\n\nEl número de parámetros es incorrecto\n");
    printf("Use: %s <puerto>\n\n",argv[0]);
    exit(EXIT_FAILURE);
  }

  // Creamos el socket del servidor
	descriptor_socket_servidor = socket(AF_INET, SOCK_DGRAM,0);
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
    printf("ERROR al unir el socket a la dirección de la máquina servidora\n");
    close(descriptor_socket_servidor);
    exit(EXIT_FAILURE);
  }



  signal(SIGINT, &catch);

    printf("\n***Servidor ACTIVO escuchando en el puerto: %s ...\n",argv[1]);  

    do {
    //Quedo a la espera de algun mensaje
    destino_tam=sizeof(socket_cliente);
    recibidos= recvfrom(descriptor_socket_servidor, mensaje_entrada, MAX_TAM_MENSAJE, 0, (struct sockaddr*)&socket_cliente, &destino_tam);
    if(recibidos <0) {
      printf("ERROR de recvfrom() \n");
      exit(EXIT_FAILURE);
    } else
    {
    printf("***Servidor recibe dato del cliente: %d.\n",socket_cliente.sin_addr.s_addr);
	printf("<<Client envía >>: %s\n", mensaje_entrada);
    }
      //Envia el mensaje al cliente
	sprintf(mensaje_salida, "El mensaje recibido fue --- %s ---.",mensaje_entrada);

    enviados = sendto(descriptor_socket_servidor, strcat(mensaje_salida,"\0"), MAX_TAM_MENSAJE, 0, (struct sockaddr*)&socket_cliente, destino_tam);
		  if (enviados<0) {
        printf("Error en sendto() \n");
        exit(EXIT_SUCCESS);
      } else
        printf("<<Server replica>>: %s\n", mensaje_salida);
    }while(1);    
    

  //Cierra el servidor
  printf("***Cerrando servicio ...\n");
  close(descriptor_socket_servidor);
  printf("***Servicio cerrado.\n");
  exit(EXIT_SUCCESS);
}