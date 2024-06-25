# Industrial-Smart-Agriculture-System
This is a prototype-based project that incorporates IoT technology for soil monitoring in agriculture industry. 
The hardware components consist of ESP32 microcontroller as the backbone of the system as well as industrial-grade soil sensors for measuring the soil parameters such as soil moisture, EC, temperature, NPK etc.
The IoT network setup integrates MQTT messaging protocol to facilitate data transfer from devices to IoT platforms such as Datacake as used
in this project to display the data on dashboards available both on mobile and web applications.

List of hardware components used in the system:

1.  12V battery supply
2.  ESP32-C6 FireBeetle 2 microcontroller
3.  DC-DC buck converter
4.  Industrial-grade soil moisture, EC and temperature sensor (RS485 interface)
5.  Industrial-grade NPK sensor (RS485 interface)
6.  Weatherproof temperature and humidity sensor (RS485 interface)
7.  RS485 to UART TTL converter (MAX485)
8.  128x64 OLED display
9.  n-MOSFET transistor

Application programs:

1.  Arduino IDE for programming the microcontroller
2.  Modbus Serial software for configuring the sensors' address and baudrate

IoT platforms:

1.  HiveMQ MQTT client for data transfer
2.  Datacake for data display on dashboards : https://app.datacake.de/pd/ef621e8b-f27f-4776-9fb9-9be6ba70b714

Future updates:
- Detail explanation on the hardware selection and the connection of each pin involved
- Actual coding flowchart
- PCB design
