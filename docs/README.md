
**Caution Beta!**

To do: 
      
  **Home Tab**
  ____
  - Brew button(for PIDONLY=0)
  - ~~Show Brewtimer~~ (not comitted yet) when brew is detected (open Window and show timer)
  - Set brew °C value (click on Input Label to set value?)+ - button on top and below input label!
  - ~~Implement getScale from scale.h and show in Arc (map weight to arc angle)~~
  - fix style ("white flash bug")
          
   **Graph Tab**
  - Plot "Output" to 2nd graph line (this should be optional, checkbox in Settings tab)
  - Plot "Soll °C" as 3rd graph line
         
   **Settings tab**
   ____
       
  - Save (event)
  - More Settings (goal is to set all config values load/save via eeprom)
  - Fix + - Button Events for PID
  - init values from blynk
  - ~~Table/ButtonArray -> Windows for Settings(?)~~
  
___
      More:
          --~~Create Font including 0-9 a-z A-Z ° + Glyphs(Fontawesome)~~
	  -- implement wifi.reconnect
	  -- show wifi.status
	  -- blynk reconnect
	  -- to activate the new Font, include it into the fonts folder of lvgl and paste 
```
#define LV_FONT_CUSTOM_DECLARE LV_FONT_DECLARE(default_16)
/*Always set a default font from the built-in fonts*/
#define LV_FONT_DEFAULT        &default_16
```

into the lvgl conf. I'll add the lib to this project, later.
                           
___      
	Helpful Links:


- https://randomnerdtutorials.com/esp32-pinout-reference-gpios/ (ESP32 useable Pins)


- https://docs.littlevgl.com/en/html/index.html (lvgl documentation)
