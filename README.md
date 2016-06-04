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

INSTRUCTIONS:

git clone https://github.com/k5dru/my_alert_jt

cd my_alert_jt/

chmod +x make.sh 

./make.sh 

chmod +x follow_log.sh 

./follow_log.sh
  
That's it.  Enjoy.  K5DRU
