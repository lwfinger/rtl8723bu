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
#define _RTW_MP_C_

#include <drv_types.h>
#include "../hal/odm_precomp.h"
#if defined(CONFIG_RTL8723A) || defined(CONFIG_RTL8723B)
#include <rtw_bt_mp.h>
#endif


#ifdef CONFIG_MP_INCLUDED

u32 read_macreg(_adapter *padapter, u32 addr, u32 sz)
{
	u32 val = 0;

	switch(sz)
	{
		case 1:
			val = rtw_read8(padapter, addr);
			break;
		case 2:
			val = rtw_read16(padapter, addr);
			break;
		case 4:
			val = rtw_read32(padapter, addr);
			break;
		default:
			val = 0xffffffff;
			break;
	}

	return val;

}

void write_macreg(_adapter *padapter, u32 addr, u32 val, u32 sz)
{
	switch(sz)
	{
		case 1:
			rtw_write8(padapter, addr, (u8)val);
			break;
		case 2:
			rtw_write16(padapter, addr, (u16)val);
			break;
		case 4:
			rtw_write32(padapter, addr, val);
			break;
		default:
			break;
	}

}

u32 read_bbreg(_adapter *padapter, u32 addr, u32 bitmask)
{
	return rtw_hal_read_bbreg(padapter, addr, bitmask);
}

void write_bbreg(_adapter *padapter, u32 addr, u32 bitmask, u32 val)
{
	rtw_hal_write_bbreg(padapter, addr, bitmask, val);
}

u32 _read_rfreg(PADAPTER padapter, u8 rfpath, u32 addr, u32 bitmask)
{
	return rtw_hal_read_rfreg(padapter, rfpath, addr, bitmask);
}

void _write_rfreg(PADAPTER padapter, u8 rfpath, u32 addr, u32 bitmask, u32 val)
{
	rtw_hal_write_rfreg(padapter, rfpath, addr, bitmask, val);
}

u32 read_rfreg(PADAPTER padapter, u8 rfpath, u32 addr)
{
	return _read_rfreg(padapter, rfpath, addr, bRFRegOffsetMask);
}

void write_rfreg(PADAPTER padapter, u8 rfpath, u32 addr, u32 val)
{
	_write_rfreg(padapter, rfpath, addr, bRFRegOffsetMask, val);
}

static void _init_mp_priv_(struct mp_priv *pmp_priv)
{
	WLAN_BSSID_EX *pnetwork;

	_rtw_memset(pmp_priv, 0, sizeof(struct mp_priv));

	pmp_priv->mode = MP_OFF;

	pmp_priv->channel = 1;
	pmp_priv->bandwidth = CHANNEL_WIDTH_20;
	pmp_priv->prime_channel_offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
	pmp_priv->rateidx = MPT_RATE_1M;
	pmp_priv->txpoweridx = 0x2A;

	pmp_priv->antenna_tx = ANTENNA_A;
	pmp_priv->antenna_rx = ANTENNA_AB;

	pmp_priv->check_mp_pkt = 0;

	pmp_priv->tx_pktcount = 0;

	pmp_priv->rx_bssidpktcount=0;
	pmp_priv->rx_pktcount = 0;
	pmp_priv->rx_crcerrpktcount = 0;

	pmp_priv->network_macaddr[0] = 0x00;
	pmp_priv->network_macaddr[1] = 0xE0;
	pmp_priv->network_macaddr[2] = 0x4C;
	pmp_priv->network_macaddr[3] = 0x87;
	pmp_priv->network_macaddr[4] = 0x66;
	pmp_priv->network_macaddr[5] = 0x55;

	pmp_priv->bSetRxBssid = _FALSE;

	pnetwork = &pmp_priv->mp_network.network;
	_rtw_memcpy(pnetwork->MacAddress, pmp_priv->network_macaddr, ETH_ALEN);

	pnetwork->Ssid.SsidLength = 8;
	_rtw_memcpy(pnetwork->Ssid.Ssid, "mp_871x", pnetwork->Ssid.SsidLength);
}

static int init_mp_priv_by_os(struct mp_priv *pmp_priv)
{
	int i, res;
	struct mp_xmit_frame *pmp_xmitframe;

	if (pmp_priv == NULL) return _FAIL;

	_rtw_init_queue(&pmp_priv->free_mp_xmitqueue);

	pmp_priv->pallocated_mp_xmitframe_buf = NULL;
	pmp_priv->pallocated_mp_xmitframe_buf = rtw_zmalloc(NR_MP_XMITFRAME * sizeof(struct mp_xmit_frame) + 4);
	if (pmp_priv->pallocated_mp_xmitframe_buf == NULL) {
		res = _FAIL;
		goto _exit_init_mp_priv;
	}

	pmp_priv->pmp_xmtframe_buf = pmp_priv->pallocated_mp_xmitframe_buf + 4 - ((SIZE_PTR) (pmp_priv->pallocated_mp_xmitframe_buf) & 3);

	pmp_xmitframe = (struct mp_xmit_frame*)pmp_priv->pmp_xmtframe_buf;

	for (i = 0; i < NR_MP_XMITFRAME; i++)
	{
		_rtw_init_listhead(&pmp_xmitframe->list);
		rtw_list_insert_tail(&pmp_xmitframe->list, &pmp_priv->free_mp_xmitqueue.queue);

		pmp_xmitframe->pkt = NULL;
		pmp_xmitframe->frame_tag = MP_FRAMETAG;
		pmp_xmitframe->padapter = pmp_priv->papdater;

		pmp_xmitframe++;
	}

	pmp_priv->free_mp_xmitframe_cnt = NR_MP_XMITFRAME;

	res = _SUCCESS;

_exit_init_mp_priv:

	return res;
}

static void mp_init_xmit_attrib(struct mp_tx *pmptx, PADAPTER padapter)
{
	struct pkt_attrib *pattrib;

	// init xmitframe attribute
	pattrib = &pmptx->attrib;
	_rtw_memset(pattrib, 0, sizeof(struct pkt_attrib));
	_rtw_memset(pmptx->desc, 0, TXDESC_SIZE);

	pattrib->ether_type = 0x8712;
	//_rtw_memcpy(pattrib->src, padapter->eeprompriv.mac_addr, ETH_ALEN);
//	_rtw_memcpy(pattrib->ta, pattrib->src, ETH_ALEN);
	_rtw_memset(pattrib->dst, 0xFF, ETH_ALEN);

//	pattrib->dhcp_pkt = 0;
//	pattrib->pktlen = 0;
	pattrib->ack_policy = 0;
//	pattrib->pkt_hdrlen = ETH_HLEN;
	pattrib->hdrlen = WLAN_HDR_A3_LEN;
	pattrib->subtype = WIFI_DATA;
	pattrib->priority = 0;
	pattrib->qsel = pattrib->priority;
//	do_queue_select(padapter, pattrib);
	pattrib->nr_frags = 1;
	pattrib->encrypt = 0;
	pattrib->bswenc = _FALSE;
	pattrib->qos_en = _FALSE;
}

s32 init_mp_priv(PADAPTER padapter)
{
	struct mp_priv *pmppriv = &padapter->mppriv;

	_init_mp_priv_(pmppriv);
	pmppriv->papdater = padapter;
	pmppriv->mp_dm =0;
	pmppriv->tx.stop = 1;
	pmppriv->bSetTxPower=0;		//for  manually set tx power
	pmppriv->bTxBufCkFail=_FALSE;
	pmppriv->pktInterval=0;

	mp_init_xmit_attrib(&pmppriv->tx, padapter);

	switch (padapter->registrypriv.rf_config) {
		case RF_1T1R:
			pmppriv->antenna_tx = ANTENNA_A;
			pmppriv->antenna_rx = ANTENNA_A;
			break;
		case RF_1T2R:
		default:
			pmppriv->antenna_tx = ANTENNA_A;
			pmppriv->antenna_rx = ANTENNA_AB;
			break;
		case RF_2T2R:
		case RF_2T2R_GREEN:
			pmppriv->antenna_tx = ANTENNA_AB;
			pmppriv->antenna_rx = ANTENNA_AB;
			break;
		case RF_2T4R:
			pmppriv->antenna_tx = ANTENNA_AB;
			pmppriv->antenna_rx = ANTENNA_ABCD;
			break;
	}

	return _SUCCESS;
}

void free_mp_priv(struct mp_priv *pmp_priv)
{
	if (pmp_priv->pallocated_mp_xmitframe_buf) {
		rtw_mfree(pmp_priv->pallocated_mp_xmitframe_buf, 0);
		pmp_priv->pallocated_mp_xmitframe_buf = NULL;
	}
	pmp_priv->pmp_xmtframe_buf = NULL;
}


static VOID PHY_IQCalibrate_default(
	IN	PADAPTER	pAdapter,
	IN	BOOLEAN		bReCovery
	)
{
	DBG_871X("%s\n", __func__);
}

static VOID PHY_LCCalibrate_default(
	IN	PADAPTER	pAdapter
	)
{
	DBG_871X("%s\n", __func__);
}

static VOID PHY_SetRFPathSwitch_default(
	IN	PADAPTER	pAdapter,
	IN	BOOLEAN		bMain
	)
{
	DBG_871X("%s\n", __func__);
}


void mpt_InitHWConfig(PADAPTER Adapter)
{
	if (IS_HARDWARE_TYPE_8723B(Adapter))
	{
		// TODO: <20130114, Kordan> The following setting is only for DPDT and Fixed board type.
		// TODO:  A better solution is configure it according EFUSE during the run-time.

		PHY_SetMacReg(Adapter, 0x64, BIT20, 0x0);		   //0x66[4]=0
		PHY_SetMacReg(Adapter, 0x64, BIT24, 0x0);		   //0x66[8]=0
		PHY_SetMacReg(Adapter, 0x40, BIT4, 0x0);		   //0x40[4]=0
		PHY_SetMacReg(Adapter, 0x40, BIT3, 0x1);		   //0x40[3]=1
		PHY_SetMacReg(Adapter, 0x4C, BIT24, 0x1);		   //0x4C[24:23]=10
		PHY_SetMacReg(Adapter, 0x4C, BIT23, 0x0);		   //0x4C[24:23]=10
		PHY_SetBBReg(Adapter, 0x944, BIT1|BIT0, 0x3);	  //0x944[1:0]=11
		PHY_SetBBReg(Adapter, 0x930, bMaskByte0, 0x77);   //0x930[7:0]=77
		PHY_SetMacReg(Adapter, 0x38, BIT11, 0x1);		   //0x38[11]=1

		// TODO: <20130206, Kordan> The default setting is wrong, hard-coded here.
		PHY_SetMacReg(Adapter, 0x778, 0x3, 0x3);					// Turn off hardware PTA control (Asked by Scott)
		PHY_SetMacReg(Adapter, 0x64, bMaskDWord, 0x36000000);	 //Fix BT S0/S1
		PHY_SetMacReg(Adapter, 0x948, bMaskDWord, 0x0);			   //Fix BT can't Tx

		// <20130522, Kordan> Turn off equalizer to improve Rx sensitivity. (Asked by EEChou)
		PHY_SetBBReg(Adapter, 0xA00, BIT8, 0x0);			//0xA01[0] = 0
	}
}

#ifdef CONFIG_RTL8723B
static void PHY_IQCalibrate(PADAPTER padapter, u8 bReCovery)
{
	PHAL_DATA_TYPE pHalData;
	u8 b2ant;	//false:1ant, true:2-ant
	u8 RF_Path;	//0:S1, 1:S0


	pHalData = GET_HAL_DATA(padapter);
	b2ant = pHalData->EEPROMBluetoothAntNum==Ant_x2?_TRUE:_FALSE;

	PHY_IQCalibrate_8723B(padapter, bReCovery, _FALSE, b2ant, pHalData->ant_path);
}

#define PHY_LCCalibrate(a)	PHY_LCCalibrate_8723B(&(GET_HAL_DATA(a)->odmpriv))
#define PHY_SetRFPathSwitch(a,b)	PHY_SetRFPathSwitch_8723B(a,b)
#endif

s32
MPT_InitializeAdapter(
	IN	PADAPTER			pAdapter,
	IN	u8				Channel
	)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);
	s32		rtStatus = _SUCCESS;
	PMPT_CONTEXT	pMptCtx = &pAdapter->mppriv.MptCtx;
	u32		ledsetting;
	struct mlme_priv *pmlmepriv = &pAdapter->mlmepriv;

	pMptCtx->bMptDrvUnload = _FALSE;
	pMptCtx->bMassProdTest = _FALSE;
	pMptCtx->bMptIndexEven = _TRUE;	//default gain index is -6.0db
	pMptCtx->h2cReqNum = 0x0;
	//init for BT MP
#if defined(CONFIG_RTL8723A) || defined(CONFIG_RTL8723B) || defined(CONFIG_RTL8821A)
	pMptCtx->bMPh2c_timeout = _FALSE;
	pMptCtx->MptH2cRspEvent = _FALSE;
	pMptCtx->MptBtC2hEvent = _FALSE;
	_rtw_init_sema(&pMptCtx->MPh2c_Sema, 0);
	_init_timer( &pMptCtx->MPh2c_timeout_timer, pAdapter->pnetdev, MPh2c_timeout_handle, pAdapter );
#endif

#ifdef CONFIG_RTL8723A
	rtl8723a_InitAntenna_Selection(pAdapter);
#endif //CONFIG_RTL8723A
#ifdef CONFIG_RTL8723B
	rtl8723b_InitAntenna_Selection(pAdapter);
	if (IS_HARDWARE_TYPE_8723B(pAdapter))
	{
		mpt_InitHWConfig(pAdapter);
		// <20130522, Kordan> Turn off equalizer to improve Rx sensitivity. (Asked by EEChou)
		PHY_SetBBReg(pAdapter, 0xA00, BIT8, 0x0);		//0xA01[0] = 0
		PHY_SetRFPathSwitch(pAdapter, 1/*pHalData->bDefaultAntenna*/); //default use Main
		//<20130522, Kordan> 0x51 and 0x71 should be set immediately after path switched, or they might be overwritten.
		if ((pHalData->PackageType == PACKAGE_TFBGA79) || (pHalData->PackageType == PACKAGE_TFBGA90))
					PHY_SetRFReg(pAdapter, ODM_RF_PATH_A, 0x51, bRFRegOffsetMask, 0x6B10E);
		else
					PHY_SetRFReg(pAdapter, ODM_RF_PATH_A, 0x51, bRFRegOffsetMask, 0x6B04E);
	}
#endif

	pMptCtx->bMptWorkItemInProgress = _FALSE;
	pMptCtx->CurrMptAct = NULL;
	pMptCtx->MptRfPath = ODM_RF_PATH_A;
	//-------------------------------------------------------------------------

#if 1
	// Don't accept any packets
	rtw_write32(pAdapter, REG_RCR, 0);
#else
	// Accept CRC error and destination address
	//pHalData->ReceiveConfig |= (RCR_ACRC32|RCR_AAP);
	//rtw_write32(pAdapter, REG_RCR, pHalData->ReceiveConfig);
	rtw_write32(pAdapter, REG_RCR, 0x70000101);
#endif

	//ledsetting = rtw_read32(pAdapter, REG_LEDCFG0);
	//rtw_write32(pAdapter, REG_LEDCFG0, ledsetting & ~LED0DIS);

	if(IS_HARDWARE_TYPE_8192DU(pAdapter))
	{
		rtw_write32(pAdapter, REG_LEDCFG0, 0x8888);
	}
	else
	{
		//rtw_write32(pAdapter, REG_LEDCFG0, 0x08080);
		ledsetting = rtw_read32(pAdapter, REG_LEDCFG0);
	}

	PHY_LCCalibrate(pAdapter);
	PHY_IQCalibrate(pAdapter, _FALSE);
	//dm_CheckTXPowerTracking(&pHalData->odmpriv);	//trigger thermal meter

	PHY_SetRFPathSwitch(pAdapter, 1/*pHalData->bDefaultAntenna*/); //default use Main

	pMptCtx->backup0xc50 = (u1Byte)PHY_QueryBBReg(pAdapter, rOFDM0_XAAGCCore1, bMaskByte0);
	pMptCtx->backup0xc58 = (u1Byte)PHY_QueryBBReg(pAdapter, rOFDM0_XBAGCCore1, bMaskByte0);
	pMptCtx->backup0xc30 = (u1Byte)PHY_QueryBBReg(pAdapter, rOFDM0_RxDetector1, bMaskByte0);

	//set ant to wifi side in mp mode
	rtw_write16(pAdapter, 0x870, 0x300);
	rtw_write16(pAdapter, 0x860, 0x110);

	return	rtStatus;
}

/*-----------------------------------------------------------------------------
 * Function:	MPT_DeInitAdapter()
 *
 * Overview:	Extra DeInitialization for Mass Production Test.
 *
 * Input:		PADAPTER	pAdapter
 *
 * Output:		NONE
 *
 * Return:		NONE
 *
 * Revised History:
 *	When		Who		Remark
 *	05/08/2007	MHC		Create Version 0.
 *	05/18/2007	MHC		Add normal driver MPHalt code.
 *
 *---------------------------------------------------------------------------*/
VOID
MPT_DeInitAdapter(
	IN	PADAPTER	pAdapter
	)
{
	PMPT_CONTEXT		pMptCtx = &pAdapter->mppriv.MptCtx;

	pMptCtx->bMptDrvUnload = _TRUE;
	#if defined(CONFIG_RTL8723A) || defined(CONFIG_RTL8723B)
	_rtw_free_sema(&(pMptCtx->MPh2c_Sema));
	_cancel_timer_ex( &pMptCtx->MPh2c_timeout_timer);
	#endif
	#if	defined(CONFIG_RTL8723B)
	PHY_SetBBReg(pAdapter,0xA01, BIT0, 1); ///suggestion  by jerry for MP Rx.
	#endif
}

static u8 mpt_ProStartTest(PADAPTER padapter)
{
	PMPT_CONTEXT pMptCtx = &padapter->mppriv.MptCtx;

	pMptCtx->bMassProdTest = _TRUE;
	pMptCtx->bStartContTx = _FALSE;
	pMptCtx->bCckContTx = _FALSE;
	pMptCtx->bOfdmContTx = _FALSE;
	pMptCtx->bSingleCarrier = _FALSE;
	pMptCtx->bCarrierSuppression = _FALSE;
	pMptCtx->bSingleTone = _FALSE;

	return _SUCCESS;
}

/*
 * General use
 */
s32 SetPowerTracking(PADAPTER padapter, u8 enable)
{

	Hal_SetPowerTracking( padapter, enable );
	return 0;
}

void GetPowerTracking(PADAPTER padapter, u8 *enable)
{
	Hal_GetPowerTracking( padapter, enable );
}

static void disable_dm(PADAPTER padapter)
{
#ifndef CONFIG_RTL8723A
	u8 v8;
#endif
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);
	struct dm_priv	*pdmpriv = &pHalData->dmpriv;


	//3 1. disable firmware dynamic mechanism
	// disable Power Training, Rate Adaptive
#ifdef CONFIG_RTL8723A
	SetBcnCtrlReg(padapter, 0, EN_BCN_FUNCTION);
#else
	v8 = rtw_read8(padapter, REG_BCN_CTRL);
	v8 &= ~EN_BCN_FUNCTION;
	rtw_write8(padapter, REG_BCN_CTRL, v8);
#endif

	//3 2. disable driver dynamic mechanism
	// disable Dynamic Initial Gain
	// disable High Power
	// disable Power Tracking
	Switch_DM_Func(padapter, DYNAMIC_FUNC_DISABLE, _FALSE);

	// enable APK, LCK and IQK but disable power tracking
	pdmpriv->TxPowerTrackControl = _FALSE;
	Switch_DM_Func(padapter, DYNAMIC_RF_CALIBRATION, _TRUE);
}


void MPT_PwrCtlDM(PADAPTER padapter, u32 bstart)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);
	struct dm_priv	*pdmpriv = &pHalData->dmpriv;
	PDM_ODM_T		pDM_Odm = &pHalData->odmpriv;

	if (bstart==1){
		DBG_871X("in MPT_PwrCtlDM start \n");
		Switch_DM_Func(padapter, DYNAMIC_RF_TX_PWR_TRACK, _TRUE);
		pdmpriv->InitODMFlag |= ODM_RF_TX_PWR_TRACK ;
		pdmpriv->InitODMFlag |= ODM_RF_CALIBRATION ;
		pdmpriv->TxPowerTrackControl = _TRUE;
		pDM_Odm->RFCalibrateInfo.TxPowerTrackControl = _TRUE;
		padapter->mppriv.mp_dm =1;
		odm_TXPowerTrackingInit(pDM_Odm);
		ODM_ClearTxPowerTrackingState(pDM_Odm);

	}else{
		DBG_871X("in MPT_PwrCtlDM stop \n");
		disable_dm(padapter);
		pdmpriv->InitODMFlag = 0 ;
		pdmpriv->TxPowerTrackControl = _FALSE;
		pDM_Odm->RFCalibrateInfo.TxPowerTrackControl = _FALSE;
		padapter->mppriv.mp_dm = 0;
		{
			TXPWRTRACK_CFG	c;
			u1Byte	chnl =0 ;

			ConfigureTxpowerTrack(pDM_Odm, &c);
			ODM_ClearTxPowerTrackingState(pDM_Odm);
			(*c.ODM_TxPwrTrackSetPwr)(pDM_Odm, BBSWING, ODM_RF_PATH_A, chnl);
			(*c.ODM_TxPwrTrackSetPwr)(pDM_Odm, BBSWING, ODM_RF_PATH_B, chnl);
		}
	}

}


u32 mp_join(PADAPTER padapter,u8 mode)
{
	WLAN_BSSID_EX bssid;
	struct sta_info *psta;
	u32 length;
	u8 val8;
	_irqL irqL;
	s32 res = _SUCCESS;

	struct mp_priv *pmppriv = &padapter->mppriv;
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct wlan_network *tgt_network = &pmlmepriv->cur_network;

	// 1. initialize a new WLAN_BSSID_EX
	_rtw_memset(&bssid, 0, sizeof(WLAN_BSSID_EX));
	DBG_8192C("%s ,pmppriv->network_macaddr=%x %x %x %x %x %x \n",__func__,
				pmppriv->network_macaddr[0],pmppriv->network_macaddr[1],pmppriv->network_macaddr[2],pmppriv->network_macaddr[3],pmppriv->network_macaddr[4],pmppriv->network_macaddr[5]);
	_rtw_memcpy(bssid.MacAddress, pmppriv->network_macaddr, ETH_ALEN);

	if( mode==WIFI_FW_ADHOC_STATE ){
		bssid.Ssid.SsidLength = strlen("mp_pseudo_adhoc");
		_rtw_memcpy(bssid.Ssid.Ssid, (u8*)"mp_pseudo_adhoc", bssid.Ssid.SsidLength);
		bssid.InfrastructureMode = Ndis802_11IBSS;
		bssid.NetworkTypeInUse = Ndis802_11DS;
		bssid.IELength = 0;

	}else if(mode==WIFI_FW_STATION_STATE){
		bssid.Ssid.SsidLength = strlen("mp_pseudo_STATION");
		_rtw_memcpy(bssid.Ssid.Ssid, (u8*)"mp_pseudo_STATION", bssid.Ssid.SsidLength);
		bssid.InfrastructureMode = Ndis802_11Infrastructure;
		bssid.NetworkTypeInUse = Ndis802_11DS;
		bssid.IELength = 0;
	}


	length = get_WLAN_BSSID_EX_sz(&bssid);
	if (length % 4)
		bssid.Length = ((length >> 2) + 1) << 2; //round up to multiple of 4 bytes.
	else
		bssid.Length = length;

	_enter_critical_bh(&pmlmepriv->lock, &irqL);

	if (check_fwstate(pmlmepriv, WIFI_MP_STATE) == _TRUE)
		goto end_of_mp_start_test;

	//init mp_start_test status
	if (check_fwstate(pmlmepriv, _FW_LINKED) == _TRUE) {
		rtw_disassoc_cmd(padapter, 500, _TRUE);
		rtw_indicate_disconnect(padapter);
		rtw_free_assoc_resources(padapter, 1);
	}
	pmppriv->prev_fw_state = get_fwstate(pmlmepriv);
	pmlmepriv->fw_state = WIFI_MP_STATE;

	set_fwstate(pmlmepriv, _FW_UNDER_LINKING);

	//3 2. create a new psta for mp driver
	//clear psta in the cur_network, if any
	psta = rtw_get_stainfo(&padapter->stapriv, tgt_network->network.MacAddress);
	if (psta) rtw_free_stainfo(padapter, psta);

	psta = rtw_alloc_stainfo(&padapter->stapriv, bssid.MacAddress);
	if (psta == NULL) {
		RT_TRACE(_module_mp_, _drv_err_, ("mp_start_test: Can't alloc sta_info!\n"));
		pmlmepriv->fw_state = pmppriv->prev_fw_state;
		res = _FAIL;
		goto end_of_mp_start_test;
	}
	set_fwstate(pmlmepriv,WIFI_ADHOC_MASTER_STATE);
	//3 3. join psudo AdHoc
	tgt_network->join_res = 1;
	tgt_network->aid = psta->aid = 1;
	_rtw_memcpy(&tgt_network->network, &bssid, length);

	rtw_indicate_connect(padapter);
	_clr_fwstate_(pmlmepriv, _FW_UNDER_LINKING);
	set_fwstate(pmlmepriv,_FW_LINKED);

end_of_mp_start_test:

	_exit_critical_bh(&pmlmepriv->lock, &irqL);

	if(1) //(res == _SUCCESS)
	{
		// set MSR to WIFI_FW_ADHOC_STATE
		if( mode==WIFI_FW_ADHOC_STATE ){

			val8 = rtw_read8(padapter, MSR) & 0xFC; // 0x0102
			val8 |= WIFI_FW_ADHOC_STATE;
			rtw_write8(padapter, MSR, val8); // Link in ad hoc network
		}
		else {
			Set_MSR(padapter, WIFI_FW_STATION_STATE);

			DBG_8192C("%s , pmppriv->network_macaddr =%x %x %x %x %x %x\n",__func__,
						pmppriv->network_macaddr[0],pmppriv->network_macaddr[1],pmppriv->network_macaddr[2],pmppriv->network_macaddr[3],pmppriv->network_macaddr[4],pmppriv->network_macaddr[5]);

			rtw_hal_set_hwreg(padapter, HW_VAR_BSSID, pmppriv->network_macaddr);
		}
	}

	return res;
}
//This function initializes the DUT to the MP test mode
s32 mp_start_test(PADAPTER padapter)
{
	struct mp_priv *pmppriv = &padapter->mppriv;
	s32 res = _SUCCESS;

	padapter->registrypriv.mp_mode = 1;

	//3 disable dynamic mechanism
	disable_dm(padapter);
#ifdef CONFIG_RTL8723B
	rtl8723b_InitHalDm(padapter);
#endif

	//3 0. update mp_priv

	if (padapter->registrypriv.rf_config == RF_MAX_TYPE) {
//		switch (phal->rf_type) {
		switch (GET_RF_TYPE(padapter)) {
			case RF_1T1R:
				pmppriv->antenna_tx = ANTENNA_A;
				pmppriv->antenna_rx = ANTENNA_A;
				break;
			case RF_1T2R:
			default:
				pmppriv->antenna_tx = ANTENNA_A;
				pmppriv->antenna_rx = ANTENNA_AB;
				break;
			case RF_2T2R:
			case RF_2T2R_GREEN:
				pmppriv->antenna_tx = ANTENNA_AB;
				pmppriv->antenna_rx = ANTENNA_AB;
				break;
			case RF_2T4R:
				pmppriv->antenna_tx = ANTENNA_AB;
				pmppriv->antenna_rx = ANTENNA_ABCD;
				break;
	}
	}

	mpt_ProStartTest(padapter);

	mp_join(padapter,WIFI_FW_ADHOC_STATE);

	return res;
}
//------------------------------------------------------------------------------
//This function change the DUT from the MP test mode into normal mode
void mp_stop_test(PADAPTER padapter)
{
	struct mp_priv *pmppriv = &padapter->mppriv;
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct wlan_network *tgt_network = &pmlmepriv->cur_network;
	struct sta_info *psta;

	_irqL irqL;

	if(pmppriv->mode==MP_ON)
	{
		pmppriv->bSetTxPower=0;
		_enter_critical_bh(&pmlmepriv->lock, &irqL);
		if (check_fwstate(pmlmepriv, WIFI_MP_STATE) == _FALSE)
			goto end_of_mp_stop_test;

		//3 1. disconnect psudo AdHoc
		rtw_indicate_disconnect(padapter);

		//3 2. clear psta used in mp test mode.
		//rtw_free_assoc_resources(padapter, 1);
		psta = rtw_get_stainfo(&padapter->stapriv, tgt_network->network.MacAddress);
		if (psta) rtw_free_stainfo(padapter, psta);

		//3 3. return to normal state (default:station mode)
		pmlmepriv->fw_state = pmppriv->prev_fw_state; // WIFI_STATION_STATE;

		//flush the cur_network
		_rtw_memset(tgt_network, 0, sizeof(struct wlan_network));

		_clr_fwstate_(pmlmepriv, WIFI_MP_STATE);

end_of_mp_stop_test:

		_exit_critical_bh(&pmlmepriv->lock, &irqL);

#ifdef CONFIG_RTL8723B
		rtl8723b_InitHalDm(padapter);
#endif
	}
}
/*---------------------------hal\rtl8192c\MPT_Phy.c---------------------------*/

/*-----------------------------------------------------------------------------
 * Function:	mpt_SwitchRfSetting
 *
 * Overview:	Change RF Setting when we siwthc channel/rate/BW for MP.
 *
 * Input:       IN	PADAPTER				pAdapter
 *
 * Output:      NONE
 *
 * Return:      NONE
 *
 * Revised History:
 * When			Who		Remark
 * 01/08/2009	MHC		Suggestion from SD3 Willis for 92S series.
 * 01/09/2009	MHC		Add CCK modification for 40MHZ. Suggestion from SD3.
 *
 *---------------------------------------------------------------------------*/
static void mpt_SwitchRfSetting(PADAPTER pAdapter)
{
	Hal_mpt_SwitchRfSetting(pAdapter);
    }

/*---------------------------hal\rtl8192c\MPT_Phy.c---------------------------*/
/*---------------------------hal\rtl8192c\MPT_HelperFunc.c---------------------------*/
static void MPT_CCKTxPowerAdjust(PADAPTER Adapter, BOOLEAN bInCH14)
{
	Hal_MPT_CCKTxPowerAdjust(Adapter,bInCH14);
}

static void MPT_CCKTxPowerAdjustbyIndex(PADAPTER pAdapter, BOOLEAN beven)
{
	Hal_MPT_CCKTxPowerAdjustbyIndex(pAdapter,beven);
	}

/*---------------------------hal\rtl8192c\MPT_HelperFunc.c---------------------------*/

/*
 * SetChannel
 * Description
 *	Use H2C command to change channel,
 *	not only modify rf register, but also other setting need to be done.
 */
void SetChannel(PADAPTER pAdapter)
{
	Hal_SetChannel(pAdapter);
}

/*
 * Notice
 *	Switch bandwitdth may change center frequency(channel)
 */
void SetBandwidth(PADAPTER pAdapter)
{
	Hal_SetBandwidth(pAdapter);

}

static void SetCCKTxPower(PADAPTER pAdapter, u8 *TxPower)
{
	Hal_SetCCKTxPower(pAdapter,TxPower);
}

static void SetOFDMTxPower(PADAPTER pAdapter, u8 *TxPower)
{
	Hal_SetOFDMTxPower(pAdapter,TxPower);
	}


void SetAntenna(PADAPTER pAdapter)
{
	Hal_SetAntenna(pAdapter);
}

void	SetAntennaPathPower(PADAPTER pAdapter)
{
	Hal_SetAntennaPathPower(pAdapter);
}

int SetTxPower(PADAPTER pAdapter)
{
	HAL_DATA_TYPE	*pHalData	= GET_HAL_DATA(pAdapter);
	u1Byte			CurrChannel;
	BOOLEAN			bResult = _TRUE;
	PMPT_CONTEXT	pMptCtx = &(pAdapter->mppriv.MptCtx);
	u1Byte			rf, TxPower[2];

	u8 u1TxPower = pAdapter->mppriv.txpoweridx;
	CurrChannel = pMptCtx->MptChannelToSw;

	if(HAL_IsLegalChannel(pAdapter, CurrChannel) == _FALSE)
	{
		DBG_871X("SetTxPower(): CurrentChannel:%d is not valid\n", CurrChannel);
		return _FALSE;
	}

	TxPower[ODM_RF_PATH_A] = (u1Byte)(u1TxPower&0xff);
	TxPower[ODM_RF_PATH_B] = (u1Byte)((u1TxPower&0xff00)>>8);
	DBG_871X("TxPower(A, B) = (0x%x, 0x%x)\n", TxPower[ODM_RF_PATH_A], TxPower[ODM_RF_PATH_B]);

	for(rf=0; rf<2; rf++)
	{
		if(TxPower[rf] > MAX_TX_PWR_INDEX_N_MODE) {
			DBG_871X("===> SetTxPower: The power index is too large.\n");
			return _FALSE;
		}
		pMptCtx->TxPwrLevel[rf] = TxPower[rf];
	}
	Hal_SetTxPower(pAdapter);
	return _TRUE;
}

void SetTxAGCOffset(PADAPTER pAdapter, u32 ulTxAGCOffset)
{
	u32 TxAGCOffset_B, TxAGCOffset_C, TxAGCOffset_D,tmpAGC;

	TxAGCOffset_B = (ulTxAGCOffset&0x000000ff);
	TxAGCOffset_C = ((ulTxAGCOffset&0x0000ff00)>>8);
	TxAGCOffset_D = ((ulTxAGCOffset&0x00ff0000)>>16);

	tmpAGC = (TxAGCOffset_D<<8 | TxAGCOffset_C<<4 | TxAGCOffset_B);
	write_bbreg(pAdapter, rFPGA0_TxGainStage,
			(bXBTxAGC|bXCTxAGC|bXDTxAGC), tmpAGC);
}

void SetDataRate(PADAPTER pAdapter)
{
	Hal_SetDataRate(pAdapter);
}

void MP_PHY_SetRFPathSwitch(PADAPTER pAdapter ,BOOLEAN bMain)
{

	PHY_SetRFPathSwitch(pAdapter,bMain);

}


s32 SetThermalMeter(PADAPTER pAdapter, u8 target_ther)
{
	return Hal_SetThermalMeter( pAdapter, target_ther);
}

static void TriggerRFThermalMeter(PADAPTER pAdapter)
{
	Hal_TriggerRFThermalMeter(pAdapter);
}

static u8 ReadRFThermalMeter(PADAPTER pAdapter)
{
	return Hal_ReadRFThermalMeter(pAdapter);
}

void GetThermalMeter(PADAPTER pAdapter, u8 *value)
{
	Hal_GetThermalMeter(pAdapter,value);
}

void SetSingleCarrierTx(PADAPTER pAdapter, u8 bStart)
{
	PhySetTxPowerLevel(pAdapter);
	Hal_SetSingleCarrierTx(pAdapter,bStart);
}

void SetSingleToneTx(PADAPTER pAdapter, u8 bStart)
{
	PhySetTxPowerLevel(pAdapter);
	Hal_SetSingleToneTx(pAdapter,bStart);
}

void SetCarrierSuppressionTx(PADAPTER pAdapter, u8 bStart)
{
	PhySetTxPowerLevel(pAdapter);
	Hal_SetCarrierSuppressionTx(pAdapter, bStart);
}

void SetCCKContinuousTx(PADAPTER pAdapter, u8 bStart)
{
	PhySetTxPowerLevel(pAdapter);
	Hal_SetCCKContinuousTx(pAdapter,bStart);
}

void SetOFDMContinuousTx(PADAPTER pAdapter, u8 bStart)
{
	PhySetTxPowerLevel(pAdapter);
	Hal_SetOFDMContinuousTx( pAdapter, bStart);
}/* mpt_StartOfdmContTx */

void SetContinuousTx(PADAPTER pAdapter, u8 bStart)
{
	PhySetTxPowerLevel(pAdapter);
	Hal_SetContinuousTx(pAdapter,bStart);
}


void PhySetTxPowerLevel(PADAPTER pAdapter)
{
	struct mp_priv *pmp_priv = &pAdapter->mppriv;

	if (pmp_priv->bSetTxPower==0) // for NO manually set power index
	{
#if defined(CONFIG_RTL8723B)
		PHY_SetTxPowerLevel8723B(pAdapter,pmp_priv->channel);
#endif
	mpt_ProQueryCalTxPower(pAdapter,pmp_priv->antenna_tx);

	}
}

//------------------------------------------------------------------------------
static void dump_mpframe(PADAPTER padapter, struct xmit_frame *pmpframe)
{
	rtw_hal_mgnt_xmit(padapter, pmpframe);
}

static struct xmit_frame *alloc_mp_xmitframe(struct xmit_priv *pxmitpriv)
{
	struct xmit_frame	*pmpframe;
	struct xmit_buf	*pxmitbuf;

	if ((pmpframe = rtw_alloc_xmitframe(pxmitpriv)) == NULL)
	{
		return NULL;
	}

	if ((pxmitbuf = rtw_alloc_xmitbuf(pxmitpriv)) == NULL)
	{
		rtw_free_xmitframe(pxmitpriv, pmpframe);
		return NULL;
	}

	pmpframe->frame_tag = MP_FRAMETAG;

	pmpframe->pxmitbuf = pxmitbuf;

	pmpframe->buf_addr = pxmitbuf->pbuf;

	pxmitbuf->priv_data = pmpframe;

	return pmpframe;

}

static thread_return mp_xmit_packet_thread(thread_context context)
{
	struct xmit_frame	*pxmitframe;
	struct mp_tx		*pmptx;
	struct mp_priv	*pmp_priv;
	struct xmit_priv	*pxmitpriv;
	PADAPTER padapter;

	pmp_priv = (struct mp_priv *)context;
	pmptx = &pmp_priv->tx;
	padapter = pmp_priv->papdater;
	pxmitpriv = &(padapter->xmitpriv);

	thread_enter("RTW_MP_THREAD");

	DBG_871X("%s:pkTx Start\n", __func__);
	while (1) {
		pxmitframe = alloc_mp_xmitframe(pxmitpriv);
		if (pxmitframe == NULL) {
			if (pmptx->stop ||
			    padapter->bSurpriseRemoved ||
			    padapter->bDriverStopped) {
				goto exit;
			}
			else {
				rtw_usleep_os(10);
				continue;
			}
		}
		_rtw_memcpy((u8 *)(pxmitframe->buf_addr+TXDESC_OFFSET), pmptx->buf, pmptx->write_size);
		_rtw_memcpy(&(pxmitframe->attrib), &(pmptx->attrib), sizeof(struct pkt_attrib));


		rtw_usleep_os(padapter->mppriv.pktInterval);
		dump_mpframe(padapter, pxmitframe);

		pmptx->sended++;
		pmp_priv->tx_pktcount++;

		if (pmptx->stop ||
		    padapter->bSurpriseRemoved ||
		    padapter->bDriverStopped)
			goto exit;
		if ((pmptx->count != 0) &&
		    (pmptx->count == pmptx->sended))
			goto exit;

		flush_signals_thread();
	}

exit:
	//DBG_871X("%s:pkTx Exit\n", __func__);
	rtw_mfree(pmptx->pallocated_buf, pmptx->buf_size);
	pmptx->pallocated_buf = NULL;
	pmptx->stop = 1;

	thread_exit();
}

void fill_txdesc_for_mp(PADAPTER padapter, u8 *ptxdesc)
{
	struct mp_priv *pmp_priv = &padapter->mppriv;
	_rtw_memcpy(ptxdesc, pmp_priv->tx.desc, TXDESC_SIZE);
}

#if defined(CONFIG_RTL8723B)
void fill_tx_desc_8723b(PADAPTER padapter)
{
	struct mp_priv *pmp_priv = &padapter->mppriv;
	struct pkt_attrib *pattrib = &(pmp_priv->tx.attrib);
	u8 *ptxdesc = pmp_priv->tx.desc;

	SET_TX_DESC_AGG_BREAK_8723B(ptxdesc, 1);
	SET_TX_DESC_MACID_8723B(ptxdesc, pattrib->mac_id);
	SET_TX_DESC_QUEUE_SEL_8723B(ptxdesc, pattrib->qsel);

	SET_TX_DESC_RATE_ID_8723B(ptxdesc, pattrib->raid);
	SET_TX_DESC_SEQ_8723B(ptxdesc, pattrib->seqnum);
	SET_TX_DESC_HWSEQ_EN_8723B(ptxdesc, 1);
	SET_TX_DESC_USE_RATE_8723B(ptxdesc, 1);
	SET_TX_DESC_DISABLE_FB_8723B(ptxdesc, 1);

	if (pmp_priv->preamble)
		if (pmp_priv->rateidx <=  MPT_RATE_54M) {
			SET_TX_DESC_DATA_SHORT_8723B(ptxdesc, 1);
		}

	if (pmp_priv->bandwidth == CHANNEL_WIDTH_40) {
		SET_TX_DESC_DATA_BW_8723B(ptxdesc, 1);
	}

	SET_TX_DESC_TX_RATE_8723B(ptxdesc, pmp_priv->rateidx);

	SET_TX_DESC_DATA_RATE_FB_LIMIT_8723B(ptxdesc, 0x1F);
	SET_TX_DESC_RTS_RATE_FB_LIMIT_8723B(ptxdesc, 0xF);
}
#endif

static void Rtw_MPSetMacTxEDCA(PADAPTER padapter)
{

	rtw_write32(padapter, 0x508 , 0x00a422); //Disable EDCA BE Txop for MP pkt tx adjust Packet interval
	//DBG_871X("%s:write 0x508~~~~~~ 0x%x\n", __func__,rtw_read32(padapter, 0x508));
	PHY_SetMacReg(padapter, 0x458 ,bMaskDWord , 0x0);
	//DBG_8192C("%s()!!!!! 0x460 = 0x%x\n" ,__func__,PHY_QueryBBReg(padapter, 0x460, bMaskDWord));
	PHY_SetMacReg(padapter, 0x460 ,bMaskLWord , 0x0);//fast EDCA queue packet interval & time out vaule
	//PHY_SetMacReg(padapter, ODM_EDCA_VO_PARAM ,bMaskLWord , 0x431C);
	//PHY_SetMacReg(padapter, ODM_EDCA_BE_PARAM ,bMaskLWord , 0x431C);
	//PHY_SetMacReg(padapter, ODM_EDCA_BK_PARAM ,bMaskLWord , 0x431C);
	DBG_8192C("%s()!!!!! 0x460 = 0x%x\n" ,__func__,PHY_QueryBBReg(padapter, 0x460, bMaskDWord));

}

void SetPacketTx(PADAPTER padapter)
{
	u8 *ptr, *pkt_start, *pkt_end,*fctrl;
	u32 pkt_size,offset,startPlace,i;
	struct rtw_ieee80211_hdr *hdr;
	u8 payload;
	s32 bmcast;
	struct pkt_attrib *pattrib;
	struct mp_priv *pmp_priv;

	pmp_priv = &padapter->mppriv;

	if (pmp_priv->tx.stop) return;
	pmp_priv->tx.sended = 0;
	pmp_priv->tx.stop = 0;
	pmp_priv->tx_pktcount = 0;

	//3 1. update_attrib()
	pattrib = &pmp_priv->tx.attrib;
	_rtw_memcpy(pattrib->src, padapter->eeprompriv.mac_addr, ETH_ALEN);
	_rtw_memcpy(pattrib->ta, pattrib->src, ETH_ALEN);
	_rtw_memcpy(pattrib->ra, pattrib->dst, ETH_ALEN);
	bmcast = IS_MCAST(pattrib->ra);
	if (bmcast) {
		pattrib->mac_id = 1;
		pattrib->psta = rtw_get_bcmc_stainfo(padapter);
	} else {
		pattrib->mac_id = 0;
		pattrib->psta = rtw_get_stainfo(&padapter->stapriv, get_bssid(&padapter->mlmepriv));
	}
	pattrib->mbssid = 0;

	pattrib->last_txcmdsz = pattrib->hdrlen + pattrib->pktlen;

	//3 2. allocate xmit buffer
	pkt_size = pattrib->last_txcmdsz;

	if (pmp_priv->tx.pallocated_buf)
		rtw_mfree(pmp_priv->tx.pallocated_buf, pmp_priv->tx.buf_size);
	pmp_priv->tx.write_size = pkt_size;
	pmp_priv->tx.buf_size = pkt_size + XMITBUF_ALIGN_SZ;
	pmp_priv->tx.pallocated_buf = rtw_zmalloc(pmp_priv->tx.buf_size);
	if (pmp_priv->tx.pallocated_buf == NULL) {
		DBG_871X("%s: malloc(%d) fail!!\n", __func__, pmp_priv->tx.buf_size);
		return;
	}
	pmp_priv->tx.buf = (u8 *)N_BYTE_ALIGMENT((SIZE_PTR)(pmp_priv->tx.pallocated_buf), XMITBUF_ALIGN_SZ);
	ptr = pmp_priv->tx.buf;

	_rtw_memset(pmp_priv->tx.desc, 0, TXDESC_SIZE);
	pkt_start = ptr;
	pkt_end = pkt_start + pkt_size;

	//3 3. init TX descriptor
#if defined(CONFIG_RTL8723B)
	if(IS_HARDWARE_TYPE_8723B(padapter))
		fill_tx_desc_8723b(padapter);
#endif


	//3 4. make wlan header, make_wlanhdr()
	hdr = (struct rtw_ieee80211_hdr *)pkt_start;
	SetFrameSubType(&hdr->frame_ctl, pattrib->subtype);
	//
	SetFrDs(&hdr->frame_ctl);
	_rtw_memcpy(hdr->addr1, pattrib->dst, ETH_ALEN); // DA
	_rtw_memcpy(hdr->addr2, pattrib->src, ETH_ALEN); // SA
	_rtw_memcpy(hdr->addr3, get_bssid(&padapter->mlmepriv), ETH_ALEN); // RA, BSSID

	//3 5. make payload
	ptr = pkt_start + pattrib->hdrlen;

	switch (pmp_priv->tx.payload) {
		case 0:
			payload = 0x00;
			break;
		case 1:
			payload = 0x5a;
			break;
		case 2:
			payload = 0xa5;
			break;
		case 3:
			payload = 0xff;
			break;
		default:
			payload = 0x00;
			break;
	}
	pmp_priv->TXradomBuffer = rtw_zmalloc(4096);
	if(pmp_priv->TXradomBuffer == NULL)
	{
		DBG_871X("mp create random buffer fail!\n");
		goto exit;
	}

	for(i = 0; i < 4096; i++)
		pmp_priv->TXradomBuffer[i] = rtw_random32() %0xFF;

	//startPlace = (u32)(rtw_random32() % 3450);
	_rtw_memcpy(ptr, pmp_priv->TXradomBuffer,pkt_end - ptr);
	//_rtw_memset(ptr, payload, pkt_end - ptr);
	rtw_mfree(pmp_priv->TXradomBuffer,4096);

	//3 6. start thread
	pmp_priv->tx.PktTxThread = kthread_run(mp_xmit_packet_thread, pmp_priv, "RTW_MP_THREAD");
	if (IS_ERR(pmp_priv->tx.PktTxThread))
		DBG_871X("Create PktTx Thread Fail !!!!!\n");
	Rtw_MPSetMacTxEDCA(padapter);

exit:
	return;
}

void SetPacketRx(PADAPTER pAdapter, u8 bStartRx)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);
	struct mp_priv *pmppriv = &pAdapter->mppriv;
	u8	type;
	type = _HW_STATE_AP_;
	if(bStartRx)
	{
#ifdef CONFIG_RTL8723B
		PHY_SetMacReg(pAdapter, 0xe70, BIT23|BIT22, 0x3);// Power on adc  (in RX_WAIT_CCA state)
		write_bbreg(pAdapter, 0xa01, BIT0, bDisable);// improve Rx performance by jerry
#endif
		if(	pmppriv->bSetRxBssid == _TRUE ){
			//pHalData->ReceiveConfig = RCR_APM | RCR_AM | RCR_AB |RCR_CBSSID_DATA| RCR_CBSSID_BCN| RCR_APP_ICV | RCR_AMF | RCR_HTC_LOC_CTRL | RCR_APP_MIC | RCR_APP_PHYST_RXFF;
			pHalData->ReceiveConfig = RCR_AAP | RCR_APM | RCR_AM | RCR_AB |RCR_AMF | RCR_APP_ICV | RCR_AMF | RCR_HTC_LOC_CTRL | RCR_APP_MIC | RCR_APP_PHYST_RXFF  ;
			pHalData->ReceiveConfig |= ACRC32;

			DBG_8192C("%s , pmppriv->network_macaddr =%x %x %x %x %x %x\n",__func__,
								pmppriv->network_macaddr[0],pmppriv->network_macaddr[1],pmppriv->network_macaddr[2],pmppriv->network_macaddr[3],pmppriv->network_macaddr[4],pmppriv->network_macaddr[5]);
			//Set_MSR(pAdapter, WIFI_FW_AP_STATE);
			//rtw_hal_set_hwreg(pAdapter, HW_VAR_BSSID, pmppriv->network_macaddr);
			//rtw_hal_set_hwreg(pAdapter, HW_VAR_SET_OPMODE, (u8 *)(&type));
		}
		else
		{
		pHalData->ReceiveConfig = AAP | APM | AM | AB | APP_ICV | ADF | AMF | HTC_LOC_CTRL | APP_MIC | APP_PHYSTS;
		pHalData->ReceiveConfig |= ACRC32;
		rtw_write32(pAdapter, REG_RCR, pHalData->ReceiveConfig);
		// Accept all data frames
		rtw_write16(pAdapter, REG_RXFLTMAP2, 0xFFFF);
		// Accept CRC error and destination address
		}
	}
	else
	{
#ifdef CONFIG_RTL8723B
		PHY_SetMacReg(pAdapter, 0xe70, BIT23|BIT22, 0x00);// Power off adc  (in RX_WAIT_CCA state)
		write_bbreg(pAdapter, 0xa01, BIT0, bEnable);// improve Rx performance by jerry
#endif
		rtw_write32(pAdapter, REG_RCR, 0);
	}
}

void ResetPhyRxPktCount(PADAPTER pAdapter)
{
	u32 i, phyrx_set = 0;

	for (i = 0; i <= 0xF; i++) {
		phyrx_set = 0;
		phyrx_set |= _RXERR_RPT_SEL(i);	//select
		phyrx_set |= RXERR_RPT_RST;	// set counter to zero
		rtw_write32(pAdapter, REG_RXERR_RPT, phyrx_set);
	}
}

static u32 GetPhyRxPktCounts(PADAPTER pAdapter, u32 selbit)
{
	//selection
	u32 phyrx_set = 0, count = 0;

	phyrx_set = _RXERR_RPT_SEL(selbit & 0xF);
	rtw_write32(pAdapter, REG_RXERR_RPT, phyrx_set);

	//Read packet count
	count = rtw_read32(pAdapter, REG_RXERR_RPT) & RXERR_COUNTER_MASK;

	return count;
}

u32 GetPhyRxPktReceived(PADAPTER pAdapter)
{
	u32 OFDM_cnt = 0, CCK_cnt = 0, HT_cnt = 0;

	OFDM_cnt = GetPhyRxPktCounts(pAdapter, RXERR_TYPE_OFDM_MPDU_OK);
	CCK_cnt = GetPhyRxPktCounts(pAdapter, RXERR_TYPE_CCK_MPDU_OK);
	HT_cnt = GetPhyRxPktCounts(pAdapter, RXERR_TYPE_HT_MPDU_OK);

	return OFDM_cnt + CCK_cnt + HT_cnt;
}

u32 GetPhyRxPktCRC32Error(PADAPTER pAdapter)
{
	u32 OFDM_cnt = 0, CCK_cnt = 0, HT_cnt = 0;

	OFDM_cnt = GetPhyRxPktCounts(pAdapter, RXERR_TYPE_OFDM_MPDU_FAIL);
	CCK_cnt = GetPhyRxPktCounts(pAdapter, RXERR_TYPE_CCK_MPDU_FAIL);
	HT_cnt = GetPhyRxPktCounts(pAdapter, RXERR_TYPE_HT_MPDU_FAIL);

	return OFDM_cnt + CCK_cnt + HT_cnt;
}

//reg 0x808[9:0]: FFT data x
//reg 0x808[22]:  0  -->  1  to get 1 FFT data y
//reg 0x8B4[15:0]: FFT data y report
static u32 rtw_GetPSDData(PADAPTER pAdapter, u32 point)
{
	u32 psd_val=0;

	u16 psd_reg = 0x808;
	u16 psd_regL= 0x8B4;

	psd_val = rtw_read32(pAdapter, psd_reg);

	psd_val &= 0xFFBFFC00;
	psd_val |= point;

	rtw_write32(pAdapter, psd_reg, psd_val);
	rtw_mdelay_os(1);
	psd_val |= 0x00400000;

	rtw_write32(pAdapter, psd_reg, psd_val);
	rtw_mdelay_os(1);

	psd_val = rtw_read32(pAdapter, psd_regL);
	psd_val &= 0x0000FFFF;

	return psd_val;
}

/*
 * pts	start_point_min		stop_point_max
 * 128	64			64 + 128 = 192
 * 256	128			128 + 256 = 384
 * 512	256			256 + 512 = 768
 * 1024	512			512 + 1024 = 1536
 *
 */
u32 mp_query_psd(PADAPTER pAdapter, u8 *data)
{
	u32 i, psd_pts=0, psd_start=0, psd_stop=0;
	u32 psd_data=0;


	if (!netif_running(pAdapter->pnetdev)) {
		RT_TRACE(_module_mp_, _drv_warning_, ("mp_query_psd: Fail! interface not opened!\n"));
		return 0;
	}

	if (check_fwstate(&pAdapter->mlmepriv, WIFI_MP_STATE) == _FALSE) {
		RT_TRACE(_module_mp_, _drv_warning_, ("mp_query_psd: Fail! not in MP mode!\n"));
		return 0;
	}

	if (strlen(data) == 0) { //default value
		psd_pts = 128;
		psd_start = 64;
		psd_stop = 128;
	} else {
		sscanf(data, "pts=%d,start=%d,stop=%d", &psd_pts, &psd_start, &psd_stop);
	}

	data[0]='\0';

	i = psd_start;
	while (i < psd_stop)
	{
		if (i >= psd_pts) {
			psd_data = rtw_GetPSDData(pAdapter, i-psd_pts);
		} else {
			psd_data = rtw_GetPSDData(pAdapter, i);
		}
		sprintf(data, "%s%x ", data, psd_data);
		i++;
	}

	#ifdef CONFIG_LONG_DELAY_ISSUE
	rtw_msleep_os(100);
	#else
	rtw_mdelay_os(100);
	#endif

	return strlen(data)+1;
}



ULONG getPowerDiffByRate8188E(
	IN	PADAPTER	pAdapter,
	IN	u1Byte		CurrChannel,
	IN	ULONG		RfPath
	)
{
	PMPT_CONTEXT			pMptCtx = &(pAdapter->mppriv.MptCtx);
	HAL_DATA_TYPE	*pHalData	= GET_HAL_DATA(pAdapter);
	ULONG	PwrGroup=0;
	ULONG	TxPower=0, Limit=0;
	ULONG	Pathmapping = (RfPath == ODM_RF_PATH_A?0:8);

	switch(pHalData->EEPROMRegulatory)
	{
		case 0: // driver-defined maximum power offset for longer communication range
				// refer to power by rate table
			PwrGroup = 0;
			Limit = 0xff;
			break;
		case 1: // Power-limit table-defined maximum power offset range
				// choosed by min(power by rate, power limit).
			{
				if(pHalData->pwrGroupCnt == 1)
					PwrGroup = 0;
				if(pHalData->pwrGroupCnt >= 3)
				{
					if(CurrChannel <= 3)
						PwrGroup = 0;
					else if(CurrChannel >= 4 && CurrChannel <= 9)
						PwrGroup = 1;
					else if(CurrChannel > 9)
						PwrGroup = 2;

					if(pHalData->CurrentChannelBW == CHANNEL_WIDTH_20)
						PwrGroup++;
					else
						PwrGroup+=4;
				}
				Limit = 0xff;
			}
			break;
		case 2: // not support power offset by rate.
				// don't increase any power diff
			PwrGroup = 0;
			Limit = 0;
			break;
		default:
			PwrGroup = 0;
			Limit = 0xff;
			break;
	}


	{
		switch(pMptCtx->MptRateIndex)
		{
			case MPT_RATE_1M:
			case MPT_RATE_2M:
			case MPT_RATE_55M:
			case MPT_RATE_11M:
				//CCK rates, don't add any tx power index.
				//RT_DISP(FPHY, PHY_TXPWR,("CCK rates!\n"));
				break;
			case MPT_RATE_6M:	//0xe00 [31:0] = 18M,12M,09M,06M
				TxPower += ((pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][0+Pathmapping])&0xff);
				//RT_DISP(FPHY, PHY_TXPWR,("MCSTxPowerLevelOriginalOffset[%d][0] = 0x%x, OFDM 6M, TxPower = %d\n",
				//	PwrGroup, pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][0], TxPower));
				break;
			case MPT_RATE_9M:
				TxPower += (((pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][0+Pathmapping])&0xff00)>>8);
				//RT_DISP(FPHY, PHY_TXPWR,("MCSTxPowerLevelOriginalOffset[%d][0] = 0x%x, OFDM 9M, TxPower = %d\n",
				//	PwrGroup, pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][0], TxPower));
				break;
			case MPT_RATE_12M:
				TxPower += (((pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][0+Pathmapping])&0xff0000)>>16);
				//RT_DISP(FPHY, PHY_TXPWR,("MCSTxPowerLevelOriginalOffset[%d][0] = 0x%x, OFDM 12M, TxPower = %d\n",
				//	PwrGroup, pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][0], TxPower));
				break;
			case MPT_RATE_18M:
				TxPower += (((pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][0+Pathmapping])&0xff000000)>>24);
				//RT_DISP(FPHY, PHY_TXPWR,("MCSTxPowerLevelOriginalOffset[%d][0] = 0x%x, OFDM 24M, TxPower = %d\n",
				//	PwrGroup, pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][0], TxPower));
				break;
			case MPT_RATE_24M:	//0xe04[31:0] = 54M,48M,36M,24M
				TxPower += ((pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][1+Pathmapping])&0xff);
				//RT_DISP(FPHY, PHY_TXPWR,("MCSTxPowerLevelOriginalOffset[%d][1] = 0x%x, OFDM 24M, TxPower = %d\n",
				//	PwrGroup, pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][1], TxPower));
				break;
			case MPT_RATE_36M:
				TxPower += (((pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][1+Pathmapping])&0xff00)>>8);
				//RT_DISP(FPHY, PHY_TXPWR,("MCSTxPowerLevelOriginalOffset[%d][1] = 0x%x, OFDM 36M, TxPower = %d\n",
				//	PwrGroup, pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][1], TxPower));
				break;
			case MPT_RATE_48M:
				TxPower += (((pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][1+Pathmapping])&0xff0000)>>16);
				//RT_DISP(FPHY, PHY_TXPWR,("MCSTxPowerLevelOriginalOffset[%d][1] = 0x%x, OFDM 48M, TxPower = %d\n",
				//	PwrGroup, pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][1], TxPower));
				break;
			case MPT_RATE_54M:
				TxPower += (((pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][1+Pathmapping])&0xff000000)>>24);
				//RT_DISP(FPHY, PHY_TXPWR,("MCSTxPowerLevelOriginalOffset[%d][1] = 0x%x, OFDM 54M, TxPower = %d\n",
				//	PwrGroup, pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][1], TxPower));
				break;
			case MPT_RATE_MCS0: //0xe10[31:0]= MCS=03,02,01,00
				TxPower += ((pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][2+Pathmapping])&0xff);
				//RT_DISP(FPHY, PHY_TXPWR,("MCSTxPowerLevelOriginalOffset[%d][2] = 0x%x, MCS0, TxPower = %d\n",
				//	PwrGroup, pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][2], TxPower));
				break;
			case MPT_RATE_MCS1:
				TxPower += (((pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][2+Pathmapping])&0xff00)>>8);
				//RT_DISP(FPHY, PHY_TXPWR,("MCSTxPowerLevelOriginalOffset[%d][2] = 0x%x, MCS1, TxPower = %d\n",
				//	PwrGroup, pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][2], TxPower));
				break;
			case MPT_RATE_MCS2:
				TxPower += (((pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][2+Pathmapping])&0xff0000)>>16);
				//RT_DISP(FPHY, PHY_TXPWR,("MCSTxPowerLevelOriginalOffset[%d][2] = 0x%x, MCS2, TxPower = %d\n",
				//	PwrGroup, pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][2], TxPower));
				break;
			case MPT_RATE_MCS3:
				TxPower += (((pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][2+Pathmapping])&0xff000000)>>24);
				//RT_DISP(FPHY, PHY_TXPWR,("MCSTxPowerLevelOriginalOffset[%d][2] = 0x%x, MCS3, TxPower = %d\n",
				//	PwrGroup, pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][2], TxPower));
				break;
			case MPT_RATE_MCS4: //0xe14[31:0]= MCS=07,06,05,04
				TxPower += ((pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][3+Pathmapping])&0xff);
				//RT_DISP(FPHY, PHY_TXPWR,("MCSTxPowerLevelOriginalOffset[%d][3] = 0x%x, MCS4, TxPower = %d\n",
				//	PwrGroup, pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][3], TxPower));
				break;
			case MPT_RATE_MCS5:
				TxPower += (((pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][3+Pathmapping])&0xff00)>>8);
				//RT_DISP(FPHY, PHY_TXPWR,("MCSTxPowerLevelOriginalOffset[%d][3] = 0x%x, MCS5, TxPower = %d\n",
				//	PwrGroup, pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][3], TxPower));
				break;
			case MPT_RATE_MCS6:
				TxPower += (((pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][3+Pathmapping])&0xff0000)>>16);
				//RT_DISP(FPHY, PHY_TXPWR,("MCSTxPowerLevelOriginalOffset[%d][3] = 0x%x, MCS6, TxPower = %d\n",
				//	PwrGroup, pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][3], TxPower));
				break;
			case MPT_RATE_MCS7:
				TxPower += (((pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][3+Pathmapping])&0xff000000)>>24);
				//RT_DISP(FPHY, PHY_TXPWR,("MCSTxPowerLevelOriginalOffset[%d][3] = 0x%x, MCS7, TxPower = %d\n",
				//	PwrGroup, pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][3], TxPower));
				break;

			case MPT_RATE_MCS8: //0xe18[31:0]= MCS=11,10,09,08
				TxPower += ((pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][4+Pathmapping])&0xff);
				//RT_DISP(FPHY, PHY_TXPWR,("MCSTxPowerLevelOriginalOffset[%d][4] = 0x%x, MCS8, TxPower = %d\n",
				//	PwrGroup, pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][4], TxPower));
				break;
			case MPT_RATE_MCS9:
				TxPower += (((pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][4+Pathmapping])&0xff00)>>8);
				//RT_DISP(FPHY, PHY_TXPWR,("MCSTxPowerLevelOriginalOffset[%d][4] = 0x%x, MCS9, TxPower = %d\n",
				//	PwrGroup, pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][4], TxPower));
				break;
			case MPT_RATE_MCS10:
				TxPower += (((pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][4+Pathmapping])&0xff0000)>>16);
				//RT_DISP(FPHY, PHY_TXPWR,("MCSTxPowerLevelOriginalOffset[%d][4] = 0x%x, MCS10, TxPower = %d\n",
				//	PwrGroup, pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][4], TxPower));
				break;
			case MPT_RATE_MCS11:
				TxPower += (((pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][4+Pathmapping])&0xff000000)>>24);
				//RT_DISP(FPHY, PHY_TXPWR,("MCSTxPowerLevelOriginalOffset[%d][4] = 0x%x, MCS11, TxPower = %d\n",
				//	PwrGroup, pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][4], TxPower));
				break;
			case MPT_RATE_MCS12:	//0xe1c[31:0]= MCS=15,14,13,12
				TxPower += ((pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][5+Pathmapping])&0xff);
				//RT_DISP(FPHY, PHY_TXPWR,("MCSTxPowerLevelOriginalOffset[%d][5] = 0x%x, MCS12, TxPower = %d\n",
				//	PwrGroup, pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][5], TxPower));
				break;
			case MPT_RATE_MCS13:
				TxPower += (((pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][5+Pathmapping])&0xff00)>>8);
				//RT_DISP(FPHY, PHY_TXPWR,("MCSTxPowerLevelOriginalOffset[%d][5] = 0x%x, MCS13, TxPower = %d\n",
				//	PwrGroup, pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][5], TxPower));
				break;
			case MPT_RATE_MCS14:
				TxPower += (((pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][5+Pathmapping])&0xff0000)>>16);
				//RT_DISP(FPHY, PHY_TXPWR,("MCSTxPowerLevelOriginalOffset[%d][5] = 0x%x, MCS14, TxPower = %d\n",
				//	PwrGroup, pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][5], TxPower));
				break;
			case MPT_RATE_MCS15:
				TxPower += (((pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][5+Pathmapping])&0xff000000)>>24);
				//RT_DISP(FPHY, PHY_TXPWR,("MCSTxPowerLevelOriginalOffset[%d][5] = 0x%x, MCS15, TxPower = %d\n",
				//	PwrGroup, pHalData->MCSTxPowerLevelOriginalOffset[PwrGroup][5], TxPower));
				break;
			default:
				break;
		}
	}

	if(TxPower > Limit)
		TxPower = Limit;

	return TxPower;
}



static	ULONG
mpt_ProQueryCalTxPower_8188E(
	IN	PADAPTER		pAdapter,
	IN	u1Byte			RfPath
	)
{
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(pAdapter);
	u1Byte				TxCount=TX_1S, i = 0;	//default set to 1S
	//PMGNT_INFO			pMgntInfo = &(pAdapter->MgntInfo);
	ULONG				TxPower = 1, PwrGroup=0, PowerDiffByRate=0;
	ULONG				TxPowerCCK = 1, TxPowerOFDM = 1, TxPowerBW20 = 1, TxPowerBW40 = 1 ;
	PMPT_CONTEXT		pMptCtx = &(pAdapter->mppriv.MptCtx);
	u1Byte				CurrChannel = pHalData->CurrentChannel;
	u1Byte				index = (CurrChannel -1);
	u1Byte				rf_path=(RfPath), rfPath;
	u1Byte				limit = 0, rate = 0;

	if(HAL_IsLegalChannel(pAdapter, CurrChannel) == FALSE)
	{
		CurrChannel = 1;
	}

	if(pMptCtx->MptRateIndex <= MPT_RATE_11M )
	{
		TxPower = pHalData->Index24G_CCK_Base[rf_path][index];
	}
	else if(pMptCtx->MptRateIndex >= MPT_RATE_6M &&
		pMptCtx->MptRateIndex <= MPT_RATE_54M )
	{
		TxPower = pHalData->Index24G_BW40_Base[rf_path][index];
	}
	else if(pMptCtx->MptRateIndex >= MPT_RATE_MCS0 &&
		pMptCtx->MptRateIndex <= MPT_RATE_MCS7 )
	{
		TxPower = pHalData->Index24G_BW40_Base[rf_path][index];
	}

	//RT_DISP(FPHY, PHY_TXPWR, ("HT40 rate(%d) Tx power(RF-%c) = 0x%x\n", pMptCtx->MptRateIndex, ((rf_path==0)?'A':'B'), TxPower));


	if(pMptCtx->MptRateIndex >= MPT_RATE_6M &&
		pMptCtx->MptRateIndex <= MPT_RATE_54M )
	{
		TxPower += pHalData->OFDM_24G_Diff[rf_path][TxCount];
		///RT_DISP(FPHY, PHY_TXPWR, ("+OFDM_PowerDiff(RF-%c) = 0x%x\n", ((rf_path==0)?'A':'B'),
		//	pHalData->OFDM_24G_Diff[rf_path][TxCount]));
	}

	if(pMptCtx->MptRateIndex >= MPT_RATE_MCS0)
	{
		if (pHalData->CurrentChannelBW == CHANNEL_WIDTH_20)
		{
			TxPower += pHalData->BW20_24G_Diff[rf_path][TxCount];
		//	RT_DISP(FPHY, PHY_TXPWR, ("+HT20_PowerDiff(RF-%c) = 0x%x\n", ((rf_path==0)?'A':'B'),
		//		pHalData->BW20_24G_Diff[rf_path][TxCount]));
		}
	}


#ifdef ENABLE_POWER_BY_RATE
	PowerDiffByRate = getPowerDiffByRate8188E(pAdapter, CurrChannel, RfPath);
#else
	PowerDiffByRate = 0;
#endif

	// 2012/11/02 Awk: add power limit mechansim
	if( pMptCtx->MptRateIndex <= MPT_RATE_11M )
	{
		rate = MGN_1M;
	}
	else if(pMptCtx->MptRateIndex >= MPT_RATE_6M &&
		pMptCtx->MptRateIndex <= MPT_RATE_54M )
	{
		rate = MGN_54M;
	}
	else if(pMptCtx->MptRateIndex >= MPT_RATE_MCS0 &&
		pMptCtx->MptRateIndex <= MPT_RATE_MCS7 )
	{
		rate = MGN_MCS7;
	}

	limit = (u8)PHY_GetTxPowerLimit(pAdapter, pMptCtx->RegTxPwrLimit,
								   pHalData->CurrentBandType,
								   pHalData->CurrentChannelBW,RfPath,
								   rate, CurrChannel);

	//RT_DISP(FPHY, PHY_TXPWR, ("+PowerDiffByRate(RF-%c) = 0x%x\n", ((rf_path==0)?'A':'B'),
	//	PowerDiffByRate));
	TxPower += PowerDiffByRate;
	//RT_DISP(FPHY, PHY_TXPWR, ("PowerDiffByRate limit value(RF-%c) = %d\n", ((rf_path==0)?'A':'B'),
	//	limit));

	TxPower +=  limit > (s8) PowerDiffByRate ? PowerDiffByRate : limit;

	return TxPower;
}

u8 MptToMgntRate(u32	MptRateIdx)
{
// Mapped to MGN_XXX defined in MgntGen.h
	switch (MptRateIdx)
	{
		/* CCK rate. */
		case	MPT_RATE_1M:			return MGN_1M;
		case	MPT_RATE_2M:			return MGN_2M;
		case	MPT_RATE_55M:			return MGN_5_5M;
		case	MPT_RATE_11M:			return MGN_11M;

		/* OFDM rate. */
		case	MPT_RATE_6M:			return MGN_6M;
		case	MPT_RATE_9M:			return MGN_9M;
		case	MPT_RATE_12M:			return MGN_12M;
		case	MPT_RATE_18M:			return MGN_18M;
		case	MPT_RATE_24M:			return MGN_24M;
		case	MPT_RATE_36M:			return MGN_36M;
		case	MPT_RATE_48M:			return MGN_48M;
		case	MPT_RATE_54M:			return MGN_54M;

		/* HT rate. */
		case	MPT_RATE_MCS0:			return MGN_MCS0;
		case	MPT_RATE_MCS1:			return MGN_MCS1;
		case	MPT_RATE_MCS2:			return MGN_MCS2;
		case	MPT_RATE_MCS3:			return MGN_MCS3;
		case	MPT_RATE_MCS4:			return MGN_MCS4;
		case	MPT_RATE_MCS5:			return MGN_MCS5;
		case	MPT_RATE_MCS6:			return MGN_MCS6;
		case	MPT_RATE_MCS7:			return MGN_MCS7;
		case	MPT_RATE_MCS8:			return MGN_MCS8;
		case	MPT_RATE_MCS9:			return MGN_MCS9;
		case	MPT_RATE_MCS10: 		return MGN_MCS10;
		case	MPT_RATE_MCS11: 		return MGN_MCS11;
		case	MPT_RATE_MCS12: 		return MGN_MCS12;
		case	MPT_RATE_MCS13: 		return MGN_MCS13;
		case	MPT_RATE_MCS14: 		return MGN_MCS14;
		case	MPT_RATE_MCS15: 		return MGN_MCS15;
		case	MPT_RATE_MCS16: 		return MGN_MCS16;
		case	MPT_RATE_MCS17: 		return MGN_MCS17;
		case	MPT_RATE_MCS18: 		return MGN_MCS18;
		case	MPT_RATE_MCS19: 		return MGN_MCS19;
		case	MPT_RATE_MCS20: 		return MGN_MCS20;
		case	MPT_RATE_MCS21: 		return MGN_MCS21;
		case	MPT_RATE_MCS22: 		return MGN_MCS22;
		case	MPT_RATE_MCS23: 		return MGN_MCS23;
		case	MPT_RATE_MCS24: 		return MGN_MCS24;
		case	MPT_RATE_MCS25: 		return MGN_MCS25;
		case	MPT_RATE_MCS26: 		return MGN_MCS26;
		case	MPT_RATE_MCS27: 		return MGN_MCS27;
		case	MPT_RATE_MCS28: 		return MGN_MCS28;
		case	MPT_RATE_MCS29: 		return MGN_MCS29;
		case	MPT_RATE_MCS30: 		return MGN_MCS30;
		case	MPT_RATE_MCS31: 		return MGN_MCS31;

		/* VHT rate. */
		case	MPT_RATE_VHT1SS_MCS0:	return MGN_VHT1SS_MCS0;
		case	MPT_RATE_VHT1SS_MCS1:	return MGN_VHT1SS_MCS1;
		case	MPT_RATE_VHT1SS_MCS2:	return MGN_VHT1SS_MCS2;
		case	MPT_RATE_VHT1SS_MCS3:	return MGN_VHT1SS_MCS3;
		case	MPT_RATE_VHT1SS_MCS4:	return MGN_VHT1SS_MCS4;
		case	MPT_RATE_VHT1SS_MCS5:	return MGN_VHT1SS_MCS5;
		case	MPT_RATE_VHT1SS_MCS6:	return MGN_VHT1SS_MCS6;
		case	MPT_RATE_VHT1SS_MCS7:	return MGN_VHT1SS_MCS7;
		case	MPT_RATE_VHT1SS_MCS8:	return MGN_VHT1SS_MCS8;
		case	MPT_RATE_VHT1SS_MCS9:	return MGN_VHT1SS_MCS9;
		case	MPT_RATE_VHT2SS_MCS0:	return MGN_VHT2SS_MCS0;
		case	MPT_RATE_VHT2SS_MCS1:	return MGN_VHT2SS_MCS1;
		case	MPT_RATE_VHT2SS_MCS2:	return MGN_VHT2SS_MCS2;
		case	MPT_RATE_VHT2SS_MCS3:	return MGN_VHT2SS_MCS3;
		case	MPT_RATE_VHT2SS_MCS4:	return MGN_VHT2SS_MCS4;
		case	MPT_RATE_VHT2SS_MCS5:	return MGN_VHT2SS_MCS5;
		case	MPT_RATE_VHT2SS_MCS6:	return MGN_VHT2SS_MCS6;
		case	MPT_RATE_VHT2SS_MCS7:	return MGN_VHT2SS_MCS7;
		case	MPT_RATE_VHT2SS_MCS8:	return MGN_VHT2SS_MCS8;
		case	MPT_RATE_VHT2SS_MCS9:	return MGN_VHT2SS_MCS9;
		case	MPT_RATE_VHT3SS_MCS0:	return MGN_VHT3SS_MCS0;
		case	MPT_RATE_VHT3SS_MCS1:	return MGN_VHT3SS_MCS1;
		case	MPT_RATE_VHT3SS_MCS2:	return MGN_VHT3SS_MCS2;
		case	MPT_RATE_VHT3SS_MCS3:	return MGN_VHT3SS_MCS3;
		case	MPT_RATE_VHT3SS_MCS4:	return MGN_VHT3SS_MCS4;
		case	MPT_RATE_VHT3SS_MCS5:	return MGN_VHT3SS_MCS5;
		case	MPT_RATE_VHT3SS_MCS6:	return MGN_VHT3SS_MCS6;
		case	MPT_RATE_VHT3SS_MCS7:	return MGN_VHT3SS_MCS7;
		case	MPT_RATE_VHT3SS_MCS8:	return MGN_VHT3SS_MCS8;
		case	MPT_RATE_VHT3SS_MCS9:	return MGN_VHT3SS_MCS9;
		case	MPT_RATE_VHT4SS_MCS0:	return MGN_VHT4SS_MCS0;
		case	MPT_RATE_VHT4SS_MCS1:	return MGN_VHT4SS_MCS1;
		case	MPT_RATE_VHT4SS_MCS2:	return MGN_VHT4SS_MCS2;
		case	MPT_RATE_VHT4SS_MCS3:	return MGN_VHT4SS_MCS3;
		case	MPT_RATE_VHT4SS_MCS4:	return MGN_VHT4SS_MCS4;
		case	MPT_RATE_VHT4SS_MCS5:	return MGN_VHT4SS_MCS5;
		case	MPT_RATE_VHT4SS_MCS6:	return MGN_VHT4SS_MCS6;
		case	MPT_RATE_VHT4SS_MCS7:	return MGN_VHT4SS_MCS7;
		case	MPT_RATE_VHT4SS_MCS8:	return MGN_VHT4SS_MCS8;
		case	MPT_RATE_VHT4SS_MCS9:	return MGN_VHT4SS_MCS9;

		case	MPT_RATE_LAST:// fully automatic
		default:
			DBG_871X("<===MptToMgntRate(), Invalid Rate: %d!!\n", MptRateIdx);
			return 0x0;
	}
}

u8 rtw_mpRateParseFunc(PADAPTER pAdapter, u8 *targetStr)
{
	u16 i=0;
	u8* rateindex_Array[] = { "1M","2M","5.5M","11M","6M","9M","12M","18M","24M","36M","48M","54M",
								"HTMCS0","HTMCS1","HTMCS2","HTMCS3","HTMCS4","HTMCS5","HTMCS6","HTMCS7",
								"HTMCS8","HTMCS9","HTMCS10","HTMCS11","HTMCS12","HTMCS13","HTMCS14","HTMCS15",
								"HTMCS16","HTMCS17","HTMCS18","HTMCS19","HTMCS20","HTMCS21","HTMCS22","HTMCS23",
								"HTMCS24","HTMCS25","HTMCS26","HTMCS27","HTMCS28","HTMCS29","HTMCS30","HTMCS31",
								"VHT1MCS0","VHT1MCS1","VHT1MCS2","VHT1MCS3","VHT1MCS4","VHT1MCS5","VHT1MCS6","VHT1MCS7","VHT1MCS8","VHT1MCS9",
								"VHT2MCS0","VHT2MCS1","VHT2MCS2","VHT2MCS3","VHT2MCS4","VHT2MCS5","VHT2MCS6","VHT2MCS7","VHT2MCS8","VHT2MCS9",
								"VHT3MCS0","VHT3MCS1","VHT3MCS2","VHT3MCS3","VHT3MCS4","VHT3MCS5","VHT3MCS6","VHT3MCS7","VHT3MCS8","VHT3MCS9",
								"VHT4MCS0","VHT4MCS1","VHT4MCS2","VHT4MCS3","VHT4MCS4","VHT4MCS5","VHT4MCS6","VHT4MCS7","VHT4MCS8","VHT4MCS9"};

	for(i = 0; i < ARRAY_SIZE(rateindex_Array); i++){
		if(strcmp(targetStr, rateindex_Array[i]) == 0){
			DBG_871X("%s , index = %d \n",__func__ ,i);
			return i;
		}
	}

	DBG_871X("%s ,please input a Data RATE String as:",__func__);
	for(i = 0; i < ARRAY_SIZE(rateindex_Array); i++)
	{
		DBG_871X("%s ",rateindex_Array[i]);
		if(i % 10 == 0)
			DBG_871X("\n");
	}
	return _FAIL;
}

ULONG mpt_ProQueryCalTxPower(PADAPTER pAdapter, u8 RfPath)
{

	HAL_DATA_TYPE	*pHalData	= GET_HAL_DATA(pAdapter);
	ULONG			TxPower = 1, PwrGroup=0, PowerDiffByRate=0;
	u1Byte			limit = 0, rate = 0;

	#if defined(CONFIG_RTL8723B)
	if( IS_HARDWARE_TYPE_8723B(pAdapter) )
	{
		rate = MptToMgntRate(pAdapter->mppriv.rateidx);
		TxPower = PHY_GetTxPowerIndex_8723B(pAdapter, RfPath, rate,
										pHalData->CurrentChannelBW, pHalData->CurrentChannel);
	}
	#endif

	DBG_8192C("txPower=%d ,CurrentChannelBW=%d ,CurrentChannel=%d ,rate =%d\n",TxPower,pHalData->CurrentChannelBW, pHalData->CurrentChannel,rate);

	pAdapter->mppriv.txpoweridx = (u8)TxPower;
	pAdapter->mppriv.txpoweridx_b = (u8)TxPower;
	Hal_SetAntennaPathPower(pAdapter);

	return TxPower;
}


void Hal_ProSetCrystalCap (PADAPTER pAdapter , u32 CrystalCap)
{
	HAL_DATA_TYPE		*pHalData	= GET_HAL_DATA(pAdapter);

	CrystalCap = CrystalCap & 0x3F;

	if(IS_HARDWARE_TYPE_8192D(pAdapter))
	{
		PHY_SetBBReg(pAdapter, REG_AFE_XTAL_CTRL, 0xF0, CrystalCap & 0x0F);
		PHY_SetBBReg(pAdapter, REG_AFE_PLL_CTRL, 0xF0000000, (CrystalCap & 0xF0) >> 4);
	}
	else if(IS_HARDWARE_TYPE_8188E(pAdapter))
	{
		// write 0x24[16:11] = 0x24[22:17] = CrystalCap
		PHY_SetBBReg(pAdapter, REG_AFE_XTAL_CTRL, 0x7FF800, (CrystalCap | (CrystalCap << 6)));
	}
	else if(IS_HARDWARE_TYPE_8812(pAdapter))
	{
		// write 0x2C[30:25] = 0x2C[24:19] = CrystalCap
		PHY_SetBBReg(pAdapter, REG_MAC_PHY_CTRL, 0x7FF80000, (CrystalCap | (CrystalCap << 6)));
	}
	else if(IS_HARDWARE_TYPE_8821(pAdapter) || IS_HARDWARE_TYPE_8192E(pAdapter) ||
		    IS_HARDWARE_TYPE_8723B(pAdapter))
	{
		// write 0x2C[23:18] = 0x2C[17:12] = CrystalCap
		PHY_SetBBReg(pAdapter, REG_MAC_PHY_CTRL, 0xFFF000, (CrystalCap | (CrystalCap << 6)));
	}
	else
	{
		PHY_SetBBReg(pAdapter, REG_MAC_PHY_CTRL, 0xFFF000, (CrystalCap | (CrystalCap << 6)));
	}
}
#endif
