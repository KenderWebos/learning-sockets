import socket
import threading

# Datos del servidor
HOST = '127.0.0.1'  # Localhost
PORT = 65432

# Listas para los clientes y sus identificadores
clients = []
client_ids = []

# Función para manejar clientes
def handle_client(conn, addr, client_id):
    print(f"Nuevo cliente conectado: {addr} con ID {client_id}")
    conn.send(f"Bienvenido al chat! Tu ID es {client_id}\n".encode('utf-8'))
    while True:
        try:
            message = conn.recv(1024)
            if not message:
                break
            broadcast(message, conn, client_id)
        except:
            break
    # Eliminar cliente cuando se desconecta
    remove_client(conn, client_id)

def broadcast(message, conn, client_id):
    for client in clients:
        if client != conn:
            try:
                client.send(f"Cliente {client_id}: {message.decode('utf-8')}".encode('utf-8'))
            except:
                remove_client(client, client_ids[clients.index(client)])

def remove_client(conn, client_id):
    if conn in clients:
        index = clients.index(conn)
        clients.remove(conn)
        client_ids.remove(client_id)
        conn.close()
        print(f"Cliente {client_id} se ha desconectado.")

def main():
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.bind((HOST, PORT))
    server.listen()
    print(f"Servidor escuchando en {HOST}:{PORT}")

    client_counter = 1

    while True:
        conn, addr = server.accept()
        clients.append(conn)
        client_ids.append(client_counter)
        thread = threading.Thread(target=handle_client, args=(conn, addr, client_counter))
        thread.start()
        client_counter += 1

if __name__ == "__main__":
    main()
