// File header
#include <sbg_utm.h>

// Test framework
#include <gtest/gtest.h>

namespace
{
  constexpr double  METER_TOLERANCE           = 1.0e-3;
  constexpr double  SOUTHERN_HEMISPHERE_OFFSET = 10000000.0;
}

TEST(SbgUtm, defaultConstructedZoneIsNotInitialized)
{
  const sbg::Utm utm;

  EXPECT_FALSE(utm.isInit());
}

TEST(SbgUtm, initComputesZoneNumberLetterAndMeridian)
{
  // Brest, France.
  const sbg::Utm utm(48.419727, -4.472119);

  EXPECT_TRUE(utm.isInit());
  EXPECT_EQ(utm.getZoneNumber(), 30);
  EXPECT_EQ(utm.getLetterDesignator(), 'U');
  EXPECT_DOUBLE_EQ(utm.getMeridian(), -3.0);
}

TEST(SbgUtm, initHandlesNorwayExceptionZone)
{
  const sbg::Utm utm(60.0, 5.0);

  EXPECT_EQ(utm.getZoneNumber(), 32);
  EXPECT_EQ(utm.getLetterDesignator(), 'V');
}

TEST(SbgUtm, initHandlesSvalbardExceptionZones)
{
  EXPECT_EQ(sbg::Utm(75.0, 5.0).getZoneNumber(),  31);
  EXPECT_EQ(sbg::Utm(75.0, 15.0).getZoneNumber(), 33);
  EXPECT_EQ(sbg::Utm(75.0, 25.0).getZoneNumber(), 35);
  EXPECT_EQ(sbg::Utm(75.0, 35.0).getZoneNumber(), 37);
}

TEST(SbgUtm, letterDesignatorReportsOutOfRangeLatitudes)
{
  EXPECT_EQ(sbg::Utm(0.0, 0.0).getLetterDesignator(),     'N');
  EXPECT_EQ(sbg::Utm(-0.5, 0.0).getLetterDesignator(),    'M');
  EXPECT_EQ(sbg::Utm(85.0, 0.0).getLetterDesignator(),    'Z');
  EXPECT_EQ(sbg::Utm(-85.0, 0.0).getLetterDesignator(),   'Z');
}

TEST(SbgUtm, clearResetsTheZone)
{
  sbg::Utm utm(48.419727, -4.472119);

  utm.clear();

  EXPECT_FALSE(utm.isInit());
  EXPECT_EQ(utm.getZoneNumber(), 0);
}

TEST(SbgUtm, computeEastingNorthingOnZoneMeridian)
{
  const sbg::Utm utm(0.0, -3.0);

  const std::array<double, 2> easting_northing = utm.computeEastingNorthing(0.0, -3.0);

  EXPECT_NEAR(easting_northing[0], 500000.0, METER_TOLERANCE);
  EXPECT_NEAR(easting_northing[1], 0.0,      METER_TOLERANCE);
}

TEST(SbgUtm, computeEastingNorthingWestOfZoneMeridian)
{
  const sbg::Utm utm(48.419727, -4.472119);

  const std::array<double, 2> easting_northing = utm.computeEastingNorthing(48.419727, -4.472119);

  EXPECT_LT(easting_northing[0], 500000.0);
  EXPECT_GT(easting_northing[1], 0.0);
}

TEST(SbgUtm, computeEastingNorthingAppliesSouthernHemisphereOffset)
{
  const sbg::Utm utm(-1.0e-6, -3.0);

  const std::array<double, 2> easting_northing = utm.computeEastingNorthing(-1.0e-6, -3.0);

  EXPECT_NEAR(easting_northing[0], 500000.0,                   METER_TOLERANCE);
  EXPECT_NEAR(easting_northing[1], SOUTHERN_HEMISPHERE_OFFSET, 1.0);
}
