import socket
import threading
import time

# Configuración del servidor
HOST = '0.0.0.0'
PORT1 = 8001
PORT2 = 8002

clients = {}
latency_timer = None  # Guardará el tiempo cuando esp1 envía un '1'

def handle_client(client_socket, peer_key):
    global latency_timer

    while True:
        try:
            data = client_socket.recv(1024).decode().strip()
            if data in ['0', '1', '-1']:
                print(f"Recibido de {peer_key}: {data}")
                
                if peer_key == 'esp1':
                    if data == '1':
                        latency_timer = time.time()
                    if 'esp2' in clients:
                        clients['esp2'].sendall(data.encode())

                elif peer_key == 'esp2':
                    if data == '1' and latency_timer:
                        latencia = (time.time() - latency_timer) * 1000
                        print(f"latencia: {latencia:.2f} ms")
                        latency_timer = None
                    if 'esp1' in clients:
                        clients['esp1'].sendall(data.encode())

            else:
                print(f"Mensaje ignorado de {peer_key}: {data}")
        except:
            print(f"Conexión perdida con {peer_key}")
            client_socket.close()
            clients.pop(peer_key, None)
            break

def start_server(port, peer_key):
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.bind((HOST, port))
    server.listen(1)
    print(f"[ESPERANDO] ESP32 ({peer_key}) en puerto {port}...")
    conn, addr = server.accept()
    print(f"[CONECTADO] {peer_key} desde {addr}")
    clients[peer_key] = conn
    threading.Thread(target=handle_client, args=(conn, peer_key), daemon=True).start()

# Iniciar servidores
threading.Thread(target=start_server, args=(PORT1, 'esp1'), daemon=True).start()
threading.Thread(target=start_server, args=(PORT2, 'esp2'), daemon=True).start()

# Mantener servidor corriendo
while True:
    time.sleep(1)
