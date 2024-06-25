# Industrial-Smart-Agriculture-System
This is a prototype-based project that incorporates IoT technology for soil monitoring in agriculture industry. 
The hardware components consist of ESP32 microcontroller as the backbone of the system as well as industrial-grade soil sensors for measuring the soil parameters such as soil moisture, EC, temperature, NPK etc.
The IoT network setup integrates MQTT messaging protocol to facilitate data transfer between devices and IoT platforms where Datacake as used
in this project displays the data on a dashboard for real-time monitoring. The dashboard is available both on web and mobile applications.

List of hardware components used in the system:

1.  12V power supply
2.  ESP32-C6 FireBeetle 2 microcontroller
3.  DC-DC buck converter (step down 12V to 5V)
4.  Industrial-grade soil moisture, EC and temperature sensor (RS485 interface)
5.  Industrial-grade NPK sensor (RS485 interface)
6.  Weatherproof temperature and humidity sensor (RS485 interface)
7.  RS485 to TTL converter (MAX485)
8.  128x64 OLED display
9.  n-MOSFET transistor

Application programs:

1.  Arduino IDE for programming the microcontroller
2.  Modbus Serial software for configuring the sensors' address and baudrate

IoT platforms:

1.  HiveMQ MQTT client for data transfer
2.  Datacake for displaying data on dashboards : https://app.datacake.de/pd/ef621e8b-f27f-4776-9fb9-9be6ba70b714


Future updates:
- Detail explanation on the component selection and the connection of each pin involved
- Actual coding flowchart
- PCB design
