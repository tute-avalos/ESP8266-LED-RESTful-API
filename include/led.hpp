/**
 * @file led.hpp
 * @author Prof. Tute Ávalos (tute-avalos.com)
 * @brief Representación de un objeto LED
 * @version 0.1
 * @date 2024-11-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef __LED_HPP__
#define __LED_HPP__

#include <Arduino.h>

/**
 * @brief Representación de un LED
 * 
 */
class Led {
private:
  uint32_t _id;
  int _led_pin;
  int _logic;
  // Generador de ids
  static uint32_t _id_gen;

public:
  /**
   * @brief Construct a new Led object
   * 
   * Define un nuevo led y lo pone en estado apagado.
   * 
   * @param led_pin pin donde está conectado el led
   * @param logic lógica que utiliza para encender
   */
  Led(const int led_pin, int logic = HIGH)
      : _id{++_id_gen}, _led_pin{led_pin}, _logic{logic} {
    pinMode(led_pin, OUTPUT);
    digitalWrite(led_pin, !logic);
  }
  /**
   * @brief Destroy the Led object
   * 
   */
  virtual ~Led() {}
  /**
   * @brief obtiene el ID del LED
   * 
   * @return int id del led
   */
  int getID() const { return _id; }
  /**
   * @brief obtiene el estado del LED (on/off)
   * 
   * @return bool true si está prendido, false si está apagado
   */
  bool getStatus() const { return digitalRead(_led_pin) == _logic; }
  /**
   * @brief Prende el led
   * 
   */
  void on() const { digitalWrite(_led_pin, _logic); }
  /**
   * @brief Apaga el LED
   * 
   */
  void off() const { digitalWrite(_led_pin, !_logic); }
  /**
   * @brief Alterna el LED
   * 
   */
  void toggle() const { digitalWrite(_led_pin, !digitalRead(_led_pin)); }
  /**
   * @brief Pone un valor PWM en el LED de 0 a 255
   * 
   * @param value valor de brillo del LED [0-255]
   */
  void writeAnalog(uint16_t value) const {
    if (value > 255)
      value = 255;
    analogWrite(_led_pin, (_logic) ? value : 255 - value);
  }
  /**
   * @brief Representación en JSON del LED
   * 
   * @return String JSON que contiene el estado completo del LED
   */
  String toJson() const {
    String json{"{"};
    json += "\"id\":" + String(_id) + ",";
    json += "\"pin\":" + String(_led_pin) + ",";
    json += "\"logica\":" + String((_logic ? "\"HIGH\"" : "\"LOW\"")) + ",";
    json += "\"estado\":" +
            String(((getStatus()) ? "\"on\"" : "\"off\""));
    json += "}";
    return json;
  }
};

uint32_t Led::_id_gen = 0; // Inicia el contador de IDs

#endif // __LED_HPP__
