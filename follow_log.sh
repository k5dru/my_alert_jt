wsjtx_log=~/.local/share/WSJT-X/wsjtx.log
wsjtx_all=~/.local/share/WSJT-X/ALL.TXT

if [ ! -f "$wsjtx_log" ]; then 
	echo "Expecting a log file at $wsjtx_log but didn't find it"
	echo "Please edit $0 to point to your wsjt-x log file"
	exit 1 
fi 

if [ ! -f "$wsjtx_all" ]; then 
	echo "Expecting $wsjtx_all but didn't find it"
	echo "Please edit $0 to point to your wsjt-x ALL.TXT file"
	exit 1 
fi 

tail -fn200 $wsjtx_all | 
while read line; do 
  grid=$(echo $line | awk '{print $NF}')
  echo -n "$line -- " | sed 's/CQ/[33mCQ/;s/$/[m/' 
  ./check_log_for_grid $wsjtx_log $grid 
done
