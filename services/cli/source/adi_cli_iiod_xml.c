/******************************************************************************
 Copyright (c) 2025  Analog Devices Inc.
******************************************************************************/

/**
 * @file adi_cli_iiod_xml.c
 * @brief XML generation for ACE GUI communication.
 *
 * This file is responsible for generating an XML file used by the ACE GUI
 * to communicate with devices. It is adapted from the protocol section
 * of https://wiki.analog.com/resources/tools-software/linux-software/libiio_internals.
 *
 * The purpose of this file is to generate an XML file that describes the
 * device attributes, channels, and context attributes.
 *
 * @note This file can be customized or overwritten based on the device
 * attributes and requirements.
 * @{
 */

/*============= I N C L U D E S =============*/
#include "adi_cli_iiod_xml.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*============= D E F I N E S =============*/

/** Macro to get the maximum value between two numbers */
#define MAX_VALUE(x, y) (((x) > (y)) ? (x) : (y))
/** Macro for the xml buffer size */
#define XML_DESC_BUFFER_SIZE 8000

/*============= D A T A =============*/

static char dummybuffer[XML_DESC_BUFFER_SIZE];

/** Macro to convert a number to a string */
#define NO_OS_TOSTRING(x) #x

/* Header for the xml file */
static char header[] =
    "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
    "<!DOCTYPE context ["
    "<!ELEMENT context (device | context-attribute)*>"
    "<!ELEMENT context-attribute EMPTY>"
    "<!ELEMENT device (channel | attribute | debug-attribute | buffer-attribute)*>"
    "<!ELEMENT channel (scan-element?, attribute*)>"
    "<!ELEMENT attribute EMPTY>"
    "<!ELEMENT scan-element EMPTY>"
    "<!ELEMENT debug-attribute EMPTY>"
    "<!ELEMENT buffer-attribute EMPTY>"
    "<!ATTLIST context name CDATA #REQUIRED description CDATA #IMPLIED>"
    "<!ATTLIST context-attribute name CDATA #REQUIRED value CDATA #REQUIRED>"
    "<!ATTLIST device id CDATA #REQUIRED name CDATA #IMPLIED>"
    "<!ATTLIST channel id CDATA #REQUIRED type (input|output) #REQUIRED name CDATA #IMPLIED>"
    "<!ATTLIST scan-element index CDATA #REQUIRED format CDATA #REQUIRED scale CDATA #IMPLIED>"
    "<!ATTLIST attribute name CDATA #REQUIRED filename CDATA #IMPLIED>"
    "<!ATTLIST debug-attribute name CDATA #REQUIRED>"
    "<!ATTLIST buffer-attribute name CDATA #REQUIRED>"
    "]>"
    "<context name=\"xml\" description=\"no-OS/projects/" NO_OS_TOSTRING(NO_OS_PROJECT) " "
                                                                                        "1\" >";

/** Header for the xml file */
static char headerEnd[] = "</context>";

/*============= L O C A L  F U N C T I O N S =============*/

/*
 * Initialize the xml file for the IIO device.
 * @param desc - pointer to the IIO descriptor
 * @param pOutput - pointer to the output buffer
 * @return 0 in case of success or negative value otherwise.
 */
static int32_t InitXml(IioDesc *pDesc, char *pOutput);

/*
 * Add device attributes into pDst. If size is 0, no data will be written to pDst but size will be
 * returned.
 * @param pDeviceAttribute - pointer to the device attributes
 * @param pName - name of the device
 * @param pDeviceId - id of the device
 * @param pDst - pointer to the buffer where the xml will be written
 * @param size - size of the buffer
 * @return size of the xml
 */
static uint32_t AddDeviceXml(DeviceAttributes *pDeviceAttribute, char *pName, char *pDeviceId,
                             uint32_t size, char *pDst);

/*
 * Add channel attributes into pDst. If size is 0, no data will be written to pDst but size will be
 * returned.
 * @param pDeviceAttribute - pointer to the device attributes
 * @param size - size of the buffer
 * @param pDst - pointer to the buffer where the xml will be written
 * @return size of the xml
 */
static uint32_t AddChannelXml(DeviceAttributes *pDeviceAttribute, uint32_t size, int32_t *pIndex,
                              char *pDst);

/**
 * @brief Add context attributes into xml string buffer.
 * @param desc - IIo descriptor.
 * @param buff - xml buffer.
 * @param size - size of buffer
 * @return 0 in case of success or negative value otherwise.
 */
static uint32_t AddCtxAttributeXml(IioDesc *pDesc, char *pBuff, uint32_t size);

/*============= C O D E =============*/

int32_t adi_cli_GenerateIiodXml(IioDesc *pIioDesc, char *pOutput)
{
    int32_t status;
    uint32_t i;
    for (i = 0; i < pIioDesc->numDevices; i++)
    {
        snprintf(pIioDesc->pDeviceParams->deviceId, sizeof(pIioDesc->pDeviceParams->deviceId),
                 "iio:device%" PRIu32 "", i);
    }
    status = InitXml(pIioDesc, pOutput);

    return status;
}

uint32_t AddDeviceXml(DeviceAttributes *pDeviceAttribute, char *pName, char *pDeviceId,
                      uint32_t size, char *pDst)
{
    int32_t i, j, n;
    if ((int32_t)size == -1)
    {
        n = 0;
    }
    else
    {
        n = size;
    }

    if (pDst == NULL)
    {
        /* Set dummy value for buff. It is used only for counting */
        pDst = dummybuffer;
    }

    i = 0;

    i +=
        snprintf(pDst + i, MAX_VALUE(n - i, 0), "<device id=\"%s\" name=\"%s\">", pDeviceId, pName);

    /** Write channels */
    if (pDeviceAttribute->channels)
    {
        AddChannelXml(pDeviceAttribute, size, &i, pDst);
        /** Write device attributes */
        if (pDeviceAttribute->pAttributes)
        {
            for (j = 0; pDeviceAttribute->pAttributes[j].pName; j++)
            {
                i += snprintf(pDst + i, MAX_VALUE(n - i, 0), "<attribute name=\"%s\" />",
                              pDeviceAttribute->pAttributes[j].pName);
            }
        }

        /** Write debug attributes */
        if (pDeviceAttribute->pDebugAttributes)
        {
            for (j = 0; pDeviceAttribute->pDebugAttributes[j].pName; j++)
            {
                i += snprintf(pDst + i, MAX_VALUE(n - i, 0), "<debug-attribute name=\"%s\" />",
                              pDeviceAttribute->pDebugAttributes[j].pName);
            }
        }

        if (pDeviceAttribute->debugRegRWEnable)
        {
            i += snprintf(pDst + i, MAX_VALUE(n - i, 0),
                          "<debug-attribute name=\"" REG_ACCESS_ATTRIBUTE "\" />");
        }

        /** Write buffer attributes */
        if (pDeviceAttribute->pBufferAttributes)
        {
            for (j = 0; pDeviceAttribute->pBufferAttributes[j].pName; j++)
            {
                i += snprintf(pDst + i, MAX_VALUE(n - i, 0), "<buffer-attribute name=\"%s\" />",
                              pDeviceAttribute->pBufferAttributes[j].pName);
            }
        }

        i += snprintf(pDst + i, MAX_VALUE(n - i, 0), "</device>");
    }

    return i;
}

uint32_t AddChannelXml(DeviceAttributes *pDeviceAttribute, uint32_t size, int32_t *pIndex,
                       char *pDst)
{
    int32_t i, j, k, n;
    AttributeType *pAttribute;
    ChannelParams *pChannel;
    if ((int32_t)size == -1)
    {
        n = 0;
    }
    else
    {
        n = size;
    }
    i = *pIndex;
    for (j = 0; j < pDeviceAttribute->numChannel; j++)
    {
        pChannel = &pDeviceAttribute->channels[j];
        i += snprintf(pDst + i, MAX_VALUE(n - i, 0), "<channel id=\"%s%ld\"",
                      ChannelTypeName[pChannel->channelType], j);
        if (pChannel->pName)
            i += snprintf(pDst + i, MAX_VALUE(n - i, 0), " name=\"%s\"", pChannel->pName);
        i += snprintf(pDst + i, MAX_VALUE(n - i, 0), " type=\"%s\" >",
                      pChannel->ch_out ? "output" : "input");

        if (pChannel->pScanType)
        {
            i += snprintf(pDst + i, MAX_VALUE(n - i, 0),
                          "<scan-element index=\"%d\""
                          " format=\"%s:%c%d/%d>>%d\" />",
                          pChannel->scan_index, pChannel->pScanType->isBigEndian ? "be" : "le",
                          pChannel->pScanType->sign, pChannel->pScanType->realbits,
                          pChannel->pScanType->storagebits, pChannel->pScanType->shift);
        }

        /* Write channel attributes */
        if (pChannel->pAttributes)
        {
            for (k = 0; pChannel->pAttributes[k].pName; k++)
            {
                pAttribute = &pChannel->pAttributes[k];
                i += snprintf(pDst + i, MAX_VALUE(n - i, 0), "<attribute name=\"%s\" ",
                              pAttribute->pName);
                switch (pAttribute->shared)
                {
                case IIO_SHARED_BY_TYPE:
                    i += snprintf(pDst + i, MAX_VALUE(n - i, 0), "filename=\"%s_%s%ld_%s\"",
                                  pChannel->ch_out ? "out" : "in",
                                  ChannelTypeName[pChannel->channelType], j, pAttribute->pName);
                    break;
                case IIO_SHARED_BY_ALL:
                case IIO_SEPARATE:
                case IIO_SHARED_BY_DIR:
                    break;
                }
                i += snprintf(pDst + i, MAX_VALUE(n - i, 0), " />");
            }
        }
        i += snprintf(pDst + i, MAX_VALUE(n - i, 0), "</channel>");
    }
    *pIndex = i;
    return 0;
}
uint32_t AddCtxAttributeXml(IioDesc *pDesc, char *pBuff, uint32_t size)
{
    CtxAttrType *pAttr;
    int32_t i;
    int32_t j;
    int32_t n;

    if ((int32_t)size == -1)
    {
        n = 0;
    }
    else
    {
        n = size;
    }
    if (pBuff == NULL)
    {
        /* Set dummy value for buff. It is used only for counting */
        pBuff = dummybuffer;
    }
    i = 0;

    pAttr = pDesc->pCtxAttribute;
    if (pAttr)
    {
        for (j = 0; j < (int32_t)pDesc->numCtxAttribute; j++)
        {
            i += snprintf(pBuff + i, MAX_VALUE(n - i, 0), "<context-attribute name=\"%s\" ",
                          pAttr[j].name);

            i += snprintf(pBuff + i, MAX_VALUE(n - i, 0), "value=\"%s\" />", pAttr[j].value);
        }
    }

    return i;
}

int32_t InitXml(IioDesc *pDesc, char *pOutput)
{
    uint32_t i, size, of;

    /* -2 because of the 0 character */
    size = sizeof(header) + sizeof(headerEnd) - 2;

    // Counts the number of characters to be written in the xml buffer
    size += AddCtxAttributeXml(pDesc, NULL, -1);
    for (i = 0; i < pDesc->numDevices; i++)
    {
        size += AddDeviceXml(pDesc->pDeviceParams->pDeviceAttribute,
                             (char *)pDesc->pDeviceParams->pName, pDesc->pDeviceParams->deviceId,
                             -1, NULL);
    }

    pDesc->pXmlDesc = pOutput;
    if (!pDesc->pXmlDesc)
    {
        return -1;
    }

    pDesc->xmlSize = size;

    strcpy(pDesc->pXmlDesc, header);
    of = sizeof(header) - 1;
    of += AddCtxAttributeXml(pDesc, pDesc->pXmlDesc + of, size - of);
    for (i = 0; i < pDesc->numDevices; i++)
    {
        of += AddDeviceXml(pDesc->pDeviceParams->pDeviceAttribute,
                           (char *)pDesc->pDeviceParams->pName, pDesc->pDeviceParams->deviceId,
                           size - of, pDesc->pXmlDesc + of);
    }
    strcpy(pDesc->pXmlDesc + of, headerEnd);

    return 0;
}

/**
 * @}
 */
