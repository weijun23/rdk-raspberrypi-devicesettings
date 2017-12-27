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
#include "dsUtl.h"
#include "dsError.h"
#include "dsTypes.h"
#include "dsVideoPort.h"
#include "dsVideoResolutionSettings.h"
#include "dsDisplay.h"
#include "dsConfig.h"

#define INVALID_HANDLE 0
#define _RETURN_IF_ERROR(cond, err) do {\
    if (!(cond)) { \
        printf("ERROR at [%s][%d]\r\n", __FILE__, __LINE__);\
        return (err); \
    } \
}while(0);
#define ISNULL(ptr) (ptr==NULL)


#define SIZE 512
#define MINSIZE 64
#define PERSISTANT_PATH "/opt/persistent/ds/hostData"

const bool dsVideoContentProtect[dsVIDEOPORT_TYPE_MAX] = {true, false, true, true, false, true, false, true};

size_t videoPortMaxIndex = 0;
typedef struct _dsVideoPortParam_t {
	bool videoPortEnabled;
	bool videoPortConnected;
	bool dtcpContentProtection;
	bool hdcpContentProtection;
	dsVideoPortPortId_t portId;
	dsVideoPortResolution_t resolutionValue;
}dsVideoPortParam_t;

dsVideoPortParam_t* dsVideoPortHandler = NULL;


dsError_t  dsGetDFC(int handle, dsVideoZoom_t *dfc)
{
    return dsERR_GENERAL;
}

/*******************************************************************************************
FunctionName : readResolution
Inputs Arguments: File pointer ,Buffer to store output buffer, Size to read-len
Output Arguments : buffer
Return Value : -1 if failed
********************************************************************************************/

static int readResolution(FILE *f, char *buffer, size_t len)
{
   int i =0;
  if(f == NULL)
  {
        printf("File open Failed\n");
        return -1;
  }

   memset(buffer, 0, len);

   for (i = 0; i < len; i++)
   {
      int c = fgetc(f);

      if (!feof(f))
      {
         if (c == '\r')
            buffer[i] = 0;
         else if (c == '\n')
         {
            buffer[i] = 0;

            return i+1;
         }
         else
        {
            buffer[i] = c;

        }
      }
      else
      {
         return -1;
      }
   }
}
/*******************************************************************************************
FunctionName : dsVideoPortInitialize , This function is used to initialize the video ports
Inputs Arguments: port index, property to set, property value
Output Arguments : none
Return Value : -1 if failed
********************************************************************************************/

dsError_t dsVideoPortInitialize(size_t index,char* prop,char* values)
{

    if(strcmp(prop,"videoPortEnabled") == 0) {
        sscanf(values,"%d",&dsVideoPortHandler[index].videoPortEnabled);
    	printf(" \n dsVideoPortHandler[index].videoPortEnabled %x \n ", dsVideoPortHandler[index].videoPortEnabled );
        return dsERR_NONE;
    }
    if(strcmp(prop,"videoPortConnected") == 0) {
        sscanf(values,"%d",&dsVideoPortHandler[index].videoPortConnected);
        printf(" \n dsVideoPortHandler[index].videoPortConnected %x \n ", dsVideoPortHandler[index].videoPortConnected );
        return dsERR_NONE;
    }
    if (strcmp(prop,"dtcpContentProtection") == 0) {
        sscanf(values,"%d",&dsVideoPortHandler[index].dtcpContentProtection);
        printf(" \n dsVideoPortHandler[index].dtcpContentProtection %x \n ", dsVideoPortHandler[index].dtcpContentProtection );
        return dsERR_NONE;
	}
    if (strcmp(prop,"hdcpContentProtection") == 0) {
        sscanf(values,"%d",&dsVideoPortHandler[index].hdcpContentProtection);
        printf(" \n dsVideoPortHandler[index].hdcpContentProtection %x \n ", dsVideoPortHandler[index].hdcpContentProtection );
        return dsERR_NONE;
	}
    if (strcmp(prop,"type") == 0) {
        sscanf(values,"%d",&dsVideoPortHandler[index].portId.type);
        printf(" \n dsVideoPortHandler[index].portId.type %x \n ", dsVideoPortHandler[index].portId.type );
        return dsERR_NONE;
	}
    if (strcmp(prop,"index") == 0) {
        sscanf(values,"%d",&dsVideoPortHandler[index].portId.index);
        printf(" \n dsVideoPortHandler[index].portId.index %x \n ", dsVideoPortHandler[index].portId.index );
        return dsERR_NONE;
	}
    if(strcmp(prop,"name") == 0) {
        //dsVideoPortHandler[index].resolutionValue.name = (char*) malloc(strlen(values) + 1);
        sscanf(values,"%s",((char*)(dsVideoPortHandler[index].resolutionValue.name)));
        printf(" \n dsVideoPortHandler[index].resolutionValue.name %s \n ", dsVideoPortHandler[index].resolutionValue.name );
       	return dsERR_NONE;
	}
    if (strcmp(prop,"pixelResolution") == 0) {
        sscanf(values,"%d",&dsVideoPortHandler[index].resolutionValue.pixelResolution);
        printf(" \n dsVideoPortHandler[index].resolutionValue.pixelResolution %x \n ", dsVideoPortHandler[index].resolutionValue.pixelResolution );
        return dsERR_NONE;
	}
    if (strcmp(prop,"aspectRatio") == 0) {
        sscanf(values,"%d",&dsVideoPortHandler[index].resolutionValue.aspectRatio);
        printf(" \n dsVideoPortHandler[index].resolutionValue.aspectRatio %d \n ", dsVideoPortHandler[index].resolutionValue.aspectRatio );
        return dsERR_NONE;
	}
    if (strcmp(prop,"stereoScopicMode") == 0) {
        sscanf(values,"%d",&dsVideoPortHandler[index].resolutionValue.stereoScopicMode);
        printf(" \n dsVideoPortHandler[index].resolutionValue.stereoScopicMode %d \n ", dsVideoPortHandler[index].resolutionValue.stereoScopicMode );
        return dsERR_NONE;
	}
    if (strcmp(prop,"frameRate") == 0) {
        sscanf(values,"%d",&dsVideoPortHandler[index].resolutionValue.frameRate);
        printf(" \n dsVideoPortHandler[index].resolutionValue.frameRate %d \n ", dsVideoPortHandler[index].resolutionValue.frameRate );
        return dsERR_NONE;
	}
    if (strcmp(prop,"interlaced") == 0) {
        sscanf(values,"%d",&dsVideoPortHandler[index].resolutionValue.interlaced);
        printf(" \n dsVideoPortHandler[index].resolutionValue.interlaced %d \n ", dsVideoPortHandler[index].resolutionValue.interlaced );
        return dsERR_NONE;
	}


    return dsERR_GENERAL;
}

/*******************************************************************************************
FunctionName : dsIsVideoPortHandleValid , This function is used to check video port handler is valid
Inputs Arguments: video port handler
Output Arguments : none
Return Value : -1 if failed
********************************************************************************************/
dsError_t dsIsVideoPortHandleValid(int handle)
{
	dsError_t returnValue	=dsERR_NONE;
	dsVideoPortParam_t  *videoPortHdl;
	size_t index = 0;

	videoPortHdl = (dsVideoPortParam_t *)handle;
	for(index = 0; index < videoPortMaxIndex; index++) {
		if (&dsVideoPortHandler[index] == videoPortHdl) {
		  	break;
		}
	}
	if ( index < videoPortMaxIndex ) {
	   printf("\n Port Type %x, Handler %p \n",index, &dsVideoPortHandler[index]);
	}
	else {
	   printf("\n Invalid handle \n");
	   returnValue = dsERR_INVALID_PARAM;
	}
	return returnValue;
}


bool dsIsOperationSupported(int handle)
{
	dsVideoPortParam_t  *videoPortHdl;
	size_t index = 0;

	videoPortHdl = (dsVideoPortParam_t *)handle;
	for(; index < videoPortMaxIndex; index++) {
		if (&dsVideoPortHandler[index] == videoPortHdl) {
		  	break;
		}
	}
	return dsVideoContentProtect[index];
}


dsError_t dsVideoPortAlloc(size_t index)
{
    dsError_t returnValue = dsERR_NONE;
    size_t i =0;

    if ((dsVideoPortHandler = (dsVideoPortParam_t *)malloc(8 * sizeof(dsVideoPortParam_t)))== NULL) {
        dsVideoPortTerm();
        returnValue = dsERR_GENERAL;
    }
    return returnValue;
}
/***************************************************************************************************************************
FunctionName : dsVideoPortInit , This function read the information from platform file and set the same
Inputs Arguments: port index, property to set, property value
Output Arguments : none
Return Value : -1 if failed
*************************************************************************************************************************/
dsError_t  dsVideoPortInit()
{
    dsError_t ret = dsERR_NONE;
    size_t index = 0;
    videoPortMaxIndex = dsGetIndexFrmCfg("ds.max.video.output.port");

    if (videoPortMaxIndex > 0) 
    {
       ret = dsVideoPortAlloc(videoPortMaxIndex);
       if (ret == dsERR_NONE) 
	{
       	   for (;index<videoPortMaxIndex;index++) 
	   {

            	if (dsReadCfgFile(index,"video.output.port.type",dsVideoPortInitialize) == dsERR_NONE) 
		 {
                	ret = dsERR_NONE;
			printf("All video ports are initialized(%s)\n",__FUNCTION__);
		}
           }
       }
    }
   
    return ret;
}

/*******************************************************************************************
FunctionName : dsGetVideoPort , This function is used to get the video port handler
Inputs Arguments: video port type ,video port index 
Output Arguments :handle
Return Value : -1 if failed
********************************************************************************************/
dsError_t  dsGetVideoPort(dsVideoPortType_t type, int index, int *handle)
{

	dsError_t ret = dsERR_NONE;
	int test = dsVideoPortType_isValid(type);

	_RETURN_IF_ERROR(((dsVideoPortType_isValid(type)) && (index >=0 && index < videoPortMaxIndex)), dsERR_OPERATION_NOT_SUPPORTED);

	*handle = (int)&dsVideoPortHandler[index];
	return ret;
}
/**************************************************************************************************
FunctionName : dsIsVideoPortEnabled, This function is used to check whether video port is enabled
Inputs Arguments: port handler
Output Arguments : true or false
Return Value : -1 if failed
********************************************************************************************/

dsError_t dsIsVideoPortEnabled(int handle, bool *enabled)
{
    dsError_t ret = dsERR_NONE;
	dsVideoPortParam_t *videoPortHdl;

	_RETURN_IF_ERROR(handle > INVALID_HANDLE, dsERR_INVALID_PARAM);
	_RETURN_IF_ERROR(enabled != NULL, dsERR_INVALID_PARAM);

	videoPortHdl = (dsVideoPortParam_t *)handle;
	ret = dsIsVideoPortHandleValid(handle);

	if (ret == dsERR_NONE) {
        *((char*)(enabled))= *((char*)(&(videoPortHdl->videoPortEnabled)));
        if (videoPortHdl->videoPortEnabled) {
			printf("\n Given Video Port is enabled %d \n", *enabled);
	    }
	    else {
			printf("\n Given Video Port is not enabled \n");
	    }
	}
    return ret;
}
/*******************************************************************************************
FunctionName : dsEnableVideoPort , This function is used to enable the video ports
Inputs Arguments: port handler
Output Arguments : none
Return Value : -1 if failed
********************************************************************************************/
dsError_t  dsEnableVideoPort(int handle, bool enabled)
{
	dsError_t ret = dsERR_NONE;
	dsVideoPortParam_t *videoPortHdl;

	_RETURN_IF_ERROR(handle > INVALID_HANDLE, dsERR_INVALID_PARAM);
	_RETURN_IF_ERROR(dsIsOperationSupported(handle)!= false, dsERR_OPERATION_NOT_SUPPORTED);

	videoPortHdl = (dsVideoPortParam_t *)handle;
	ret = dsIsVideoPortHandleValid(handle);

	if (ret == dsERR_NONE) {
		videoPortHdl->videoPortEnabled = enabled;
		if (videoPortHdl->videoPortEnabled) {
			printf("The video port is enabled\n");
		}
		else {
			printf("The video port is disabled\n");
		}
	}
	return ret;
}
/*******************************************************************************************
FunctionName : dsIsVideoPortHandleValid , This function is used tocheck whether display is connected
Inputs Arguments: port handler
Output Arguments : true or false
Return Value : -1 if failed
********************************************************************************************/
dsError_t  dsIsDisplayConnected(int handle, bool *connected)
{
    dsError_t ret = dsERR_NONE;
		dsVideoPortParam_t *videoPortHdl;

	_RETURN_IF_ERROR((dsVideoPortParam_t *)handle > INVALID_HANDLE, dsERR_INVALID_PARAM);
	_RETURN_IF_ERROR(connected != NULL, dsERR_INVALID_PARAM);

	videoPortHdl = (dsVideoPortParam_t *)handle;
	ret = dsIsVideoPortHandleValid(handle);

	if (ret == dsERR_NONE) {
        *((char*)(connected)) = *((char*)(&(videoPortHdl->videoPortConnected)));
        if (videoPortHdl->videoPortConnected) {
			printf("\n Given Video Port is connected to a displaying unit %d \n", *connected);
	   }
	   else {
			printf("\n Given Video Port is not connected to any displaying unit \n");
	   }
	}
    return ret;
}

dsError_t  dsEnableDTCP(int handle, bool contentProtect)
{
    return dsERR_OPERATION_NOT_SUPPORTED;
}


dsError_t  dsEnableHDCP(int handle, bool contentProtect, char *hdcpKey, size_t keySize)
{
    dsError_t ret = dsERR_NONE;
    return ret;
}

dsError_t  dsIsDTCPEnabled (int handle, bool* pContentProtected)
{
    return dsERR_OPERATION_NOT_SUPPORTED;
}

dsError_t  dsIsHDCPEnabled (int handle, bool* pContentProtected)
{
    dsError_t ret = dsERR_NONE;
    return ret;
}
/*******************************************************************************************
FunctionName : dsSetWidthAndHeight , This function is used to store the resolution 
Inputs Arguments: width and height
Output Arguments : none
Return Value : -1 if failed
********************************************************************************************/
dsError_t dsSetWidthAndHeight( char *width , char *height)
{
	dsError_t ret = dsERR_NONE;
	FILE *fp ;
	fp = fopen ( "/var/resolution.dat" , "w");
	if(fp)
	{

		fputs(width,fp);
		fputc(',',fp);
		fputs(height,fp);
	}
	else
	{
		printf("file open failed\n");
	}
	fclose(fp);
	return ret;
}
/*******************************************************************************************
FunctionName : dsStoreResolution , This function is used to store the video resolution
Inputs Arguments: resolution structure
Output Arguments : none
Return Value : -1 if failed
********************************************************************************************/
dsError_t  dsStoreResolution(dsVideoResolution_t resolution )
{
    dsError_t ret = dsERR_NONE;

    char width[10] = {'\0'};
    char height[10] = {'\0'};

    FILE *fp;
    switch(resolution)
    {
	case dsVIDEO_PIXELRES_720x480:
		strcpy(width,"720");
		strcpy(height ,"480");
	        ret = dsSetWidthAndHeight(width,height);				
		break;
	case dsVIDEO_PIXELRES_720x576:
		strcpy(width,"720");
		strcpy(height ,"576");
	        ret = dsSetWidthAndHeight(width,height);				
		break;
        case dsVIDEO_PIXELRES_1280x720:
		strcpy(width,"1280");
		strcpy(height ,"720");
	        ret = dsSetWidthAndHeight(width,height);				
		break;

	case dsVIDEO_PIXELRES_1920x1080:
		strcpy(width,"1920");
		strcpy(height ,"1080");
	        ret = dsSetWidthAndHeight(width,height);				
		break;

	default:
		printf("Invalid Resolution\n");
    }   
    
}
/*******************************************************************************************
FunctionName : dsGetResolution , This function is used to get the curernt resolution
Inputs Arguments: handler 
Output Arguments : resolution
Return Value : -1 if failed
********************************************************************************************/
dsError_t  dsGetResolution(int handle, dsVideoPortResolution_t *resolution)
{
	dsVideoPortParam_t* videoPortHdl;
	videoPortHdl = (dsVideoPortParam_t *)handle;
	int ret = 0;
	char strResolution[SIZE], displayType[MINSIZE], Resolution[MINSIZE];
	char *heightinfo;

	int height; 

	FILE *fp = fopen ( PERSISTANT_PATH, "r" );

	size_t index = 0;
	for (; index < videoPortMaxIndex; index++) {
		if (&dsVideoPortHandler[index] == videoPortHdl) {
			break;
		}
	}
	if(fp && resolution){
	while(ret != -1)
        {
	ret = readResolution(fp,strResolution, SIZE);
        sscanf(strResolution, "%s" "%s",displayType, Resolution);
	if(strstr(displayType, "HDMI0"))
		strcpy (resolution->name, Resolution);
		 if(strcmp(resolution->name,"480i")==0)
                   {
                       resolution->pixelResolution = dsVIDEO_PIXELRES_720x480;
                    }
                 else if(strcmp(resolution->name,"576")==0)
                   {
                    resolution->pixelResolution = dsVIDEO_PIXELRES_720x576;
		   }
                else if(strcmp(resolution->name,"720p")==0)
                     {
                      resolution->pixelResolution=dsVIDEO_PIXELRES_1280x720;
                     }
                  else if(strcmp(resolution->name,"1080p24")==0)
                     {
                       resolution->pixelResolution=dsVIDEO_PIXELRES_1920x1080;
                     }
                   else
                      {
                       resolution->pixelResolution=dsVIDEO_PIXELRES_1280x720;
                      }
		   dsStoreResolution(resolution->pixelResolution);

	}
	fclose(fp);
	}
	else
	{
		printf(" No File Found\n");
		resolution->interlaced=dsVideoPortHandler[index].resolutionValue.interlaced;
		resolution->pixelResolution=dsVideoPortHandler[index].resolutionValue.pixelResolution;
		resolution->aspectRatio=dsVideoPortHandler[index].resolutionValue.aspectRatio;
		resolution->frameRate=dsVideoPortHandler[index].resolutionValue.frameRate;
		strncpy(resolution->name, dsVideoPortHandler[index].resolutionValue.name,strlen(dsVideoPortHandler[index].resolutionValue.name));
	}
	return dsERR_GENERAL;
}
/*******************************************************************************************
FunctionName : dsSetResolution, This function is used to set the resolution
Inputs Arguments: port handler and resolution structure
Output Arguments : none
Return Value : -1 if failed
********************************************************************************************/
dsError_t  dsSetResolution(int handle, dsVideoPortResolution_t *resolution)
{


	dsError_t ret = dsERR_NONE;
	dsVideoPortParam_t  *videoPortHdl;
	size_t index = 0;
	FILE *fp;
	_RETURN_IF_ERROR(handle > INVALID_HANDLE, dsERR_INVALID_PARAM);
	_RETURN_IF_ERROR(resolution != NULL, dsERR_INVALID_PARAM);

	videoPortHdl = (dsVideoPortParam_t *)handle;

	for (; index < videoPortMaxIndex; index++) {
		if (&dsVideoPortHandler[index] == videoPortHdl) {
			break;
		}
	}
	if (index < videoPortMaxIndex && resolution) {
		if(strlen(resolution->name) != 0) {
			strcpy( dsVideoPortHandler[index].resolutionValue.name, resolution->name );
		} 
		dsVideoPortHandler[index].resolutionValue.interlaced = resolution->interlaced;

		if (dsVideoPortPixelResolution_isValid(resolution->pixelResolution)) {
			dsVideoPortHandler[index].resolutionValue.pixelResolution = resolution->pixelResolution;
		}
		else {
			printf("Maximum number of resolutions is %d\n",dsVIDEO_PIXELRES_MAX);
			printf("The given resolution is invalid\n");
		}
		if (dsVideoPortAspectRatio_isValid(resolution->aspectRatio)) {
			dsVideoPortHandler[index].resolutionValue.aspectRatio=resolution->aspectRatio;
		}
		else {
			printf("Maximum number of aspect ratios is %d\n",dsVIDEO_ASPECT_RATIO_MAX);
			printf("The given aspect ratio is invalid\n");
		}
		if (dsVideoPortFrameRate_isValid(resolution->frameRate)) {
			dsVideoPortHandler[index].resolutionValue.frameRate=resolution->frameRate;
		}
		else {
			printf("Maximum number of video frame rates is %d\n",dsVIDEO_FRAMERATE_MAX);
			printf("The given frame rate is invalid\n");
		}


		ret = dsStoreResolution(resolution->pixelResolution);
		fp = fopen ( PERSISTANT_PATH , "w");
		if(fp)
		{
			fputs("HDMI0.resolution",fp);
			fputc('\t',fp);	 
			fputs(resolution->name,fp);
			fclose(fp);
		}


	}

	return ret;
}

dsError_t dsSetActiveSource(int handle)
{
    dsError_t ret = dsERR_OPERATION_NOT_SUPPORTED;
    return ret;
}

dsError_t  dsVideoPortTerm()
{
    dsError_t ret = dsERR_NONE;
	    if (!ISNULL(dsVideoPortHandler)) {
        free(dsVideoPortHandler);
      	printf("\n address of freeing memory index %p \n", dsVideoPortHandler);
    }
    
    return ret;
}
