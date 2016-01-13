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

#ifndef	__ODM_PRECOMP_H__
#define __ODM_PRECOMP_H__

#include "phydm_types.h"

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
#include "Precomp.h"		// We need to include mp_precomp.h due to batch file setting.
#else
#define		TEST_FALG___		1
#endif

#if (DM_ODM_SUPPORT_TYPE ==ODM_CE) 
#define 	RTL8192CE_SUPPORT 				0
#define 	RTL8192CU_SUPPORT 				0
#define 	RTL8192C_SUPPORT 				0	

#define 	RTL8192DE_SUPPORT 				0
#define 	RTL8192DU_SUPPORT 				0
#define 	RTL8192D_SUPPORT 				0	

#define 	RTL8723AU_SUPPORT				0
#define 	RTL8723AS_SUPPORT				0
#define 	RTL8723AE_SUPPORT				0
#define 	RTL8723A_SUPPORT				0
#define 	RTL8723_FPGA_VERIFICATION		0
#endif

//2 Config Flags and Structs - defined by each ODM Type

#if (DM_ODM_SUPPORT_TYPE == ODM_AP)
	#include "../8192cd_cfg.h"
	#include "../odm_inc.h"

	#include "../8192cd.h"
	#include "../8192cd_util.h"

	#ifdef AP_BUILD_WORKAROUND
	#include "../8192cd_headers.h"
	#include "../8192cd_debug.h"		
	#endif

#elif (DM_ODM_SUPPORT_TYPE ==ODM_CE)
	#define BEAMFORMING_SUPPORT 0
	#define __PACK
	#define __WLAN_ATTRIB_PACK__
#elif (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	#include "Mp_Precomp.h"
	#define __PACK
	#define __WLAN_ATTRIB_PACK__
#endif

//2 OutSrc Header Files
 
#include "phydm.h" 
#include "phydm_HWConfig.h"
#include "phydm_debug.h"
#include "phydm_RegDefine11AC.h"
#include "phydm_RegDefine11N.h"
#include "phydm_interface.h"
#include "phydm_reg.h"
#include "HalPhyRf.h"

#if (DM_ODM_SUPPORT_TYPE & (ODM_CE|ODM_AP))
#define RTL8821B_SUPPORT		0
#define RTL8822B_SUPPORT		0
#define RTL8703B_SUPPORT		0
#define RTL8188F_SUPPORT		0
#endif

#if (RTL8723B_SUPPORT==1) 
#include "HalHWImg8723B_MAC.h"
#include "HalHWImg8723B_RF.h"
#include "HalHWImg8723B_BB.h"
#include "HalHWImg8723B_FW.h"
#include "phydm_RegConfig8723B.h"
#include "phydm_RTL8723B.h"
#include "HalPhyRf_8723B.h"
#include "Hal8723BReg.h"
#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	#include "HalHWImg8723B_MP.h"
	#include "rtl8723b_hal.h"
#endif
#endif

#if (RTL8821A_SUPPORT==1) 
#include "rtl8821a/HalHWImg8821A_MAC.h"
#include "rtl8821a/HalHWImg8821A_RF.h"
#include "rtl8821a/HalHWImg8821A_BB.h"
#include "rtl8821a/HalHWImg8821A_FW.h"
#include "rtl8821a/phydm_RegConfig8821A.h"
#include "rtl8821a/phydm_RTL8821A.h"
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	#include "rtl8821a/HalPhyRf_8821A_WIN.h"
#elif (DM_ODM_SUPPORT_TYPE == ODM_CE)
	#include "rtl8821a/HalPhyRf_8821A_CE.h"
	#include "rtl8821a/PhyDM_IQK_8821A_CE.h"/*for IQK*/
	#include "rtl8812a/HalPhyRf_8812A.h"/*for IQK,LCK,Power-tracking*/
	#include "rtl8812a_hal.h"
#else
#endif
#endif

#if (RTL8821B_SUPPORT==1) 
#include "rtl8821b/HalHWImg8821B_MAC.h"
#include "rtl8821b/HalHWImg8821B_RF.h"
#include "rtl8821b/HalHWImg8821B_BB.h"
#include "rtl8821b/HalHWImg8821B_FW.h"
#include "rtl8821b/phydm_RegConfig8821B.h"
#include "rtl8821b/HalHWImg8821B_TestChip_MAC.h"
#include "rtl8821b/HalHWImg8821B_TestChip_RF.h"
#include "rtl8821b/HalHWImg8821B_TestChip_BB.h"
#include "rtl8821b/HalHWImg8821B_TestChip_FW.h"
#include "rtl8821b/HalPhyRf_8821B.h"
#endif

#if (RTL8822B_SUPPORT==1) 
#include "rtl8822B/HalHWImg8822B_MAC.h"
#include "rtl8822B/HalHWImg8822B_RF.h"
#include "rtl8822B/HalHWImg8822B_BB.h"
#include "rtl8822B/HalHWImg8822B_FW.h"
#include "rtl8822B/phydm_RegConfig8822B.h"
#include "rtl8822B/HalHWImg8822B_TestChip_MAC.h"
#include "rtl8822B/HalHWImg8822B_TestChip_RF.h"
#include "rtl8822B/HalHWImg8822B_TestChip_BB.h"
#include "rtl8822B/HalHWImg8822B_TestChip_FW.h"
#include "rtl8822b/HalPhyRf_8822B.h"
#endif

#if (RTL8703B_SUPPORT==1) 
#include "rtl8703b/phydm_RegConfig8703B.h"
#include "rtl8703b/HalHWImg8703B_TestChip_MAC.h"
#include "rtl8703b/HalHWImg8703B_TestChip_RF.h"
#include "rtl8703b/HalHWImg8703B_TestChip_BB.h"
#include "rtl8703b/HalHWImg8703B_FW.h"
#endif

#if (RTL8188F_SUPPORT==1) 
#include "rtl8188f/phydm_RegConfig8188F.h"
#include "rtl8188f/HalHWImg8188F_TestChip_MAC.h"
#include "rtl8188f/HalHWImg8188F_TestChip_RF.h"
#include "rtl8188f/HalHWImg8188F_TestChip_BB.h"
#include "rtl8188f/HalHWImg8188F_FW.h"
#endif

#endif	// __ODM_PRECOMP_H__

