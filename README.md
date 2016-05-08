# Home-Secure
HomeSecure is an Arduino Uno based home security system. It primarily revolves around the idea of building a system which 
ensures a safe home for the user by providing him information about his house on his Android smartphone. Any instance of a
potential risk like a gas leak or a fire situation or even an intrusion will be immediately notified to the user to ensure 
immediate remedy.

# Technologies Used
- *Arduino C* is used to program Arduino Uno for the above given problem definition as well as to send the sensed data through
ESP8266 to a local server.
- *Java* based server is used to receive the sensor data from Arduino.
- An *Android* Application is created for the user to get instant updates about the condition of his house.

# Sensors Used
- DHT22 : Temperature and Humidity sensor
- MQ6 : LPG gas sensor
- ESP8266 : WiFi Module

# References
- https://hackaday.io/project/3072-esp8266-retro-browser
