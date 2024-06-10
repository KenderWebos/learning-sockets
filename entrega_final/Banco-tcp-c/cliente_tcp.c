/* CLIENTE */
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

#define MAX_TAM_MENSAJE 256 // Número de caracteres máximo del mensaje

int descriptor_socket_cliente;
char mensaje[MAX_TAM_MENSAJE];
char rut[12], clave[20];
int autenticado = 0;

/**********************************************************/
/* función catch que captura una interrupción             */
/**********************************************************/
void catch(int sig)
{
    strcpy(mensaje, "terminar();");
}

/**********************************************************/
/* función MAIN                                           */
/* Orden Parámetros: IP destino, puerto                   */
/**********************************************************/
int main(int argc, char *argv[])
{
    struct sockaddr_in socket_destino;
    char respuesta[MAX_TAM_MENSAJE];
    int recibidos, enviados; // bytes recibidos y enviados

    if (argc != 3)
    {
        printf("\n\nEl número de parámetros es incorrecto\n");
        printf("Use: %s <IP_servidor> <puerto>\n\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Creamos el socket del cliente
    descriptor_socket_cliente = socket(AF_INET, SOCK_STREAM, 0);
    if (descriptor_socket_cliente == -1)
    {
        printf("ERROR: El socket del cliente no se ha creado correctamente!\n");
        exit(EXIT_FAILURE);
    }

    // Se prepara la dirección de la máquina servidora
    socket_destino.sin_family = AF_INET;
    socket_destino.sin_addr.s_addr = inet_addr(argv[1]);
    socket_destino.sin_port = htons(atoi(argv[2]));

    // Establece una conexión con el servidor
    if (connect(descriptor_socket_cliente, (struct sockaddr *)&socket_destino, sizeof(socket_destino)) == -1) {
        printf("ERROR al conectarse al servidor\n");
        close(descriptor_socket_cliente);
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, &catch);

    while (1)
    {
        // Mostrar menú de opciones al usuario
        int opcion;
        printf("\nOpciones:\n");
        printf("1. Registrar usuario\n");
        printf("2. Autenticar usuario\n");
        printf("3. Consultar saldo\n");
        printf("4. Transferir fondos\n");
        printf("5. Cerrar sesión\n");
        printf("6. Salir\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);

        switch (opcion)
        {
            case 1:
                {
                    printf("Ingrese RUT: ");
                    scanf("%s", rut);
                    printf("Ingrese clave: ");
                    scanf("%s", clave);
                    float saldo_inicial;
                    printf("Ingrese saldo inicial: ");
                    scanf("%f", &saldo_inicial);
                    sprintf(mensaje, "REGISTRAR %s %s %.2f", rut, clave, saldo_inicial);
                }
                break;
            case 2:
                {
                    
                    printf("Ingrese RUT: ");
                    scanf("%s", rut);
                    printf("Ingrese clave: ");
                    scanf("%s", clave);
                    sprintf(mensaje, "AUTENTICAR %s %s", rut, clave);
                }
                break;
            case 3:
                {
                    if (autenticado)
                    {
                        sprintf(mensaje, "CONSULTAR_SALDO %s %s", rut, clave);
                    }
                    else
                    {
                        printf("Debe autenticarse primero.\n");
                        continue;
                    }
                }
                break;
            case 4:
                {
                    if (autenticado)
                    {
                        char rut_destino[12];
                        float monto;
                        printf("Ingrese RUT del destinatario: ");
                        scanf("%s", rut_destino);
                        printf("Ingrese monto a transferir: ");
                        scanf("%f", &monto);
                        sprintf(mensaje, "TRANSFERIR %s %s %s %.2f", rut, clave, rut_destino, monto);
                    }
                    else
                    {
                        printf("Debe autenticarse primero.\n");
                        continue;
                    }
                }
                break;
            case 5: // Cerrar sesión
                if (autenticado)
                {
                    autenticado = 0;
                    printf("Sesión cerrada. Debe autenticarse nuevamente para realizar operaciones.\n");
                }
                else
                {
                    printf("No hay sesión activa.\n");
                }
                continue;
            case 6:
                printf("Saliendo...\n");
                strcpy(mensaje, "terminar();");
                break;
            default:
                printf("Opción no válida. Intente de nuevo.\n");
                continue;
        }

        // Envía el mensaje al servidor
        enviados = send(descriptor_socket_cliente, mensaje, strlen(mensaje), 0);
        if (enviados < 0)
        {
            printf("ERROR en send() \n");
            close(descriptor_socket_cliente);
            exit(EXIT_FAILURE);
        }

        // Recibe la respuesta del servidor
        recibidos = recv(descriptor_socket_cliente, respuesta, sizeof(respuesta), 0);
        if (recibidos < 0)
        {
            printf("ERROR en recv() \n");
            close(descriptor_socket_cliente);
            exit(EXIT_FAILURE);
        }
        else if (recibidos == 0)
        {
            printf("Conexión cerrada por el servidor\n");
            close(descriptor_socket_cliente);
            exit(EXIT_SUCCESS);
        }

        respuesta[recibidos] = '\0'; // Asegurar la terminación de la cadena
        printf("<<Servidor>>: %s\n", respuesta);

        // Verificar autenticación exitosa
        if (opcion == 2 && strncmp(respuesta, "Usuario", 7) == 0 && strstr(respuesta, "autenticado correctamente"))
        {
            autenticado = 1;
        }

        // Salir si el mensaje es "terminar();"
        if (strcmp(mensaje, "terminar();") == 0) {
            break;
        }
    }

    // Se cierra la conexión (socket)
    printf("\nCliente termina.\n");
    close(descriptor_socket_cliente);
    exit(EXIT_SUCCESS);
}