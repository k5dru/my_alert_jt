/* 

  for each new log entry, send an alert of some sort if it is a 
  new grid, new country, or new state/location within country. 

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define FAILURE 1
#define SUCCESS 0


int grid_to_latlon(char *grid, double *out_lat, double *out_lon) 
{ 
  /* from http://stackoverflow.com/questions/31991921/maidenhead-grid-square-functions-in-sas-php-or-javascript */

  char fullgrid[11] = "LL55LL55LL"; 
  int i;  
  double latmult=10;  /*the amount to multiply latitude values by (starting out) */
  double lonmult=20;  /*the amount to multiply longitude values by (starting out) */
  double lon=-180;    /*the zero point for longitude in this system */
  double lat=-90;     /*the zero point for latitude in this system */

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

void calcLocator(char *dst, double lat, double lon) {
/* copied directly from http://ham.stackexchange.com/questions/221/how-can-one-convert-from-lat-long-to-grid-square */
  int o1, o2, o3;
  int a1, a2, a3;
  double remainder;
  // longitude
  remainder = lon + 180.0;
  o1 = (int)(remainder / 20.0);
  remainder = remainder - (double)o1 * 20.0;
  o2 = (int)(remainder / 2.0);
  remainder = remainder - 2.0 * (double)o2;
  o3 = (int)(12.0 * remainder);

  // latitude
  remainder = lat + 90.0;
  a1 = (int)(remainder / 10.0);
  remainder = remainder - (double)a1 * 10.0;
  a2 = (int)(remainder);
  remainder = remainder - (double)a2;
  a3 = (int)(24.0 * remainder);
  dst[0] = (char)o1 + 'A';
  dst[1] = (char)a1 + 'A';
  dst[2] = (char)o2 + '0';
  dst[3] = (char)a2 + '0';
  dst[4] = (char)o3 + 'A';
  dst[5] = (char)a3 + 'A';
  dst[6] = (char)0;
}

double distance_km(double lat1, double lon1, double lat2, double lon2) 
{
  /* simple distance calculation assuming the world is flatish between two nearby points.
     This does NOT work correctly for very far points, but will give a very good value  
     to use to determine relative distances.
   todo:  replace with haversine formula to calculate correctly distance to any point on the globe. */
  return sqrt (
    pow( 110.574 * (lat1 - lat2), 2) + 
    pow( 111.320 * (lon1 - lon2) * cos( ((lat2 + lat1) / 2.0) * 0.017453293), 2)
  );
}

int main(int argc, char **argv) 
{ 
  char uppergrid[11]; 
  int i;

  char linebuffer[1024]; 
  char shortgrid[5];
  double dlat, dlon;
  double centerlat, centerlon;
  char *p, *q; 
  double km;

/* 
  fj,waiyevo government station,Waiyevo Government Station,03,,-16.8,-179.9833333

*/
  char *country, *city, *accentcity, *region, *pop, *lat, *lon;

  memset(linebuffer, ',', sizeof(linebuffer));

  while (fgets(linebuffer, sizeof(linebuffer), stdin)) 
  { 
     p=linebuffer; 

     linebuffer[strlen(linebuffer)-1]=0;

     q = p; while (*q && *q != ',') q++; country = p; p = q + 1; 
     q = p; while (*q && *q != ',') q++; city = p; p = q + 1; 
     q = p; while (*q && *q != ',') q++; accentcity = p; p = q + 1; 
     q = p; while (*q && *q != ',') q++; region = p; p = q + 1; 
     q = p; while (*q && *q != ',') q++; pop = p; p = q + 1; 
     q = p; while (*q && *q != ',') q++; lat = p; p = q + 1; 
     q = p; while (*q && *q != ',') q++; lon = p; p = q + 1; 

     dlat = atof(lat); 
     dlon = atof(lon); 
     calcLocator(uppergrid, dlat, dlon);  
     memcpy(shortgrid, uppergrid, 4); 
     shortgrid[4] = 0; 
     grid_to_latlon(shortgrid, &centerlat, &centerlon); 
     km=distance_km(centerlat, centerlon, dlat, dlon); 

     printf ("%s,%s,%s,%.2f\n", linebuffer, uppergrid, shortgrid, km); 
  }

  exit(0); 
}
