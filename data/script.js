//let ws = new WebSocket('ws://' + window.location.hostname + ':81/');

function initWebSocket() {
  ws = new WebSocket('ws://' + window.location.hostname + '/ws');
  // ws.onopen = function(event) { console.log('Connected to WebSocket'); };
  // ws.onclose = function(event) { console.log('Disconnected from WebSocket'); };
  // ws.onmessage = function(event) { handleWebSocketMessage(JSON.parse(event.data)); };
}

ws.onmessage = function(event) {
    let data = JSON.parse(event.data);
    console.log("Message from server: ", data);

    // Update Last race table
    let lastRaceTable = document.getElementById("lastRaceTable");
    lastRaceTable.innerHTML = '<tr><th>Pilot Name</th><th>Channel Band</th><th>Channel Number</th></tr>' + data.lastRace;
    
    // Update current race table
    let currentRaceTable = document.getElementById("currentRaceTable");
    currentRaceTable.innerHTML = '<tr><th>Pilot Name</th><th>Lap Number</th><th>Lap Times</th><th>Position</th></tr>' + data.currentRace;

};

ws.onopen = function() {
  console.log("WebSocket connection established");
};

ws.onclose = function() {
  console.log("WebSocket connection closed");
};

window.onload = function(event) {
  initWebSocket();
}