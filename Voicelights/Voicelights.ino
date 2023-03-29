#include <FastLED.h>

// Arduino Music Visualizer 0.2

// This music visualizer works off of analog input from a 3.5mm headphone jack
// Just touch jumper wire from A0 to tip of 3.5mm headphone jack

// The code is dynamic and can handle variable amounts of LEDs
// as long as you adjust NUM_LEDS according to the amount of LEDs you are using

// LED LIGHTING SETUP
#define LED_PIN     8
#define NUM_LEDS    50
#define BRIGHTNESS  255
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 100

// AUDIO INPUT SETUP
int audio = 14;

// STANDARD VISUALIZER VARIABLES
int loop_max = 0;
int k = 255; // COLOR WHEEL POSITION
int decay = 0; // HOW MANY MS BEFORE ONE LIGHT DECAY
int decay_check = 0;
long pre_react = 0; // NEW SPIKE CONVERSION
long react = 0; // NUMBER OF LEDs BEING LIT
long post_react = 0; // OLD SPIKE CONVERSION

// RAINBOW WAVE SETTINGS
int wheel_speed = 0;

void setup()
{
  // LED LIGHTING SETUP
  delay( 3000 ); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );

  // CLEAR LEDS
  for (int i = 0; i < NUM_LEDS; i++)
    leds[i] = CRGB(0, 0, 0);
  FastLED.show();

  // SERIAL AND INPUT SETUP
  Serial.begin(115200);
  pinMode(audio, INPUT);
  Serial.println("\nListening...");
}

// FUNCTION TO GENERATE COLOR BASED ON VIRTUAL WHEEL
// https://github.com/NeverPlayLegit/Rainbow-Fader-FastLED/blob/master/rainbow.ino
CRGB Scroll(int pos) {
  CRGB color (0,0,0);
  if(pos < 50) {
    color.g = 0;
    color.r = ((float)pos / 85.0f) * 255.0f;
    color.b = 255 - color.r;
  } else if(pos < 128) {
    color.g = ((float)(pos - 85) / 85.0f) * 255.0f;
    color.r = 255 - color.g;
    color.b = 0;
  } else if(pos < 256) {
    color.b = ((float)(pos - 170) / 85.0f) * 255.0f;
    color.g = 255 - color.b;
    color.r = 1;
  }
  return color;
}

// FUNCTION TO GET AND SET COLOR
// THE ORIGINAL FUNCTION WENT BACKWARDS
// THE MODIFIED FUNCTION SENDS WAVES OUT FROM FIRST LED
// https://github.com/NeverPlayLegit/Rainbow-Fader-FastLED/blob/master/rainbow.ino
void set_lights()
{
  for(int i = NUM_LEDS / 2 - 1; i >= 0; i--) {
    if (i < react){
      int c = (i * 256 / 50 + k) % 256;
      leds[NUM_LEDS / 2 + i] = Scroll(c);
      leds[NUM_LEDS / 2 - i - 1] = Scroll(c);
    } else {
      leds[NUM_LEDS / 2 + i] = CRGB(0, 0, 0);
      leds[NUM_LEDS / 2 - i - 1] = CRGB(0, 0, 0);
    }
  }
  FastLED.show(); 
}

int numbers[10] = {0,0,0,0,0,0,0,0,0};
int indi = 0;

int average(int list[])
{
  int sum = 0;
  for(int i = 0; i < 10; i++){
    sum += abs(list[i]);
  }

  return sum / 10;
  
}

void loop()
{
  int audio_input = analogRead(audio); // ADD x2 HERE FOR MORE SENSITIVITY  
  audio_input -= 350;
  
  numbers[indi] = audio_input;
  indi = (indi + 1) % 10;

  int avgn = average(numbers);

  if (audio_input > 0)
  {
    pre_react = log(((long)NUM_LEDS * (long)audio_input) / (1023L / 6)) * 7; // 1023/6 = 170.5 // TRANSLATE AUDIO LEVEL TO NUMBER OF LEDs

    if (pre_react > react) // ONLY ADJUST LEVEL OF LED IF LEVEL HIGHER THAN CURRENT LEVEL
      react = pre_react;

    Serial.print(audio_input);
    Serial.print(" -> ");
    Serial.println(pre_react);
  }

  set_lights(); // APPLY COLOR

  k = k - wheel_speed; // SPEED OF COLOR WHEEL
  if (k < 0) // RESET COLOR WHEEL
    k = 255;

  // REMOVE LEDs
  decay_check++;
  if (decay_check > decay)
  {
    decay_check = 0;
    if (react > 0)
      react--;
  }
  delay(10);
}
