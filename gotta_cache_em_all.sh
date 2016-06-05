./check_log_for_grid ~/.local/share/WSJT-X/wsjtx.log EM35

cat ~/.local/share/WSJT-X/ALL.TXT ~/.local/share/WSJT-X/ALL_WSPR.TXT | 
tr ' ' '\n' | grep '^[A-Z][A-Z][0-9][0-9]$' | sort -u | 
while read thing; do 
  ./check_log_for_grid /dev/null $thing
done

for i in A B C D E F G H I J K L M N O P Q R ; do 
for j in A B C D E F G H I J K L M N O P Q R ; do 
for k in 0 1 2 3 4 5 6 7 8 9; do 
for l in 0 1 2 3 4 5 6 7 8 9; do 
  ./check_log_for_grid /dev/null $i$j$k$l
done 
done 
done 
done 
