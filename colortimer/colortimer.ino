#include <avr/sleep.h> // enables sleep and power options for AVR
#include <avr/power.h> // enables power management options for AVR
#include <Adafruit_NeoPixel.h>

#define PIN 6

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, PIN, NEO_GRB + NEO_KHZ800);

int sound = 7; // pin for playing audio alert 
int running = 1; // variable to let system know countdown is active
int startCount = 300; // starting time for countdown in seconds 
int seconds = startCount; // this actually keeps track of current second in countdown
unsigned long previousMillis = 0; // used in time tracking logic
int segments = 0; // sets number of divisions in poll interval 
int percentRemaining = 0; // the variable name says it all
int interval = 500; // sets interval for time check
int intPerSec = 1000/interval; // calculates intervals in a second
int flashing = 0; // defines if the light should be flashing
int sleepDelay = 0; //setting up base value for sleep delay time

void setup() {
    strip.begin();
    strip.setBrightness(170);
    strip.show(); // Initialize all pixels to 'off'
    pinMode(sound, OUTPUT);
    Serial.begin(9600);
}

void loop() {
    
    if (running == 1) //is the timer running
    {
        unsigned long currentMillis = millis();  // check current time
        if (currentMillis - previousMillis > interval) // check if interval defined above has passed
        {
            // Does time check on partial seconds 
            previousMillis = currentMillis;
            segments = segments+1;
            Serial.println("testing");
      
	        if (segments == intPerSec)
            {
                segments = 0;
     
            //Logic to deal with last second
                if (seconds == 1)
                {
                    running = 0;  // stop count down
                    seconds = 0;  // clear the last second
                }
                else
                {
                    seconds = seconds - 1;
                }
                // The heavy work
				percentRemaining = int(seconds*100/startCount);
                Serial.println(percentRemaining);
				
				// Doing math converting percentage of time to value between 0 and 255
                int pixelOffset = (percentRemaining * 255/100)-segments;
                Serial.println(pixelOffset);
				
				// Make that pixel light up
                strip.setPixelColor(0, 255-pixelOffset, pixelOffset, 0);
                strip.show();
                Serial.println(seconds);
            }
        }
    }
    else
    {
        if (flashing == 1)
        {  
            // Blink the light red
			strip.setPixelColor(0, 255, 0, 0);
            strip.show();
            // Make the beep tone 
			tone(sound, 740, interval/2);
            delay(interval/2);
            tone(sound, 440, interval/2);
            delay(interval/2);
            flashing = 0; //toggle flashing 
        }
        else
        {
            // Turn the light off
			strip.setPixelColor(0, 0, 0, 0);
            strip.show();
            flashing = 1; //toggle flashing 
            Serial.println(sleepDelay);
            
			if (sleepDelay == 20)
            {
                // Shut down the arduino and put it in sleep mode
                Serial.println("Powering Down");
                enterSleep();
            }
        
		    sleepDelay = sleepDelay + 1;
        }
        
		delay(interval);
    }
}
 
void enterSleep(void)  
{  
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();  
    sleep_mode();   
  
    sleep_disable();   
}  

