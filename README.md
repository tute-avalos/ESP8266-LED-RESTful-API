# LED RESTful API

Esta API REST corre en un ESP8266, permite gestionar un conjunto de LEDs conectados a una placa o dispositivo. Permite crear, consultar, modificar y eliminar LEDs, así como cambiar su estado.

## Tabla de Contenidos

- [LED RESTful API](#led-restful-api)
  - [Tabla de Contenidos](#tabla-de-contenidos)
  - [Descripción](#descripción)
  - [Requisitos](#requisitos)
  - [Uso](#uso)
  - [Endpoints de la API](#endpoints-de-la-api)
    - [GET /led](#get-led)
    - [GET /led/{n}](#get-ledn)
    - [POST /led](#post-led)
    - [PATCH /led/{n}](#patch-ledn)
    - [DELETE /led/{n}](#delete-ledn)
  - [Ejemplos de Uso](#ejemplos-de-uso)
  - [Aclaraciones](#aclaraciones)
  - [Licencia](#licencia)

## Descripción

La **LED RESTful API** permite gestionar y controlar LEDs de forma remota mediante solicitudes HTTP. Este proyecto está diseñado para dar un ejemplo concreto de lo que se puede hacer en un sistema embebido del tipo ESP32/ESP8266. Tiene el objetivo de ser didáctico, no debería tomarse en serio para producción.

## Requisitos

Placa ESP8266/NodeMCU o equivalente, este proyecto fue desarrollado utilizando [PlatformIO](https://platformio.org/) y es lo que se sugiere utilizar.

## Uso

Para interactuar con la API, se pueden realizar solicitudes HTTP usando herramientas como [Postman](https://www.postman.com/) o `curl` desde la terminal.

## Endpoints de la API

### GET /led

- **Descripción:** Obtiene una lista de todos los LEDs registrados.
- **Respuesta:** Retorna un array de objetos JSON, cada uno representando un LED.
- **Código de estado:** 200 OK
- **Ejemplo de respuesta:**
  ```json
  [
    { "id": 1, "pin": 5, "logica": "HIGH", "estado": "on" },
    { "id": 2, "pin": 7, "logica": "LOW", "estado": "off" }
  ]
  ```

### GET /led/{n}

- **Descripción:** Obtiene el LED con el ID especificado.
- **Parámetros de URL:** `{n}` - ID del LED a obtener.
- **Respuesta:** Retorna un objeto JSON con los detalles del LED.
- **Códigos de estado:** 200 OK si existe, 404 Not Found si no se encuentra.
- **Ejemplo de respuesta:**
  ```json
  { "id": 1, "pin": 5, "logica": "HIGH", "estado": "on" }
  ```

### POST /led

- **Descripción:** Crea un nuevo LED.
- **Cuerpo de la solicitud:** JSON con la estructura `{ "pin": number, "logica": "HIGH" | "LOW" }`
- **Respuesta:** Retorna el LED creado con su ID asignado.
- **Códigos de estado:** 201 Created en caso de éxito, 400 Bad Request en caso de error de validación.
- **Ejemplo de solicitud:**
  ```json
  { "pin": 8, "logica": "HIGH" }
  ```
- **Ejemplo de respuesta:**
  ```json
  { "id": 3, "pin": 8, "logica": "HIGH", "estado": "off" }
  ```

### PATCH /led/{n}

- **Descripción:** Modifica el estado del LED con el ID especificado.
- **Parámetros de URL:** `{n}` - ID del LED a modificar.
- **Cuerpo de la solicitud:** JSON con la estructura `{ "estado": "on" | "off" | "toggle" }`
- **Respuesta:** Retorna el LED actualizado.
- **Códigos de estado:** 200 OK en caso de éxito, 400 Bad Request si el estado es inválido, 404 Not Found si el LED no existe.
- **Ejemplo de solicitud:**
  ```json
  { "estado": "toggle" }
  ```
- **Ejemplo de respuesta:**
  ```json
  { "id": 1, "pin": 5, "logica": "HIGH", "estado": "off" }
  ```

### DELETE /led/{n}

- **Descripción:** Elimina el LED con el ID especificado.
- **Parámetros de URL:** `{n}` - ID del LED a eliminar.
- **Respuesta:** Retorna un mensaje de confirmación.
- **Códigos de estado:** 200 OK si se elimina con éxito, 404 Not Found si el LED no existe.
- **Ejemplo de respuesta:**
  ```json
  { "message": "LED '1' borrado." }
  ```

## Ejemplos de Uso

Aquí algunos ejemplos usando `curl`:

- **Obtener todos los LEDs:**
  ```bash
  curl -X GET http://192.168.4.1/led
  ```

- **Crear un LED:**
  ```bash
  curl -X POST http://192.168.4.1/led -H "Content-Type: application/json" -d '{"pin": 8, "logica": "HIGH"}'
  ```

- **Modificar el estado de un LED:**
  ```bash
  curl -X PATCH http://192.168.4.1/led/1 -H "Content-Type: application/json" -d '{"estado": "toggle"}'
  ```

- **Eliminar un LED:**
  ```bash
  curl -X DELETE http://192.168.4.1/led/1
  ```


## Aclaraciones

En este proyecto se proporciona además en la carpeta [html](./html) que contiene una página web estática de prueba para los endpoints descriptos para una fácil comprobación del funcionamiento.

Además, puede definir un archivo `credentials.json` que tenga el siguiente formato para conectar a su red local:

```json
{
    "WIFI_SSID": "NOMBRE_DE_SU_RED_LOCAL",
    "WIFI_PASSWORD": "PASSWORD_DE_SU_RED"
}
```

Debe verificar por terminal Serie qué IP le fue asignada, en caso de tener esta configuración activa. De lo contrario el ESP8266 se pondrá en modo AP con:

 * SSID: "LED RESTful API"
 * PASSWORD: "asdf1234"
 * IP: 192.168.4.1

## Licencia

Este proyecto está bajo la Licencia GPLv3. Ver [LICENSE](./LICENSE) para más detalles.
