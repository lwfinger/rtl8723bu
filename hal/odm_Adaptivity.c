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

//============================================================
// include files
//============================================================
#include "odm_precomp.h"


VOID
Phydm_CheckAdaptivity(
	IN		PVOID			pDM_VOID
	)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	if(pDM_Odm->SupportAbility & ODM_BB_ADAPTIVITY)
	{
		if(pDM_Odm->bAdaOn == TRUE)
		{
			if(pDM_Odm->DynamicLinkAdaptivity == TRUE)
			{
				if(pDM_Odm->bLinked && pDM_Odm->bCheck == FALSE)
				{
					Phydm_NHMCounterStatistics(pDM_Odm);
					Phydm_CheckEnvironment(pDM_Odm);
				}
				else if(!pDM_Odm->bLinked)
				{
				pDM_Odm->bCheck = FALSE;
				}
			}
			else
			{
				Phydm_MACEDCCAState(pDM_Odm, PhyDM_DONT_IGNORE_EDCCA);
				pDM_Odm->adaptivity_flag = TRUE;
			}
		}
		else
		{
			Phydm_MACEDCCAState(pDM_Odm, PhyDM_IGNORE_EDCCA);
			pDM_Odm->adaptivity_flag = FALSE;
		}
	}
}

VOID
Phydm_NHMCounterStatisticsInit(
	IN		PVOID			pDM_VOID
	)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;

	if(pDM_Odm->SupportICType & ODM_IC_11AC_SERIES)
	{
		//PHY parameters initialize for ac series
		ODM_Write2Byte(pDM_Odm, ODM_REG_NHM_TIMER_11AC+2, 0xC350);	//0x990[31:16]=0xC350	Time duration for NHM unit: us, 0xc350=200ms
		ODM_Write2Byte(pDM_Odm, ODM_REG_NHM_TH9_TH10_11AC+2, 0xffff);	//0x994[31:16]=0xffff	th_9, th_10
		//ODM_Write4Byte(pDM_Odm, ODM_REG_NHM_TH3_TO_TH0_11AC, 0xffffff5c);	//0x998=0xffffff5c 		th_3, th_2, th_1, th_0
		ODM_Write4Byte(pDM_Odm, ODM_REG_NHM_TH3_TO_TH0_11AC, 0xffffff50);	//0x998=0xffffff52 		th_3, th_2, th_1, th_0
		ODM_Write4Byte(pDM_Odm, ODM_REG_NHM_TH7_TO_TH4_11AC, 0xffffffff);	//0x99c=0xffffffff		th_7, th_6, th_5, th_4
		ODM_SetBBReg(pDM_Odm, ODM_REG_NHM_TH8_11AC, bMaskByte0, 0xff);		//0x9a0[7:0]=0xff		th_8
		//ODM_SetBBReg(pDM_Odm, ODM_REG_NHM_TH9_TH10_11AC, BIT8|BIT9|BIT10, 0x7);	//0x994[9:8]=3			enable CCX
		ODM_SetBBReg(pDM_Odm, ODM_REG_NHM_TH9_TH10_11AC, BIT8|BIT9|BIT10, 0x1);	//0x994[10:8]=1	ignoreCCA ignore PHYTXON	enable CCX
		ODM_SetBBReg(pDM_Odm, ODM_REG_NHM_9E8_11AC, BIT0, 0x1);		//0x9e8[7]=1			max power among all RX ants

	}
	else if (pDM_Odm->SupportICType & ODM_IC_11N_SERIES)
	{
		//PHY parameters initialize for n series
		ODM_Write2Byte(pDM_Odm, ODM_REG_NHM_TIMER_11N+2, 0xC350);	//0x894[31:16]=0x0xC350	Time duration for NHM unit: us, 0xc350=200ms
		//ODM_Write2Byte(pDM_Odm, ODM_REG_NHM_TIMER_11N+2, 0x4e20);	//0x894[31:16]=0x4e20	Time duration for NHM unit: 4us, 0x4e20=80ms
		ODM_Write2Byte(pDM_Odm, ODM_REG_NHM_TH9_TH10_11N+2, 0xffff);	//0x890[31:16]=0xffff	th_9, th_10
		//ODM_Write4Byte(pDM_Odm, ODM_REG_NHM_TH3_TO_TH0_11N, 0xffffff5c);	//0x898=0xffffff5c 		th_3, th_2, th_1, th_0
		ODM_Write4Byte(pDM_Odm, ODM_REG_NHM_TH3_TO_TH0_11N, 0xffffff50);	//0x898=0xffffff52 		th_3, th_2, th_1, th_0
		ODM_Write4Byte(pDM_Odm, ODM_REG_NHM_TH7_TO_TH4_11N, 0xffffffff);	//0x89c=0xffffffff		th_7, th_6, th_5, th_4
		ODM_SetBBReg(pDM_Odm, ODM_REG_FPGA0_IQK_11N, bMaskByte0, 0xff);		//0xe28[7:0]=0xff		th_8
		//ODM_SetBBReg(pDM_Odm, ODM_REG_NHM_TH9_TH10_11N, BIT10|BIT9|BIT8, 0x7);	//0x890[9:8]=3			enable CCX
		ODM_SetBBReg(pDM_Odm, ODM_REG_NHM_TH9_TH10_11N, BIT10|BIT9|BIT8, 0x1);	//0x890[10:8]=1		ignoreCCA ignore PHYTXON	enable CCX
		ODM_SetBBReg(pDM_Odm, ODM_REG_OFDM_FA_RSTC_11N, BIT7, 0x1);		//0xc0c[7]=1			max power among all RX ants
	}
}

VOID
Phydm_NHMCounterStatistics(
	IN		PVOID			pDM_VOID
	)
{
	PDM_ODM_T	pDM_Odm = (PDM_ODM_T)pDM_VOID;

	if(!(pDM_Odm->SupportAbility & ODM_BB_NHM_CNT))
		return;

	// Get NHM report
	Phydm_GetNHMCounterStatistics(pDM_Odm);

	// Reset NHM counter
	Phydm_NHMCounterStatisticsReset(pDM_Odm);
}

VOID
Phydm_GetNHMCounterStatistics(
	IN		PVOID			pDM_VOID
	)
{
	PDM_ODM_T	pDM_Odm = (PDM_ODM_T)pDM_VOID;
	u4Byte		value32 = 0;

	if (pDM_Odm->SupportICType & ODM_IC_11AC_SERIES)
		value32 = ODM_GetBBReg(pDM_Odm, ODM_REG_NHM_CNT_11AC, bMaskDWord);
	else if (pDM_Odm->SupportICType & ODM_IC_11N_SERIES)
		value32 = ODM_GetBBReg(pDM_Odm, ODM_REG_NHM_CNT_11N, bMaskDWord);

	pDM_Odm->NHM_cnt_0 = (u1Byte)(value32 & bMaskByte0);
	pDM_Odm->NHM_cnt_1 = (u1Byte)((value32 & bMaskByte1)>>8);

}

VOID
Phydm_NHMCounterStatisticsReset(
	IN		PVOID			pDM_VOID
	)
{
	PDM_ODM_T	pDM_Odm = (PDM_ODM_T)pDM_VOID;

	if (pDM_Odm->SupportICType & ODM_IC_11AC_SERIES)
	{
    		ODM_SetBBReg(pDM_Odm, ODM_REG_NHM_TH9_TH10_11AC, BIT1, 0);
    		ODM_SetBBReg(pDM_Odm, ODM_REG_NHM_TH9_TH10_11AC, BIT1, 1);
	}
	else if (pDM_Odm->SupportICType & ODM_IC_11N_SERIES)
	{
    		ODM_SetBBReg(pDM_Odm, ODM_REG_NHM_TH9_TH10_11N, BIT1, 0);
    		ODM_SetBBReg(pDM_Odm, ODM_REG_NHM_TH9_TH10_11N, BIT1, 1);
	}
}

VOID
Phydm_NHMBBInit(
	IN		PVOID			pDM_VOID
)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;

	pDM_Odm->adaptivity_flag = FALSE;
	pDM_Odm->tolerance_cnt = 3;
	pDM_Odm->NHMLastTxOkcnt = 0;
	pDM_Odm->NHMLastRxOkcnt = 0;
	pDM_Odm->NHMCurTxOkcnt = 0;
	pDM_Odm->NHMCurRxOkcnt = 0;
}

VOID
Phydm_SetEDCCAThreshold(
	IN	PVOID	pDM_VOID,
	IN	s1Byte	H2L,
	IN	s1Byte	L2H
)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;

	if(pDM_Odm->SupportICType & ODM_IC_11N_SERIES)
		{
		ODM_SetBBReg(pDM_Odm,rOFDM0_ECCAThreshold, bMaskByte0, (u1Byte)L2H);
		ODM_SetBBReg(pDM_Odm,rOFDM0_ECCAThreshold, bMaskByte2, (u1Byte)H2L);
		}
	else if(pDM_Odm->SupportICType & ODM_IC_11AC_SERIES)
		{
		ODM_SetBBReg(pDM_Odm, rFPGA0_XB_LSSIReadBack, bMaskByte0, (u1Byte)L2H);
		ODM_SetBBReg(pDM_Odm, rFPGA0_XB_LSSIReadBack, bMaskByte1, (u1Byte)H2L);
		}
}

VOID
Phydm_SetTRxMux(
	IN	PVOID				pDM_VOID,
	IN	PhyDM_Trx_MUX_Type	txMode,
	IN	PhyDM_Trx_MUX_Type	rxMode
)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;

	if (pDM_Odm->SupportICType & ODM_IC_11N_SERIES)
	{
		ODM_SetBBReg(pDM_Odm, ODM_REG_CCK_RPT_FORMAT_11N, BIT3|BIT2|BIT1, txMode);	// set TXmod to standby mode to remove outside noise affect
		ODM_SetBBReg(pDM_Odm, ODM_REG_CCK_RPT_FORMAT_11N, BIT22|BIT21|BIT20, rxMode);	// set RXmod to standby mode to remove outside noise affect
		if(pDM_Odm->RFType > ODM_1T1R)
		{
			ODM_SetBBReg(pDM_Odm, ODM_REG_CCK_RPT_FORMAT_11N_B, BIT3|BIT2|BIT1, txMode);	// set TXmod to standby mode to remove outside noise affect
			ODM_SetBBReg(pDM_Odm, ODM_REG_CCK_RPT_FORMAT_11N_B, BIT22|BIT21|BIT20, rxMode);	// set RXmod to standby mode to remove outside noise affect
		}
	}
	else if(pDM_Odm->SupportICType & ODM_IC_11AC_SERIES)
	{
		ODM_SetBBReg(pDM_Odm, ODM_REG_TRMUX_11AC, BIT11|BIT10|BIT9|BIT8, txMode);	// set TXmod to standby mode to remove outside noise affect
		ODM_SetBBReg(pDM_Odm, ODM_REG_TRMUX_11AC, BIT7|BIT6|BIT5|BIT4, rxMode);	// set RXmod to standby mode to remove outside noise affect
		if(pDM_Odm->RFType > ODM_1T1R)
		{
			ODM_SetBBReg(pDM_Odm, ODM_REG_TRMUX_11AC_B, BIT11|BIT10|BIT9|BIT8, txMode);	// set TXmod to standby mode to remove outside noise affect
			ODM_SetBBReg(pDM_Odm, ODM_REG_TRMUX_11AC_B, BIT7|BIT6|BIT5|BIT4, rxMode);	// set RXmod to standby mode to remove outside noise affect
		}
	}

}

VOID
Phydm_MACEDCCAState(
	IN	PVOID					pDM_VOID,
	IN	PhyDM_MACEDCCA_Type		State
)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	if(State == PhyDM_IGNORE_EDCCA)
	{
		ODM_SetMACReg(pDM_Odm, REG_TX_PTCL_CTRL, BIT15, 1);	//ignore EDCCA	reg520[15]=1
		ODM_SetMACReg(pDM_Odm, REG_RD_CTRL, BIT11, 0);		//reg524[11]=0
	}
	else		// don't set MAC ignore EDCCA signal
	{
		ODM_SetMACReg(pDM_Odm, REG_TX_PTCL_CTRL, BIT15, 0);	//don't ignore EDCCA	 reg520[15]=0
		ODM_SetMACReg(pDM_Odm, REG_RD_CTRL, BIT11, 1);	//reg524[11]=1
	}

	pDM_Odm->EDCCA_enable_state = State;

	ODM_RT_TRACE(pDM_Odm,PHYDM_COMP_ADAPTIVITY, ODM_DBG_LOUD, ("EDCCA enable State = %d \n", State));

}

BOOLEAN
Phydm_CalNHMcnt(
	IN		PVOID		pDM_VOID
)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	u2Byte			Base = 0;

	Base = pDM_Odm->NHM_cnt_0 + pDM_Odm->NHM_cnt_1;

	if(Base != 0)
	{
		pDM_Odm->NHM_cnt_0 = ((pDM_Odm->NHM_cnt_0) << 8) / Base;
		pDM_Odm->NHM_cnt_1 = ((pDM_Odm->NHM_cnt_1) << 8) / Base;
	}
	if((pDM_Odm->NHM_cnt_0 - pDM_Odm->NHM_cnt_1) >= 100)
		return TRUE;			// clean environment
	else
		return FALSE;		//noisy environment

}


VOID
Phydm_CheckEnvironment(
	IN	PVOID	pDM_VOID
)
{
	PDM_ODM_T	pDM_Odm = (PDM_ODM_T)pDM_VOID;
	BOOLEAN 	isCleanEnvironment = FALSE;
	u1Byte		i, clean = 0;

	if(pDM_Odm->bFirstLink == TRUE)
	{
		pDM_Odm->adaptivity_flag = TRUE;
		pDM_Odm->bFirstLink = FALSE;
		return;
	}
	else
	{
		if(pDM_Odm->NHMWait < 3)			// Start enter NHM after 4 NHMWait
		{
			pDM_Odm->NHMWait ++;
			Phydm_NHMCounterStatistics(pDM_Odm);
			return;
		}
		else
		{
			Phydm_NHMCounterStatistics(pDM_Odm);
			isCleanEnvironment = Phydm_CalNHMcnt(pDM_Odm);
			if(isCleanEnvironment == TRUE)
			{
				Phydm_MACEDCCAState(pDM_Odm, PhyDM_DONT_IGNORE_EDCCA);
#if(DM_ODM_SUPPORT_TYPE & (ODM_AP|ODM_ADSL))
				pDM_Odm->TH_L2H_ini = pDM_Odm->TH_L2H_ini_backup;			//mode 1
				pDM_Odm->TH_EDCCA_HL_diff= pDM_Odm->TH_EDCCA_HL_diff_backup;
#endif
				pDM_Odm->adaptivity_flag = TRUE;
			}
			else
			{
#if(DM_ODM_SUPPORT_TYPE & (ODM_WIN|ODM_CE))
				Phydm_MACEDCCAState(pDM_Odm, PhyDM_IGNORE_EDCCA);
#else
				Phydm_MACEDCCAState(pDM_Odm, PhyDM_DONT_IGNORE_EDCCA);
				pDM_Odm->TH_L2H_ini = pDM_Odm->TH_L2H_ini_mode2;			// for AP mode 2
				pDM_Odm->TH_EDCCA_HL_diff= pDM_Odm->TH_EDCCA_HL_diff_mode2;
#endif
				pDM_Odm->adaptivity_flag = FALSE;
			}

			pDM_Odm->bFirstLink = TRUE;
			pDM_Odm->bCheck = TRUE;
		}

	}


}


VOID
Phydm_NHMBB(
	IN		PVOID			pDM_VOID
)
{
	PDM_ODM_T	pDM_Odm = (PDM_ODM_T)pDM_VOID;
	BOOLEAN		bCleanEnvironment;

	bCleanEnvironment = Phydm_CalNHMcnt(pDM_Odm);

	pDM_Odm->NHMCurTxOkcnt = *(pDM_Odm->pNumTxBytesUnicast) - pDM_Odm->NHMLastTxOkcnt;
	pDM_Odm->NHMCurRxOkcnt = *(pDM_Odm->pNumRxBytesUnicast) - pDM_Odm->NHMLastRxOkcnt;
	pDM_Odm->NHMLastTxOkcnt = *(pDM_Odm->pNumTxBytesUnicast);
	pDM_Odm->NHMLastRxOkcnt = *(pDM_Odm->pNumRxBytesUnicast);
	ODM_RT_TRACE(pDM_Odm,PHYDM_COMP_ADAPTIVITY, ODM_DBG_LOUD, ("cnt_0=%d, cnt_1=%d, bCleanEnvironment = %d, NHMCurTxOkcnt = %llu, NHMCurRxOkcnt = %llu\n",
		pDM_Odm->NHM_cnt_0, pDM_Odm->NHM_cnt_1, bCleanEnvironment, pDM_Odm->NHMCurTxOkcnt, pDM_Odm->NHMCurRxOkcnt));

	if(pDM_Odm->NHMWait < 4)			// Start enter NHM after 4 NHMWait
	{
		pDM_Odm->NHMWait ++;
		Phydm_MACEDCCAState(pDM_Odm, PhyDM_IGNORE_EDCCA);
	}
	else if ( ((pDM_Odm->NHMCurTxOkcnt>>10) > 2) && ((pDM_Odm->NHMCurTxOkcnt) + 1 > (u8Byte)(pDM_Odm->NHMCurRxOkcnt<<2) + 1))		//Tx > 4*Rx and Tx > 2Mb possible for adaptivity test
	{
		if(bCleanEnvironment == TRUE || pDM_Odm->adaptivity_flag == TRUE)
		{
			//Enable EDCCA since it is possible running Adaptivity testing
			pDM_Odm->adaptivity_flag = TRUE;
			Phydm_MACEDCCAState(pDM_Odm, PhyDM_DONT_IGNORE_EDCCA);
			pDM_Odm->tolerance_cnt = 0;
#if(DM_ODM_SUPPORT_TYPE & (ODM_AP|ODM_ADSL))
			pDM_Odm->TH_L2H_ini = pDM_Odm->TH_L2H_ini_backup;
			pDM_Odm->TH_EDCCA_HL_diff = pDM_Odm->TH_EDCCA_HL_diff_backup ;
#endif
            	}
		else
		{
			if(pDM_Odm->tolerance_cnt < 3)
				pDM_Odm->tolerance_cnt ++;
			else
			{
#if(DM_ODM_SUPPORT_TYPE & (ODM_AP|ODM_ADSL))
			pDM_Odm->TH_L2H_ini = pDM_Odm->TH_L2H_ini_mode2;
			pDM_Odm->TH_EDCCA_HL_diff = pDM_Odm->TH_EDCCA_HL_diff_mode2 ;
#else
			Phydm_MACEDCCAState(pDM_Odm, PhyDM_IGNORE_EDCCA);
#endif
            		pDM_Odm->adaptivity_flag = FALSE;
			}
		}
	}
	else	// TX<RX
	{
		if(pDM_Odm->adaptivity_flag == TRUE && bCleanEnvironment == FALSE)
		{
			Phydm_MACEDCCAState(pDM_Odm, PhyDM_DONT_IGNORE_EDCCA);
			pDM_Odm->tolerance_cnt = 0;
#if(DM_ODM_SUPPORT_TYPE & (ODM_AP|ODM_ADSL))
			pDM_Odm->TH_L2H_ini = pDM_Odm->TH_L2H_ini_backup;
			pDM_Odm->TH_EDCCA_HL_diff = pDM_Odm->TH_EDCCA_HL_diff_backup ;
#endif
		}
#if(DM_ODM_SUPPORT_TYPE & ODM_AP)		// for repeater mode add by YuChen 2014.06.23
#ifdef UNIVERSAL_REPEATER
		else if((bCleanEnvironment == TRUE) && (pDM_Odm->VXD_bLinked) && ((pDM_Odm->NHMCurTxOkcnt>>10) > 1))		// clean environment and VXD linked and Tx TP>1Mb
		{
			pDM_Odm->adaptivity_flag = TRUE;
			Phydm_MACEDCCAState(pDM_Odm, PhyDM_DONT_IGNORE_EDCCA);
			pDM_Odm->tolerance_cnt = 0;
			pDM_Odm->TH_L2H_ini = pDM_Odm->TH_L2H_ini_backup;
			pDM_Odm->TH_EDCCA_HL_diff = pDM_Odm->TH_EDCCA_HL_diff_backup ;
		}
#endif
#endif									// for repeater mode add by YuChen 2014.06.23
		else
		{
			if(pDM_Odm->tolerance_cnt < 3)
				pDM_Odm->tolerance_cnt ++;
			else
			{
#if(DM_ODM_SUPPORT_TYPE & (ODM_AP|ODM_ADSL))
			pDM_Odm->TH_L2H_ini = pDM_Odm->TH_L2H_ini_mode2;
			pDM_Odm->TH_EDCCA_HL_diff = pDM_Odm->TH_EDCCA_HL_diff_mode2 ;
#else
			Phydm_MACEDCCAState(pDM_Odm, PhyDM_IGNORE_EDCCA);
#endif
			pDM_Odm->adaptivity_flag = FALSE;
			}
		}
	}

	ODM_RT_TRACE(pDM_Odm,PHYDM_COMP_ADAPTIVITY, ODM_DBG_LOUD, ("adaptivity_flag = %d\n ", pDM_Odm->adaptivity_flag));
}

VOID
Phydm_SearchPwdBLowerBound(
	IN		PVOID		pDM_VOID
)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	u4Byte			value32 =0;
	u1Byte			cnt, IGI_Pause = 0x7f, IGI_Resume = 0x20, IGI = 0x50;	//IGI = 0x50 for cal EDCCA lower bound
	u1Byte			txEdcca1 = 0, txEdcca0 = 0;
	BOOLEAN			bAdjust=TRUE;
	s1Byte 			TH_L2H_dmc, TH_H2L_dmc, IGI_target = 0x32;
	s1Byte 			Diff;

	Phydm_SetTRxMux(pDM_Odm, PhyDM_STANDBY_MODE, PhyDM_STANDBY_MODE);
	ODM_Write_DIG(pDM_Odm, IGI_Pause);

	Diff = IGI_target -(s1Byte)IGI;
	TH_L2H_dmc = pDM_Odm->TH_L2H_ini + Diff;
		if(TH_L2H_dmc > 10)
			TH_L2H_dmc = 10;
	TH_H2L_dmc = TH_L2H_dmc - pDM_Odm->TH_EDCCA_HL_diff;

	Phydm_SetEDCCAThreshold(pDM_Odm, TH_H2L_dmc, TH_L2H_dmc);
	ODM_delay_ms(5);

		while(bAdjust)
			{
			for(cnt=0; cnt<20; cnt ++)
				{
				if (pDM_Odm->SupportICType & ODM_IC_11N_SERIES)
					value32 = ODM_GetBBReg(pDM_Odm,ODM_REG_RPT_11N, bMaskDWord);
				else if(pDM_Odm->SupportICType & ODM_IC_11AC_SERIES)
					value32 = ODM_GetBBReg(pDM_Odm,ODM_REG_RPT_11AC, bMaskDWord);

				if (value32 & BIT30 && (pDM_Odm->SupportICType & (ODM_RTL8723A|ODM_RTL8723B|ODM_RTL8188E)))
					txEdcca1 = txEdcca1 + 1;
				else if(value32 & BIT29)
					txEdcca1 = txEdcca1 + 1;
				else
					txEdcca0 = txEdcca0 + 1;
				}

				if(txEdcca1 > 9 )
				{
					IGI = IGI -1;
					TH_L2H_dmc = TH_L2H_dmc + 1;
						if(TH_L2H_dmc > 10)
							TH_L2H_dmc = 10;
					TH_H2L_dmc = TH_L2H_dmc - pDM_Odm->TH_EDCCA_HL_diff;

					Phydm_SetEDCCAThreshold(pDM_Odm, TH_H2L_dmc, TH_L2H_dmc);

					txEdcca1 = 0;
					txEdcca0 = 0;

					if(TH_L2H_dmc == 10)
						{
						bAdjust = FALSE;
						pDM_Odm->H2L_lb = TH_H2L_dmc;
						pDM_Odm->L2H_lb = TH_L2H_dmc;
						pDM_Odm->Adaptivity_IGI_upper = IGI;
						}
				}
				else
				{
					bAdjust = FALSE;
					pDM_Odm->H2L_lb = TH_H2L_dmc;
					pDM_Odm->L2H_lb = TH_L2H_dmc;
					pDM_Odm->Adaptivity_IGI_upper = IGI;
				}
			}

	Phydm_SetTRxMux(pDM_Odm, PhyDM_TX_MODE, PhyDM_RX_MODE);
	ODM_Write_DIG(pDM_Odm, IGI_Resume);
	Phydm_SetEDCCAThreshold(pDM_Odm, 0x7f, 0x7f); // resume to no link state
}

VOID
Phydm_AdaptivityInit(
	IN 	PVOID	 	pDM_VOID
)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
#if(DM_ODM_SUPPORT_TYPE == ODM_WIN)
	PADAPTER		pAdapter	= pDM_Odm->Adapter;
	PMGNT_INFO		pMgntInfo = &(pAdapter->MgntInfo);
	pDM_Odm->Carrier_Sense_enable = (BOOLEAN)pMgntInfo->RegEnableCarrierSense;
	pDM_Odm->NHM_enable = (BOOLEAN)pMgntInfo->RegNHMEnable;
	pDM_Odm->DynamicLinkAdaptivity = (BOOLEAN)pMgntInfo->RegDmLinkAdaptivity;
#elif(DM_ODM_SUPPORT_TYPE == ODM_CE)
	pDM_Odm->Carrier_Sense_enable = (pDM_Odm->Adapter->registrypriv.adaptivity_mode!=0)?TRUE:FALSE;
	pDM_Odm->NHM_enable = (BOOLEAN)pDM_Odm->Adapter->registrypriv.nhm_en;
	pDM_Odm->DynamicLinkAdaptivity = FALSE; // Jeff please add this
#endif

#if(DM_ODM_SUPPORT_TYPE & (ODM_CE|ODM_WIN))

	if(pDM_Odm->Carrier_Sense_enable == FALSE)
	{
#if(DM_ODM_SUPPORT_TYPE == ODM_WIN)
		if( pMgntInfo->RegL2HForAdaptivity != 0 )
			pDM_Odm->TH_L2H_ini = pMgntInfo->RegL2HForAdaptivity;
		else
#endif
			pDM_Odm->TH_L2H_ini = 0xf5; // -7
	}
	else
	{
#if(DM_ODM_SUPPORT_TYPE == ODM_WIN)
		if( pMgntInfo->RegL2HForAdaptivity != 0 )
			pDM_Odm->TH_L2H_ini = pMgntInfo->RegL2HForAdaptivity;
		else
#endif
		pDM_Odm->TH_L2H_ini = 0xa;
	}

	pDM_Odm->AdapEn_RSSI = 20;

#if(DM_ODM_SUPPORT_TYPE == ODM_WIN)
	if( pMgntInfo->RegHLDiffForAdaptivity != 0 )
		pDM_Odm->TH_EDCCA_HL_diff = pMgntInfo->RegHLDiffForAdaptivity;
	else
#endif
	pDM_Odm->TH_EDCCA_HL_diff = 7;

	ODM_RT_TRACE(pDM_Odm,PHYDM_COMP_ADAPTIVITY, ODM_DBG_LOUD, ("TH_L2H_ini = 0x%x, TH_EDCCA_HL_diff = 0x%x\n", pDM_Odm->TH_L2H_ini, pDM_Odm->TH_EDCCA_HL_diff));

#elif (DM_ODM_SUPPORT_TYPE & (ODM_AP|ODM_ADSL))
	prtl8192cd_priv				priv = pDM_Odm->priv;

	if(pDM_Odm->Carrier_Sense_enable){
		pDM_Odm->TH_L2H_ini = 10;
		pDM_Odm->TH_EDCCA_HL_diff = 7;
		pDM_Odm->AdapEn_RSSI = 30;
	}
	else
	{
		pDM_Odm->TH_L2H_ini = pDM_Odm->TH_L2H_ini_backup;	//set by mib
		pDM_Odm->TH_EDCCA_HL_diff = 7;
		pDM_Odm->AdapEn_RSSI = 20;
	}

	pDM_Odm->TH_L2H_ini_mode2 = 20;
	pDM_Odm->TH_EDCCA_HL_diff_mode2 = 8;
	//pDM_Odm->TH_L2H_ini_backup = pDM_Odm->TH_L2H_ini;
	pDM_Odm->TH_EDCCA_HL_diff_backup = pDM_Odm->TH_EDCCA_HL_diff ;
	if(priv->pshare->rf_ft_var.adaptivity_enable == 2)
		pDM_Odm->DynamicLinkAdaptivity = TRUE;
	else
		pDM_Odm->DynamicLinkAdaptivity = FALSE;
//	pDM_Odm->NHM_enable = FALSE;
#endif

	pDM_Odm->IGI_Base = 0x32;
	pDM_Odm->IGI_target = 0x1c;
	pDM_Odm->ForceEDCCA = 0;
	pDM_Odm->H2L_lb= 0;
	pDM_Odm->L2H_lb= 0;
	pDM_Odm->Adaptivity_IGI_upper = 0;
	pDM_Odm->NHMWait = 0;
	Phydm_NHMBBInit(pDM_Odm);
	pDM_Odm->bCheck = FALSE;
	pDM_Odm->bFirstLink = TRUE;
	pDM_Odm->bAdaOn = TRUE;

	ODM_SetBBReg(pDM_Odm, REG_RD_CTRL, BIT11, 1); // stop counting if EDCCA is asserted

	//Search pwdB lower bound
	{
	if (pDM_Odm->SupportICType & ODM_IC_11N_SERIES)
		ODM_SetBBReg(pDM_Odm,ODM_REG_DBG_RPT_11N, bMaskDWord, 0x208);
	else if(pDM_Odm->SupportICType & ODM_IC_11AC_SERIES)
		ODM_SetBBReg(pDM_Odm,ODM_REG_DBG_RPT_11AC, bMaskDWord, 0x209);
	Phydm_SearchPwdBLowerBound(pDM_Odm);
	}
	Phydm_MACEDCCAState(pDM_Odm, PhyDM_IGNORE_EDCCA);
}


BOOLEAN
Phydm_Adaptivity(
	IN		PVOID			pDM_VOID,
	IN		u1Byte			IGI
)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	s1Byte TH_L2H_dmc, TH_H2L_dmc, L2H_nolink_Band4 = 0x7f, H2L_nolink_Band4 = 0x7f;
	s1Byte Diff, IGI_target;
	BOOLEAN EDCCA_State = FALSE;

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	PADAPTER		pAdapter	= pDM_Odm->Adapter;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);
	BOOLEAN		bFwCurrentInPSMode=FALSE;
	PMGNT_INFO				pMgntInfo = &(pAdapter->MgntInfo);

	pAdapter->HalFunc.GetHwRegHandler(pAdapter, HW_VAR_FW_PSMODE_STATUS, (pu1Byte)(&bFwCurrentInPSMode));

	// Disable EDCCA mode while under LPS mode, added by Roger, 2012.09.14.
	if(bFwCurrentInPSMode)
		return FALSE;
#endif

	if(!(pDM_Odm->SupportAbility & ODM_BB_ADAPTIVITY))
	{
		ODM_RT_TRACE(pDM_Odm,PHYDM_COMP_ADAPTIVITY, ODM_DBG_LOUD, ("Go to odm_DynamicEDCCA() \n"));
		// Add by Neil Chen to enable edcca to MP Platform
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
		// Adjust EDCCA.
		if(pDM_Odm->SupportICType & ODM_IC_11N_SERIES)
			Phydm_DynamicEDCCA(pDM_Odm);
#endif
		return FALSE;
	}

#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN|ODM_CE))
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	if(pMgntInfo->RegEnableAdaptivity== 2)
#else
	if (pDM_Odm->Adapter->registrypriv.adaptivity_en == 2)
#endif
	{
		if(pDM_Odm->Carrier_Sense_enable == FALSE)		// check domain Code for Adaptivity or CarrierSense
		{
			if ((*pDM_Odm->pBandType == ODM_BAND_5G) &&
				!(pDM_Odm->odm_Regulation5G == REGULATION_ETSI || pDM_Odm->odm_Regulation5G == REGULATION_WW))
			{
				ODM_RT_TRACE(pDM_Odm,PHYDM_COMP_ADAPTIVITY, ODM_DBG_LOUD, ("Adaptivity skip 5G domain code : %d \n", pDM_Odm->odm_Regulation5G));
				return FALSE;
			}

			else if((*pDM_Odm->pBandType == ODM_BAND_2_4G) &&
				!(pDM_Odm->odm_Regulation2_4G == REGULATION_ETSI || pDM_Odm->odm_Regulation2_4G == REGULATION_WW))
			{
				ODM_RT_TRACE(pDM_Odm,PHYDM_COMP_ADAPTIVITY, ODM_DBG_LOUD, ("Adaptivity skip 2.4G domain code : %d \n", pDM_Odm->odm_Regulation2_4G));
				return FALSE;

			}
			else if ((*pDM_Odm->pBandType != ODM_BAND_2_4G) && (*pDM_Odm->pBandType != ODM_BAND_5G))
			{
				ODM_RT_TRACE(pDM_Odm,PHYDM_COMP_ADAPTIVITY, ODM_DBG_LOUD, ("Adaptivity neither 2G nor 5G band, return\n"));
				return FALSE;
			}
		}
		else
		{
			if ((*pDM_Odm->pBandType == ODM_BAND_5G) &&
				!(pDM_Odm->odm_Regulation5G == REGULATION_ETSI || pDM_Odm->odm_Regulation5G == REGULATION_WW))
			{
				ODM_RT_TRACE(pDM_Odm,PHYDM_COMP_ADAPTIVITY, ODM_DBG_LOUD, ("CarrierSense skip 5G domain code : %d\n", pDM_Odm->odm_Regulation5G));
				return FALSE;
			}

			else if((*pDM_Odm->pBandType == ODM_BAND_2_4G) &&
				!(pDM_Odm->odm_Regulation2_4G == REGULATION_ETSI || pDM_Odm->odm_Regulation2_4G == REGULATION_WW))
			{
				ODM_RT_TRACE(pDM_Odm,PHYDM_COMP_ADAPTIVITY, ODM_DBG_LOUD, ("CarrierSense skip 2.4G domain code : %d\n", pDM_Odm->odm_Regulation2_4G));
				return FALSE;

			}
			else if ((*pDM_Odm->pBandType != ODM_BAND_2_4G) && (*pDM_Odm->pBandType != ODM_BAND_5G))
			{
				ODM_RT_TRACE(pDM_Odm,PHYDM_COMP_ADAPTIVITY, ODM_DBG_LOUD, ("CarrierSense neither 2G nor 5G band, return\n"));
				return FALSE;
			}
		}
	}
#endif


	ODM_RT_TRACE(pDM_Odm,PHYDM_COMP_ADAPTIVITY, ODM_DBG_LOUD, ("odm_Adaptivity() =====> \n"));
	ODM_RT_TRACE(pDM_Odm,PHYDM_COMP_ADAPTIVITY, ODM_DBG_LOUD, ("ForceEDCCA=%d, IGI_Base=0x%x, TH_L2H_ini = %d, TH_EDCCA_HL_diff = %d, AdapEn_RSSI = %d\n",
		pDM_Odm->ForceEDCCA, pDM_Odm->IGI_Base, pDM_Odm->TH_L2H_ini, pDM_Odm->TH_EDCCA_HL_diff, pDM_Odm->AdapEn_RSSI));

	if(pDM_Odm->SupportICType & ODM_IC_11AC_SERIES)
		ODM_SetBBReg(pDM_Odm, 0x800, BIT10, 0); //ADC_mask enable

	if(*pDM_Odm->pBandWidth == ODM_BW20M) //CHANNEL_WIDTH_20
		IGI_target = pDM_Odm->IGI_Base;
	else if(*pDM_Odm->pBandWidth == ODM_BW40M)
		IGI_target = pDM_Odm->IGI_Base + 2;
	else if(*pDM_Odm->pBandWidth == ODM_BW80M)
		IGI_target = pDM_Odm->IGI_Base + 2;
	else
		IGI_target = pDM_Odm->IGI_Base;
	pDM_Odm->IGI_target = (u1Byte) IGI_target;

	if(*pDM_Odm->pChannel >= 149)		// Band4 -> for AP : mode2, for sd4 and sd7 : turnoff adaptivity
	{
#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
		if(pDM_Odm->bLinked)
		{
		Diff = IGI_target -(s1Byte)IGI;
		L2H_nolink_Band4 = pDM_Odm->TH_L2H_ini_mode2 + Diff;
		if(L2H_nolink_Band4 > 10)
			L2H_nolink_Band4 = 10;
		H2L_nolink_Band4 = L2H_nolink_Band4 - pDM_Odm->TH_EDCCA_HL_diff_mode2;
		}
#endif
		Phydm_SetEDCCAThreshold(pDM_Odm, H2L_nolink_Band4, L2H_nolink_Band4);
		return FALSE;
	}

	if(!pDM_Odm->ForceEDCCA)
	{
		if(pDM_Odm->RSSI_Min > pDM_Odm->AdapEn_RSSI)
			EDCCA_State = 1;
		else if(pDM_Odm->RSSI_Min < (pDM_Odm->AdapEn_RSSI - 5))
			EDCCA_State = 0;
	}
	else
		EDCCA_State = 1;

	if(pDM_Odm->Carrier_Sense_enable == FALSE && pDM_Odm->NHM_enable == TRUE)
		Phydm_NHMBB(pDM_Odm);

	ODM_RT_TRACE(pDM_Odm,PHYDM_COMP_ADAPTIVITY, ODM_DBG_LOUD, ("BandWidth=%s, IGI_target=0x%x, EDCCA_State=%d, EDCCA_enable_state = %d\n",
		(*pDM_Odm->pBandWidth==ODM_BW80M)?"80M":((*pDM_Odm->pBandWidth==ODM_BW40M)?"40M":"20M"), IGI_target, EDCCA_State, pDM_Odm->EDCCA_enable_state));
	ODM_RT_TRACE(pDM_Odm,PHYDM_COMP_ADAPTIVITY, ODM_DBG_LOUD, ("RSSI_min = %d, AdapIGIUpper= 0x %x\n", pDM_Odm->RSSI_Min, pDM_Odm->Adaptivity_IGI_upper));


	if(EDCCA_State == 1)
	{
		Diff = IGI_target -(s1Byte)IGI;
		TH_L2H_dmc = pDM_Odm->TH_L2H_ini + Diff;
		if(TH_L2H_dmc > 10)
			TH_L2H_dmc = 10;

		TH_H2L_dmc = TH_L2H_dmc - pDM_Odm->TH_EDCCA_HL_diff;

		//replace lower bound to prevent EDCCA always equal 1
			if(TH_H2L_dmc < pDM_Odm->H2L_lb)
				TH_H2L_dmc = pDM_Odm->H2L_lb;
			if(TH_L2H_dmc < pDM_Odm->L2H_lb)
				TH_L2H_dmc = pDM_Odm->L2H_lb;
	}
	else
	{
		TH_L2H_dmc = 0x7f;
		TH_H2L_dmc = 0x7f;
	}
	ODM_RT_TRACE(pDM_Odm,PHYDM_COMP_ADAPTIVITY, ODM_DBG_LOUD, ("IGI=0x%x, TH_L2H_dmc = %d, TH_H2L_dmc = %d, adaptivity_flg = %d, bAdaOn = %d, DynamicLinkAdaptivity = %d, NHM_enable = %d\n",
		IGI, TH_L2H_dmc, TH_H2L_dmc, pDM_Odm->adaptivity_flag, pDM_Odm->bAdaOn, pDM_Odm->DynamicLinkAdaptivity, pDM_Odm->NHM_enable));

	Phydm_SetEDCCAThreshold(pDM_Odm, TH_H2L_dmc, TH_L2H_dmc);
	return TRUE;
}


#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
VOID
Phydm_EnableEDCCA(
	IN		PVOID					pDM_VOID
)
{

	// This should be moved out of OUTSRC
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	PADAPTER		pAdapter	= pDM_Odm->Adapter;
	// Enable EDCCA. The value is suggested by SD3 Wilson.

	//
	// Revised for ASUS 11b/g performance issues, suggested by BB Neil, 2012.04.13.
	//
	if((pDM_Odm->SupportICType == ODM_RTL8723A)&&(IS_WIRELESS_MODE_G(pAdapter)))
	{
		//PlatformEFIOWrite1Byte(Adapter, rOFDM0_ECCAThreshold, 0x00);
		ODM_Write1Byte(pDM_Odm,rOFDM0_ECCAThreshold,0x00);
		ODM_Write1Byte(pDM_Odm,rOFDM0_ECCAThreshold+2,0xFD);

	}
	else
	{
		//PlatformEFIOWrite1Byte(Adapter, rOFDM0_ECCAThreshold, 0x03);
		ODM_Write1Byte(pDM_Odm,rOFDM0_ECCAThreshold,0x03);
		ODM_Write1Byte(pDM_Odm,rOFDM0_ECCAThreshold+2,0x00);
	}

	//PlatformEFIOWrite1Byte(Adapter, rOFDM0_ECCAThreshold+2, 0x00);
}

VOID
Phydm_DisableEDCCA(
	IN		PVOID					pDM_VOID
)
{
	// Disable EDCCA..
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	ODM_Write1Byte(pDM_Odm, rOFDM0_ECCAThreshold, 0x7f);
	ODM_Write1Byte(pDM_Odm, rOFDM0_ECCAThreshold+2, 0x7f);
}

//
// Description: According to initial gain value to determine to enable or disable EDCCA.
//
// Suggested by SD3 Wilson. Added by tynli. 2011.11.25.
//
VOID
Phydm_DynamicEDCCA(
	IN		PVOID					pDM_VOID
)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	PADAPTER		pAdapter	= pDM_Odm->Adapter;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);
	u1Byte			RegC50, RegC58;
	BOOLEAN			bEDCCAenable = FALSE;

#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN))
	BOOLEAN			bFwCurrentInPSMode=FALSE;

	pAdapter->HalFunc.GetHwRegHandler(pAdapter, HW_VAR_FW_PSMODE_STATUS, (pu1Byte)(&bFwCurrentInPSMode));

	// Disable EDCCA mode while under LPS mode, added by Roger, 2012.09.14.
	if(bFwCurrentInPSMode)
		return;
#endif
	//
	// 2013/11/14 Ken According to BB team Jame's suggestion, we need to disable soft AP mode EDCCA.
	// 2014/01/08 MH For Miracst AP mode test. We need to disable EDCCA. Otherwise, we may stop
	// to send beacon in noisy environment or platform.
	//
	if(ACTING_AS_AP(pAdapter) || ACTING_AS_AP(GetFirstAPAdapter(pAdapter)))
	//if(ACTING_AS_AP(pAdapter))
	{
		ODM_RT_TRACE(pDM_Odm,PHYDM_COMP_ADAPTIVITY, ODM_DBG_LOUD, ("At least One Port as AP disable EDCCA\n"));
		Phydm_DisableEDCCA(pDM_Odm);
		if(pHalData->bPreEdccaEnable)
			Phydm_DisableEDCCA(pDM_Odm);
		pHalData->bPreEdccaEnable = FALSE;
		return;
	}

	RegC50 = (u1Byte)ODM_GetBBReg(pDM_Odm, rOFDM0_XAAGCCore1, bMaskByte0);
	RegC58 = (u1Byte)ODM_GetBBReg(pDM_Odm, rOFDM0_XBAGCCore1, bMaskByte0);


 	if((RegC50 > 0x28 && RegC58 > 0x28) ||
  		((pDM_Odm->SupportICType == ODM_RTL8723A && IS_WIRELESS_MODE_G(pAdapter) && RegC50>0x26)) ||
  		(pDM_Odm->SupportICType == ODM_RTL8188E && RegC50 > 0x28))
	{
		if(!pHalData->bPreEdccaEnable)
		{
			Phydm_EnableEDCCA(pDM_Odm);
			pHalData->bPreEdccaEnable = TRUE;
		}

	}
	else if((RegC50 < 0x25 && RegC58 < 0x25) || (pDM_Odm->SupportICType == ODM_RTL8188E && RegC50 < 0x25))
	{
		if(pHalData->bPreEdccaEnable)
		{
			Phydm_DisableEDCCA(pDM_Odm);
			pHalData->bPreEdccaEnable = FALSE;
		}
	}
}

#endif
