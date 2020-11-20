#include "gpsdata.h"

double dValue;
double GetValue(double dIndex)
{
    double i;
    i = sqrt(dIndex)+1.0;
    return i;
}

unsigned int wgtochina_lb(int wg_flag, unsigned int wg_lng, unsigned int wg_lat, int wg_heit,  int wg_week, unsigned int wg_time, unsigned  int *china_lng, unsigned int *china_lat)
{
    double dGps;
    dGps = GetValue(wg_heit);
    dGps += 0 ;

    *china_lng = wg_lng+20000;
    *china_lat = wg_lat+20000;

    return 0;
}

