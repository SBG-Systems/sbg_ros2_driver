// File header
#include "sbg_utm.h"

// STL headers
#include <cmath>

using sbg::Utm;

Utm::Utm(double latitude, double longitude)
{
  init(latitude, longitude);
}

void Utm::init(double latitude, double longitude)
{
  zone_number_ = computeZoneNumber(latitude, longitude);
  letter_designator_ = computeLetterDesignator(latitude);
  meridian_ = computeMeridian();
  is_init_ = true;
}

void Utm::clear()
{
  is_init_ = false;
  meridian_ = {};
  zone_number_ = {};
  letter_designator_ = {};
}

void Utm::reset(double latitude, double longitude)
{
  clear();
  init(latitude, longitude);
}

bool Utm::isInit() const
{
  return is_init_;
}

int Utm::getZoneNumber() const
{
  return zone_number_;
}

double Utm::getMeridian() const
{
  return meridian_;
}

char Utm::getLetterDesignator() const
{
  return letter_designator_;
}

std::array<double, 2> Utm::computeEastingNorthing(double latitude, double longitude) const
{
  constexpr double RADIANS_PER_DEGREE = M_PI/180.0;

  // WGS84 Parameters
  constexpr double WGS84_A = 6378137.0;        // major axis
  constexpr double WGS84_E = 0.0818191908;     // first eccentricity

  // UTM Parameters
  constexpr double UTM_K0 = 0.9996;              // scale factor
  constexpr double UTM_E2 = (WGS84_E * WGS84_E); // e^2

  constexpr double a = WGS84_A;
  constexpr double eccSquared = UTM_E2;
  constexpr double k0 = UTM_K0;

  double LongOrigin;
  double eccPrimeSquared;
  double N, T, C, A, M;

  // Make sure the longitude is between -180.00 .. 179.9
  double LongTemp = (longitude + 180) - int((longitude + 180) / 360) * 360 - 180;

  double LatRad = latitude * RADIANS_PER_DEGREE;
  double LongRad = LongTemp * RADIANS_PER_DEGREE;
  double LongOriginRad;

  // +3 puts origin in middle of zone
  LongOrigin = (zone_number_ - 1) * 6 - 180 + 3;
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

  double utm_easting = (double)(k0 * N * (A + (1 - T + C) * A * A * A / 6
                                          + (5-18*T+T*T+72*C-58*eccPrimeSquared)*A*A*A*A*A/120)
                                + 500000.0);

  double utm_northing = (double)(k0 * (M + N * tan(LatRad) * (A * A / 2 + (5 - T + 9 * C + 4 * C * C) * A * A * A * A / 24
                                                              + (61-58*T+T*T+600*C-330*eccPrimeSquared)*A*A*A*A*A*A/720)));

  if (latitude < 0)
  {
    utm_northing += 10000000.0; //10000000 meter offset for southern hemisphere
  }

  std::array<double, 2> easting_northing{utm_easting, utm_northing};
  return (easting_northing);
}

int Utm::computeZoneNumber(double latitude, double longitude)
{
  int zoneNumber;

  // Make sure the longitude is between -180.00 .. 179.9
  double LongTemp = (longitude + 180) - int((longitude + 180) / 360) * 360 - 180;

  zoneNumber = int((LongTemp + 180)/6) + 1;

  if ( latitude >= 56.0 && latitude < 64.0 && LongTemp >= 3.0 && LongTemp < 12.0 )
  {
    zoneNumber = 32;
  }

  // Special zones for Svalbard
  if ( latitude >= 72.0 && latitude < 84.0 )
  {
    if (      LongTemp >= 0.0  && LongTemp <  9.0 ) zoneNumber = 31;
    else if ( LongTemp >= 9.0  && LongTemp < 21.0 ) zoneNumber = 33;
    else if ( LongTemp >= 21.0 && LongTemp < 33.0 ) zoneNumber = 35;
    else if ( LongTemp >= 33.0 && LongTemp < 42.0 ) zoneNumber = 37;
  }

  return zoneNumber;
}

char Utm::computeLetterDesignator(double latitude)
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
  else if ((-16 > latitude) && (latitude >= -24)) LetterDesignator = 'K';
  else if ((-24 > latitude) && (latitude >= -32)) LetterDesignator = 'J';
  else if ((-32 > latitude) && (latitude >= -40)) LetterDesignator = 'H';
  else if ((-40 > latitude) && (latitude >= -48)) LetterDesignator = 'G';
  else if ((-48 > latitude) && (latitude >= -56)) LetterDesignator = 'F';
  else if ((-56 > latitude) && (latitude >= -64)) LetterDesignator = 'E';
  else if ((-64 > latitude) && (latitude >= -72)) LetterDesignator = 'D';
  else if ((-72 > latitude) && (latitude >= -80)) LetterDesignator = 'C';
    // 'Z' is an error flag, the Latitude is outside the UTM limits
  else LetterDesignator = 'Z';
  return LetterDesignator;
}

double Utm::computeMeridian() const
{
  return (zone_number_ == 0) ? 0.0 : (zone_number_ - 1) * 6.0 - 177.0;
}
