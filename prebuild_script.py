# prebuild_script.py
import os
import json

Import("env")

try:
    with open('credentials.json', 'r') as f:
        credentials = json.load(f)
except (FileNotFoundError, json.JSONDecodeError) as e:
    print(f"No existe '{e}'")
    credentials = {}

# Verifica si las variables de entorno están definidas
wifi_ssid = credentials.get("WIFI_SSID")
wifi_password = credentials.get("WIFI_PASSWORD")

if wifi_ssid and wifi_password:
    # Si están definidas, compila en modo cliente
    print("Compilando en modo cliente (conexión WiFi)...")
    env.Append(
        CPPDEFINES=[
            ("WIFI_SSID", f'\\"{wifi_ssid}\\"'),
            ("WIFI_PASSWORD", f'\\"{wifi_password}\\"')
        ]
    )
else:
    # Si no están definidas, compila en modo AP
    print("Compilando en modo AP...")
    env.Append(
        CPPDEFINES=[
            "AP_MODE",
            ("WIFI_SSID", '\\"LED RESTful API\\"'),
            ("WIFI_PASSWORD", '\\"asdf1234\\"'),
        ]
    )
