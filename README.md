LED-Twitter-Due
================

Searches Twitter for hashtag and then pushes to a queue. 

Depending on the number of Tweets calculates display time.

During each internval pops from the queue and updates message array to display on a 8x8 LED Matrix.

Hardware available on [https://www.tindie.com/stores/FriedCircuits/](https://www.tindie.com/stores/FriedCircuits/)

Notes:

There are two files, one for Ethernet and one for use with the WiFly Shield from SparkFun.com

For WiFly make sure you use this library:
https://github.com/dubhunter/WiFly-Shield/tree/client-hardening
You just need to add "#define PSTR(x)  x" at the top of WiFly.h



Also the pins need to be jumpered:
https://forum.sparkfun.com/viewtopic.php?f=13&t=35187



TODO:
Combine both Sketches with option to select if you are using Ethernet or Wifly. 
 
 




License: All source code and designs are released under 

Creative Commons NonCommercial-ShareAlike 

CC BY-NC-SA

![CC BY-NC-SA](http://i.creativecommons.org/l/by-nc-sa/3.0/88x31.png)

Please contact me for commercial opportunities. 