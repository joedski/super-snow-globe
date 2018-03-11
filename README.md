Super Snow Globe!
=================

A simple Adafruit Trinket based project to make glowy lights that do things when shaken!

I also wanted to make the base animation code a bit easier to reuse with the power of C++ classes. (well, structs.  But C++ structs are just classes that are default-public.)  Hopefully I don't eat all of my progmem. :B



## Background

I created the base animation code in [`hive13-apiary-arduino`](https://github.com/joedski/hive13-apiary-arduino), but didn't go so macro-happy here, so hopefully it's easy to read.  I'm trying to stick with keeping the implementation in the other files, but keeping all the actual control code in the main file so it's easier to understand what happens.

It's not the most efficient, I'm still using more instantiations than I am mutations, but there's not really enough going on here that this is a problem, not yet anyawy.  Just something to keep in mind.


### Why Structs?

Because I'm a dirty dirty Javascripter and everything is public (unless closed over).


### Animation Methodology

Animations are done based on how much time passed and the animation's given rate.  This way, regardless of the interval between ticks the animation carries out at approximately the same rate, that is an animation that takes 1 second really does take approximately 1 second.

Even in spite of this, I tend to set the "frame" rate to something lke 60Hz, though this really isn't necessary.
