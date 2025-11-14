// sbgCommonLib headers
#include <sbgCommon.h>

// Local headers
#include "sbgEComDefsGnss.h"

//----------------------------------------------------------------------//
//- Private definitions                                                -//
//----------------------------------------------------------------------//

/*!
 * GNSS signal descriptor.
 */
typedef struct _SbgEComSignalIdDesc
{
    SbgEComSignalId                      id;                                        /*!< Signal ID. */
    const char                          *pName;                                     /*!< Corresponding NULL terminated C string. */
} SbgEComSignalIdDesc;

//----------------------------------------------------------------------//
//- Public functions                                                   -//
//----------------------------------------------------------------------//

SbgEComConstellationId sbgEComGetConstellationFromSignalId(SbgEComSignalId signalId)
{
    SbgEComConstellationId  constellationId = SBG_ECOM_CONSTELLATION_ID_UNKNOWN;

    //
    // Don't use a default catch to explicitly handle ALL enum values
    //
    switch (signalId)
    {
    case SBG_ECOM_SIGNAL_ID_UNKNOWN:
        constellationId = SBG_ECOM_CONSTELLATION_ID_UNKNOWN;
        break;

    //
    // GPS constellation
    //
    case SBG_ECOM_SIGNAL_ID_GPS_L1C_DP:
    case SBG_ECOM_SIGNAL_ID_GPS_L1C_D:
    case SBG_ECOM_SIGNAL_ID_GPS_L1C_P:
    case SBG_ECOM_SIGNAL_ID_GPS_L1_W:
    case SBG_ECOM_SIGNAL_ID_GPS_L1_CA:
    case SBG_ECOM_SIGNAL_ID_GPS_L1P:
    case SBG_ECOM_SIGNAL_ID_GPS_L1_PY:
    case SBG_ECOM_SIGNAL_ID_GPS_L1M:
    case SBG_ECOM_SIGNAL_ID_GPS_L2C_ML:
    case SBG_ECOM_SIGNAL_ID_GPS_L2C_L:
    case SBG_ECOM_SIGNAL_ID_GPS_L2_SEMICL:
    case SBG_ECOM_SIGNAL_ID_GPS_L2_W:
    case SBG_ECOM_SIGNAL_ID_GPS_L2_CA:
    case SBG_ECOM_SIGNAL_ID_GPS_L2C_M:
    case SBG_ECOM_SIGNAL_ID_GPS_L2_PY:
    case SBG_ECOM_SIGNAL_ID_GPS_L2M:
    case SBG_ECOM_SIGNAL_ID_GPS_L2P:
    case SBG_ECOM_SIGNAL_ID_GPS_L5_IQ:
    case SBG_ECOM_SIGNAL_ID_GPS_L5_I:
    case SBG_ECOM_SIGNAL_ID_GPS_L5_Q:
        constellationId = SBG_ECOM_CONSTELLATION_ID_GPS;
        break;

    //
    // GLONASS constellation
    //
    case SBG_ECOM_SIGNAL_ID_GLONASS_G1_P:
    case SBG_ECOM_SIGNAL_ID_GLONASS_G1_CA:
    case SBG_ECOM_SIGNAL_ID_GLONASS_G2_P:
    case SBG_ECOM_SIGNAL_ID_GLONASS_G2_CA:
    case SBG_ECOM_SIGNAL_ID_GLONASS_G3_I:
    case SBG_ECOM_SIGNAL_ID_GLONASS_G3_Q:
    case SBG_ECOM_SIGNAL_ID_GLONASS_G3_IQ:
        constellationId = SBG_ECOM_CONSTELLATION_ID_GLONASS;
        break;

    //
    // Galileo constellation
    //
    case SBG_ECOM_SIGNAL_ID_GALILEO_E1_BC:
    case SBG_ECOM_SIGNAL_ID_GALILEO_E1_C:
    case SBG_ECOM_SIGNAL_ID_GALILEO_E1_B:
    case SBG_ECOM_SIGNAL_ID_GALILEO_E1_A:
    case SBG_ECOM_SIGNAL_ID_GALILEO_E1_ABC:
    case SBG_ECOM_SIGNAL_ID_GALILEO_E5B_IQ:
    case SBG_ECOM_SIGNAL_ID_GALILEO_E5B_I:
    case SBG_ECOM_SIGNAL_ID_GALILEO_E5B_Q:
    case SBG_ECOM_SIGNAL_ID_GALILEO_E5A_IQ:
    case SBG_ECOM_SIGNAL_ID_GALILEO_E5A_I:
    case SBG_ECOM_SIGNAL_ID_GALILEO_E5A_Q:
    case SBG_ECOM_SIGNAL_ID_GALILEO_E5_IQ:
    case SBG_ECOM_SIGNAL_ID_GALILEO_E5_I:
    case SBG_ECOM_SIGNAL_ID_GALILEO_E5_Q:
    case SBG_ECOM_SIGNAL_ID_GALILEO_E6_BC:
    case SBG_ECOM_SIGNAL_ID_GALILEO_E6_C:
    case SBG_ECOM_SIGNAL_ID_GALILEO_E6_B:
    case SBG_ECOM_SIGNAL_ID_GALILEO_E6_ABC:
    case SBG_ECOM_SIGNAL_ID_GALILEO_E6_A:
        constellationId = SBG_ECOM_CONSTELLATION_ID_GALILEO;
        break;

    //
    // BeiDou constellation
    //
    case SBG_ECOM_SIGNAL_ID_BEIDOU_B1IQ:
    case SBG_ECOM_SIGNAL_ID_BEIDOU_B1I:
    case SBG_ECOM_SIGNAL_ID_BEIDOU_B1Q:
    case SBG_ECOM_SIGNAL_ID_BEIDOU_B1C_P:
    case SBG_ECOM_SIGNAL_ID_BEIDOU_B1C_DP:
    case SBG_ECOM_SIGNAL_ID_BEIDOU_B1C_D:
    case SBG_ECOM_SIGNAL_ID_BEIDOU_B1A_P:
    case SBG_ECOM_SIGNAL_ID_BEIDOU_B1A_DP:
    case SBG_ECOM_SIGNAL_ID_BEIDOU_B1A_D:
    case SBG_ECOM_SIGNAL_ID_BEIDOU_B2IQ:
    case SBG_ECOM_SIGNAL_ID_BEIDOU_B2I:
    case SBG_ECOM_SIGNAL_ID_BEIDOU_B2A_P:
    case SBG_ECOM_SIGNAL_ID_BEIDOU_B2A_DP:
    case SBG_ECOM_SIGNAL_ID_BEIDOU_B2A_D:
    case SBG_ECOM_SIGNAL_ID_BEIDOU_B2Q:
    case SBG_ECOM_SIGNAL_ID_BEIDOU_B2B_P:
    case SBG_ECOM_SIGNAL_ID_BEIDOU_B2B_DP:
    case SBG_ECOM_SIGNAL_ID_BEIDOU_B2B_D:
    case SBG_ECOM_SIGNAL_ID_BEIDOU_B2AB_P:
    case SBG_ECOM_SIGNAL_ID_BEIDOU_B2AB_DP:
    case SBG_ECOM_SIGNAL_ID_BEIDOU_B2AB_D:
    case SBG_ECOM_SIGNAL_ID_BEIDOU_B3IQ:
    case SBG_ECOM_SIGNAL_ID_BEIDOU_B3I:
    case SBG_ECOM_SIGNAL_ID_BEIDOU_B3Q:
    case SBG_ECOM_SIGNAL_ID_BEIDOU_B3A_D:
    case SBG_ECOM_SIGNAL_ID_BEIDOU_B3A_P:
    case SBG_ECOM_SIGNAL_ID_BEIDOU_B3A_DP:
        constellationId = SBG_ECOM_CONSTELLATION_ID_BEIDOU;
        break;

    //
    // QZSS constellation
    //
    case SBG_ECOM_SIGNAL_ID_QZSS_L1C_DP:
    case SBG_ECOM_SIGNAL_ID_QZSS_L1C_D:
    case SBG_ECOM_SIGNAL_ID_QZSS_L1C_P:
    case SBG_ECOM_SIGNAL_ID_QZSS_L1_CA:
    case SBG_ECOM_SIGNAL_ID_QZSS_L1_SAIF:
    case SBG_ECOM_SIGNAL_ID_QZSS_L1_SB:
    case SBG_ECOM_SIGNAL_ID_QZSS_L2C_ML:
    case SBG_ECOM_SIGNAL_ID_QZSS_L2C_L:
    case SBG_ECOM_SIGNAL_ID_QZSS_L2C_M:
    case SBG_ECOM_SIGNAL_ID_QZSS_L5_IQ:
    case SBG_ECOM_SIGNAL_ID_QZSS_L5_I:
    case SBG_ECOM_SIGNAL_ID_QZSS_L5_Q:
    case SBG_ECOM_SIGNAL_ID_QZSS_L5S_IQ:
    case SBG_ECOM_SIGNAL_ID_QZSS_L5S_I:
    case SBG_ECOM_SIGNAL_ID_QZSS_L5S_Q:
    case SBG_ECOM_SIGNAL_ID_QZSS_L6_P:
    case SBG_ECOM_SIGNAL_ID_QZSS_L6_DP:
    case SBG_ECOM_SIGNAL_ID_QZSS_L6_D:
    case SBG_ECOM_SIGNAL_ID_QZSS_L6_E:
    case SBG_ECOM_SIGNAL_ID_QZSS_L6_DE:
        constellationId = SBG_ECOM_CONSTELLATION_ID_QZSS;
        break;

    //
    // SBAS system
    //
    case SBG_ECOM_SIGNAL_ID_SBAS_L1_CA:
    case SBG_ECOM_SIGNAL_ID_SBAS_L5_I:
    case SBG_ECOM_SIGNAL_ID_SBAS_L5_Q:
    case SBG_ECOM_SIGNAL_ID_SBAS_L5_IQ:
        constellationId = SBG_ECOM_CONSTELLATION_ID_SBAS;
        break;

    //
    // IRNSS constellation
    //
    case SBG_ECOM_SIGNAL_ID_IRNSS_L5_A:
    case SBG_ECOM_SIGNAL_ID_IRNSS_L5_B:
    case SBG_ECOM_SIGNAL_ID_IRNSS_L5_C:
    case SBG_ECOM_SIGNAL_ID_IRNSS_L5_BC:
    case SBG_ECOM_SIGNAL_ID_IRNSS_S9_A:
    case SBG_ECOM_SIGNAL_ID_IRNSS_S9_B:
    case SBG_ECOM_SIGNAL_ID_IRNSS_S9_C:
    case SBG_ECOM_SIGNAL_ID_IRNSS_S9_BC:
        constellationId = SBG_ECOM_CONSTELLATION_ID_IRNSS;
        break;

    //
    // L-Band system
    //
    case SBG_ECOM_SIGNAL_ID_LBAND:
        constellationId = SBG_ECOM_CONSTELLATION_ID_LBAND;
        break;
    }

    return constellationId;
}

bool sbgEComSignalIdIsValid(uint8_t signalId)
{
    if (    (signalId == SBG_ECOM_SIGNAL_ID_UNKNOWN) ||
            (sbgEComGetConstellationFromSignalId(signalId) != SBG_ECOM_CONSTELLATION_ID_UNKNOWN))
    {
        return true;
    }
    else 
    {
        return false;
    }
}

const char *sbgEComSignalToStr(SbgEComSignalId signalId)
{
    static const SbgEComSignalIdDesc    signalIdDesc[] =
    {
        { SBG_ECOM_SIGNAL_ID_UNKNOWN,           "unknown"           },

        { SBG_ECOM_SIGNAL_ID_GPS_L1C_DP,        "gpsL1C_DP"         },
        { SBG_ECOM_SIGNAL_ID_GPS_L1C_D,         "gpsL1C_D"          },
        { SBG_ECOM_SIGNAL_ID_GPS_L1C_P,         "gpsL1C_P"          },
        { SBG_ECOM_SIGNAL_ID_GPS_L1_W,          "gpsL1_W"           },
        { SBG_ECOM_SIGNAL_ID_GPS_L1_CA,         "gpsL1_CA"          },
        { SBG_ECOM_SIGNAL_ID_GPS_L1P,           "gpsL1P"            },
        { SBG_ECOM_SIGNAL_ID_GPS_L1_PY,         "gpsL1_PY"          },
        { SBG_ECOM_SIGNAL_ID_GPS_L1M,           "gpsL1M"            },
        { SBG_ECOM_SIGNAL_ID_GPS_L2C_ML,        "gpsL2C_ML"         },
        { SBG_ECOM_SIGNAL_ID_GPS_L2C_L,         "gpsL2C_L"          },
        { SBG_ECOM_SIGNAL_ID_GPS_L2_SEMICL,     "gpsL2_SEMICL"      },
        { SBG_ECOM_SIGNAL_ID_GPS_L2_W,          "gpsL2_W"           },
        { SBG_ECOM_SIGNAL_ID_GPS_L2_CA,         "gpsL2_CA"          },
        { SBG_ECOM_SIGNAL_ID_GPS_L2C_M,         "gpsL2C_M"          },
        { SBG_ECOM_SIGNAL_ID_GPS_L2_PY,         "gpsL2_PY"          },
        { SBG_ECOM_SIGNAL_ID_GPS_L2M,           "gpsL2M"            },
        { SBG_ECOM_SIGNAL_ID_GPS_L2P,           "gpsL2P"            },
        { SBG_ECOM_SIGNAL_ID_GPS_L5_IQ,         "gpsL5_IQ"          },
        { SBG_ECOM_SIGNAL_ID_GPS_L5_I,          "gpsL5_I"           },
        { SBG_ECOM_SIGNAL_ID_GPS_L5_Q,          "gpsL5_Q"           },

        { SBG_ECOM_SIGNAL_ID_GLONASS_G1_P,      "glonassG1_P"       },
        { SBG_ECOM_SIGNAL_ID_GLONASS_G1_CA,     "glonassG1_CA"      },
        { SBG_ECOM_SIGNAL_ID_GLONASS_G2_P,      "glonassG2_P"       },
        { SBG_ECOM_SIGNAL_ID_GLONASS_G2_CA,     "glonassG2_CA"      },
        { SBG_ECOM_SIGNAL_ID_GLONASS_G3_I,      "glonassG3_I"       },
        { SBG_ECOM_SIGNAL_ID_GLONASS_G3_Q,      "glonassG3_Q"       },
        { SBG_ECOM_SIGNAL_ID_GLONASS_G3_IQ,     "glonassG3_IQ"      },

        { SBG_ECOM_SIGNAL_ID_GALILEO_E1_BC,     "galileoE1_BC"      },
        { SBG_ECOM_SIGNAL_ID_GALILEO_E1_C,      "galileoE1_C"       },
        { SBG_ECOM_SIGNAL_ID_GALILEO_E1_B,      "galileoE1_B"       },
        { SBG_ECOM_SIGNAL_ID_GALILEO_E1_A,      "galileoE1_A"       },
        { SBG_ECOM_SIGNAL_ID_GALILEO_E1_ABC,    "galileoE1_ABC"     },
        { SBG_ECOM_SIGNAL_ID_GALILEO_E5B_IQ,    "galileoE5B_IQ"     },
        { SBG_ECOM_SIGNAL_ID_GALILEO_E5B_I,     "galileoE5B_I"      },
        { SBG_ECOM_SIGNAL_ID_GALILEO_E5B_Q,     "galileoE5B_Q"      },
        { SBG_ECOM_SIGNAL_ID_GALILEO_E5A_IQ,    "galileoE5A_IQ"     },
        { SBG_ECOM_SIGNAL_ID_GALILEO_E5A_I,     "galileoE5A_I"      },
        { SBG_ECOM_SIGNAL_ID_GALILEO_E5A_Q,     "galileoE5A_Q"      },
        { SBG_ECOM_SIGNAL_ID_GALILEO_E5_IQ,     "galileoE5_IQ"      },
        { SBG_ECOM_SIGNAL_ID_GALILEO_E5_I,      "galileoE5_I"       },
        { SBG_ECOM_SIGNAL_ID_GALILEO_E5_Q,      "galileoE5_Q"       },
        { SBG_ECOM_SIGNAL_ID_GALILEO_E6_BC,     "galileoE6_BC"      },
        { SBG_ECOM_SIGNAL_ID_GALILEO_E6_C,      "galileoE6_C"       },
        { SBG_ECOM_SIGNAL_ID_GALILEO_E6_B,      "galileoE6_B"       },
        { SBG_ECOM_SIGNAL_ID_GALILEO_E6_ABC,    "galileoE6_ABC"     },
        { SBG_ECOM_SIGNAL_ID_GALILEO_E6_A,      "galileoE6_A"       },

        { SBG_ECOM_SIGNAL_ID_BEIDOU_B1IQ,       "beidouB1IQ"        },
        { SBG_ECOM_SIGNAL_ID_BEIDOU_B1I,        "beidouB1I"         },
        { SBG_ECOM_SIGNAL_ID_BEIDOU_B1Q,        "beidouB1Q"         },
        { SBG_ECOM_SIGNAL_ID_BEIDOU_B1C_P,      "beidouB1C_P"       },
        { SBG_ECOM_SIGNAL_ID_BEIDOU_B1C_DP,     "beidouB1C_DP"      },
        { SBG_ECOM_SIGNAL_ID_BEIDOU_B1C_D,      "beidouB1C_D"       },
        { SBG_ECOM_SIGNAL_ID_BEIDOU_B1A_P,      "beidouB1A_P"       },
        { SBG_ECOM_SIGNAL_ID_BEIDOU_B1A_DP,     "beidouB1A_DP"      },
        { SBG_ECOM_SIGNAL_ID_BEIDOU_B1A_D,      "beidouB1A_D"       },
        { SBG_ECOM_SIGNAL_ID_BEIDOU_B2IQ,       "beidouB2IQ"        },
        { SBG_ECOM_SIGNAL_ID_BEIDOU_B2I,        "beidouB2I"         },
        { SBG_ECOM_SIGNAL_ID_BEIDOU_B2A_P,      "beidouB2A_P"       },
        { SBG_ECOM_SIGNAL_ID_BEIDOU_B2A_DP,     "beidouB2A_DP"      },
        { SBG_ECOM_SIGNAL_ID_BEIDOU_B2A_D,      "beidouB2A_D"       },
        { SBG_ECOM_SIGNAL_ID_BEIDOU_B2Q,        "beidouB2Q"         },
        { SBG_ECOM_SIGNAL_ID_BEIDOU_B2B_P,      "beidouB2B_P"       },
        { SBG_ECOM_SIGNAL_ID_BEIDOU_B2B_DP,     "beidouB2B_DP"      },
        { SBG_ECOM_SIGNAL_ID_BEIDOU_B2B_D,      "beidouB2B_D"       },
        { SBG_ECOM_SIGNAL_ID_BEIDOU_B2AB_P,     "beidouB2AB_P"      },
        { SBG_ECOM_SIGNAL_ID_BEIDOU_B2AB_DP,    "beidouB2AB_DP"     },
        { SBG_ECOM_SIGNAL_ID_BEIDOU_B2AB_D,     "beidouB2AB_D"      },
        { SBG_ECOM_SIGNAL_ID_BEIDOU_B3IQ,       "beidouB3IQ"        },
        { SBG_ECOM_SIGNAL_ID_BEIDOU_B3I,        "beidouB3I"         },
        { SBG_ECOM_SIGNAL_ID_BEIDOU_B3Q,        "beidouB3Q"         },
        { SBG_ECOM_SIGNAL_ID_BEIDOU_B3A_D,      "beidouB3A_D"       },
        { SBG_ECOM_SIGNAL_ID_BEIDOU_B3A_P,      "beidouB3A_P"       },
        { SBG_ECOM_SIGNAL_ID_BEIDOU_B3A_DP,     "beidouB3A_DP"      },

        { SBG_ECOM_SIGNAL_ID_QZSS_L1C_DP,       "qzssL1C_DP"        },
        { SBG_ECOM_SIGNAL_ID_QZSS_L1C_D,        "qzssL1C_D"         },
        { SBG_ECOM_SIGNAL_ID_QZSS_L1C_P,        "qzssL1C_P"         },
        { SBG_ECOM_SIGNAL_ID_QZSS_L1_CA,        "qzssL1_CA"         },
        { SBG_ECOM_SIGNAL_ID_QZSS_L1_SAIF,      "qzssL1_SAIF"       },
        { SBG_ECOM_SIGNAL_ID_QZSS_L1_SB,        "qzssL1_SB"         },
        { SBG_ECOM_SIGNAL_ID_QZSS_L2C_ML,       "qzssL2C_ML"        },
        { SBG_ECOM_SIGNAL_ID_QZSS_L2C_L,        "qzssL2C_L"         },
        { SBG_ECOM_SIGNAL_ID_QZSS_L2C_M,        "qzssL2C_M"         },
        { SBG_ECOM_SIGNAL_ID_QZSS_L5_IQ,        "qzssL5_IQ"         },
        { SBG_ECOM_SIGNAL_ID_QZSS_L5_I,         "qzssL5_I"          },
        { SBG_ECOM_SIGNAL_ID_QZSS_L5_Q,         "qzssL5_Q"          },
        { SBG_ECOM_SIGNAL_ID_QZSS_L5S_IQ,       "qzssL5S_IQ"        },
        { SBG_ECOM_SIGNAL_ID_QZSS_L5S_I,        "qzssL5S_I"         },
        { SBG_ECOM_SIGNAL_ID_QZSS_L5S_Q,        "qzssL5S_Q"         },
        { SBG_ECOM_SIGNAL_ID_QZSS_L6_P,         "qzssL6_P"          },
        { SBG_ECOM_SIGNAL_ID_QZSS_L6_DP,        "qzssL6_DP"         },
        { SBG_ECOM_SIGNAL_ID_QZSS_L6_D,         "qzssL6_D"          },
        { SBG_ECOM_SIGNAL_ID_QZSS_L6_E,         "qzssL6_E"          },
        { SBG_ECOM_SIGNAL_ID_QZSS_L6_DE,        "qzssL6_DE"         },

        { SBG_ECOM_SIGNAL_ID_SBAS_L1_CA,        "sbasL1_CA"         },
        { SBG_ECOM_SIGNAL_ID_SBAS_L5_I,         "sbasL5_I"          },
        { SBG_ECOM_SIGNAL_ID_SBAS_L5_Q,         "sbasL5_Q"          },
        { SBG_ECOM_SIGNAL_ID_SBAS_L5_IQ,        "sbasL5_IQ"         },

        { SBG_ECOM_SIGNAL_ID_IRNSS_L5_A,        "irnssL5_A"         },
        { SBG_ECOM_SIGNAL_ID_IRNSS_L5_B,        "irnssL5_B"         },
        { SBG_ECOM_SIGNAL_ID_IRNSS_L5_C,        "irnssL5_C"         },
        { SBG_ECOM_SIGNAL_ID_IRNSS_L5_BC,       "irnssL5_BC"        },
        { SBG_ECOM_SIGNAL_ID_IRNSS_S9_A,        "irnssS9_A"         },
        { SBG_ECOM_SIGNAL_ID_IRNSS_S9_B,        "irnssS9_B"         },
        { SBG_ECOM_SIGNAL_ID_IRNSS_S9_C,        "irnssS9_C"         },
        { SBG_ECOM_SIGNAL_ID_IRNSS_S9_BC,       "irnssS9_BC"        },

        { SBG_ECOM_SIGNAL_ID_LBAND,             "lband"             }
    };

    for (size_t i = 0; i < SBG_ARRAY_SIZE(signalIdDesc); i++)
    {
        if (signalIdDesc[i].id == signalId)
        {
            return signalIdDesc[i].pName;
        }
    }

    //
    // Enforce that the first item is the unknown signal ID and return it
    //
    assert(signalIdDesc[0].id == SBG_ECOM_SIGNAL_ID_UNKNOWN);
    return signalIdDesc[0].pName;
}

bool sbgEComConstellationIdIsValid(uint8_t constellationId)
{
    bool    constellationIdIsValid = false;

    switch (constellationId)
    {
    case SBG_ECOM_CONSTELLATION_ID_UNKNOWN:
    case SBG_ECOM_CONSTELLATION_ID_GPS:
    case SBG_ECOM_CONSTELLATION_ID_QZSS:
    case SBG_ECOM_CONSTELLATION_ID_GLONASS:
    case SBG_ECOM_CONSTELLATION_ID_GALILEO:
    case SBG_ECOM_CONSTELLATION_ID_BEIDOU:
    case SBG_ECOM_CONSTELLATION_ID_SBAS:
    case SBG_ECOM_CONSTELLATION_ID_IRNSS:
    case SBG_ECOM_CONSTELLATION_ID_LBAND:
        constellationIdIsValid = true;
        break;
    }

    return constellationIdIsValid;
}

const char *sbgEComConstellationToStr(SbgEComConstellationId constellationId)
{
    static const char               *enumToStrLut[] = 
    {
        [SBG_ECOM_CONSTELLATION_ID_UNKNOWN]     = "unknown",
        [SBG_ECOM_CONSTELLATION_ID_GPS]         = "gps",
        [SBG_ECOM_CONSTELLATION_ID_GLONASS]     = "glonass",
        [SBG_ECOM_CONSTELLATION_ID_GALILEO]     = "galileo",
        [SBG_ECOM_CONSTELLATION_ID_BEIDOU]      = "beidou",
        [SBG_ECOM_CONSTELLATION_ID_QZSS]        = "qzss",
        [SBG_ECOM_CONSTELLATION_ID_SBAS]        = "sbas",
        [SBG_ECOM_CONSTELLATION_ID_IRNSS]       = "irnss",
        [SBG_ECOM_CONSTELLATION_ID_LBAND]       = "lband",
    };

    if (constellationId < SBG_ARRAY_SIZE(enumToStrLut))
    {
        return enumToStrLut[constellationId];
    }
    else
    {
        return enumToStrLut[SBG_ECOM_CONSTELLATION_ID_UNKNOWN];
    }
}
