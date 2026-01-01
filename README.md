# Attendance-o-meter
Homemade mmwave people counter to assist room occupancy measurement.

# Repository contents
-	[Context – Attendance-o-meter](https://github.com/chad-casa/Attendance-o-meter/blob/main/README.md#context--attendance-o-meter)
-	[Introduction - Project Attendance-o-meter](https://github.com/chad-casa/Attendance-o-meter/blob/main/README.md#introduction--project-attendance-o-meter)
-	[System Overview](https://github.com/chad-casa/Attendance-o-meter/blob/main/README.md#system-overview)
-	[How does it work?](https://github.com/chad-casa/Attendance-o-meter/blob/main/README.md#how-does-it-work)
- [Architecture](https://github.com/chad-casa/Attendance-o-meter/blob/main/README.md#architecture)
- [The Attendance-o-meter IoT system](https://github.com/chad-casa/Vespera_IoT/blob/main/README.md#the-vespera-iot-system)
- [Component breakdown ](https://github.com/chad-casa/Attendance-o-meter/blob/main/README.md#component-breakdown)
- [Assembly](https://github.com/chad-casa/Attendance-o-meter/blob/main/README.md#assembly)
-	[Test Scripts – Does it work?](https://github.com/chad-casa/Attendance-o-meter/blob/main/README.md#test-scripts--does-it-work)
-	[Arduino Code](https://github.com/chad-casa/Attendance-o-meter/blob/main/README.md#arduino-code)
-	[Evaluation](https://github.com/chad-casa/Attendance-o-meter/blob/main/README.md#evaluation)
-	[Road Map](https://github.com/chad-casa/Attendance-o-meter/blob/main/README.md#road-map)
-	[Assumptions](https://github.com/chad-casa/Attendance-o-meter/blob/main/README.md#assumptions)
-	[Sources](https://github.com/chad-casa/Attendance-o-meter/blob/main/README.md#sources)

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

# Design
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
+	Logic:
    -	Movement from Near to Far is considered as entry to the space, movement from Far to Near is considered exit from the space. Near is defined as <1500mm and Far is defined as >3000mm. This is communicated to the Arduino by serial communication (the RX receieve and TX transmit wires).
    -	Readings take 800ms to begin tracking and allow for 4000ms to cross between Near and Far zones with a 2000ms cooldown period before counting again to prevent double counting.
    -	Three consecutive measurements at the same distance lead to a static reading, which is therefore discounted.
+	Microcontroller Unit: The Arduino MKR 1010 microcontroller unit has multiple libraries installed enabling it to connect to Wi-Fi using Wi-FiNINA with a secrets file Arduino_secrets.h, PubSubClient To publish and subscribe to specific topic channels

<b>3.	Connectivity</b>
+	Payload: The readings from the sensor are processed by the microcontroller using multiple libraries including SPI, WiFiNINA, PubSubCLient,Wire, Waveshare_LCD1602_RGB and utility/wifi_drv. With distance values linked to attendance counting thresholds and corresponding JSON payload values, we can send MQTT messages over Wi-Fi.
+	WIFI Gateway: CE-Wi-Fi located in One Pool Street
+	MQTT Broker: (Mqtt.cetools.org) receives the published payload and if the credentials are correct, it will relay the message to the device display.

<b>4.	Data Display </b>
+	Physical: LCD screen is updated by the JSON serial data output.
+	Function: LCD Screen flashes green when attendee enters the detection area and In tally adjusts accordingly. LCD flashes red when attendee exits the detection area leading to the out tally increasing.
+	The difference between in and out is the number of people in the room or current occupancy.
+	New observations are appended until session is terminated by operator by typing'r' to reset the session in the Arduino IDE output console.

# Build
  # Component Breakdown
-	**Breadboard**
  -	**Sensor – 24GHz mmWave Sensor from XIAO:** A milimeter wave sensor measuring distance covered by moving and static people
  -	**Connectivity enabled microcontroller – Arduino:** Arduino MKR1010 is a Wi-Fi enabled micro controller capable of sending MQTT messages.
-	**MQTT Broker** (mqtt.cetools.org)
-	**Display** LCD Screen

  # Assembly
1. Solder male pins to the bottom of the MMWave Sensor
2. Connect the cirucit using the diagram and wiring instructions below
   
 # Circuit Diagram
<img width="5200" height="7104" alt="Attendance-o-meter circuit diagram" src="https://github.com/user-attachments/assets/b92cd83b-8147-4835-bfd7-9b2d4168a3f2" />

 # Wiring

- VCC Arduino to +ve breadboard (Voltage In)
- GND Arduino to -ve breadboard (Ground)
- SCL Ardunio to SCL LED (Serial Clock Line)
- SDA Arduino to SDA LED (Serial Data Line)
- 3V3 MMWave Sensor to +ve breadboard (Voltage In)
- GND MMWave Sensor to -ve breadboard (Ground)
- TX Arduino (transmit) to RX MMWave Sensor (receive)
- TX Sensor (transmit) to RX Arduino (receive)

 # Enclosure
  The device casing should allow mmwaves from the sensor to travel freely to the detection area to validate human presence. Materials used in front of the sensor should allow for ease of wave transmission and limit relfect and refraction. Behind the sensor a metal guard is used to ensure the observation area is limited to what is in front of the sensor.
+ **Layer 1: Plastic** - To provide a splash guard from any potential liquid spills
+ **Layer 2: Foil** - To function as a metal shield to prevent interference (double counting or underreporting) caused by any operator behind the sensor, as advised by the [manufacturer](https://files.seeedstudio.com/wiki/mmwave-for-xiao/24GHz_mmWave_for_XIAO-Datasheet-V1.00.pdf)  
+ **Layer 3: Paper** - To hide contents of the device and wiring from public view
+ **Layer 4: Felt** - To provide an indoor asthetic that blends in and feels less industrial and more consumer friendly 

# Functionality
# Test Scripts – Does it work?

1. Does the board work? Use the [Blink](Tests/Blink.ino) script to tested the Arduino MKR1000's functionality

2. Can it connect to WI-FI? Use the [SimpleWifi tester](Tests/SimpleWebServerWiFi-webled-mkr1010.ino) Arduino code to validate that the device can connect to the internet over WI-FI 

3. Can it send MQTT messages? Use the [MQTT Simple](Tests/mkr1010_mqtt_simple.ino) Sketch

4. Does the mmwave sensor work? Use The Manufacturer's HLKRadarTool App and adjust the baud rate to 256000. For the exact instructions follow [this link](https://wiki.seeedstudio.com/mmwave_for_xiao/)
   - A functioning sensor will provide readings impacting Energy Thresholds for example:
  <img width="671" height="1256" alt="image" src="https://github.com/user-attachments/assets/6c3d5d83-3165-448b-8e46-f0ce6d4595d6" />
 

5. Does the physical LCD display work? Use [Waveshare_LCD1602_RGB.h](Tests/Waveshare_LCD1602_RGB.cpp) sketch and observe the function of the physical LCD Screen.
   
# Arduino Code
1. There is [limited if any base code](https://github.com/Seeed-Studio/Seeed_Arduino_24GHz_Radar_Sensor) provided by the manufacturer or locatable in the existing Arduino library for the 24ghz mmwave sensor.

2. Whilst presence detection is possible with the manufacturer app, custom logic has to be devised to enable people counting.

3. Detection Zone's
<img width="559" height="108" alt="image" src="https://github.com/user-attachments/assets/ad123c9d-7230-4782-b5c0-ebfa8c9c5ed1" />

4. Direction of travel logic (entry or exit)
<img width="534" height="96" alt="image" src="https://github.com/user-attachments/assets/d80e91d3-a3ee-4a23-84bb-6518a135865d" />

5. False positive and double counting risk minimisation
<img width="669" height="76" alt="image" src="https://github.com/user-attachments/assets/4d4735bd-2c3b-4d9c-aa6e-97ea7cfc8602" />

<img width="364" height="97" alt="image" src="https://github.com/user-attachments/assets/6713d3dc-2b48-4e3a-b9a9-019743d0bb69" />

6. LCD monitor display colour changes to match change in room occupancy. Red = Out , Green = In.
<img width="365" height="76" alt="image" src="https://github.com/user-attachments/assets/1802f773-d921-4a0f-8082-7494074781b3" />

7. Claude.ai used to help bridge logic into code

8.  MQTT publishing observable via MQTT Explorer - publishing to topic 6
   <img width="543" height="102" alt="image" src="https://github.com/user-attachments/assets/138a9464-187b-46f4-afbd-484480f5bb22" />

9.  [Final code](Arduino_code/Attendance-o-meter.ino) is uploaded to the device and device is stationary on desk and plugged into laptop with Arduino IDE interface

10.  Future iterations can subscribe their Grafana visualisation to the MQTT broker to enable occupancy insights to be shared virtually beyond the room / view of the devices' LCD display

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
