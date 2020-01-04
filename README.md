# The Three Generation Lighthouse

![Lighthouse animation](lighthouse.gif)

My grandfather once built a brass lighthouse model out of a canon bullet.
Inside it, he placed an incandescent light bulb. The heat from the bulb would
cause a brass plate shaped like a fan to rotate above the bulb, along with
several window shutters connected to it. This caused several rays of light
outside the lighthouse to rotate. Eventually, the lighthouse ended up in a
cellar, out-of-order, collecting dust and oxidizing.

Many years later, my father decided to refurbish it. He repaired broken parts,
and polished the brass. He coated it to prevent it from oxidizing again. One
problem, he left for me. The rotating part didn't work very well, especially
with modern LED bulbs which do not produce much heat. Instead, he wanted to
replace the rotating light with blinking light. Lighthouses can be recognized
on a sea map by their blinking pattern, and have different colors in different
sectors, useful for determining the location. 

The electronics was my part. I used an Arduino Nano circuit board equipped with a ATmega328P microcontroller to control the blinking. A knob attached to a rotary encoder can be rotated to adjust the brightness. By depressing the knob, the light can be turned on, off, or put in blinking mode. By carefully studying a real lighthouse you would observe that the light takes some time to ramp up or down; it doesn't turn on or off immediately. Similarly, I made the light on the model spend 350 ms during ramp up or ramp down. One of the more challenging aspects is to get the ramping to look linear, since our eyes are far more sensitive to weak light. I found that using a power law worked well. The brightness is controlled using pulse width modulation (PWM), but it was necessary to use the full 10 bit ADC on the microcontroller (not directly accesibly by the Arduino analog output) to prevent jumps between the discrete levels to be visible for smaller brightnesses.
