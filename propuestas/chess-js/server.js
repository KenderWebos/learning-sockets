const express = require('express');
const http = require('http');
const socketIo = require('socket.io');

// Configuración del servidor
const app = express();
const server = http.createServer(app);
const io = socketIo(server);

const animalEmojis = ['🐶', '🐱', '🦊', '🐻', '🐼', '🐨', '🐯', '🦁', '🐮', '🐷', '🐸', '🐵'];
let nextUserId = 0;

online_users = [];
offline_users = [];
chessing_users = [];

// Manejador de conexiones
io.on('connection', (socket) => {
  const userId = animalEmojis[nextUserId % animalEmojis.length] + '[' + nextUserId + ']';
  nextUserId++;
  console.log(`Usuario ${userId} conectado`);
  online_users.push(userId);

  socket.emit('userId', userId);
  io.emit('online_users_feed', online_users);
  io.emit('join-chess', chessing_users);

  printInfo();

  // Manejar mensajes de chat
  socket.on('chatMessage', (msg) => {
    io.emit('chatMessage', { userId, msg });
  });

  socket.on('join-chess', (data) => {
    if (chessing_users.length < 2 && !chessing_users.includes(data)) {
      chessing_users.push(data);
      io.emit('join-chess', chessing_users);

      //Resetea el tablero si hay 2 players
      if (chessing_users.length === 2) {
        io.emit('reset-board');
      }

      printInfo();
    }
  });

  socket.on('move', (data) => {
    socket.broadcast.emit('move', data);
  });

  // Manejar la desconexión
  socket.on('disconnect', () => {
    console.log(`Usuario ${userId} desconectado`);

    offline_users.push(userId);
    online_users.splice(online_users.indexOf(userId), 1);

    // Remove from chessing_users if disconnects
    const index = chessing_users.indexOf(userId);
    if (index !== -1) {
      chessing_users.splice(index, 1);
    }

    printInfo();

    io.emit('online_users_feed', online_users);
    io.emit('join-chess', chessing_users);
  });
});

function printInfo() {
  console.log('---');
  console.log(`> usuarios conectados: [ ${online_users} ]`);
  console.log(`> usuarios desconectados: [ ${offline_users} ]`);
  console.log(`> chessing players: [ ${chessing_users} ]`);
  console.log('---');
}

// Servir archivos estáticos
app.use(express.static('public'));

// Iniciar el servidor
const PORT = process.env.PORT || 3030;
server.listen(PORT, () => {
  console.log(`Servidor escuchando en el puerto ${PORT}`);
  console.log(`http://localhost:${PORT}`);
});
