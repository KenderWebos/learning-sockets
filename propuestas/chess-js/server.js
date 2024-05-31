const express = require('express');
const http = require('http');
const socketIo = require('socket.io');

// Configuración del servidor
const app = express();
const server = http.createServer(app);
const io = socketIo(server);

const animalEmojis = ['🐶', '🐱', '🦊', '🐻', '🐼', '🐨', '🐯', '🦁', '🐮', '🐷', '🐸', '🐵'];
let nextUserId = 0;

// Manejador de conexiones
io.on('connection', (socket) => {
  const userId = animalEmojis[nextUserId % animalEmojis.length] + '[' + nextUserId + ']';
  nextUserId++;
  console.log(`Usuario ${userId} conectado`);

  // Manejar mensajes de chat
  socket.on('chatMessage', (msg) => {
    io.emit('chatMessage', { userId, msg });
  });

  // Manejar la desconexión
  socket.on('disconnect', () => {
    console.log(`Usuario ${userId} desconectado`);
  });
});

// Servir archivos estáticos
app.use(express.static('public'));

// Iniciar el servidor
const PORT = process.env.PORT || 3030;
server.listen(PORT, () => {
  console.log(`Servidor escuchando en el puerto ${PORT}`);
  console.log(`http://localhost:${PORT}`);
});
