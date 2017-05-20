# my_alert_jt
Linux command-line program to display additional information from WSJT-X logfile, 
like where a grid is and if you've worked that place before in WSJT-X.  

I banged this out in a weekend, after becoming frustrated with WSJT-X which displays 
the general area of the world you are receiving based on call-sign decodes. People 
often free-form their CQ messages, or operate from other regions, so I think decoding 
based on grid square would be more useful. What would be even better is if I could 
instantly see if I have worked a country, state/region or grid before. If I used 
Windows I could use one or more great programs for this purpose, but I don't.

This is the initial version of this program, functional for me, but still crufty and 
hard-coded in spots. By putting it here, I hope that it can be of service to both of
the other ham-radio operators on Linux who are into JT modes and frustrated by the   
lack of insight in the base program.  

I used the MaxMind data set to find the nearest (sort of) point to the center of 
a grid square. NOTE: this dataset has all sorts of obscure little places, so it is likely 
that the center of a square is some obscure little place.  Note also! The center of a 
square may be close to a different US State from which a caller is calling, particularly 
for the tiny little states in the northeast. So, this program can't be fully relied on
for WAS, but will certainly get you close.

INSTRUCTIONS:

<pre>
  git clone https://github.com/k5dru/my_alert_jt
  cd my_alert_jt/
  chmod +x follow_log.sh 
  cc -O2 check_log_for_grid.c -o check_log_for_grid -lm
  ./follow_log.sh
</pre>

That's it.  Enjoy.  K5DRU

  "This product includes data created by MaxMind, available from http://www.maxmind.com/ and
   Copyright (c) 2008 MaxMind Inc.  All Rights Reserved."

No warranties; attribution appreciated. 

Edit 2017-05-20: I probably won't be updating this project. Other projects may find 
useful the file of world cities and their calculated grid location and distance from 
grid centroid. For instance grid FN42 is best understood as "Boston, MA, USA" even 
though a point in that grid could be in any of three US states. I've attempted to 
use the most populated place name as a recognizable marker for a grid, and that has worked 
reasonably well for me. YMMV. Good luck. K5DRU 

