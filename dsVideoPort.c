/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2017 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/
 
#include <string.h> 
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "dsUtl.h"
#include "dsError.h"
#include "dsTypes.h"
#include "dsVideoPort.h"
#include "dsVideoResolutionSettings.h"
#include "dsDisplay.h"
#include "dshalUtils.h"


static bool isValidVopHandle(int handle);
static const char* dsVideoGetResolution(uint32_t mode);
static uint32_t dsGetHdmiMode(dsVideoPortResolution_t *resolution);
#define MAX_HDMI_MODE_ID (127)

typedef struct _VOPHandle_t {
	dsVideoPortType_t m_vType;
	int m_index;
	int m_nativeHandle;
	bool m_isEnabled;
} VOPHandle_t;

static VOPHandle_t _handles[dsVIDEOPORT_TYPE_MAX][2] = {
};

static dsVideoPortResolution_t _resolution;


dsError_t  dsVideoPortInit()
{
	dsError_t ret = dsERR_NONE;
    int rc;

	/*
	 * Video Port configuration for HDMI and Analog ports. 
	 */

	_handles[dsVIDEOPORT_TYPE_HDMI][0].m_vType  = dsVIDEOPORT_TYPE_HDMI;
	_handles[dsVIDEOPORT_TYPE_HDMI][0].m_nativeHandle = dsVIDEOPORT_TYPE_HDMI;
	_handles[dsVIDEOPORT_TYPE_HDMI][0].m_index = 0;
	_handles[dsVIDEOPORT_TYPE_HDMI][0].m_isEnabled = true;


	_handles[dsVIDEOPORT_TYPE_BB][0].m_vType  = dsVIDEOPORT_TYPE_BB;
	_handles[dsVIDEOPORT_TYPE_BB][0].m_nativeHandle = dsVIDEOPORT_TYPE_BB;
	_handles[dsVIDEOPORT_TYPE_BB][0].m_index = 0;
	_handles[dsVIDEOPORT_TYPE_BB][0].m_isEnabled = false;

	_resolution = kResolutions[kDefaultResIndex];
        rc = vchi_tv_init();
        if (rc != 0)
        {
             printf("Failed to initialise tv service\n");
         } 
	return ret;
}

/**
 * @brief Get the video port handle.
 * 
 * This function gets the handle for the type of video port requested. It must return
 * ::dsERR_OPERATION_NOT_SUPPORTED if the requested video port is unavailable.
 *
 * @param [in]  type       Type of video port (e.g. HDMI).
 * @param [in]  m_index      The index of the video device (0, 1, ...).
 * @param [out] *handle    The address of a location to hold the video device handle on return.
 * @return    Error Code.
 * @retval    ::dsError_t
 */

dsError_t  dsGetVideoPort(dsVideoPortType_t type, int index, int *handle)
{
	dsError_t ret = dsERR_NONE;
	
	if (index != 0 || !dsVideoPortType_isValid(type)) {
		ret = dsERR_NONE;
	}
	if (ret == dsERR_NONE) {
		*handle = (int)&_handles[type][index];

	}
	return ret;
}
 /**
 * @brief Enable/disable all video port.
 *
 * This function enables or disables the all video port.
 *
 * @param [in] enabled     Flag to control the video port state
 *                         (@a true to enable, @a false to disable)
 * @return Device Settings error code
 * @retval dsERR_NONE If sucessfully dsEnableAllVideoPort api has been called using IARM support.
 * @retval dsERR_GENERAL General failure.
 */
dsError_t  dsEnableAllVideoPort(bool enabled)
{

    /* We cannot enable all ports in raspberrypi  because by default other
     * port will be disabled when we enable any videoport on rpi */
        dsError_t ret = dsERR_NONE;
        return ret;
}


/**
 * @brief Indicate whether a video port is enabled.
 * 
 * This function indicates whether the specified video port is enabled or not.
 *
 * @param [in]  handle      Handle of the video port.
 * @param [out] *enabled    The address of a location to hold the video port enable state
 *                          on return (@a true when port is enabled, @a false otherwise).
 * @return    Error Code.
 * @retval    ::dsError_t
 */
dsError_t dsIsVideoPortEnabled(int handle, bool *enabled)
{
	dsError_t ret = dsERR_NONE;
	VOPHandle_t *vopHandle = (VOPHandle_t *) handle;

		
	if (!isValidVopHandle(handle)) {
     return dsERR_INVALID_PARAM;
    }

	if(vopHandle->m_vType == dsVIDEOPORT_TYPE_COMPONENT)
	{
		*enabled = vopHandle->m_isEnabled ;
	}
	else if (vopHandle->m_vType == dsVIDEOPORT_TYPE_HDMI)
	{
		*enabled = vopHandle->m_isEnabled;
	}
	else
	{
		*enabled = false;
		ret = dsERR_OPERATION_NOT_SUPPORTED;
	}
	return ret;
}

 /**
 * @brief Enable/disable a video port.
 *
 * This function enables or disables the specified video port.
 *
 * @param [in] handle      Handle of the video port.
 * @param [in] enabled     Flag to control the video port state 
 *                         (@a true to enable, @a false to disable)
 *
 * @return    Error Code.
 * @retval    ::dsError_t
 */
dsError_t  dsEnableVideoPort(int handle, bool enabled)
{
	dsError_t ret = dsERR_NONE;
	VOPHandle_t *vopHandle = (VOPHandle_t *) handle;
        SDTV_OPTIONS_T options;
        int res = 0;

	if (!isValidVopHandle(handle)) {
         return dsERR_INVALID_PARAM;
    }

	if(vopHandle->m_vType == dsVIDEOPORT_TYPE_BB)
	{
		if(enabled != vopHandle->m_isEnabled)
		{
                     if (enabled)
                     {
                         options.aspect = SDTV_ASPECT_16_9;
                         res = vc_tv_sdtv_power_on(SDTV_MODE_NTSC, &options);
                         if (res != 0)
                             printf("Failed to enable composite video port\n");
                     }
                     else
                     {
                         res = vc_tv_power_off();
                         if ( res != 0 )
                         {
                             printf( "Failed to disbale composite video port" );
                         }
                     }
		}
		vopHandle->m_isEnabled = enabled;
	}
	else if (vopHandle->m_vType == dsVIDEOPORT_TYPE_HDMI)
	{
		if(enabled != vopHandle->m_isEnabled)
		{
                     if (enabled)
                     {
                         res = vc_tv_hdmi_power_on_preferred();
                         if ( res != 0 )
                         {
                             printf( "Failed to power on HDMI with preferred settings" );
                         }
                     }
                     else
                     {
                         res = vc_tv_power_off();
                         if ( res != 0 )
                         {
                             printf( "Failed to disbale HDMI video port" );
                         }
                     }
		}
		vopHandle->m_isEnabled = enabled;
	}
	else
	{
		ret = dsERR_OPERATION_NOT_SUPPORTED;
	}
	return ret;
}

/**
 * @brief Indicate whether a video port is connected to a display.
 * 
 * This function is used to find out whether the video port is connected to a display or not.
 *
 * @param [in]  handle        Handle of the video port.
 * @param [out] *connected    The address of a location to hold the connection state on
 *                            return (@a true when connected, @a false otherwise).
 * @return    Error Code.
 * @retval    ::dsError_t
 */
dsError_t  dsIsDisplayConnected(int handle, bool *connected)
{
	dsError_t ret = dsERR_NONE;
	VOPHandle_t *vopHandle = (VOPHandle_t *) handle;
        TV_DISPLAY_STATE_T tvstate;
	/*Default is false*/
	 *connected = false;
    if (!isValidVopHandle(handle)) {
         return dsERR_INVALID_PARAM;
    }

	if(vopHandle->m_vType == dsVIDEOPORT_TYPE_BB)
	{
		*connected = true;
		return dsERR_NONE;
	}


	if (vopHandle->m_vType == dsVIDEOPORT_TYPE_HDMI)
	{
                printf("Isdisplayconnected HDMI port");
                if( vc_tv_get_display_state( &tvstate ) == 0) {
                     if (tvstate.state & VC_HDMI_ATTACHED) {
                         printf("HDMI is connected\n");
                         *connected = true;
                     }
                     else if (tvstate.state & VC_HDMI_UNPLUGGED) {
                         printf("HDMI is not connected");
                         *connected = false;
                     }
                     else 
                         printf("Caannot find HDMI state\n");
                } 
	}
	else
	{
		ret = dsERR_OPERATION_NOT_SUPPORTED;
	}
return ret;
}

/**
 * @brief To enable DTCP protection 
 * 
 * This function is used to set the DTCP content protection for the output
 * Must return dsERR_OPERATION_NOT_SUPPORTED when if content protection is not available
 *
 * @param [in] handle      Handle for the output video port
 * @param [in] contentProtect True to turn on content protection
 * @return dsError_t Error code.
 */
dsError_t  dsEnableDTCP(int handle, bool contentProtect)
{
	dsError_t ret = dsERR_NONE;
	
	if (isValidVopHandle(handle)) 
	{
		ret = dsERR_NONE;
	}

	return ret;
}

/**
 * @brief To enable HDCP protection 
 * 
 * This function is used to set the HDCP content protection for the output
 * Must return dsERR_OPERATION_NOT_SUPPORTED when if content protection is not available
 *
 * @param [in] handle      Handle for the output video port
 * @param [in] contentProtect True to turn on content protection
 * @param [in] hdcpKey when enabled, this should contain the key of the HDCP 
 * @param [in] keySize length of the key. 
 * @return dsError_t Error code.
 */
dsError_t  dsEnableHDCP(int handle, bool contentProtect, char *hdcpKey, size_t keySize)
{
	dsError_t ret = dsERR_NONE;
	return ret;
}

/**
 * @brief To find whether the video content is DTCP protected
 * 
 * This function is used to check whether the given video port is configured for content protection.
 * Must return dsERR_OPERATION_NOT_SUPPORTED when if content protect is not available at port level
 *
 * @param [in] handle      Handle for the output video port
 * @param [out] *pContentProtected True when output is content protected
 * @return dsError_t Error code.
 */
dsError_t  dsIsDTCPEnabled (int handle, bool* pContentProtected)
{
	*pContentProtected = false;
	return dsERR_OPERATION_NOT_SUPPORTED;
}

/**
 * @brief To find whether the video content is HDCP protected
 * 
 * This function is used to check whether the given video port is configured for content protection.
 * Must return dsERR_OPERATION_NOT_SUPPORTED when if content protect is not available at port level
 *
 * @param [in] handle      Handle for the output video port
 * @param [out] *pContentProtected True when output is content protected
 * @return dsError_t Error code.
 */
dsError_t  dsIsHDCPEnabled (int handle, bool* pContentProtected)
{
    dsError_t ret = dsERR_NONE;
	VOPHandle_t *vopHandle = (VOPHandle_t *) handle;

    if (!isValidVopHandle(handle)){
         *pContentProtected = false;
         return dsERR_INVALID_PARAM;
    }
	return ret;
}


/**
 * @brief This function is used to get the video display resolution.
 * This function gets the video resolution for the video corresponding to the specified port and handle.
 *
 * @param[in] handle         Handle of the video output port.
 * @param[in] resolution    The address of a structure containing the video output port
 *                            resolution settings.
 * @return Device Settings error code
 * @retval dsERR_NONE If sucessfully dsGetResolution api has been called using IARM support.
 * @retval dsERR_GENERAL General failure.
 */
dsError_t  dsGetResolution(int handle, dsVideoPortResolution_t *resolution)
{ 
	dsError_t ret = dsERR_NONE;
    const char *resolution_name = NULL;
    VOPHandle_t *vopHandle = (VOPHandle_t *) handle;
    TV_DISPLAY_STATE_T tvstate; 

    if (!isValidVopHandle(handle)) {
        return dsERR_INVALID_PARAM;
    }
    if( vc_tv_get_display_state( &tvstate ) == 0) {
        resolution_name = dsVideoGetResolution(tvstate.display.hdmi.mode);
    }
    if (resolution_name) 
        strncpy(resolution->name, resolution_name, strlen(resolution_name)); 
    return ret; 
}

static const char* dsVideoGetResolution(uint32_t hdmiMode)
{ 
        const char *res_name = NULL;
     size_t iCount = (sizeof(resolutionMap) / sizeof(resolutionMap[0]));
     for (int i = 0; i < iCount; i++) {
         if (resolutionMap[i].mode == hdmiMode)
         res_name = resolutionMap[i].rdkRes;
     }    
    return res_name;
}

static uint32_t dsGetHdmiMode(dsVideoPortResolution_t *resolution)
{
    uint32_t hdmi_mode = 0;
    size_t iCount = (sizeof(resolutionMap) / sizeof(resolutionMap[0]));
    for (int i = 0; i < iCount; i++) {
        size_t length = strlen(resolution->name) > strlen(resolutionMap[i].rdkRes) ? strlen(resolution->name) : strlen(resolutionMap[i].rdkRes);
        if (!strncmp(resolution->name, resolutionMap[i].rdkRes, length))
        {
            hdmi_mode = resolutionMap[i].mode;
            break;
        }
    }
    if (!hdmi_mode) {
        printf("Given resolution not found, setting default Resolution\n");
        hdmi_mode = HDMI_CEA_720p60;
    }
    return hdmi_mode;
}

/**
 * @brief Set video port's display resolution.
 *
 * This function sets the resolution for the video corresponding to the specified port handle.
 *
 * @param [in] handle         Handle of the video port.
 * @param [in] *resolution    The address of a structure containing the video port
 *                            resolution settings.
 * @oaram [in] persist        enable/disable flag for persistence storage
 * @return    Error Code.
 * @retval    ::dsError_t
 */
dsError_t  dsSetResolution(int handle, dsVideoPortResolution_t *resolution, bool persist)
{
	/* Auto Select uses 720p. Should be converted to dsVideoPortResolution_t = 720p in DS-VOPConfig, not here */
                printf("Inside dsSetResolution\n");
	dsError_t ret = dsERR_NONE;
        VOPHandle_t *vopHandle = (VOPHandle_t *) handle;
        int res = 0;
        if (!isValidVopHandle(handle)) {
            return dsERR_INVALID_PARAM;
        }
        if (vopHandle->m_vType == dsVIDEOPORT_TYPE_HDMI) {
                char command[512];
                printf("Inside set Res HDMI\n");
	        uint32_t hdmi_mode;
                hdmi_mode = dsGetHdmiMode(resolution);
		res = vc_tv_hdmi_power_on_explicit_new( HDMI_MODE_HDMI, HDMI_RES_GROUP_CEA, hdmi_mode );
		if ( res != 0 )
		{
			printf( "Failed to set resolution\n");
		}
                sleep(1);
                snprintf(command, 512, "fbset -depth 16");
                system(command);
                snprintf(command, 512, "fbset -depth 32");
                system(command);
        }
        else if (vopHandle->m_vType == dsVIDEOPORT_TYPE_BB)
        {
             SDTV_OPTIONS_T options;
             options.aspect = SDTV_ASPECT_16_9;
             if (!strncmp(resolution->name, "480i", strlen("480i"))) {
                 res = vc_tv_sdtv_power_on(SDTV_MODE_NTSC, &options);
             }
             else 
             {
                 res = vc_tv_sdtv_power_on(SDTV_MODE_PAL, &options);
             }
        }
        else
        {
            printf("Video port typr not supported\n");
        }
	return ret;
}



 /**
 * @brief Terminate the Video Port sub-system.
 *
 * This function must terminate all the video output ports. It must reset any data
 * structures used within video port module and release any video port specific handles.
 *
 * @param None
 * @return    Error Code.
 * @retval    ::dsError_t
 */

dsError_t  dsVideoPortTerm()
{
    dsError_t ret = dsERR_NONE;
    vchi_tv_uninit();
    return ret;
}


/**
 * @brief To check whether the handle is valid or not
 * 
 * This function will be used to validate the  handles that are given
 *
 * @param [in] handle  Handle for the Output Audio port
 * @return bool  true for valid handle
 */
static bool isValidVopHandle(int m_handle) {
	for (int i = 0; i < dsVIDEOPORT_TYPE_MAX; i++) {
		if ((int)&_handles[i][0] == m_handle) {
			return true;
		}
	}
	return false;
}

/**
 * @brief Indicate whether a video port is is connected to the active port of sink device.
 * 
 *  This function indicates whether the specified video port is active or not.A HDMI output port is active if
 *  it is connected to the active port of sink device. E.g. if RxSense is true.
 *  An analog output port is always considered active.
 *
 * @param [in] handle      Handle for the output video port that connects to sink
 * @param [out] *active   The address of a location to hold the video port active state
 * on return (@a true when port is active, @a false otherwise).                              
 * @return dsError_t Error code.
 */
dsError_t dsIsVideoPortActive(int handle, bool *active)
{
	dsError_t ret = dsERR_NONE;
	VOPHandle_t *vopHandle = (VOPHandle_t *) handle;
        TV_DISPLAY_STATE_T tvstate;
	/* Default to false */
	*active = false;

    if (!isValidVopHandle(handle)){
         *active =  false;
         return dsERR_INVALID_PARAM;
    }

	if (vopHandle->m_vType == dsVIDEOPORT_TYPE_HDMI) {
                if( vc_tv_get_display_state( &tvstate ) == 0) {
                     if (tvstate.state & VC_HDMI_HDMI)
                         *active = true;
                     else if (tvstate.state & VC_HDMI_UNPLUGGED)
                         *active = false;
                     else 
                         printf("Cannot find HDMI state\n");
                } 
	}
	return ret;
}

dsError_t dsGetHDCPProtocol (int handle, dsHdcpProtocolVersion_t *protocolVersion)
{
    dsError_t ret = dsERR_NONE;
    return ret;
}

dsError_t dsGetHDCPReceiverProtocol (int handle, dsHdcpProtocolVersion_t *protocolVersion)
{
    dsError_t ret = dsERR_NONE;
    return ret;
}

dsError_t dsGetHDCPCurrentProtocol (int handle, dsHdcpProtocolVersion_t *protocolVersion)
{
    dsError_t ret = dsERR_NONE;
    return ret;

}

dsError_t dsGetTVHDRCapabilities(int handle, int *capabilities)
{
    dsError_t ret = dsERR_NONE;
    return ret;
}

dsError_t dsSupportedTvResolutions(int handle, int *resolutions)
{
    dsError_t ret = dsERR_NONE;
    VOPHandle_t *vopHandle = (VOPHandle_t *) handle;

    if (resolutions != NULL && isValidVopHandle(handle) && vopHandle->m_vType == dsVIDEOPORT_TYPE_HDMI) {
        TV_SUPPORTED_MODE_NEW_T modeSupported[MAX_HDMI_MODE_ID];
        HDMI_RES_GROUP_T group;
        uint32_t mode;
        int num_of_modes;
        int i;
        num_of_modes = vc_tv_hdmi_get_supported_modes_new( HDMI_RES_GROUP_CEA, modeSupported,
                                                   vcos_countof(modeSupported),
                                                   &group,
                                                   &mode );
        if ( num_of_modes < 0 )
        {
           printf( "Failed to get modes" );
           return ret;
        }
        for (i = 0; i < num_of_modes; i++) {
            switch(modeSupported[i].code) {
                case HDMI_CEA_480p60:
                    *resolutions |= dsTV_RESOLUTION_480p;
                     break;
                case HDMI_CEA_480i60:
                     *resolutions |= dsTV_RESOLUTION_480i;
                     break;
                case HDMI_CEA_576i50:
                    *resolutions |= dsTV_RESOLUTION_576i;
                    break;
                case HDMI_CEA_576p50: 
                    *resolutions |= dsTV_RESOLUTION_576i;
                    break;
                case HDMI_CEA_720p60:
                case HDMI_CEA_720p50:
                    *resolutions |= dsTV_RESOLUTION_720p;
                    break;
                case HDMI_CEA_1080p60:
                case HDMI_CEA_1080p50:
                case HDMI_CEA_1080p24:
                case HDMI_CEA_1080p25:
                case HDMI_CEA_1080p30:
                    *resolutions |= dsTV_RESOLUTION_1080p;
                    break;
                case HDMI_CEA_1080i60:
                case HDMI_CEA_1080i50:
                    *resolutions |= dsTV_RESOLUTION_1080i;
                    break;
                default:
                    *resolutions |= dsTV_RESOLUTION_480p;
                    break;
            }
        }
    }
    else
    {
        ret = dsERR_INVALID_PARAM;
        printf("Error:Get supported resolution for TV on Non HDMI Port\r\n");
    }
    printf("%s resolutions= %x \r\n",__FUNCTION__,*resolutions);
    return ret;
}
