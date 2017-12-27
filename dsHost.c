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

#include "dsTypes.h"
#include "dsError.h"
#include "dsHost.h"

dsError_t dsHostInit()
{
    dsError_t ret = dsERR_NONE;

    return ret;
}

dsError_t dsSetHostPowerMode(int newPower)
{
    dsError_t ret = dsERR_NONE;

    /* TODO: Standby Mode to be finished */
    return ret;
}

dsError_t dsGetHostPowerMode(int *currPower)
{
    dsError_t ret = dsERR_NONE;
    return ret;
}

dsError_t dsGetPreferredSleepMode(dsSleepMode_t *pMode)
{
    dsError_t ret = dsERR_NONE;

    return ret;
}

dsError_t dsSetPreferredSleepMode(dsSleepMode_t mode)
{
    dsError_t ret = dsERR_NONE;

    return ret;
}

dsError_t dsGetCPUTemperature(float *cpuTemperature)
{
    dsError_t ret = dsERR_NONE;

    return ret;
}

dsError_t dsGetVersion(uint32_t *versionNumber)
{
    dsError_t ret = dsERR_NONE;

    return ret;
}

dsError_t dsSetVersion(uint32_t versionNumber)
{
    dsError_t ret = dsERR_NONE;

    return ret;
}

dsError_t dsHostTerm()
{
    dsError_t ret = dsERR_NONE;

    return ret;
}
