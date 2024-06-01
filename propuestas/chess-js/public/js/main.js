// const socket = io();

// const chat = document.getElementById("chat");

// const online_users = document.getElementById("online_users");

// const messageForm = document.getElementById("message-form");
// const messageInput = document.getElementById("message-input");

// let userId = null;

// socket.on("chatMessage", (data) => {
//     const item = document.createElement("li");
//     item.textContent = `${data.userId}: ${data.msg}`;
//     chat.appendChild(item);
//     chat.scrollTop = chat.scrollHeight;
// });

// socket.on("online_users_feed", (data) => {
//     online_users.textContent = `${data}`;
// });

// messageForm.addEventListener("submit", (event) => {
//     event.preventDefault();
//     const msg = messageInput.value;
//     socket.emit("chatMessage", msg);
//     messageInput.value = "";
// });