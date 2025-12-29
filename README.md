# Attendance-o-meter
Homemade mmwave people counter to assist room occupancy measurement.

# Repository contents
-	[Context – Attendance-o-meter](https://github.com/chad-casa/Attendance-o-meter/blob/main/README.md)
-	[Introduction - Project Attendance-o-meter](https://github.com/chad-casa/Vespera_IoT/blob/main/README.md#introduction--project-vespera)
-	[System Overview](https://github.com/chad-casa/Vespera_IoT/blob/main/README.md#system-overview)
-	[How does it work?](https://github.com/chad-casa/Vespera_IoT/blob/main/README.md#how-does-it-work)
- [Architecture](https://github.com/chad-casa/Vespera_IoT/blob/main/README.md#architecture)
- [The Attendance-o-meter IoT system](https://github.com/chad-casa/Vespera_IoT/blob/main/README.md#the-vespera-iot-system)
- [Component breakdown ](https://github.com/chad-casa/Vespera_IoT/blob/main/README.md#component-breakdown)
- [Assembly](https://github.com/chad-casa/Vespera_IoT/blob/main/README.md#assembly)
-	[Test Scripts – Does it work?](https://github.com/chad-casa/Vespera_IoT/blob/main/README.md#test-scripts--does-it-work)
-	[Arduino Code](https://github.com/chad-casa/Vespera_IoT/blob/main/README.md#arduino-code)
-	[Evaluation](https://github.com/chad-casa/Vespera_IoT/blob/main/README.md#evaluation)
-	[Road Map](https://github.com/chad-casa/Vespera_IoT/blob/main/README.md#road-map)
-	[Assumptions](https://github.com/chad-casa/Vespera_IoT/blob/main/README.md#assumptions)
-	[Sources](https://github.com/chad-casa/Vespera_IoT/blob/main/README.md#sources)

# Context – Attendance-o-meter
Attendance is a key business performance indicator for universities, it is currently measured using a the SEAtS App which currently has a one star rating on Apple and the Google Play store.
Users report a lack of functionality, high consequences for incorrect attendance monitoring [expulson/deportation](https://www.ucl.ac.uk/students/sites/students/files/ucl_student_visa_responsibilities_2022_final.pdf) and lecturers having to resort to manual counting during class times.
Insights are limited to the university and not readily accessble by users or third parties.      

The Attendance-o-meter seeks to provide a frictionless solution to counting the number of students who attend the class to provide insights to management.

# Introduction – Project Attendance-o-meter
Project Attendance-o-meter explores developing an IoT system to sense, network and react to the environment by displaying attendance data on an LCD screen. In this project we will use an Arduino MKR1010 Wi-Fi enabled microprocessor linked to the CE controlled by MQTT messages - to develop an IoT system to anonymously monitor classroom attendance using zonal people detection from a mmwave sensor as our key indicator.

For context, “the Internet of Things or IoT is the network of devices such as vehicles and home appliances that contain electronics software sensors actuators and connectivity which allows these things to connect interact and exchange data.” Kevin Ashton.

Simply put:

<p align="center">
<b>Physical object(s) + sensors (or actuators) + network connectivity = IoT</b>
</p>

# System Overview
In the case of Project Attendance-o-meter, we develop our own IoT system where:

<p align="center">
<b>LCD Screen + 24GHz mmWave Sensor from XIAO + Arduino MKR1010 Wi-Fi / MQTT = Attendance-o-meter IoT</b>
</p>

# How does it work? 
  
  # Architecture
<img width="7610" height="2432" alt="image" src="https://github.com/user-attachments/assets/f3bea579-74b9-4c3c-9160-0a1c22e22848" />

## The Attendance-o-meter IoT system
<b>1.	Stimulus</b>
+ Attendees (Human body): The detection area (750mm - 6000mm) is temporarily filled with a person moving towards or away from the sensor, the change in distance is measured in millimeters and is mapped to an algortihm which decides whether motion detected is sufficent to warrant counting an individual entering or exiting the space. A minimum distance threshold (300mm) must be met for an attendee to be counted as 'In' or 'Out', where the threshold is not met observations are classified as 'Static' and are not counted as entering (In) or exiting (Out) the space.

<b>2.	Data collection system (See Breadboard device below)</b>
+	Sensor: 24GHz mmWave Sensor from XIAO detects the human body by continuously emmitting out 24GHz ~ 24.25GHz millimeterwave electromagnetic waves which reflect off the subject ion the detection area tracking static people and micromovements.
+	Logic: Movement from Near to Far is considered as entry to the space, movement from Far to Near is considered exit from the space. Near is defined as <1500mm and Far is defined as >3000mm. This is communicated to the Arduino by serial communication (the RX receieve and TX transmit wires).
+	Readings take 800 ms to begin tracking and allow for 4000ms to cross between Near and Far zones wiht a 2000ms cooldown period before counting again to prevent double counting.
+	3 consecutive measurements at the same distanc elead toi a static reading which is therefore not counted.
+	Microcontroller Unit: The Arduino MKR 1010 microcontroller unit has multiple libraries installed enabling it to connect to Wi-Fi using Wi-FiNINA with a secrets file Arduino_secrets.h, PubSubClient To publish and subscribe to specific topic channels

<b>3.	Connectivity</b>
+	Payload: The readings from the sensor are processed by the microcontroller using multiple libraries including MMWave, Wire and utility/wifi_drv. With RGB values linked to CO2 PPM thresholds and corresponding hexadecimal payload values, we can send MQTT messages over Wi-Fi.
+	WIFI Gateway: CE-Wi-Fi located in One Pool Street
+	Tilt Controller: Receives the MQTT payload via WIFI and selects which topic the Vespera luminaires is subscribed. The payload is then published to the coded topic e.g. #6 and sent to the MQTT Broker.
+	MQTT Broker: (Mqtt.cetools.org) receives the published payload and if the credentials are correct, it will relay the message to the Vespera Luminare assigning the relevant LED configuration to reflect the CO2 observation.

<b>4.	Vespera Light</b>
+	Physical: Is updated when dialled into the appropriate topic set by the Tilt Controller.
+	Virtual: (IoT.io/projects/lumi/) illustrates the equivalent display shown by the physical luminaire in CASA00014 classroom.
+	Function: 3 LEDs are lit each hour with the colour reflecting the CO2 levels (CO2 <450PPm = Green, CO2 451-750ppm = Orange, CO2 >751 = Red).
+	New observations are appended until all 72 LEDs are showing the past 24 observations (24hrs work of CO2 readings) after which the light is reset.



  # Component Breakdown
-	**Breadboard**
  -	**Sensor – 24GHz mmWave Sensor from XIAO:** A milimeter wave sensor measuring distance covered by moving and static people
  -	**Connectivity enabled microcontroller – Arduino:** Arduino MKR1010 is a Wi-Fi enabled micro controller capable of sending MQTT messages.
-	**MQTT Broker** (mqtt.cetools.org)
-	**Display** LCD Screen

  # Assembly
+Circuit diagram

Follow the steps illustrated by the attached ADAFruit literature.
https://cdn-learn.adafruit.com/downloads/pdf/adafruit-ens160-mox-gas-sensor.pdf

Combined it should look like so:

- 5V to VIN (Voltage In)
- GND to GND (Ground)
- SCL to SCL (Serial Clock Line)
- SDA to SDA (Serial Data Line)

<img width="745" height="564" alt="image" src="https://github.com/user-attachments/assets/b134e1c6-66df-411f-84c4-4ab0b125a0aa" />

  # Enclosure
+ Layer 1: Plastic - to provide a splash guard from any potential liquid spills
+ Layer 2: Foil - to prevent double counting orunderreporting caused by any operator behind the sensor 
+ Layer 3: Paper - asthetic
+ Layer 4: Felt - To provide an indoor asthetic that blends in and feels less industrial and more consumer friendly 

# Test Scripts – Does it work?

1. Does the board work? Use the [Blink](https://github.com/chad-casa/Vespera_IoT/blob/8df0a0b58fe31e11d76ad6d3414e8fec8753c69a/Tests/Blink.ino) script to tested the Arduino MKR1000's functionality

2. Can it connect to WI-FI? Use the [SimpleWifi tester](https://github.com/chad-casa/Vespera_IoT/blob/8b2bf5f1b79ee9ebf1dc66fb2b9969085085122d/Tests/SimpleWebServerWiFi-webled-mkr1010.ino) Arduino code to validate that the device can connect to the internet over WI-FI 

3. Can it send MQTT messages? Use the [MQTT Simple](https://github.com/ucl-casa-ce/casa0014/blob/cc7aed6253ad8d2e7b3fdea0c4e44cc227731e9e/vespera/workshop-sketch/mkr1010_mqtt_simple/mkr1010_mqtt_simple.ino) Sketch

4. Does the CO2 sensor work? Use [ENS16x code](https://github.com/chad-casa/Vespera_IoT/blob/5368505f9161e4627b4a5e9a68e908a2f0ed466e/Tests/ens160basic_std.ino) to test the sensor takes CO2 readings
<img width="959" height="562" alt="image" src="https://github.com/user-attachments/assets/1e0c6453-26c5-4c25-958d-ce4c45b5d6e4" />

5. Are they received by Vespera Luminaire? Use [luminaire_mkr1010_controller](https://github.com/ucl-casa-ce/casa0014/blob/cc7aed6253ad8d2e7b3fdea0c4e44cc227731e9e/vespera/luminaire_mkr1010_controller/luminaire_mkr1010_controller.ino) sketch and observe the data flows in MQTT explorer and the virtual/physical Luminarie.

# Arduino Code
1.  Initial merge MQTT Simple, Luminaire and ENS160 sketches using Claude.ai
2.  System working locally without MQTT Publishing - legacy MQTT File used
3.  New MQTT file found with new port (1884 instead of 1883)
4.  To finalise the custom sketch I updated topic to 6, Arduino_Secrets for relevant WI-FI, CO2ToColour ranges to be demo appropriate, simplified colours used to Green, Orange and Red

# Evaluation

## Road Map
**Stages of development**
1. Zine mind mapping my product thesis
2. Product component selection
3. Product component assembly
4. Functional testing of components using Arduino sketch examples (Blink, WIFININA AP_SimpleWebserver, ScioSense_ENS16X)
5. Wired up a breadboard and CO2 sensor to evaluate whether my chosen sensor works to monitor ppm
6. Test the project [Luminaire script](https://github.com/ucl-casa-ce/casa0014/blob/cc7aed6253ad8d2e7b3fdea0c4e44cc227731e9e/vespera/luminaire_mkr1010_controller/luminaire_mkr1010_controller.ino) from Duncan Wilson, CASA
7. Consolidate ScioSense sketch with Luminaire script
8. Connect my device To MQTT gateway
9. Link sensor readings in serial monitor to RGB LED changes
10. Send RGB changes to Vespera online tool on topic 6 (my designated channel)
11. Save historic readings on the Vespera light to illustrate 1/hour - 3 Neopixel strips coloured represent 1 hour
    
**Functional Demo Output**
<img width="932" height="530" alt="image" src="https://github.com/user-attachments/assets/25f92f11-6e9c-4a97-bbd3-a7442d25e769" />

**Future Plans**
1. Add multiple sensors at different heights and positions into the network to ensure accurate readings
2. Enable Sleep mode to ensure energy efficiency given power intensive nature of Wifi
3. Create wireless version of product for travel

## Assumptions
- Good WI-FI signal
- Access to wired power
- Data privacy is not an important risk

## Why Arduino MKR1010?
+ Wifi connectivity
+ Open source
+ Low cost
  
## Why MQTT?
**Message Queue Telemetry Transport** is a publish and subscribe model protocol where the broker decouples senders and receivers, so they don’t need to know about one another. Benefits for CO2 sensing, small message size and continuous connection. The publish – subscribe model enables the storage of messages which ensures reliability
Low latency is not necessary due to the low frequency of sensing required. Large flows, energy efficiency is not imperative as the device is designed for indoor use so a wireless power source is not necessary and people in the environment are sedentary and in close proximity to the sensor for prolonged periods. 
Data transferred is not highly sensitivity so weak security or no default encryption is not a concern.

## Areas of Improvement
The sensor is working and is selecting RGB values based off of the PPM reading in the serial output. The current bounds were 600 - 1000 - 1500 ppm this seems fairly high as the current average is ~400. I then implemented a traffic light system: green (good ventilation, <450 PPM), orange (consider ventilating, 451-750 PPM), red (ventilate now, >751 PPM). Readings have been tweaked to run every 10 seconds to prevent spam and only register when changes are over 50ppm. In practice readings should be hourly.
<img width="954" height="561" alt="image" src="https://github.com/user-attachments/assets/b6771a0a-9750-4189-b10c-3c15db77a52c" />

“Don’t assume your measurements are valid” – Duncan Wilson

- **Reliability and validity of readings:** Multiple sensors, fine tune sensor positioning in room
- **Relevance of readings:** Consider relevant thresholds for CO2 colour change
- **Cost of measurement:** Consider appropriate cadence of measurement
- **Wireless:** Consider how to make the IoT system wireless or low power for greater versatility in use cases

# Sources: 
+ **Base code** - Arduino template libraries* [SPI, WIFININA, PubSubCLient,Arduino_secrets,Utility/wifi_drv,Wire,ScioSense_ENS160] 
+ **Base code** - Luminaire controller and MQTT Simple code by [Duncan Wilson](https://github.com/ucl-casa-ce/casa0014/blob/cc7aed6253ad8d2e7b3fdea0c4e44cc227731e9e/vespera/luminaire_mkr1010_controller/luminaire_mkr1010_controller.ino) (https://github.com/ucl-casa-ce/casa0014/tree/main/vespera)
+ **Wiring** - https://cdn-learn.adafruit.com/downloads/pdf/adafruit-ens160-mox-gas-sensor.pdf
+ **How to merge sketches** - [Claude.AI](https://claude.ai/)
+ **CO2 reading to RGB** - Chad Barrett, Arduino Libraries* and [Claude.AI](https://claude.ai/)
