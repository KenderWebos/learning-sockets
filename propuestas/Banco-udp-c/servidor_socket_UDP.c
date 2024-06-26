/* SERVIDOR */
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

#define MAX_TAM_MENSAJE 256 // Número de caracteres máximo del mensaje
#define MAX_USUARIOS 100

typedef struct {
    char rut[12];
    char clave[20];
    float saldo;
} Usuario;

Usuario usuarios[MAX_USUARIOS];
int num_usuarios = 0;
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
/* funciones auxiliares                                   */
/**********************************************************/
int buscar_usuario(char *rut)
{
    for (int i = 0; i < num_usuarios; i++) {
        if (strcmp(usuarios[i].rut, rut) == 0) {
            return i;
        }
    }
    return -1;
}

void registrar_usuario(char *rut, char *clave, float saldo_inicial)
{
    strcpy(usuarios[num_usuarios].rut, rut);
    strcpy(usuarios[num_usuarios].clave, clave);
    usuarios[num_usuarios].saldo = saldo_inicial;
    num_usuarios++;
}

int autenticar_usuario(char *rut, char *clave)
{
    int index = buscar_usuario(rut);
    if (index != -1 && strcmp(usuarios[index].clave, clave) == 0) {
        return index;
    }
    return -1;
}

void manejar_mensaje(char *mensaje_entrada, char *mensaje_salida)
{
    char comando[20], rut[12], clave[20];
    float monto;
    int usuario_index;

    sscanf(mensaje_entrada, "%s", comando);

    if (strcmp(comando, "REGISTRAR") == 0) {
        sscanf(mensaje_entrada, "%s %s %s %f", comando, rut, clave, &monto);
        if (buscar_usuario(rut) == -1) {
            registrar_usuario(rut, clave, monto);
            sprintf(mensaje_salida, "Usuario %s registrado con saldo inicial %.2f", rut, monto);
        } else {
            sprintf(mensaje_salida, "Error: Usuario %s ya existe", rut);
        }
    } else if (strcmp(comando, "AUTENTICAR") == 0) {
        sscanf(mensaje_entrada, "%s %s %s", comando, rut, clave);
        usuario_index = autenticar_usuario(rut, clave);
        if (usuario_index != -1) {
            sprintf(mensaje_salida, "Usuario %s autenticado correctamente", rut);
        } else {
            sprintf(mensaje_salida, "Error: Autenticación fallida para %s", rut);
        }
    } else if (strcmp(comando, "CONSULTAR_SALDO") == 0) {
        sscanf(mensaje_entrada, "%s %s %s", comando, rut, clave);
        usuario_index = autenticar_usuario(rut, clave);
        if (usuario_index != -1) {
            sprintf(mensaje_salida, "Saldo de %s: %.2f", rut, usuarios[usuario_index].saldo);
        } else {
            sprintf(mensaje_salida, "Error: Autenticación fallida para %s", rut);
        }
    } else if (strcmp(comando, "TRANSFERIR") == 0) {
        char rut_destino[12];
        sscanf(mensaje_entrada, "%s %s %s %s %f", comando, rut, clave, rut_destino, &monto);
        usuario_index = autenticar_usuario(rut, clave);
        int destino_index = buscar_usuario(rut_destino);
        if (usuario_index != -1 && destino_index != -1 && usuarios[usuario_index].saldo >= monto) {
            usuarios[usuario_index].saldo -= monto;
            usuarios[destino_index].saldo += monto;
            sprintf(mensaje_salida, "Transferencia de %.2f de %s a %s realizada con éxito", monto, rut, rut_destino);
        } else {
            sprintf(mensaje_salida, "Error en la transferencia: Verifique datos y saldo");
        }
    } else {
        sprintf(mensaje_salida, "Comando no reconocido");
    }
}

/**********************************************************/
/* función MAIN                                           */
/* Orden Parámetros: Puerto                               */
/**********************************************************/
int main(int argc, char *argv[])
{
    socklen_t destino_tam;
    struct sockaddr_in socket_servidor, socket_cliente;
    char mensaje_entrada[MAX_TAM_MENSAJE], mensaje_salida[MAX_TAM_MENSAJE];
    int recibidos, enviados; // bytes recibidos

    if (argc != 2)
    {
        printf("\n\nEl número de parámetros es incorrecto\n");
        printf("Use: %s <puerto>\n\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Creamos el socket del servidor
    descriptor_socket_servidor = socket(AF_INET, SOCK_DGRAM, 0);
    if (descriptor_socket_servidor == -1)
    {
        printf("ERROR: El socket del servidor no se ha creado correctamente!\n");
        exit(EXIT_FAILURE);
    }

    // Se prepara la dirección de la máquina servidora
    socket_servidor.sin_family = AF_INET;
    socket_servidor.sin_port = htons(atoi(argv[1]));
    socket_servidor.sin_addr.s_addr = htonl(INADDR_ANY);

    // Asigna una dirección local al socket
    if (bind(descriptor_socket_servidor, (struct sockaddr *)&socket_servidor, sizeof(socket_servidor)) == -1)
    {
        printf("ERROR al unir el socket a la dirección de la máquina servidora\n");
        close(descriptor_socket_servidor);
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, &catch);

    printf("\n***Servidor ACTIVO escuchando en el puerto: %s ...\n", argv[1]);

    do
    {
        // Quedo a la espera de algún mensaje
        destino_tam = sizeof(socket_cliente);
        recibidos = recvfrom(descriptor_socket_servidor, mensaje_entrada, MAX_TAM_MENSAJE, 0, (struct sockaddr *)&socket_cliente, &destino_tam);
        if (recibidos < 0)
        {
            printf("ERROR de recvfrom() \n");
            exit(EXIT_FAILURE);
        }
        else
        {
            printf("***Servidor recibe dato del cliente: %d.\n", socket_cliente.sin_addr.s_addr);
            printf("<<Cliente envía >>: %s\n", mensaje_entrada);
        }

        // Manejar el mensaje del cliente
        manejar_mensaje(mensaje_entrada, mensaje_salida);

        // Envia el mensaje al cliente
        enviados = sendto(descriptor_socket_servidor, strcat(mensaje_salida, "\0"), MAX_TAM_MENSAJE, 0, (struct sockaddr *)&socket_cliente, destino_tam);
        if (enviados < 0)
        {
            printf("Error en sendto() \n");
            exit(EXIT_SUCCESS);
        }
        else
            printf("<<Servidor replica>>: %s\n", mensaje_salida);
    } while (1);

    // Cierra el servidor
    printf("***Cerrando servicio ...\n");
    close(descriptor_socket_servidor);
    printf("***Servicio cerrado.\n");
    exit(EXIT_SUCCESS);
}
