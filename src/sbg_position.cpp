#include "sbg_position.h"

using sbg::Position;

Position::Position(double latitude, double longitude, double altitude)
{
  init(latitude, longitude, altitude);
}

void Position::init(double latitude, double longitude, double altitude)
{
  utm_position_ = convertLLtoUTM(latitude, longitude);
  altitude_ = altitude;
  is_init_ = true;
}

void Position::clear()
{
  is_init_ = false;
  altitude_ = 0.0;
  utm_position_ = {};
}

bool Position::isInit() const
{
  return is_init_;
}

const sbg::Utm &Position::getUtm() const
{
  return utm_position_;
}

double Position::getAltitude() const
{
  return altitude_;
}
