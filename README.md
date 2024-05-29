## 🌐 Sockets y Aplicaciones de Sockets

### 🔍 ¿Qué son los Sockets?

- **Definición:** Un socket es un punto final de una conexión de red.
- **Función:** Permiten la comunicación entre dos dispositivos a través de una red (por ejemplo, Internet).

### 📑 Tipos de Sockets

1. **Sockets de Dominio:** Para comunicación en una misma máquina.
2. **Sockets de Red:** Para comunicación entre dispositivos en una red.

### ⚙️ ¿Cómo Funcionan?

- **Cliente-Servidor:** 
  - **Servidor:** Escucha solicitudes en un puerto específico.
  - **Cliente:** Envía solicitudes al servidor y espera respuestas.

- **Protocolos Comunes:**
  - **TCP (Transmission Control Protocol):** Conexión fiable, orientada a conexión.
  - **UDP (User Datagram Protocol):** Conexión no fiable, sin conexión.

### 🌟 Aplicaciones de Sockets

- **Web:** Navegadores y servidores web.
- **Mensajería:** Aplicaciones de chat y correo electrónico.
- **Juegos en línea:** Comunicación en tiempo real entre jugadores.
- **IoT (Internet de las Cosas):** Dispositivos inteligentes comunicándose entre sí.

### 📈 Importancia en la Tecnología

- **Comunicación Eficiente:** Facilitan la transmisión de datos de manera rápida y fiable.
- **Versatilidad:** Utilizados en una amplia gama de aplicaciones y servicios.
- **Conectividad Global:** Permiten la interacción entre dispositivos a nivel mundial.

### 📚 Ejemplos de Uso

- **Navegadores web:** Conectan a servidores para cargar páginas web.
- **Servicios de Streaming:** Transmiten audio y video en tiempo real.
- **Aplicaciones de Videollamadas:** Permiten la comunicación en tiempo real.

---

### 🔁 Visualización de la Comunicación con Sockets

```mermaid
graph TD
    Cliente -->|Solicitud| Servidor
    Servidor -->|Respuesta| Cliente
