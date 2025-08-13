# ESP32 Relay DHT

<img alt="intro" src="https://user-images.githubusercontent.com/35195398/82018588-e3823700-9685-11ea-8242-12c46c9e09ef.png">

The above figure shows how the project looks at the delivered state. In this project, we are going to control a 4 Channel Relay module and read the temperature and humidity values of the surrounding environment with a DHT11 sensor using the ESP32 development board.

The ESP32 development board connects to the local Wi-Fi network and a DNS ([http://shanezzar.local](http://shanezzar.local)/) is initiated. Hitting this URL connects that client to the ESP32 development board by means of web sockets resulting in synchronized data exchange across all devices. The ESP32 development board also uses SPI Flash File System and a web page is burned in the board which displays temperature, humidity, and heat index gauges along with 4 buttons to toggle the relay modules ON/OFF. All the debug data is shown on the serial monitor at 115200.

Please see the the documentation.pdf in the repo for more information.

Regards,  
[Shanezzar.](https://shanezzar.com/)