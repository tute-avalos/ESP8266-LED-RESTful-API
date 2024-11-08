const baseUrl = 'http://';

// Función para obtener todos los LEDs
async function fetchLEDs() {
  try {
    const ip = document.getElementById("server-ip").value;
    const response = await fetch(`${baseUrl+ip}/led`);
    const data = await response.json();
    displayLEDList(data);
  } catch (error) {
    console.error("Error al obtener los LEDs:", error);
  }
}

// Muestra la lista de LEDs
function displayLEDList(leds) {
  const ledList = document.getElementById('led-list');
  ledList.innerHTML = leds.map(led => `
        <div>ID: ${led.id}, Pin: ${led.pin}, Lógica: ${led.logica}, Estado: ${led.estado}</div>
    `).join('');
}

// Función para obtener un LED por ID
async function fetchLEDById() {
  const id = document.getElementById('led-id').value;
  try {
    const ip = document.getElementById("server-ip").value;
    const response = await fetch(`${baseUrl+ip}/led/${id}`);
    const led = await response.json();
    document.getElementById('led-details').innerHTML = `<div>ID: ${led.id}, Pin: ${led.pin}, Lógica: ${led.logica}, Estado: ${led.estado}</div>`;
  } catch (error) {
    console.error("Error al obtener el LED:", error);
  }
}

// Función para crear un nuevo LED
async function createLED() {
  const pin = document.getElementById('new-led-pin').value;
  const logica = document.getElementById('new-led-logica').value;
  try {
    const ip = document.getElementById("server-ip").value;
    const response = await fetch(`${baseUrl+ip}/led`, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json'
      },
      body: JSON.stringify({ pin: parseInt(pin), logica: logica })
    });
    const led = await response.json();
    document.getElementById('creation-result').innerHTML = `<div>ID: ${led.id}, Pin: ${led.pin}, Lógica: ${led.logica}, Estado: ${led.estado}</div>`;;
  } catch (error) {
    console.error("Error al crear el LED:", error);
  }
}

// Función para actualizar el estado del LED
async function updateLED() {
  const id = document.getElementById('update-led-id').value;
  const estado = document.getElementById('update-led-estado').value;
  try {
    const ip = document.getElementById("server-ip").value;
    const response = await fetch(`${baseUrl+ip}/led/${id}`, {
      method: 'PATCH',
      headers: {
        'Content-Type': 'application/json'
      },
      body: JSON.stringify({ estado: estado })
    });
    const data = await response.json();
    document.getElementById('update-result').innerHTML = JSON.stringify(data);
  } catch (error) {
    console.error("Error al actualizar el estado del LED:", error);
  }
}

// Función para eliminar un LED
async function deleteLED() {
  const id = document.getElementById('delete-led-id').value;
  try {
    const ip = document.getElementById("server-ip").value;
    const response = await fetch(`${baseUrl+ip}/led/${id}`, {
      method: 'DELETE'
    });
    const data = await response.json();
    document.getElementById('deletion-result').innerHTML = JSON.stringify(data);
  } catch (error) {
    console.error("Error al eliminar el LED:", error);
  }
}
