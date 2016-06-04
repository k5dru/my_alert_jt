/* 

	for each new log entry, send an alert of some sort if it is a 
	new grid, new country, or new state/location within country. 

    weekend hack project by K5DRU 

    totally free software; improve it if you want.  

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "countrycodes.h"

#define INFILENAME "worldcitiespop.sorted_lon_lat.txt"
#define CACHEFILE "locationcache.txt" 
#define LOGFILE "/home/lemley/.local/share/WSJT-X/wsjtx.log" 
#define SEARCHDEGREES 1

FILE *infile = NULL; 
long infile_size = 0;
#define FILEBUFFERSIZE 32768
static char filebuffer[FILEBUFFERSIZE];
long current_filepos = -9999; 

#define xDEBUG 1

typedef struct { 
  char Country[120]; 
  char City[80];
  char AccentCity[80]; 
  char Region[80]; 
  char Population[80]; 
  char Latitude[80]; 
  char Longitude[80];
  char Grid[11]; 
} city_struct; 

city_struct current_city;

void print_city(city_struct *c) 
{
	printf ("%s|%s|%s|%s|%s|%s|%s|%s\n", c->Country, c->City, 
		c->AccentCity, c->Region,c->Population, c->Latitude, c->Longitude, c->Grid);
}


void addtolocationcache(city_struct *c) 
{
	FILE *outfile;
	
	if (NULL == (outfile = fopen(CACHEFILE, "ab")))
		return; 

	fprintf (outfile, "%s|%s|%s|%s|%s|%s|%s|%s|\n", c->Grid, c->Country, c->City, 
		c->AccentCity, c->Region,c->Population, c->Latitude, c->Longitude);

	fclose(outfile);
}

#define FAILURE 1
#define SUCCESS 0

int checklocationcache(char *grid, city_struct *c) 
{ 
	/* return 1 if found, 0 if not or error. */ 
	FILE *cachefile;
	char cachestring[255]; 
	int retval = FAILURE;

	if (NULL == (cachefile = fopen(CACHEFILE, "r"))) 
		return retval; 
	
	while (fgets(cachestring, sizeof(cachestring), cachefile)) 
	{ 
	if (!strncmp(cachestring, grid, strlen(grid)))
	{ 
		char *p, *q;
#ifdef DEBUG 
		printf("Got from locationcache: %s", cachestring); 
#endif 

		/* parse into current city */ 
		p = cachestring; 

#define populate(X) {q = p; while (*q != '|' && *q != '\n') q++; memcpy(c->X, p, q-p); c->X[q-p] = 0; p = q+1;}
		populate (Grid);
 		populate (Country);
		populate (City);
		populate (AccentCity);
		populate (Region);
		populate (Population);
		populate (Latitude);
		populate (Longitude);
#undef populate 

#ifdef DEBUG 
		printf("checklocationcache: setting SUCCESS\n", cachestring); 
#endif 
		retval = SUCCESS;
		break;
	}
	}
	fclose(cachefile); 
	return retval;
}


long get_city(long search_filepos) 
{ 
	char *p; 
	char *q;

#ifdef DEBUG3 
	printf ("in get_city, search_filepos = %ld, current_filepos = %ld \n", search_filepos, current_filepos); 
#endif 

	if (
            search_filepos < current_filepos ||
	    search_filepos > current_filepos + FILEBUFFERSIZE - sizeof(city_struct)
	)
	{ 
		current_filepos = search_filepos;
		if (current_filepos > infile_size - FILEBUFFERSIZE) 
			current_filepos = infile_size - FILEBUFFERSIZE; 
#ifdef DEBUG 
	printf ("reading new buffer at %ld\n", current_filepos); 
#endif 
		fseek(infile, current_filepos, SEEK_SET); 
		fread(filebuffer, FILEBUFFERSIZE, 1, infile); 
	}

	p = filebuffer + (search_filepos - current_filepos);

	while (*p != '\n')
		p++; 
	p++; /* new record */

#define populate(X) {q = p; while (*q != ',' && *q != '\n') q++; memcpy(current_city.X, p, q-p); current_city.X[q-p] = 0; p = q+1;}
 	populate (Country);
	populate (City);
	populate (AccentCity);
	populate (Region);
	populate (Population);
	populate (Latitude);
	populate (Longitude);
#undef populate 
	current_city.Grid[0] = 0; 

#ifdef DEBUG3
	print_city(&current_city);
#endif 

	/* return next city's position - actually the newline before that position */
	return current_filepos + (q - filebuffer);
}


long get_longitude_filepos(double search_lon) 
{ 
	/*  find the position of the file that is input longitude, roughly */
	long pos_a=0;
	long pos_b=infile_size;
	long testpos;

	while ( pos_b > pos_a + sizeof(city_struct) )
	{
		testpos=(pos_a + pos_b) / 2;
		get_city(testpos); 

		if  (atof(current_city.Longitude) < search_lon)
			pos_a=testpos;
		else
			pos_b=testpos;
	}
	return testpos;
}	

int grid_to_latlon(char *grid, double *out_lat, double *out_lon) 
{ 
	/* from http://stackoverflow.com/questions/31991921/maidenhead-grid-square-functions-in-sas-php-or-javascript */

	char fullgrid[11] = "LL55LL55LL"; 
	int i;	
	double latmult=10;  /*the amount to multiply latitude values by (starting out) */
	double lonmult=20;  /*the amount to multiply longitude values by (starting out) */
	double lon=-180;  /*the zero point for longitude in this system */
	double lat=-90;   /*the zero point for latitude in this system */


	if (strlen(grid) > 10 || strlen(grid) < 4)
	{ 
		return FAILURE; 
	}

	if (!strncmp(grid, "RR73", 4))
		return FAILURE; 
	if (!strncmp(grid, "TU73", 4))
		return FAILURE; 
	if (!strncmp(grid, "JT65", 4))
		return FAILURE; 

#ifdef DEBUG
	printf("checking Grid: [%s] ", grid); 
#endif 

	for (i = 0; i < strlen(grid); i++) 
	{ 	
		if (!((i >> 1) & 0x01))  /* must be letter */ 
		{
#ifdef DEBUG
	printf("L:%c", grid[i]); 
#endif 
			if ( !(grid[i] >= 'a' && grid[i] <= 'z') && !(grid[i] >= 'A' && grid[i] <= 'Z')) 
				return FAILURE; 
		}
		else 
		{ 
#ifdef DEBUG
	printf("N:%c", grid[i]); 
#endif 
			if ( !(grid[i] >= '0' && grid[i] <= '9'))
				return FAILURE; 
		}
	}
	
	memcpy(fullgrid, grid, strlen(grid));
	
#ifdef DEBUG
	printf("Grid: %s ", fullgrid); 
#endif 

	for (i = 0; i < 10; i+=2) 
	{ 
 	   fullgrid[i] = toupper(fullgrid[i]); 
 	   fullgrid[i+1] = toupper(fullgrid[i+1]); 

	   if (fullgrid[i] >= 'A' && fullgrid[i] <= 'Z')  /* letters */
	   {
		if (i > 0)
		{
			lonmult=lonmult/24;
			latmult=latmult/24;
		}
		lon += lonmult*(fullgrid[i] - 'A');
   		lat += latmult*(fullgrid[i+1]-'A');
	   } 
	   else 
	   {
		latmult=latmult/10;
		lonmult=lonmult/10;
		lon += lonmult*(fullgrid[i] - '0');
   		lat += latmult*(fullgrid[i+1]-'0');
	   }
	}
	*out_lat = lat; 
	*out_lon = lon; 

#ifdef DEBUG
	printf("lat %f lon %f\n", lat, lon); 
#endif 

	return SUCCESS; 
}

int load_city_from_grid(char *uppergrid) 
{ 
	long lowerpos, upperpos; 
	static city_struct closest_city; 
	double closest_distance = 9999;
	double this_distance;
	double searchlat; 
	double searchlon; 
	int ret;
	int found_in_locationcache = 0; 
	int i;

	found_in_locationcache = checklocationcache(uppergrid, &closest_city); 
	if (found_in_locationcache == FAILURE)
	{ 

#ifdef DEBUG 
	printf ("starting new search\n"); 
#endif 
	if (NULL == (infile = fopen(INFILENAME, "rb"))) 
		exit(1); 
	fseek(infile, 0, SEEK_END); 
	infile_size=ftell(infile); 
	fseek(infile, 0, SEEK_SET); 

	ret = grid_to_latlon(uppergrid, &searchlat, &searchlon); 

	if (ret == FAILURE) 
	{ 
		printf("Grid fail. \n"); 
		return FAILURE; 
	}

	lowerpos = get_longitude_filepos(searchlon-SEARCHDEGREES); 
	upperpos = get_longitude_filepos(searchlon+SEARCHDEGREES); 

	closest_distance=9999;
#ifdef DEBUG 
	printf("Searching %ld bytes of input file for location.\n", upperpos - lowerpos); 
#endif 

	while (lowerpos < upperpos) 
	{ 
		lowerpos = get_city(lowerpos); 
		this_distance = sqrt(
			pow(atof(current_city.Latitude) - searchlat, 2) + 
			pow(atof(current_city.Longitude) - searchlon, 2)
		);
		if (this_distance < closest_distance) 
		{ 
			closest_distance = this_distance;
			closest_city = current_city;
#ifdef DEBUG
			printf("New closest city at %f: ", closest_distance);
			print_city(&closest_city); 
#endif 
		}
		/* we can quit early if we provably move farther away in longitude than the closest disntace. */
		/* implement this later maybe. */
	}
	strncpy(closest_city.Grid, uppergrid, 10);
	/* fix up found country code */ 
	{
		char tmp[120]; 
		strncpy(tmp, countrylookup(closest_city.Country), 120); 
		strncpy(closest_city.Country, tmp, 120);
	}
	fclose(infile); 
        
	} /* end if not found in locationcache */

/*	print_city(&closest_city);  */

	if (found_in_locationcache == FAILURE)
		addtolocationcache(&closest_city);

	current_city = closest_city;

#ifdef DEBUG 
	printf("leaving load_city_from_grid SUCCESS\n"); 
#endif 
	return SUCCESS; 
}

int check_city_against_log(city_struct *input_city, city_struct *workedbefore) 
{
	/* do whatever I need to check input_city's data against all the grids in the logfile*/
	FILE *logfile;
	char logline[255];
	char *p; 
	char *q; 
	char tempgrid[11];
	
	if (NULL == (logfile = fopen(LOGFILE, "r"))) 
		return FAILURE;  /* failure */

	while (fgets(logline, sizeof(logline), logfile)) 
	{ 
		p = logline;
		while(*(p++) != ','); 
		while(*(p++) != ','); 
		while(*(p++) != ','); 
		q = p; 
		while(*q != ',')
			q++; 
		memcpy(tempgrid, p, q-p); 
		tempgrid[q-p]=0; 
		if (load_city_from_grid(tempgrid) == SUCCESS)  /* success */
		{ 
#define populate(X) {if (!strcmp(input_city->X, current_city.X)) strcpy(workedbefore->X, "Y");} 
		 	populate (Country);
			populate (City);
			populate (AccentCity);
			populate (Region);
			populate (Population);
			populate (Latitude);
			populate (Longitude);
			populate (Grid);
#undef populate 
		}
	}
	
	fclose(logfile); 
} 

int exit_normal_failure() 
{ 
	printf ("\n"); 
	exit(0); 
} 


int main(int argc, char **argv) 
{ 
	char uppergrid[11]; 
	int ret, i;
	city_struct this_city, workedbefore;

	if (argc > 1) 
	{
		double checklat, checklon; 

		if (strlen(argv[1]) < 4 || strlen(argv[1])> 10) 
			exit_normal_failure(); 

		for (i = 0; i <= strlen(argv[1]); i++) 
			uppergrid[i] = toupper(argv[1][i]); 

#ifdef DEBUG 
	printf ("checking grid validity\n"); 
#endif 

		if (FAILURE == grid_to_latlon(uppergrid, &checklat, &checklon)) 
			exit_normal_failure(); 

#ifdef DEBUG 
	printf ("calling load_city_from_grid()\n"); 
#endif 

		if (FAILURE == load_city_from_grid(uppergrid)) 
			exit_normal_failure(); 

#ifdef DEBUG 
	printf ("back from load_city_from_grid() successfully\n"); 
#endif 
	}
	else
	{ 
		printf ("Usage:  %s gridsquare\n", argv[0]); 
		exit (1); 
	}

	this_city = current_city; 
	
	memset(&workedbefore, 0, sizeof(city_struct));

#ifdef DEBUG 
	printf ("current city lookup acheived; log search starting\n"); 
#endif 

	check_city_against_log(&this_city, &workedbefore);
		
#ifdef DEBUG 
	printf ("back from check_city_against_log\n"); 
#endif 

#define OMG "\033[1;33;44m"
#define NEWREG "\033[44m"
#define NEWGRID "\033[1m"
#define NORMAL "\033[m"

	if (workedbefore.Country[0] != 'Y') printf("%sNEW COUNTRY!%s ", OMG, NORMAL); 
	else 
		if (workedbefore.Region[0] != 'Y') printf("%sNew Region!%s ", NEWREG, NORMAL); 
		else
			if (workedbefore.Grid[0] != 'Y') printf("%snew grid.%s ", NEWGRID, NORMAL); 

	print_city(&this_city); 

	return SUCCESS; 
}
