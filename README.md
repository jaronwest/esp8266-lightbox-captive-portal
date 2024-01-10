# ESP8266 Lightbox controller with captive portal 

https://github.com/yash-sanghvi/ESP32/tree/master/Captive_Portal

Big thank you and credit to yash-sanghvi for basis of this project with addition of LED control and updated for ESP8266

## This project is setup using an ESP8266 (such as 12E or nodeMCU) and WS2812 addressable LEDs

This project was used in a lightbox to make a top and bottom zone for two colors to be picked. Instead of connecting to an existing wifi network, or if there is no network, a soft access point is setup with a simple color picker UI. Upon setting your color, follow the link back home to make a new seletion.

## Customizing for your setup
- Update the arduino_secrets.h.example (drop .example) and choose an SSID name, you can also include a password if you want (untested)
- Set your board pin for LED signal to WS2812 (neopixels)
- Set the count of LEDs
- Choose your default colors, it can be any of the option values found in the html (use lower case)
- Add color options to the html if you like, then define them below in function handlePixelColor
- In the function handleStrip the first pixel is being set to black because I did not include a level shifter up to 5v, which can cause flickering or other issues, so I covered it in electric tape set it to black

## Connecting to the UI
- Select the network from your phone or computer that you set in the arduino_secrets.h file
- You may get an alert about not having internet connection when connecting
- The captive portal should automatically pop up and allow selecting a top and bottom color
- Save to update the colors
- Click the link to return home and you can continue changing the color
- Using "back" seems to cause some issues and may reset your color back to default and lose the AP until reset, so using the link back home is more reliable
- When done you can say "don't use this network" or "forget it" to disconnect (remember this network has no connection to the internet)