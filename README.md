## Materials:-

1.ESP32 Camera Module <br>
2.PIR Motion Sensor <br>
3.TTL Programmer <br>
4.Breadboard <br>
5.Jumpers <br>

**Copy my code to obtain accurate output** as the code of the youtube video is not working when PIR goes high. It captures images after certain interval continuously.

*If you are using mobile hotspot to provide internet to ESP32 then
your hotspot should be* <br> 
**Personal Hotspot or Mobile Hotspot** <br> 
ESP32 fails to connect **Portable hotspot**

## Connections:-
### ESP32_Cam & TTL connection
![ESP32_Cam & TTL connection](https://github.com/anishdalvi/Smart-Motion-Detector-using-ESP32-Cam/blob/master/ESP32_Cam%20&%20TTL%20connection.jpeg?raw=true)

### ESP32_Cam & PIR connection
> ## Pins
>
> - Black of PIR (Ground) goes to Ground of ESP32
> - Red of PIR (VCC) goes to 5V of ESP32
> - Yellow of PIR (Output) goes to GPIO13 of ESP32 <br>
> *The output of Yellow Pin determines if the PIR is low or high*

<br>

![ESP32_Cam & PIR connection](https://github.com/anishdalvi/Smart-Motion-Detector-using-ESP32-Cam/blob/master/ESP32_Cam%20&%20%20PIR%20connection.jpeg?raw=true)  

#### [Reference Website Link](https://www.viralsciencecreativity.com/post/esp32-cam-motion-alert-send-image-to-telegram)
#### [Youtube Reference Link](https://youtu.be/v36c7-s3jvA)
