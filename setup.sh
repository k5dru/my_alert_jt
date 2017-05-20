echo compiling. 
cc -O2 check_log_for_grid.c countrycodes.h -o check_log_for_grid -lm 

echo download a 33MB list of city/region data
if [ ! -e worldcitiespop.txt.gz ]; then 
wget http://download.maxmind.com/download/worldcities/worldcitiespop.txt.gz
fi

echo sorting... wait plox. 
gzip -dc worldcitiespop.txt.gz | sort -k 7,7n -k 6,6n -t , > worldcitiespop.sorted_lon_lat.txt

. choose_best_place_per_grid.sh

echo done.  fix any errors. 
echo then run ./follow_log.sh

