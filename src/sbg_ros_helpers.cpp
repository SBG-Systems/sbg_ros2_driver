// File header
#include "sbg_ros_helpers.h"

// STL includes
#include <cmath>

/*
 * Modification of gps_common::LLtoUTM() to use a constant UTM zone.
 *
 * Convert lat/long to UTM coords.  Equations from USGS Bulletin 1532
 *
 * East Longitudes are positive, West longitudes are negative.
 * North latitudes are positive, South latitudes are negative
 * Lat and Long are in fractional degrees
 *
 * Originally written by Chuck Gantz- chuck.gantz@globalstar.com.
 */
void sbg::helpers::LLtoUTM(double latitude, double longitude, int zone_number, double &utm_northing, double &utm_easting)
{
    const double RADIANS_PER_DEGREE = M_PI/180.0;

    // WGS84 Parameters
    const double WGS84_A = 6378137.0;        // major axis
    const double WGS84_E = 0.0818191908;     // first eccentricity

    // UTM Parameters
    const double UTM_K0 = 0.9996;            // scale factor
    const double UTM_E2 = (WGS84_E*WGS84_E); // e^2

    double a = WGS84_A;
    double eccSquared = UTM_E2;
    double k0 = UTM_K0;

    double LongOrigin;
    double eccPrimeSquared;
    double N, T, C, A, M;

    // Make sure the longitude is between -180.00 .. 179.9
    double LongTemp = (longitude + 180) - int((longitude + 180) / 360) * 360 - 180;

    double LatRad = latitude * RADIANS_PER_DEGREE;
    double LongRad = LongTemp*RADIANS_PER_DEGREE;
    double LongOriginRad;

    // +3 puts origin in middle of zone
    LongOrigin = (zone_number - 1) * 6 - 180 + 3;
    LongOriginRad = LongOrigin * RADIANS_PER_DEGREE;

    eccPrimeSquared = (eccSquared)/(1-eccSquared);

    N = a/sqrt(1-eccSquared*sin(LatRad)*sin(LatRad));
    T = tan(LatRad)*tan(LatRad);
    C = eccPrimeSquared*cos(LatRad)*cos(LatRad);
    A = cos(LatRad)*(LongRad-LongOriginRad);

    M = a*((1 - eccSquared/4      - 3*eccSquared*eccSquared/64     - 5*eccSquared*eccSquared*eccSquared/256)*LatRad
           - (3*eccSquared/8   + 3*eccSquared*eccSquared/32    + 45*eccSquared*eccSquared*eccSquared/1024)*sin(2*LatRad)
           + (15*eccSquared*eccSquared/256 + 45*eccSquared*eccSquared*eccSquared/1024)*sin(4*LatRad)
           - (35*eccSquared*eccSquared*eccSquared/3072)*sin(6*LatRad));

    utm_easting = (double)(k0 * N * (A + (1 - T + C) * A * A * A / 6
                                     + (5-18*T+T*T+72*C-58*eccPrimeSquared)*A*A*A*A*A/120)
                           + 500000.0);

    utm_northing = (double)(k0 * (M + N * tan(LatRad) * (A * A / 2 + (5 - T + 9 * C + 4 * C * C) * A * A * A * A / 24
                                                         + (61-58*T+T*T+600*C-330*eccPrimeSquared)*A*A*A*A*A*A/720)));

    if(latitude < 0)
    {
        utm_northing += 10000000.0; //10000000 meter offset for southern hemisphere
    }
}

/**
 * Get UTM letter designator for the given latitude.
 *
 * @returns 'Z' if latitude is outside the UTM limits of 84N to 80S
 *
 * Written by Chuck Gantz- chuck.gantz@globalstar.com
 */
char sbg::helpers::UTMLetterDesignator(double latitude)
{
    char LetterDesignator;

    if     ((84 >= latitude) && (latitude >= 72)) LetterDesignator = 'X';
    else if ((72 > latitude) && (latitude >= 64)) LetterDesignator = 'W';
    else if ((64 > latitude) && (latitude >= 56)) LetterDesignator = 'V';
    else if ((56 > latitude) && (latitude >= 48)) LetterDesignator = 'U';
    else if ((48 > latitude) && (latitude >= 40)) LetterDesignator = 'T';
    else if ((40 > latitude) && (latitude >= 32)) LetterDesignator = 'S';
    else if ((32 > latitude) && (latitude >= 24)) LetterDesignator = 'R';
    else if ((24 > latitude) && (latitude >= 16)) LetterDesignator = 'Q';
    else if ((16 > latitude) && (latitude >= 8)) LetterDesignator = 'P';
    else if ((8 > latitude) && (latitude >= 0)) LetterDesignator = 'N';
    else if ((0 > latitude) && (latitude >= -8)) LetterDesignator = 'M';
    else if ((-8 > latitude) && (latitude >= -16)) LetterDesignator = 'L';
    else if((-16 > latitude) && (latitude >= -24)) LetterDesignator = 'K';
    else if((-24 > latitude) && (latitude >= -32)) LetterDesignator = 'J';
    else if((-32 > latitude) && (latitude >= -40)) LetterDesignator = 'H';
    else if((-40 > latitude) && (latitude >= -48)) LetterDesignator = 'G';
    else if((-48 > latitude) && (latitude >= -56)) LetterDesignator = 'F';
    else if((-56 > latitude) && (latitude >= -64)) LetterDesignator = 'E';
    else if((-64 > latitude) && (latitude >= -72)) LetterDesignator = 'D';
    else if((-72 > latitude) && (latitude >= -80)) LetterDesignator = 'C';
        // 'Z' is an error flag, the Latitude is outside the UTM limits
    else LetterDesignator = 'Z';
    return LetterDesignator;
}