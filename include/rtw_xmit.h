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
#ifndef _RTW_XMIT_H_
#define _RTW_XMIT_H_


#ifdef CONFIG_USB_TX_AGGREGATION
	#if defined(CONFIG_PLATFORM_ARM_SUNxI) || defined(CONFIG_PLATFORM_ARM_SUN6I) || defined(CONFIG_PLATFORM_ARM_SUN7I) || defined(CONFIG_PLATFORM_ARM_SUN8I)
		#define MAX_XMITBUF_SZ (12288)  //12k 1536*8
	#elif defined (CONFIG_PLATFORM_MSTAR)
		#define MAX_XMITBUF_SZ	7680	// 7.5k
	#else
	#define MAX_XMITBUF_SZ	(20480)	// 20k
	#endif
#else
#define MAX_XMITBUF_SZ	(2048)
#endif

#ifdef CONFIG_SINGLE_XMIT_BUF
#define NR_XMITBUFF	(1)
#else
#define NR_XMITBUFF	(4)
#endif //CONFIG_SINGLE_XMIT_BUF

#ifdef PLATFORM_OS_CE
#define XMITBUF_ALIGN_SZ 4
#else
#ifdef USB_XMITBUF_ALIGN_SZ
#define XMITBUF_ALIGN_SZ (USB_XMITBUF_ALIGN_SZ)
#else
#define XMITBUF_ALIGN_SZ 512
#endif
#endif

// xmit extension buff defination
#define MAX_XMIT_EXTBUF_SZ	(1536)

#ifdef CONFIG_SINGLE_XMIT_BUF
#define NR_XMIT_EXTBUFF	(1)
#else
#define NR_XMIT_EXTBUFF	(32)
#endif

#define MAX_CMDBUF_SZ	(5120)	//(4096)

#define MAX_NUMBLKS		(1)

#define XMIT_VO_QUEUE (0)
#define XMIT_VI_QUEUE (1)
#define XMIT_BE_QUEUE (2)
#define XMIT_BK_QUEUE (3)

#define VO_QUEUE_INX		0
#define VI_QUEUE_INX		1
#define BE_QUEUE_INX		2
#define BK_QUEUE_INX		3
#define BCN_QUEUE_INX		4
#define MGT_QUEUE_INX		5
#define HIGH_QUEUE_INX		6
#define TXCMD_QUEUE_INX	7

#define HW_QUEUE_ENTRY	8

#define WEP_IV(pattrib_iv, dot11txpn, keyidx)\
do{\
	pattrib_iv[0] = dot11txpn._byte_.TSC0;\
	pattrib_iv[1] = dot11txpn._byte_.TSC1;\
	pattrib_iv[2] = dot11txpn._byte_.TSC2;\
	pattrib_iv[3] = ((keyidx & 0x3)<<6);\
	dot11txpn.val = (dot11txpn.val == 0xffffff) ? 0: (dot11txpn.val+1);\
}while(0)


#define TKIP_IV(pattrib_iv, dot11txpn, keyidx)\
do{\
	pattrib_iv[0] = dot11txpn._byte_.TSC1;\
	pattrib_iv[1] = (dot11txpn._byte_.TSC1 | 0x20) & 0x7f;\
	pattrib_iv[2] = dot11txpn._byte_.TSC0;\
	pattrib_iv[3] = BIT(5) | ((keyidx & 0x3)<<6);\
	pattrib_iv[4] = dot11txpn._byte_.TSC2;\
	pattrib_iv[5] = dot11txpn._byte_.TSC3;\
	pattrib_iv[6] = dot11txpn._byte_.TSC4;\
	pattrib_iv[7] = dot11txpn._byte_.TSC5;\
	dot11txpn.val = dot11txpn.val == 0xffffffffffffULL ? 0: (dot11txpn.val+1);\
}while(0)

#define AES_IV(pattrib_iv, dot11txpn, keyidx)\
do{\
	pattrib_iv[0] = dot11txpn._byte_.TSC0;\
	pattrib_iv[1] = dot11txpn._byte_.TSC1;\
	pattrib_iv[2] = 0;\
	pattrib_iv[3] = BIT(5) | ((keyidx & 0x3)<<6);\
	pattrib_iv[4] = dot11txpn._byte_.TSC2;\
	pattrib_iv[5] = dot11txpn._byte_.TSC3;\
	pattrib_iv[6] = dot11txpn._byte_.TSC4;\
	pattrib_iv[7] = dot11txpn._byte_.TSC5;\
	dot11txpn.val = dot11txpn.val == 0xffffffffffffULL ? 0: (dot11txpn.val+1);\
}while(0)


#define HWXMIT_ENTRY	4

// For Buffer Descriptor ring architecture

#define TXDESC_SIZE 40

#ifdef CONFIG_TX_EARLY_MODE
#define EARLY_MODE_INFO_SIZE	8
#endif


#ifdef USB_PACKET_OFFSET_SZ
#define PACKET_OFFSET_SZ (USB_PACKET_OFFSET_SZ)
#else
#define PACKET_OFFSET_SZ (8)
#endif
#define TXDESC_OFFSET (TXDESC_SIZE + PACKET_OFFSET_SZ)

#ifdef CONFIG_WOWLAN
// The following foramt is 40 bytes tx description.
// It supports 8192E, 8723B, 8812a.
// Dword 0
#define GET_TX_DESC_OWN(__pTxDesc)			LE_BITS_TO_4BYTE(__pTxDesc, 31, 1)
#define SET_TX_DESC_PKT_SIZE(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc, 0, 16, __Value)
#define SET_TX_DESC_OFFSET(__pTxDesc, __Value)		SET_BITS_TO_LE_4BYTE(__pTxDesc, 16, 8, __Value)
#define SET_TX_DESC_BMC(__pTxDesc, __Value)		SET_BITS_TO_LE_4BYTE(__pTxDesc, 24, 1, __Value)
#define SET_TX_DESC_HTC(__pTxDesc, __Value)		SET_BITS_TO_LE_4BYTE(__pTxDesc, 25, 1, __Value)
#define SET_TX_DESC_LAST_SEG(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc, 26, 1, __Value)
#define SET_TX_DESC_FIRST_SEG(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc, 27, 1, __Value)
#define SET_TX_DESC_LINIP(__pTxDesc, __Value)		SET_BITS_TO_LE_4BYTE(__pTxDesc, 28, 1, __Value)
#define SET_TX_DESC_NO_ACM(__pTxDesc, __Value)		SET_BITS_TO_LE_4BYTE(__pTxDesc, 29, 1, __Value)
#define SET_TX_DESC_GF(__pTxDesc, __Value)		SET_BITS_TO_LE_4BYTE(__pTxDesc, 30, 1, __Value)
#define SET_TX_DESC_OWN(__pTxDesc, __Value)		SET_BITS_TO_LE_4BYTE(__pTxDesc, 31, 1, __Value)
// Dword 1
#define SET_TX_DESC_MACID(__pTxDesc, __Value)		SET_BITS_TO_LE_4BYTE(__pTxDesc+4, 0, 7, __Value)
#define SET_TX_DESC_QUEUE_SEL(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+4, 8, 5, __Value)
#define SET_TX_DESC_RDG_NAV_EXT(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+4, 13, 1, __Value)
#define SET_TX_DESC_LSIG_TXOP_EN(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+4, 14, 1, __Value)
#define SET_TX_DESC_PIFS(__pTxDesc, __Value)		SET_BITS_TO_LE_4BYTE(__pTxDesc+4, 15, 1, __Value)
#define SET_TX_DESC_RATE_ID(__pTxDesc, __Value)		SET_BITS_TO_LE_4BYTE(__pTxDesc+4, 16, 5, __Value)
#define SET_TX_DESC_EN_DESC_ID(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+4, 21, 1, __Value)
#define SET_TX_DESC_SEC_TYPE(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+4, 22, 2, __Value)
#define SET_TX_DESC_PKT_OFFSET(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+4, 24, 5, __Value)
// Dword 2
#define SET_TX_DESC_PAID(__pTxDesc, __Value)		SET_BITS_TO_LE_4BYTE(__pTxDesc+8, 0,  9, __Value)
#define SET_TX_DESC_CCA_RTS(__pTxDesc, __Value)		SET_BITS_TO_LE_4BYTE(__pTxDesc+8, 10, 2, __Value)
#define SET_TX_DESC_AGG_ENABLE(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+8, 12, 1, __Value)
#define SET_TX_DESC_RDG_ENABLE(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+8, 13, 1, __Value)
#define SET_TX_DESC_AGG_BREAK(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+8, 16, 1, __Value)
#define SET_TX_DESC_MORE_FRAG(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+8, 17, 1, __Value)
#define SET_TX_DESC_RAW(__pTxDesc, __Value)		SET_BITS_TO_LE_4BYTE(__pTxDesc+8, 18, 1, __Value)
#define SET_TX_DESC_SPE_RPT(__pTxDesc, __Value)		SET_BITS_TO_LE_4BYTE(__pTxDesc+8, 19, 1, __Value)
#define SET_TX_DESC_AMPDU_DENSITY(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+8, 20, 3, __Value)
#define SET_TX_DESC_BT_INT(__pTxDesc, __Value)		SET_BITS_TO_LE_4BYTE(__pTxDesc+8, 23, 1, __Value)
#define SET_TX_DESC_GID(__pTxDesc, __Value)		SET_BITS_TO_LE_4BYTE(__pTxDesc+8, 24, 6, __Value)
// Dword 3
#define SET_TX_DESC_WHEADER_LEN(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+12, 0, 4, __Value)
#define SET_TX_DESC_CHK_EN(__pTxDesc, __Value)		SET_BITS_TO_LE_4BYTE(__pTxDesc+12, 4, 1, __Value)
#define SET_TX_DESC_EARLY_MODE(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+12, 5, 1, __Value)
#define SET_TX_DESC_HWSEQ_SEL(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+12, 6, 2, __Value)
#define SET_TX_DESC_USE_RATE(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+12, 8, 1, __Value)
#define SET_TX_DESC_DISABLE_RTS_FB(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+12, 9, 1, __Value)
#define SET_TX_DESC_DISABLE_FB(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+12, 10, 1, __Value)
#define SET_TX_DESC_CTS2SELF(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+12, 11, 1, __Value)
#define SET_TX_DESC_RTS_ENABLE(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+12, 12, 1, __Value)
#define SET_TX_DESC_HW_RTS_ENABLE(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+12, 13, 1, __Value)
#define SET_TX_DESC_NAV_USE_HDR(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+12, 15, 1, __Value)
#define SET_TX_DESC_USE_MAX_LEN(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+12, 16, 1, __Value)
#define SET_TX_DESC_MAX_AGG_NUM(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+12, 17, 5, __Value)
#define SET_TX_DESC_NDPA(__pTxDesc, __Value)		SET_BITS_TO_LE_4BYTE(__pTxDesc+12, 22, 2, __Value)
#define SET_TX_DESC_AMPDU_MAX_TIME(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+12, 24, 8, __Value)
// Dword 4
#define SET_TX_DESC_TX_RATE(__pTxDesc, __Value)		SET_BITS_TO_LE_4BYTE(__pTxDesc+16, 0, 7, __Value)
#define SET_TX_DESC_DATA_RATE_FB_LIMIT(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+16, 8, 5, __Value)
#define SET_TX_DESC_RTS_RATE_FB_LIMIT(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+16, 13, 4, __Value)
#define SET_TX_DESC_RETRY_LIMIT_ENABLE(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+16, 17, 1, __Value)
#define SET_TX_DESC_DATA_RETRY_LIMIT(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+16, 18, 6, __Value)
#define SET_TX_DESC_RTS_RATE(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+16, 24, 5, __Value)
// Dword 5
#define SET_TX_DESC_DATA_SC(__pTxDesc, __Value)		SET_BITS_TO_LE_4BYTE(__pTxDesc+20, 0, 4, __Value)
#define SET_TX_DESC_DATA_SHORT(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+20, 4, 1, __Value)
#define SET_TX_DESC_DATA_BW(__pTxDesc, __Value)		SET_BITS_TO_LE_4BYTE(__pTxDesc+20, 5, 2, __Value)
#define SET_TX_DESC_DATA_LDPC(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+20, 7, 1, __Value)
#define SET_TX_DESC_DATA_STBC(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+20, 8, 2, __Value)
#define SET_TX_DESC_CTROL_STBC(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+20, 10, 2, __Value)
#define SET_TX_DESC_RTS_SHORT(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+20, 12, 1, __Value)
#define SET_TX_DESC_RTS_SC(__pTxDesc, __Value)		SET_BITS_TO_LE_4BYTE(__pTxDesc+20, 13, 4, __Value)
// Dword 6
#define SET_TX_DESC_SW_DEFINE(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+24, 0, 12, __Value)
#define SET_TX_DESC_ANTSEL_A(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+24, 16, 3, __Value)
#define SET_TX_DESC_ANTSEL_B(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+24, 19, 3, __Value)
#define SET_TX_DESC_ANTSEL_C(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+24, 22, 3, __Value)
#define SET_TX_DESC_ANTSEL_D(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+24, 25, 3, __Value)
// Dword 7
#define SET_TX_DESC_TX_DESC_CHECKSUM(__pTxDesc, __Value)SET_BITS_TO_LE_4BYTE(__pTxDesc+28, 0, 16, __Value)
#define SET_TX_DESC_USB_TXAGG_NUM(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+28, 24, 8, __Value)

// Dword 8
#define SET_TX_DESC_HWSEQ_EN(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+32, 15, 1, __Value)
// Dword 9
#define SET_TX_DESC_SEQ(__pTxDesc, __Value)		SET_BITS_TO_LE_4BYTE(__pTxDesc+36, 12, 12, __Value)
// Dword 10
#define SET_TX_DESC_TX_BUFFER_ADDRESS(__pTxDesc, __Value)SET_BITS_TO_LE_4BYTE(__pTxDesc+40, 0, 32, __Value)
#define GET_TX_DESC_TX_BUFFER_ADDRESS(__pTxDesc)	LE_BITS_TO_4BYTE(__pTxDesc+40, 0, 32)

// Dword 11
#define SET_TX_DESC_NEXT_DESC_ADDRESS(__pTxDesc, __Value)SET_BITS_TO_LE_4BYTE(__pTxDesc+48, 0, 32, __Value)
// 40 bytes tx description end.
#endif

enum TXDESC_SC{
	SC_DONT_CARE = 0x00,
	SC_UPPER= 0x01,
	SC_LOWER=0x02,
	SC_DUPLICATE=0x03
};

#define TXDESC_40_BYTES

struct tx_desc {
	unsigned int txdw0;
	unsigned int txdw1;
	unsigned int txdw2;
	unsigned int txdw3;
	unsigned int txdw4;
	unsigned int txdw5;
	unsigned int txdw6;
	unsigned int txdw7;

#if defined(TXDESC_40_BYTES) || defined(TXDESC_64_BYTES)
	unsigned int txdw8;
	unsigned int txdw9;
#endif // TXDESC_40_BYTES

#ifdef TXDESC_64_BYTES
	unsigned int txdw10;
	unsigned int txdw11;

	// 2008/05/15 MH Because PCIE HW memory R/W 4K limit. And now,  our descriptor
	// size is 40 bytes. If you use more than 102 descriptor( 103*40>4096), HW will execute
	// memoryR/W CRC error. And then all DMA fetch will fail. We must decrease descriptor
	// number or enlarge descriptor size as 64 bytes.
	unsigned int txdw12;
	unsigned int txdw13;
	unsigned int txdw14;
	unsigned int txdw15;
#endif
};

union txdesc {
	struct tx_desc txdesc;
	unsigned int value[TXDESC_SIZE>>2];
};

struct	hw_xmit	{
	//_lock xmit_lock;
	//_list	pending;
	_queue *sta_queue;
	//struct hw_txqueue *phwtxqueue;
	//sint	txcmdcnt;
	int	accnt;
};

//reduce size
struct pkt_attrib
{
	u8	type;
	u8	subtype;
	u8	bswenc;
	u8	dhcp_pkt;
	u16	ether_type;
	u16	seqnum;
	u8 	hw_ssn_sel;	//for HW_SEQ0,1,2,3
	u16	pkt_hdrlen;	//the original 802.3 pkt header len
	u16	hdrlen;		//the WLAN Header Len
	u32	pktlen;		//the original 802.3 pkt raw_data len (not include ether_hdr data)
	u32	last_txcmdsz;
	u8	nr_frags;
	u8	encrypt;	//when 0 indicate no encrypt. when non-zero, indicate the encrypt algorith
	u8	iv_len;
	u8	icv_len;
	u8	iv[18];
	u8	icv[16];
	u8	priority;
	u8	ack_policy;
	u8	mac_id;
	u8	vcs_mode;	//virtual carrier sense method
	u8	dst[ETH_ALEN];
	u8	src[ETH_ALEN];
	u8	ta[ETH_ALEN];
	u8	ra[ETH_ALEN];
	u8	key_idx;
	u8	qos_en;
	u8	ht_en;
	u8	raid;//rate adpative id
	u8	bwmode;
	u8	ch_offset;//PRIME_CHNL_OFFSET
	u8	sgi;//short GI
	u8	ampdu_en;//tx ampdu enable
	u8	ampdu_spacing; //ampdu_min_spacing for peer sta's rx
	u8	mdata;//more data bit
	u8	pctrl;//per packet txdesc control enable
	u8	triggered;//for ap mode handling Power Saving sta
	u8	qsel;
	u8	order;//order bit
	u8	eosp;
	u8	rate;
	u8	intel_proxim;
	u8	retry_ctrl;
	u8   mbssid;
	u8	ldpc;
	u8	stbc;
	struct sta_info * psta;
#ifdef CONFIG_TCP_CSUM_OFFLOAD_TX
	u8	hw_tcp_csum;
#endif

	u8 rtsen;
	u8 cts2self;
	union Keytype	dot11tkiptxmickey;
	//union Keytype	dot11tkiprxmickey;
	union Keytype	dot118021x_UncstKey;

#ifdef CONFIG_TDLS
	u8 direct_link;
	struct sta_info *ptdls_sta;
#endif //CONFIG_TDLS

	u8 icmp_pkt;

};

#define WLANHDR_OFFSET	64

#define NULL_FRAMETAG		(0x0)
#define DATA_FRAMETAG		0x01
#define L2_FRAMETAG		0x02
#define MGNT_FRAMETAG		0x03
#define AMSDU_FRAMETAG	0x04

#define EII_FRAMETAG		0x05
#define IEEE8023_FRAMETAG  0x06

#define MP_FRAMETAG		0x07

#define TXAGG_FRAMETAG	0x08

enum {
	XMITBUF_DATA = 0,
	XMITBUF_MGNT = 1,
	XMITBUF_CMD = 2,
};

struct  submit_ctx{
	u32 submit_time; /* */
	u32 timeout_ms; /* <0: not synchronous, 0: wait forever, >0: up to ms waiting */
	int status; /* status for operation */
	struct completion done;
};

enum {
	RTW_SCTX_SUBMITTED = -1,
	RTW_SCTX_DONE_SUCCESS = 0,
	RTW_SCTX_DONE_UNKNOWN,
	RTW_SCTX_DONE_TIMEOUT,
	RTW_SCTX_DONE_BUF_ALLOC,
	RTW_SCTX_DONE_BUF_FREE,
	RTW_SCTX_DONE_WRITE_PORT_ERR,
	RTW_SCTX_DONE_TX_DESC_NA,
	RTW_SCTX_DONE_TX_DENY,
	RTW_SCTX_DONE_CCX_PKT_FAIL,
	RTW_SCTX_DONE_DRV_STOP,
	RTW_SCTX_DONE_DEV_REMOVE,
	RTW_SCTX_DONE_CMD_ERROR,
};


void rtw_sctx_init(struct submit_ctx *sctx, int timeout_ms);
int rtw_sctx_wait(struct submit_ctx *sctx, const char *msg);
void rtw_sctx_done_err(struct submit_ctx **sctx, int status);
void rtw_sctx_done(struct submit_ctx **sctx);

struct xmit_buf
{
	_list	list;

	_adapter *padapter;

	u8 *pallocated_buf;

	u8 *pbuf;

	void *priv_data;

	u16 buf_tag; // 0: Normal xmitbuf, 1: extension xmitbuf, 2:cmd xmitbuf
	u16 flags;
	u32 alloc_sz;

	u32  len;

	struct submit_ctx *sctx;

	//u32 sz[8];
	u32	ff_hwaddr;

	PURB	pxmit_urb[8];
	dma_addr_t dma_transfer_addr;	/* (in) dma addr for transfer_buffer */
	u8 bpending[8];

	sint last[8];

#if defined(DBG_XMIT_BUF )|| defined(DBG_XMIT_BUF_EXT)
	u8 no;
#endif

};


struct xmit_frame
{
	_list	list;

	struct pkt_attrib attrib;

	_pkt *pkt;

	int	frame_tag;

	_adapter *padapter;

	u8	*buf_addr;

	struct xmit_buf *pxmitbuf;

#ifdef CONFIG_USB_TX_AGGREGATION
	u8	agg_num;
#endif
	s8	pkt_offset;

#ifdef CONFIG_XMIT_ACK
	u8 ack_report;
#endif

	u8 *alloc_addr; /* the actual address this xmitframe allocated */
	u8 ext_tag; /* 0:data, 1:mgmt */

};

struct tx_servq {
	_list	tx_pending;
	_queue	sta_pending;
	int qcnt;
};


struct sta_xmit_priv
{
	_lock	lock;
	sint	option;
	sint	apsd_setting;	//When bit mask is on, the associated edca queue supports APSD.


	//struct tx_servq blk_q[MAX_NUMBLKS];
	struct tx_servq	be_q;			//priority == 0,3
	struct tx_servq	bk_q;			//priority == 1,2
	struct tx_servq	vi_q;			//priority == 4,5
	struct tx_servq	vo_q;			//priority == 6,7
	_list	legacy_dz;
	_list  apsd;

	u16 txseq_tid[16];

	//uint	sta_tx_bytes;
	//u64	sta_tx_pkts;
	//uint	sta_tx_fail;


};


struct	hw_txqueue	{
	volatile sint	head;
	volatile sint	tail;
	volatile sint	free_sz;	//in units of 64 bytes
	volatile sint      free_cmdsz;
	volatile sint	 txsz[8];
	uint	ff_hwaddr;
	uint	cmd_hwaddr;
	sint	ac_tag;
};

struct agg_pkt_info{
	u16 offset;
	u16 pkt_len;
};

enum cmdbuf_type {
	CMDBUF_BEACON = 0x00,
	CMDBUF_RSVD,
	CMDBUF_MAX
};

u8 rtw_get_hwseq_no(_adapter *padapter);

struct	xmit_priv	{

	_lock	lock;

	_sema	xmit_sema;
	_sema	terminate_xmitthread_sema;

	//_queue	blk_strms[MAX_NUMBLKS];
	_queue	be_pending;
	_queue	bk_pending;
	_queue	vi_pending;
	_queue	vo_pending;
	_queue	bm_pending;

	//_queue	legacy_dz_queue;
	//_queue	apsd_queue;

	u8 *pallocated_frame_buf;
	u8 *pxmit_frame_buf;
	uint free_xmitframe_cnt;
	_queue	free_xmit_queue;

	//uint mapping_addr;
	//uint pkt_sz;

	u8 *xframe_ext_alloc_addr;
	u8 *xframe_ext;
	uint free_xframe_ext_cnt;
	_queue free_xframe_ext_queue;

	//struct	hw_txqueue	be_txqueue;
	//struct	hw_txqueue	bk_txqueue;
	//struct	hw_txqueue	vi_txqueue;
	//struct	hw_txqueue	vo_txqueue;
	//struct	hw_txqueue	bmc_txqueue;

	uint	frag_len;

	_adapter	*adapter;

	u8   vcs_setting;
	u8	vcs;
	u8	vcs_type;
	//u16  rts_thresh;

	u64	tx_bytes;
	u64	tx_pkts;
	u64	tx_drop;
	u64	last_tx_pkts;

	struct hw_xmit *hwxmits;
	u8	hwxmit_entry;

	u8	wmm_para_seq[4];//sequence for wmm ac parameter strength from large to small. it's value is 0->vo, 1->vi, 2->be, 3->bk.

	_sema	tx_retevt;//all tx return event;
	u8		txirp_cnt;//

	struct tasklet_struct xmit_tasklet;
	//per AC pending irp
	int beq_cnt;
	int bkq_cnt;
	int viq_cnt;
	int voq_cnt;

	_queue free_xmitbuf_queue;
	_queue pending_xmitbuf_queue;
	u8 *pallocated_xmitbuf;
	u8 *pxmitbuf;
	uint free_xmitbuf_cnt;

	_queue free_xmit_extbuf_queue;
	u8 *pallocated_xmit_extbuf;
	u8 *pxmit_extbuf;
	uint free_xmit_extbuf_cnt;

	struct xmit_buf	pcmd_xmitbuf[CMDBUF_MAX];
	u8   hw_ssn_seq_no;//mapping to REG_HW_SEQ 0,1,2,3
	u16	nqos_ssn;
	#ifdef CONFIG_TX_EARLY_MODE

	#define MAX_AGG_PKT_NUM 256 //Max tx ampdu coounts

	struct agg_pkt_info agg_pkt[MAX_AGG_PKT_NUM];
	#endif

#ifdef CONFIG_XMIT_ACK
	int	ack_tx;
	_mutex ack_tx_mutex;
	struct submit_ctx ack_tx_ops;
	u8 seq_no;
#endif
	_lock lock_sctx;
};

extern struct xmit_frame *__rtw_alloc_cmdxmitframe(struct xmit_priv *pxmitpriv,
		enum cmdbuf_type buf_type);
#define rtw_alloc_cmdxmitframe(p) __rtw_alloc_cmdxmitframe(p, CMDBUF_RSVD)
#define rtw_alloc_bcnxmitframe(p) __rtw_alloc_cmdxmitframe(p, CMDBUF_BEACON)

extern struct xmit_buf *rtw_alloc_xmitbuf_ext(struct xmit_priv *pxmitpriv);
extern s32 rtw_free_xmitbuf_ext(struct xmit_priv *pxmitpriv, struct xmit_buf *pxmitbuf);

extern struct xmit_buf *rtw_alloc_xmitbuf(struct xmit_priv *pxmitpriv);
extern s32 rtw_free_xmitbuf(struct xmit_priv *pxmitpriv, struct xmit_buf *pxmitbuf);

void rtw_count_tx_stats(_adapter *padapter, struct xmit_frame *pxmitframe, int sz);
extern void rtw_update_protection(_adapter *padapter, u8 *ie, uint ie_len);
static s32 update_attrib_sec_info(_adapter *padapter, struct pkt_attrib *pattrib, struct sta_info *psta);
static void update_attrib_phy_info(_adapter *padapter, struct pkt_attrib *pattrib, struct sta_info *psta);
extern s32 rtw_make_wlanhdr(_adapter *padapter, u8 *hdr, struct pkt_attrib *pattrib);
extern s32 rtw_put_snap(u8 *data, u16 h_proto);

extern struct xmit_frame *rtw_alloc_xmitframe(struct xmit_priv *pxmitpriv);
struct xmit_frame *rtw_alloc_xmitframe_ext(struct xmit_priv *pxmitpriv);
struct xmit_frame *rtw_alloc_xmitframe_once(struct xmit_priv *pxmitpriv);
extern s32 rtw_free_xmitframe(struct xmit_priv *pxmitpriv, struct xmit_frame *pxmitframe);
extern void rtw_free_xmitframe_queue(struct xmit_priv *pxmitpriv, _queue *pframequeue);
struct tx_servq *rtw_get_sta_pending(_adapter *padapter, struct sta_info *psta, sint up, u8 *ac);
extern s32 rtw_xmitframe_enqueue(_adapter *padapter, struct xmit_frame *pxmitframe);
extern struct xmit_frame* rtw_dequeue_xframe(struct xmit_priv *pxmitpriv, struct hw_xmit *phwxmit_i, sint entry);

extern s32 rtw_xmit_classifier(_adapter *padapter, struct xmit_frame *pxmitframe);
extern u32 rtw_calculate_wlan_pkt_size_by_attribue(struct pkt_attrib *pattrib);
#define rtw_wlan_pkt_size(f) rtw_calculate_wlan_pkt_size_by_attribue(&f->attrib)
extern s32 rtw_xmitframe_coalesce(_adapter *padapter, _pkt *pkt, struct xmit_frame *pxmitframe);
#ifdef CONFIG_IEEE80211W
extern s32 rtw_mgmt_xmitframe_coalesce(_adapter *padapter, _pkt *pkt, struct xmit_frame *pxmitframe);
#endif //CONFIG_IEEE80211W
#ifdef CONFIG_TDLS
extern struct tdls_txmgmt *ptxmgmt;
s32 rtw_xmit_tdls_coalesce(_adapter *padapter, struct xmit_frame *pxmitframe, struct tdls_txmgmt *ptxmgmt);
s32 update_tdls_attrib(_adapter *padapter, struct pkt_attrib *pattrib);
#endif
s32 _rtw_init_hw_txqueue(struct hw_txqueue* phw_txqueue, u8 ac_tag);
void _rtw_init_sta_xmit_priv(struct sta_xmit_priv *psta_xmitpriv);


s32 rtw_txframes_pending(_adapter *padapter);
s32 rtw_txframes_sta_ac_pending(_adapter *padapter, struct pkt_attrib *pattrib);
void rtw_init_hwxmits(struct hw_xmit *phwxmit, sint entry);


s32 _rtw_init_xmit_priv(struct xmit_priv *pxmitpriv, _adapter *padapter);
void _rtw_free_xmit_priv (struct xmit_priv *pxmitpriv);


void rtw_alloc_hwxmits(_adapter *padapter);
void rtw_free_hwxmits(_adapter *padapter);


s32 rtw_xmit(_adapter *padapter, _pkt **pkt);
bool xmitframe_hiq_filter(struct xmit_frame *xmitframe);
#if defined(CONFIG_AP_MODE) || defined(CONFIG_TDLS)
sint xmitframe_enqueue_for_sleeping_sta(_adapter *padapter, struct xmit_frame *pxmitframe);
void stop_sta_xmit(_adapter *padapter, struct sta_info *psta);
void wakeup_sta_to_xmit(_adapter *padapter, struct sta_info *psta);
void xmit_delivery_enabled_frames(_adapter *padapter, struct sta_info *psta);
#endif

u8	query_ra_short_GI(struct sta_info *psta);

u8	qos_acm(u8 acm_mask, u8 priority);

#ifdef CONFIG_XMIT_THREAD_MODE
void	enqueue_pending_xmitbuf(struct xmit_priv *pxmitpriv, struct xmit_buf *pxmitbuf);
void enqueue_pending_xmitbuf_to_head(struct xmit_priv *pxmitpriv, struct xmit_buf *pxmitbuf);
struct xmit_buf*	dequeue_pending_xmitbuf(struct xmit_priv *pxmitpriv);
struct xmit_buf*	dequeue_pending_xmitbuf_under_survey(struct xmit_priv *pxmitpriv);
sint	check_pending_xmitbuf(struct xmit_priv *pxmitpriv);
thread_return	rtw_xmit_thread(thread_context context);
#endif

static void do_queue_select(_adapter * padapter, struct pkt_attrib * pattrib);
u32	rtw_get_ff_hwaddr(struct xmit_frame	*pxmitframe);

#ifdef CONFIG_XMIT_ACK
int rtw_ack_tx_wait(struct xmit_priv *pxmitpriv, u32 timeout_ms);
void rtw_ack_tx_done(struct xmit_priv *pxmitpriv, int status);
#endif //CONFIG_XMIT_ACK


//include after declaring struct xmit_buf, in order to avoid warning
#include <xmit_osdep.h>

#endif	//_RTL871X_XMIT_H_
