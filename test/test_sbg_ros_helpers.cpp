// File header
#include <sbg_ros_helpers.h>

// Test framework
#include <gtest/gtest.h>

// STL headers
#include <cmath>

using sbg::helpers::NmeaGGAQuality;

namespace
{
  constexpr float   ANGLE_TOLERANCE     = 1.0e-5f;
  constexpr double  POSITION_TOLERANCE  = 1.0e-3;

  constexpr double  EQUATORIAL_RADIUS   = 6378137.0;
  constexpr double  POLAR_RADIUS        = 6356752.314245;
}

TEST(SbgRosHelpers, wrapAnglePiKeepsAngleInRange)
{
  EXPECT_NEAR(sbg::helpers::wrapAnglePi(0.0f),                  0.0f,           ANGLE_TOLERANCE);
  EXPECT_NEAR(sbg::helpers::wrapAnglePi(SBG_PI_F / 2.0f),       SBG_PI_F / 2.0f, ANGLE_TOLERANCE);
  EXPECT_NEAR(sbg::helpers::wrapAnglePi(-SBG_PI_F / 2.0f),      -SBG_PI_F / 2.0f, ANGLE_TOLERANCE);
  EXPECT_NEAR(sbg::helpers::wrapAnglePi(3.0f * SBG_PI_F / 2.0f), -SBG_PI_F / 2.0f, ANGLE_TOLERANCE);
  EXPECT_NEAR(sbg::helpers::wrapAnglePi(-3.0f * SBG_PI_F / 2.0f), SBG_PI_F / 2.0f, ANGLE_TOLERANCE);
  EXPECT_NEAR(sbg::helpers::wrapAnglePi(2.0f * SBG_PI_F),       0.0f,           ANGLE_TOLERANCE);
}

TEST(SbgRosHelpers, wrapAngle360KeepsAngleInRange)
{
  EXPECT_NEAR(sbg::helpers::wrapAngle360(0.0f),     0.0f,   ANGLE_TOLERANCE);
  EXPECT_NEAR(sbg::helpers::wrapAngle360(90.0f),    90.0f,  ANGLE_TOLERANCE);
  EXPECT_NEAR(sbg::helpers::wrapAngle360(-90.0f),   270.0f, ANGLE_TOLERANCE);
  EXPECT_NEAR(sbg::helpers::wrapAngle360(450.0f),   90.0f,  ANGLE_TOLERANCE);
  EXPECT_NEAR(sbg::helpers::wrapAngle360(-450.0f),  270.0f, ANGLE_TOLERANCE);
}

TEST(SbgRosHelpers, isLeapYearFollowsGregorianRules)
{
  EXPECT_TRUE(sbg::helpers::isLeapYear(2000));
  EXPECT_TRUE(sbg::helpers::isLeapYear(2024));
  EXPECT_FALSE(sbg::helpers::isLeapYear(1900));
  EXPECT_FALSE(sbg::helpers::isLeapYear(2023));
}

TEST(SbgRosHelpers, getNumberOfDaysInYear)
{
  EXPECT_EQ(sbg::helpers::getNumberOfDaysInYear(2023), 365u);
  EXPECT_EQ(sbg::helpers::getNumberOfDaysInYear(2024), 366u);
}

TEST(SbgRosHelpers, getNumberOfDaysInMonth)
{
  EXPECT_EQ(sbg::helpers::getNumberOfDaysInMonth(2023, 1),  31u);
  EXPECT_EQ(sbg::helpers::getNumberOfDaysInMonth(2023, 2),  28u);
  EXPECT_EQ(sbg::helpers::getNumberOfDaysInMonth(2024, 2),  29u);
  EXPECT_EQ(sbg::helpers::getNumberOfDaysInMonth(2023, 4),  30u);
  EXPECT_EQ(sbg::helpers::getNumberOfDaysInMonth(2023, 12), 31u);
}

TEST(SbgRosHelpers, getUtcOffsetReturnsDefaultWhenUtcIsNotValid)
{
  EXPECT_EQ(sbg::helpers::getUtcOffset(false, 0, 0), 18);
}

TEST(SbgRosHelpers, getUtcOffsetIsComputedFromValidUtc)
{
  //
  // GPS time of week is 100 s, so the GPS second in minute is 40 while the UTC one is 22.
  //
  EXPECT_EQ(sbg::helpers::getUtcOffset(true, 100000, 22), 18);
}

TEST(SbgRosHelpers, getUtcOffsetHandlesMinuteRollover)
{
  //
  // GPS time of week is 10 s, so the GPS second in minute is 10 while the UTC one is 52.
  //
  EXPECT_EQ(sbg::helpers::getUtcOffset(true, 10000, 52), 18);
}

TEST(SbgRosHelpers, convertSbgGpsTypeToNmeaGpsType)
{
  EXPECT_EQ(sbg::helpers::convertSbgGpsTypeToNmeaGpsType(SBG_ECOM_GNSS_POS_TYPE_NO_SOLUTION), NmeaGGAQuality::INVALID);
  EXPECT_EQ(sbg::helpers::convertSbgGpsTypeToNmeaGpsType(SBG_ECOM_GNSS_POS_TYPE_SINGLE), NmeaGGAQuality::SINGLE);
  EXPECT_EQ(sbg::helpers::convertSbgGpsTypeToNmeaGpsType(SBG_ECOM_GNSS_POS_TYPE_SBAS), NmeaGGAQuality::DGPS);
  EXPECT_EQ(sbg::helpers::convertSbgGpsTypeToNmeaGpsType(SBG_ECOM_GNSS_POS_TYPE_PPP_INT), NmeaGGAQuality::PPS);
  EXPECT_EQ(sbg::helpers::convertSbgGpsTypeToNmeaGpsType(SBG_ECOM_GNSS_POS_TYPE_RTK_INT), NmeaGGAQuality::RTK_FIXED);
  EXPECT_EQ(sbg::helpers::convertSbgGpsTypeToNmeaGpsType(SBG_ECOM_GNSS_POS_TYPE_RTK_FLOAT), NmeaGGAQuality::RTK_FLOAT);
}

TEST(SbgRosHelpers, convertLLAtoECEFAtOrigin)
{
  const sbg::SbgVector3d ecef = sbg::helpers::convertLLAtoECEF(0.0, 0.0, 0.0);

  EXPECT_NEAR(ecef(0), EQUATORIAL_RADIUS, POSITION_TOLERANCE);
  EXPECT_NEAR(ecef(1), 0.0,               POSITION_TOLERANCE);
  EXPECT_NEAR(ecef(2), 0.0,               POSITION_TOLERANCE);
}

TEST(SbgRosHelpers, convertLLAtoECEFAtNorthPole)
{
  const sbg::SbgVector3d ecef = sbg::helpers::convertLLAtoECEF(90.0, 0.0, 0.0);

  EXPECT_NEAR(ecef(0), 0.0,           POSITION_TOLERANCE);
  EXPECT_NEAR(ecef(1), 0.0,           POSITION_TOLERANCE);
  EXPECT_NEAR(ecef(2), POLAR_RADIUS,  POSITION_TOLERANCE);
}

TEST(SbgRosHelpers, convertLLAtoECEFAppliesAltitude)
{
  const sbg::SbgVector3d ecef = sbg::helpers::convertLLAtoECEF(0.0, 90.0, 1000.0);

  EXPECT_NEAR(ecef(0), 0.0,                           POSITION_TOLERANCE);
  EXPECT_NEAR(ecef(1), EQUATORIAL_RADIUS + 1000.0,    POSITION_TOLERANCE);
  EXPECT_NEAR(ecef(2), 0.0,                           POSITION_TOLERANCE);
}
