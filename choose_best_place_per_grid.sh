mv locationcache.txt locationcache.txt.bak

if [ ! -f worldcitiespop.bestpergrid.txt ]; then
gzip -dc worldcitiespop.txt.gz | 
./latlon_to_grid | 
sort -k 9,9 -k 5,5nr -k 10,10n -t , | 
awk -v FS=, '{if ($9 != LASTGRID) { LASTGRID=$9; print;} }' |
tee worldcitiespop.bestpergrid.txt
fi

cat worldcitiespop.bestpergrid.txt |
while read line; do 
	GRID=$(echo "$line" | cut -f 9 -d ,) 
	CCODE=$(echo "$line" | cut -f 1 -d ,) 
	COUNTRY=$(grep -i "^$CCODE" countrycodes.txt | cut -f 2 -d '|')
	RESTOFLINE=$(echo "$line" | cut -f 2-7 -d , | tr ',' '|')
	echo "$GRID|$COUNTRY|$RESTOFLINE|"
done | tee locationcache.txt 


