// File header
#include <sbgEComLib.h>

// Test framework
#include <gtest/gtest.h>

// STL headers
#include <cstdint>
#include <vector>

namespace
{
  /*!
   * Raw little endian bytes of the first IMU_SHORT Z delta velocity of the replay fixture.
   *
   * Its most significant byte is 0xFF, which is the case that used to shift a signed int.
   */
  const std::vector<uint8_t>  IMU_SHORT_DELTA_VELOCITY_Z_BYTES{0x9E, 0xBB, 0x68, 0xFF};
  constexpr int32_t           IMU_SHORT_DELTA_VELOCITY_Z = -9913442;

  /*!
   * Read an int32 out of raw little endian bytes.
   */
  int32_t readInt32LE(const std::vector<uint8_t> &ref_bytes)
  {
    SbgStreamBuffer stream_buffer;

    sbgStreamBufferInitForRead(&stream_buffer, ref_bytes.data(), ref_bytes.size());

    const int32_t value = sbgStreamBufferReadInt32LE(&stream_buffer);

    EXPECT_EQ(sbgStreamBufferGetLastError(&stream_buffer), SBG_NO_ERROR);

    return value;
  }

  /*!
   * Write an int32 then read it back through the little endian accessors.
   */
  int32_t roundTripInt32LE(int32_t value)
  {
    uint8_t         buffer[sizeof(int32_t)];
    SbgStreamBuffer stream_buffer;

    sbgStreamBufferInitForWrite(&stream_buffer, buffer, sizeof(buffer));
    EXPECT_EQ(sbgStreamBufferWriteInt32LE(&stream_buffer, value), SBG_NO_ERROR);

    sbgStreamBufferInitForRead(&stream_buffer, buffer, sizeof(buffer));

    return sbgStreamBufferReadInt32LE(&stream_buffer);
  }

  /*!
   * Write an int64 then read it back through the little endian accessors.
   */
  int64_t roundTripInt64LE(int64_t value)
  {
    uint8_t         buffer[sizeof(int64_t)];
    SbgStreamBuffer stream_buffer;

    sbgStreamBufferInitForWrite(&stream_buffer, buffer, sizeof(buffer));
    EXPECT_EQ(sbgStreamBufferWriteInt64LE(&stream_buffer, value), SBG_NO_ERROR);

    sbgStreamBufferInitForRead(&stream_buffer, buffer, sizeof(buffer));

    return sbgStreamBufferReadInt64LE(&stream_buffer);
  }
}

//---------------------------------------------------------------------//
//- Signed readers                                                    -//
//---------------------------------------------------------------------//

TEST(SbgStreamBuffer, readInt32LeDecodesNegativeValues)
{
  EXPECT_EQ(readInt32LE({0x00, 0x00, 0x00, 0x00}), 0);
  EXPECT_EQ(readInt32LE({0xFF, 0xFF, 0xFF, 0xFF}), -1);
  EXPECT_EQ(readInt32LE({0xFF, 0xFF, 0xFF, 0x7F}), INT32_MAX);
  EXPECT_EQ(readInt32LE({0x00, 0x00, 0x00, 0x80}), INT32_MIN);

  //
  // A real device value, taken from the first IMU_SHORT log of the replay fixture.
  //
  EXPECT_EQ(readInt32LE(IMU_SHORT_DELTA_VELOCITY_Z_BYTES), IMU_SHORT_DELTA_VELOCITY_Z);
}

TEST(SbgStreamBuffer, readInt32LeRoundTripsSignedValues)
{
  EXPECT_EQ(roundTripInt32LE(0), 0);
  EXPECT_EQ(roundTripInt32LE(-1), -1);
  EXPECT_EQ(roundTripInt32LE(1), 1);
  EXPECT_EQ(roundTripInt32LE(INT32_MAX), INT32_MAX);
  EXPECT_EQ(roundTripInt32LE(INT32_MIN), INT32_MIN);
  EXPECT_EQ(roundTripInt32LE(IMU_SHORT_DELTA_VELOCITY_Z), IMU_SHORT_DELTA_VELOCITY_Z);
}

TEST(SbgStreamBuffer, writeInt32LeKeepsTheSerializedRepresentation)
{
  uint8_t         buffer[sizeof(int32_t)];
  SbgStreamBuffer stream_buffer;

  sbgStreamBufferInitForWrite(&stream_buffer, buffer, sizeof(buffer));
  ASSERT_EQ(sbgStreamBufferWriteInt32LE(&stream_buffer, IMU_SHORT_DELTA_VELOCITY_Z), SBG_NO_ERROR);

  for (size_t i = 0; i < IMU_SHORT_DELTA_VELOCITY_Z_BYTES.size(); i++)
  {
    EXPECT_EQ(buffer[i], IMU_SHORT_DELTA_VELOCITY_Z_BYTES[i]) << "byte " << i;
  }
}

TEST(SbgStreamBuffer, readInt64LeRoundTripsSignedValues)
{
  EXPECT_EQ(roundTripInt64LE(0), 0);
  EXPECT_EQ(roundTripInt64LE(-1), -1);
  EXPECT_EQ(roundTripInt64LE(INT64_MAX), INT64_MAX);
  EXPECT_EQ(roundTripInt64LE(INT64_MIN), INT64_MIN);

  //
  // The most significant 32 bit word has its top bit set, which is the case that used to
  // shift a signed int64.
  //
  EXPECT_EQ(roundTripInt64LE(INT64_C(-4294967296)), INT64_C(-4294967296));
}

TEST(SbgStreamBuffer, readInt16LeRoundTripsSignedValues)
{
  uint8_t         buffer[sizeof(int16_t)];
  SbgStreamBuffer stream_buffer;

  for (const int16_t value : {static_cast<int16_t>(0), static_cast<int16_t>(-1),
                              static_cast<int16_t>(INT16_MAX), static_cast<int16_t>(INT16_MIN)})
  {
    sbgStreamBufferInitForWrite(&stream_buffer, buffer, sizeof(buffer));
    ASSERT_EQ(sbgStreamBufferWriteInt16LE(&stream_buffer, value), SBG_NO_ERROR);

    sbgStreamBufferInitForRead(&stream_buffer, buffer, sizeof(buffer));

    EXPECT_EQ(sbgStreamBufferReadInt16LE(&stream_buffer), value);
  }
}

//---------------------------------------------------------------------//
//- Unsigned readers, left untouched by the signed shift fix           -//
//---------------------------------------------------------------------//

TEST(SbgStreamBuffer, readUint32LeRoundTripsTheFullRange)
{
  uint8_t         buffer[sizeof(uint32_t)];
  SbgStreamBuffer stream_buffer;

  for (const uint32_t value : {0u, 1u, 0x80000000u, UINT32_MAX})
  {
    sbgStreamBufferInitForWrite(&stream_buffer, buffer, sizeof(buffer));
    ASSERT_EQ(sbgStreamBufferWriteUint32LE(&stream_buffer, value), SBG_NO_ERROR);

    sbgStreamBufferInitForRead(&stream_buffer, buffer, sizeof(buffer));

    EXPECT_EQ(sbgStreamBufferReadUint32LE(&stream_buffer), value);
  }
}

TEST(SbgStreamBuffer, readUint64LeRoundTripsTheFullRange)
{
  uint8_t         buffer[sizeof(uint64_t)];
  SbgStreamBuffer stream_buffer;

  for (const uint64_t value : {UINT64_C(0), UINT64_C(1), UINT64_C(0x8000000000000000), UINT64_MAX})
  {
    sbgStreamBufferInitForWrite(&stream_buffer, buffer, sizeof(buffer));
    ASSERT_EQ(sbgStreamBufferWriteUint64LE(&stream_buffer, value), SBG_NO_ERROR);

    sbgStreamBufferInitForRead(&stream_buffer, buffer, sizeof(buffer));

    EXPECT_EQ(sbgStreamBufferReadUint64LE(&stream_buffer), value);
  }
}
