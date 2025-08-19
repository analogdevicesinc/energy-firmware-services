/******************************************************************************
 Copyright (c) 2025  Analog Devices Inc.
******************************************************************************/

/**
 *  @file        adi_cli_iiod_xml.h
 *  @brief       Functions for generating XML files.
 * @{
 */

#ifndef __ADI_CLI_IIOD_XML_H__
#define __ADI_CLI_IIOD_XML_H__

/*============= I N C L U D E S =============*/

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/*============= F U N C T I O N  P R O T O T Y P E S =============*/

/** Maximum character count of device id */
#define MAX_DEVICE_ID 64
/** Register access attribute */
#define REG_ACCESS_ATTRIBUTE "direct_reg_access"

/**
 * Enum holding the channel types
 */
typedef enum
{
    IIO_VOLTAGE,
    IIO_CURRENT,
    IIO_ALTVOLTAGE,
    IIO_ANGL_VEL,
    IIO_TEMP,
    IIO_CAPACITANCE,
    IIO_ACCEL,
    IIO_RESISTANCE,
    IIO_MAGN,
    IIO_INCLI,
    IIO_VELOCITY,
    IIO_ANGL,
    IIO_ROT,
    IIO_COUNT,
    IIO_DELTA_ANGL,
    IIO_DELTA_VELOCITY,
} CHANNEL_TYPE;

/**
 * @struct scanType
 * @brief Structure holding the scan type members
 */
typedef struct
{
    /** 's' or 'u' to specify signed or unsigned */
    char sign;
    /** Number of valid bits of data */
    uint8_t realbits;
    /** Realbits + padding */
    uint8_t storagebits;
    /** Shift right by this before masking out realbits. */
    uint8_t shift;
    /** True if big endian, false if little endian */
    bool isBigEndian;
} scanType;

/**
 * @struct CtxAttrType
 * @brief Structure holding the context attribute members
 */
typedef struct
{
    /** Attribute name */
    const char *name;
    /** Attribute value */
    const char *value;
} CtxAttrType;

/**
 * Enum holding the attribute sharedness
 */
typedef enum
{
    IIO_SHARED_BY_TYPE,
    IIO_SHARED_BY_DIR,
    IIO_SHARED_BY_ALL,
    IIO_SEPARATE,
} ATTRIBUTE_SHARED;

/**
 * Enum holding the datatype of the attribute
 */
typedef enum
{
    /** Attribute type is int32_t */
    ADI_ATTR_TYPE_INT32 = 0,
    /** Attribute type is uint32_t */
    ADI_TYPE_UINT32,
    /** Attribute type is float */
    ADI_ATTR_TYPE_FLOAT,
    /** Attribute type is uint8_t */
    ADI_ATTR_TYPE_UINT8,
    /** Attribute type is string*/
    ADI_ATTR_TYPE_STRING,
} ADI_ATTR_TYPE;

/**
 * @struct AttributeType
 * @brief Structure holding pointers to show and store functions.
 */
typedef struct
{
    /** Attribute name */
    const char *pName;
    /** Attribute id */
    int32_t id;
    /** Attribute value type */
    ADI_ATTR_TYPE dataType;
    /** Whether this attribute is shared by all channels of the same type, or direction
     * or simply by all channels. If left uninitialized, the sharedness defaults to
     * separate.
     */
    ATTRIBUTE_SHARED shared;
} AttributeType;

/**
 * @struct ChannelParams
 * @brief Structure holding attributes of a channel.
 */
typedef struct
{
    /** Channel name */
    const char *pName;
    /** Chanel type */
    CHANNEL_TYPE channelType;
    /** Channel number when the same channel type */
    int channel;
    /** If modified is set, this provides the modifier. E.g. IIO_MOD_X
     *  for angular rate when applied to channel2 will make make the
     *  IIO_ANGL_VEL have anglvel_x which corresponds to the x-axis. */
    int channel2;
    /** Driver specific identifier. */
    unsigned long address;
    /** Index to give ordering in scans when read  from a buffer. */
    int scan_index;
    /** scan type */
    scanType *pScanType;
    /** Array of attributes. Last one should have its name set to NULL */
    AttributeType *pAttributes;
    /** if true, the channel is an output channel */
    bool ch_out;
    /** Set if channel has a modifier. Use channel2 property to
     *  select the modifier to use.*/
    bool modified;
    /** Specify if channel has a numerical index. If not set, channel
     *  number will be suppressed. */
    bool indexed;
    /** Set if the channel is differential. */
    bool diferential;
} ChannelParams;

/**
 * @brief Array of channel types
 */
static const char *const ChannelTypeName[] = {
    [IIO_VOLTAGE] = "voltage",
    [IIO_CURRENT] = "current",
    [IIO_ALTVOLTAGE] = "altvoltage",
    [IIO_ANGL_VEL] = "anglvel",
    [IIO_TEMP] = "temp",
    [IIO_CAPACITANCE] = "capacitance",
    [IIO_ACCEL] = "accel",
    [IIO_RESISTANCE] = "resistance",
    [IIO_MAGN] = "magn",
    [IIO_INCLI] = "incli",
    [IIO_VELOCITY] = "velocity",
    [IIO_ANGL] = "angl",
    [IIO_ROT] = "rot",
    [IIO_COUNT] = "count",
    [IIO_DELTA_ANGL] = "deltaangl",
    [IIO_DELTA_VELOCITY] = "deltavelocity",
};

/**
 * @struct DeviceAttributes
 * @brief Structure holding channels and attributes of a device.
 */
typedef struct
{
    /** Device number of channels */
    uint16_t numChannel;
    /** List of channels */
    ChannelParams *channels;
    /** Array of attributes. Last one should have its name set to NULL */
    AttributeType *pAttributes;
    /** Array of attributes. Last one should have its name set to NULL */
    AttributeType *pDebugAttributes;
    /** Array of attributes. Last one should have its name set to NULL */
    AttributeType *pBufferAttributes;
    /** Debug register read/write enable */
    uint8_t debugRegRWEnable;
} DeviceAttributes;

/**
 * @struct DeviceParams
 * @brief Structure holding the device parameters
 */
typedef struct
{
    /** Device ID */
    char deviceId[MAX_DEVICE_ID];
    /** Device name */
    char *pName;
    /** Device pointer */
    void *pDev;
    /** Device attributes */
    DeviceAttributes *pDeviceAttribute;
} DeviceParams;

/**
 * @brief Structure to hold the IIO information.
 */
typedef struct
{
    /** Pointer to the xml description */
    char *pXmlDesc;
    /** Size of the xml description */
    uint32_t xmlSize;
    /** Pointer to the context attributes */
    CtxAttrType *pCtxAttribute;
    /** Number of context attributes */
    uint32_t numCtxAttribute;
    /** Pointer to the device parameters */
    DeviceParams *pDeviceParams;
    /** Number of devices */
    uint32_t numDevices;
} IioDesc;

/**
 * @brief Generates the XML for the IIO device.
 * @param pIioDesc - Pointer to the IIO description.
 * @param pOutput - Pointer to the output buffer.
 * @return 0 in case of success or negative value otherwise.
 */
int32_t adi_cli_GenerateIiodXml(IioDesc *pIioDesc, char *pOutput);

#ifdef __cplusplus
}
#endif

#endif /* __ADI_CLI_IIOD_XML_H__ */
/**
 * @}
 */
