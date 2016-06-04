tail -fn200  ~/.local/share/WSJT-X/ALL.TXT  | while read line; do grid=$(echo $line | awk '{print $NF}'); echo -n $line ' -- ' | sed 's/CQ/[33mCQ/;s/$/[m/'; ./check_log_for_grid $grid; done
