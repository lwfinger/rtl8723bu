/******************************************************************************
 *
 * Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 *
 ******************************************************************************/

#ifndef __HAL_ODM_PRECOMP_H__
#define __HAL_ODM_PRECOMP_H__

#include "odm_types.h"

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
#include "Precomp.h"		// We need to include mp_precomp.h due to batch file setting.

#else

#define		TEST_FALG___		1

#endif

//2 Config Flags and Structs - defined by each ODM Type

#if (DM_ODM_SUPPORT_TYPE == ODM_AP)
	#include "../8192cd_cfg.h"
	#include "../odm_inc.h"

	#include "../8192cd.h"
	#include "../8192cd_util.h"
	#ifdef _BIG_ENDIAN_
	#define	ODM_ENDIAN_TYPE				ODM_ENDIAN_BIG
	#else
	#define	ODM_ENDIAN_TYPE				ODM_ENDIAN_LITTLE
	#endif

	#ifdef AP_BUILD_WORKAROUND
	#include "../8192cd_headers.h"
	#include "../8192cd_debug.h"
	#endif

#elif (DM_ODM_SUPPORT_TYPE == ODM_ADSL)
	// Flags
	#include "8192cd_cfg.h"			// OUTSRC needs ADSL config flags.
	#include "odm_inc.h"			// OUTSRC needs some extra flags.
	// Data Structure
	#include "common_types.h"	// OUTSRC and rtl8192cd both needs basic type such as UINT8 and BIT0.
	#include "8192cd.h"			// OUTSRC needs basic ADSL struct definition.
	#include "8192cd_util.h"		// OUTSRC needs basic I/O function.
	#ifdef _BIG_ENDIAN_
	#define	ODM_ENDIAN_TYPE				ODM_ENDIAN_BIG
	#else
	#define	ODM_ENDIAN_TYPE				ODM_ENDIAN_LITTLE
	#endif

	#ifdef ADSL_AP_BUILD_WORKAROUND
	// NESTED_INC: Functions defined outside should not be included!! Marked by Annie, 2011-10-14.
	#include "../8192cd_headers.h"
	#include "../8192cd_debug.h"
	#endif

#elif (DM_ODM_SUPPORT_TYPE ==ODM_CE)
#define BEAMFORMING_SUPPORT 0
#elif (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	#include "Mp_Precomp.h"
	#define	ODM_ENDIAN_TYPE				ODM_ENDIAN_LITTLE
#endif

//2 OutSrc Header Files
#include "odm.h"
#include "odm_HWConfig.h"
#include "odm_debug.h"
#include "odm_RegDefine11AC.h"
#include "odm_RegDefine11N.h"
#include "odm_AntDiv.h"
#include "odm_interface.h"
#include "odm_reg.h"

#if (DM_ODM_SUPPORT_TYPE == ODM_AP)

#elif (DM_ODM_SUPPORT_TYPE == ODM_ADSL)
	#include "rtl8192c/HalDMOutSrc8192C_ADSL.h"

#elif (DM_ODM_SUPPORT_TYPE == ODM_CE)
	//#include "hal_com.h"
	#include "HalPhyRf.h"

		#include "HalPhyRf_8723B.h"//for IQK,LCK,Power-tracking
		#include "rtl8723b_hal.h"
#endif

#include "HalHWImg8723B_MAC.h"
#include "HalHWImg8723B_RF.h"
#include "HalHWImg8723B_BB.h"
#include "HalHWImg8723B_FW.h"
#include "HalHWImg8723B_MP.h"
#include "Hal8723BReg.h"
#include "odm_RTL8723B.h"
#include "odm_RegConfig8723B.h"

#endif	// __ODM_PRECOMP_H__
