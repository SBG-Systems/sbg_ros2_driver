// File header
#include "sbg_utm.h"

// Project headers
#include <sbg_ros_helpers.h>

using sbg::SbgUtm;

//---------------------------------------------------------------------//
//- Parameters                                                        -//
//---------------------------------------------------------------------//

double SbgUtm::getEasting() const
{
  return easting_;
}

double SbgUtm::getNorthing() const
{
  return northing_;
}

double SbgUtm::getAltitude() const
{
  return altitude_;
}

int SbgUtm::getZoneNumber() const
{
  return zone_;
}

//---------------------------------------------------------------------//
//- Operations                                                        -//
//---------------------------------------------------------------------//

void SbgUtm::init(double latitude, double longitude, double altitude)
{
  int zoneNumber;

  // Make sure the longitude is between -180.00 .. 179.9
  double LongTemp = (longitude + 180) - int((longitude + 180) / 360) * 360 - 180;

  zoneNumber = int((LongTemp + 180)/6) + 1;

  if( latitude >= 56.0 && latitude < 64.0 && LongTemp >= 3.0 && LongTemp < 12.0 )
  {
    zoneNumber = 32;
  }

  // Special zones for Svalbard
  if( latitude >= 72.0 && latitude < 84.0 )
  {
    if(      LongTemp >= 0.0  && LongTemp <  9.0 ) zoneNumber = 31;
    else if( LongTemp >= 9.0  && LongTemp < 21.0 ) zoneNumber = 33;
    else if( LongTemp >= 21.0 && LongTemp < 33.0 ) zoneNumber = 35;
    else if( LongTemp >= 33.0 && LongTemp < 42.0 ) zoneNumber = 37;
  }

  zone_ = zoneNumber;
  altitude_ = altitude;
  sbg::helpers::LLtoUTM(latitude, longitude, zone_, northing_, easting_);
}