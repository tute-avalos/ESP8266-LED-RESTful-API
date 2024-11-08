/**
 * @file main.cpp
 * @author Prof. Tute Ávalos (tute-avalos.com)
 * @brief LED API RESTful
 * @version 0.1
 * @date 2024-11-08
 * 
 * @copyright Copyright (c) 2024
 * 
 * Copyright (C) 2024 Matías S. Ávalos (tute-avalos.com)
 *
 * This file is part of LED API RESTful.
 * 
 * LED API RESTful is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * LED API RESTful is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with LED API RESTful.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "led.hpp"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>

#ifndef SBAUDRATE
#define SBAUDRATE (115200)
#endif

#ifndef WIFI_SSID
#define WIFI_SSID "LED RESTful API"
#endif
#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD "asdf1234"
#endif

// Información de la Red
const char *SSID{WIFI_SSID};
const char *PSWD{WIFI_PASSWORD};

// Mensaje a devolverle al cliente.
const char *LIMIT_REACHED = "{\"error\":\"Límite de LEDs alcanzado.\"}\n";
const char *INVALID_JSON = "{\"error\":\"JSON inválido.\"}\n";
const char *INVALID_STATE = "{\"error\":\"'estado' inválido.\"}\n";
const char *NOT_FOUND = "{\"error\":\"'id' no encontrado.\"}\n";
const char *MISSING_PIN_LOGIC =
    "{\"error\":\"Faltan campos obligatorios: 'pin' y 'logica'.\"}\n";
const char *MISSING_LED_STATE =
    "{\"error\":\"Faltan el campo obligatorio: 'estado'.\"}\n";

// Cantidad máxima de LEDs a crear/registrar.
const size_t MAX_LEDS{5};

// Servidor Web ejecutandose en el puerto 80
AsyncWebServer server{80};

// Arreglo de LEDs a crear y manipular
Led *leds[MAX_LEDS]{};
size_t cantLedsActivos{0};

/**
 * @brief Obtiene la información del body proveniente de un request
 *
 * @param data uint8_t* datos que llegaron del request
 * @param len size_t longitud de los datos
 * @param body String donde ir acumulando los datos
 */
void handleRequestBody(uint8_t *data, size_t len, String &body) {
  body.reserve(body.length() + len);
  for (size_t i = 0; i < len; ++i) body += char(data[i]);
}

/**
 * @brief Endpoints GET /led y /led/{n}
 * 
 * En caso de solicitar /led se devuelve una lista en formato JSON de los LEDs
 * creados hasta el momento.
 * 
 * En caso de solicitar /led/{n}, n indica el id del LED solicitado y devuelve
 * el JSON del estado acutal del LED (200) o un mensaje que no pudo encontrarse
 * dicho LED (404).
 * 
 * @param req AsyncWebServerRequest* solicitud recibida por el cliente
 */
void readLed(AsyncWebServerRequest *req) {
  if (req->pathArg(0) != "") {
    // *** Si se pidió un solo LED ***
    int id{req->pathArg(0).substring(1).toInt()};
    //                    ^^^^^^^^^^^^^
    // en pathArg(0) hay un string del tipo "/X" donde X es un número entero.
    // por eso se filtra el '/' inicial utilizando substring().

    Led *led_found = nullptr;
    for (auto &led : leds) {
      if (led != nullptr && led->getID() == id) {
        led_found = led;
        break;
      }
    }
    if (led_found) req->send(200, "application/json", led_found->toJson() + '\n');
    else req->send(404, "application/json", NOT_FOUND);

  } else {
    // *** Si se pidieron todos los LEDs ***
    String response = "[";
    for (auto &led : leds) {
      if (led != nullptr) response += led->toJson() + ',';
    }

    if (response.endsWith(",")) response.remove(response.length() - 1);

    response += "]\n";
    req->send(200, "application/json", response);
  }
}

/**
 * @brief Endpoint POST /led
 * 
 * Se recibe un json con el formato {"pin": n, "logica": l} donde 'n' es el número del
 * pin donde se encuentra conectado el LED y 'l' es la lógica que utiliza para encender:
 * 
 *    - "HIGH": El LED enciende con HIGH (ánodo conectado al pin y cátodo a GND)
 *    - "LOW": El LED enciende con LOW (cátodo conectado al pin y ánodo a +V)
 * 
 * @param req AsyncWebServerRequest* request del cliente
 * @param data uint8_t* información del body del paquete actual
 * @param len size_t longitud del paquete actual
 * @param index size_t indice del paquete actual
 * @param total size_t tamaño total del body
 */
void createLed(AsyncWebServerRequest *req, uint8_t *data, size_t len,
               size_t index, size_t total) {
  static String body{};
  if (!index) body = "";

  handleRequestBody(data, len, body);
  if ((index + len) == total) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, body);
    if (error) {
      req->send(400, "application/json", INVALID_JSON);
      return;
    }
    if (!doc["pin"].is<int>() || !doc["logica"].is<String>()) {
      req->send(400, "application/json", MISSING_PIN_LOGIC);
      return;
    }
    if (cantLedsActivos >= MAX_LEDS) {
      req->send(409, "application/json", LIMIT_REACHED);
      return;
    }
    int pin{doc["pin"]};
    String logic{doc["logica"]};
    logic.toUpperCase();
    Led *createdLed{nullptr};
    for (auto &led : leds) {
      if (led == nullptr) {
        led = new Led{pin, int(logic == "HIGH")};
        createdLed = led;
        cantLedsActivos++;
        break;
      }
    }
    req->send(201, "application/json", createdLed->toJson() + '\n');
  }
}

/**
 * @brief Endpoint PATCH /led/{n}
 * 
 * Se recibe un json con el formato {"estado": e } donde 'e' puede ser:
 *    - "on": prende el led
 *    - "off": apaga el led
 *    - "toggle": alterna el led
 * 
 * @param req AsyncWebServerRequest* request del cliente
 * @param data uint8_t* información del body del paquete actual
 * @param len size_t longitud del paquete actual
 * @param index size_t indice del paquete actual
 * @param total size_t tamaño total del body
 */
void updateLed(AsyncWebServerRequest *req, uint8_t *data, size_t len,
               size_t index, size_t total) {
  static String body{};
  if (!index) body = "";

  handleRequestBody(data, len, body);
  if ((index + len) == total) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, body);
    if (error) {
      req->send(400, "application/json", INVALID_JSON);
      return;
    }
    if (!doc["estado"].is<String>()) {
      req->send(400, "application/json", MISSING_LED_STATE);
      return;
    }
    int id{req->pathArg(0).toInt()};
    String estado{doc["estado"]};
    estado.toLowerCase();
    Led *led_found = nullptr;
    for (auto &led : leds) {
      if (led != nullptr && led->getID() == id) {
        led_found = led;
        if (estado == "on") led->on();
        else if (estado == "off") led->off();
        else if (estado == "toggle") led->toggle();
        else {
          req->send(409, "application/json", INVALID_STATE);
          return;
        }
        break;
      }
    }
    if (led_found) req->send(200, "application/json", led_found->toJson() + '\n');
    else req->send(404, "application/json", NOT_FOUND);
  }
}

/**
 * @brief Endpint DELETE /led/{n}
 * 
 * Se procede a buscar el led con id n, si se encuentra se envía el mensaje con el
 * mensaje de éxito (200), de lo contrario se envía un mensaje de que el led no se
 * encontró (404).
 * 
 * @param req AsyncWebServerRequest* request del cliente.
 */
void deleteLed(AsyncWebServerRequest *req) {
  if (req->pathArg(0) != "") {
    int id{req->pathArg(0).toInt()};
    bool isDeleted = false;
    for (auto &led : leds) {
      if (led != nullptr && led->getID() == id) {
        delete led;
        led = nullptr;
        isDeleted = true;
        cantLedsActivos--;
        break;
      }
    }
    if (isDeleted)
      req->send(200, "application/json", "{\"message\":\"LED '" + req->pathArg(0) + "' borrado.\"}\n");
    else req->send(404, "application/json", NOT_FOUND);

  }
}

/**
 * @brief Se inicializa el arreglo de Leds, inicialmente todos a nullptr
 * 
 */
void initLEDArray() {
  for (auto &led : leds) {
    led = nullptr;
  }
}

/**
 * @brief Configuraciones iniciales
 * 
 * Aquí se inicializa el puerto Serial, WiFi y cualquier otro periférico que sea necesario.
 * 
 */
void initConfig() {
  Serial.begin(SBAUDRATE);
  initLEDArray();
#ifdef AP_MODE
  WiFi.softAP(SSID, PSWD);
#else
  WiFi.begin(SSID, PSWD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.write('.');
    delay(250);
  }
  Serial.println(WiFi.localIP());
#endif
}

/**
 * @brief Configuración del servidor
 * 
 * Se configuran los endpoints:
 * GET    /led      : obtiene todos los leds creados
 * GET    /led/{n}  : obtiene el led con id "n"
 * POST   /led      : crea un led (formato: {"pin": n, "logica": ("HIGH"|"LOW")})
 * PATCH  /led/{n}  : Modifica el estado del led con id "n" (formato: {"estado":("on"|"off"|"toggle")})
 * DELETE /led/{n}  : Borra el led con id "n"
 * 
 * Se configura el CORS para aceptar peticiones desde los navegadores.
 * 
 * Y se inicializa el servidor para recibir las peticiones.
 */
void initServer() {
  /********************* Configuración de los endpoints *********************/
  server.onNotFound([](AsyncWebServerRequest *req) {
    req->send(404, "application/json", "{\"message\": \"404 - Not found\"}\n");
  });
  server.on("^\\/led(\\/[0-9]+)?$", HTTP_GET, readLed);
  server.on("/led", HTTP_POST, [](AsyncWebServerRequest *req) {}, NULL, createLed);
  server.on(
      "^\\/led\\/([0-9]+)$", HTTP_PATCH, [](AsyncWebServerRequest *req) {},
      NULL, updateLed);
  server.on("^\\/led\\/([0-9]+)$", HTTP_DELETE, deleteLed);
  
  /**********************  Configuración para el CORS **********************/
  server.on("/led", HTTP_OPTIONS, [](AsyncWebServerRequest *req) {
    AsyncWebServerResponse *res = req->beginResponse(204);
    res->addHeader("Access-Control-Allow-Methods", "GET, POST, PATCH, DELETE, OPTIONS");
    res->addHeader("Access-Control-Allow-Headers", "Content-Type");
    req->send(res);
  });
  // Cabecera por default en cada petición:
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");

  // Inicialización del AsyncWebServer
  server.begin();
}

void setup() {
  initConfig();
  initServer();
}

void loop() {}
