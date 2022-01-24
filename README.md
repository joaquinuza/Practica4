# Practica4
El objetivo de esta practica es manejar comunicaciones IP a bajo nivel mediante sockets y protocolos a alto nivel, HTTP, FTP, NTP, MQTT.

## Ejercicio 1
En este [ejercicio](https://github.com/joaquinuza/Practica4/blob/main/_1_ntp_server_WIFI/_1_ntp_server_WIFI.ino) el esp32 es conectado mediante el protocolo WiFi a un servidor (NTP) el cual es utilizado para sincronizar el reloj del esp32 según el sistema universal UTC. Para ajustarlo al horario local se añade un Offset.
## Ejercicio 2
En este [ejercicio](https://github.com/joaquinuza/Practica4/blob/main/_2_socket_TCP_WIFI/_2_socket_TCP_WIFI.ino) se crea un socket TCP sobre una red local, el servidor esta localizado en un ordenador y el cliente es el microcontrolador esp32. Se envían cadenas de caracteres desde el cliente que informan al servidor del tiempo actual.
Además, se añade una capa de control por la cual el servidor una cadena de caracteres al cliente el inicio o fin del envió de datos.
## Ejercicio 3
En este [ejercicio](https://github.com/joaquinuza/Practica4/blob/main/_3_web_server/_3_web_server.ino) se monta un servidor web en el microcontrolador el cual aloja una página web, la página utiliza el formato HTML junto con su fichero de estilos css, ambos también son almacenados en la memoria flash del ESP32.
En esta web se muestra la hora la cual es obtenida de un servidor NTP, también se ofrece la posibilidad de resetear la hora del ESP32 
## Ejercicio 4
En este [ejercicio](https://github.com/joaquinuza/Practica4/blob/main/_4_ftpclient_json/_4_ftpclient_json.ino) el ESP32 funciona como datalogger el cual envía los datos capturados por un sensor de temperatura en formato json a un servidor ftp. El formato del archivo .json  sigue las especificaciones definidas por SenML sobre la representación de medidas de sensores. Para la creación del archivo json se hace uso de la librería ArduinoJson.
## Ejercicio 5
En este [ejercicio](https://github.com/joaquinuza/Practica4/blob/main/_5_mqtt_protocol/_5_mqtt_protocol/_5_mqtt_protocol.ino) los datos de un sensor de temperatura y en formato json son subidos a la nube mediante el protocolo MQTT, para ello se utiliza el servicio gratuito proporcionado por Adafrutit. Se crea una feed para los datos, el esp32 se subscribe a la feed que se encuentra en el servidor MQTT y publica mensajes, es decir, datos del sensor de temperatura cada 5 segundos. 
El servidor MQTT al estar alojado en Internet permite la visualización de los datos a todos los dispositivos conectados a él. 

