| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- |

# Modbus Scratch

## Modbus TCP/IP from scratch (Master mode)

Esta aplicación hace uso de la biblioteca esp-modbus de ESP-IDF e implementa la funcionalidad del protocolo Modbus TCP/IP en el microcontrolador ESP32-S3.

La conexión de red se realiza a través de WiFi, para lo cual se hace uso de la biblioteca JRC_Wifi, la cual desarrollé en un proyecto anterior.
Para establecer la conexión de red, es necesario especificar las credenciales (SSID y Password) para conectarse al router.  Estas credenciales se encuentran en el archivo `secrets.h` en la carpeta`components/JRC_WiFi`. Se deben modificar las macros:

    #define  WIFI_SSID  "XXXXXXX"
    #define  WIFI_PASSWORD  "xxxxxxxxx"

Una vez conectado a la red local, el ESP32-S3 actuará como cliente (master) y se conectará al servidor (esclavo) especificado mediante la dirección IP que se define en la macro:

    #define SLAVE_IP_ADDRESS "xxx.xxx.xxx.xxx"

Para efectuar la simulación, se recomienda hacer uso del programa Modbus Slave, el cual se puede descargar desde https://www.modbustools.com/.

Se debe establecer una conexión mediante TCP/IP y es importante marcar la opción `Ignore Unit ID`en el cuadro de diálogo para establecer la conexión. También se deben configurar las ventanas con los registros necesarios, de acuerdo al diccionario de datos especificado en el programa. Los registros necesarios son:

>  - Holding Registers: Registros 0 y 1 
>  - Coils: Registro 0 
>  - Input Status: Registro 0

En ejecución, el programa establecerá el valor de un contador en el registro Holding register 0 , leerá el valor del Holding register 1 desde el esclavo y si este valor es menor a 1000, escribirá el valor 0 en el registro Coil 0 del esclavo, de lo contrario escibirá el vaor 1. El registro Input 0, al establecerse en 1, el microcontrolador activará la salida definida en la macro:

    #define  ledGreen  36
    
Al establecerse el registro de entrada Input 0 en 0, la salida se desactivará.
