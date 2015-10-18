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
#define _HAL_USB_C_

#include <drv_types.h>
#include <hal_data.h>


#ifdef CONFIG_USB_SUPPORT_ASYNC_VDN_REQ
int usb_write_async(struct usb_device *udev, u32 addr, void *pdata, u16 len)
{
	u8 request;
	u8 requesttype;
	u16 wvalue;
	u16 index;
	int ret;

	requesttype = VENDOR_WRITE;//write_out
	request = REALTEK_USB_VENQT_CMD_REQ;
	index = REALTEK_USB_VENQT_CMD_IDX;//n/a

	wvalue = (u16)(addr&0x0000ffff);

	ret = _usbctrl_vendorreq_async_write(udev, request, wvalue, index, pdata, len, requesttype);

	return ret;
}

int usb_async_write8(struct intf_hdl *pintfhdl, u32 addr, u8 val)
{
	u8 data;
	int ret;
	struct dvobj_priv  *pdvobjpriv = (struct dvobj_priv  *)pintfhdl->pintf_dev;
	struct usb_device *udev=pdvobjpriv->pusbdev;

	_func_enter_;
	data = val;
	ret = usb_write_async(udev, addr, &data, 1);
	_func_exit_;

	return ret;
}

int usb_async_write16(struct intf_hdl *pintfhdl, u32 addr, u16 val)
{
	u16 data;
	int ret;
	struct dvobj_priv  *pdvobjpriv = (struct dvobj_priv  *)pintfhdl->pintf_dev;
	struct usb_device *udev=pdvobjpriv->pusbdev;

	_func_enter_;
	data = val;
	ret = usb_write_async(udev, addr, &data, 2);
	_func_exit_;

	return ret;
}

int usb_async_write32(struct intf_hdl *pintfhdl, u32 addr, u32 val)
{
	u32 data;
	int ret;
	struct dvobj_priv  *pdvobjpriv = (struct dvobj_priv  *)pintfhdl->pintf_dev;
	struct usb_device *udev=pdvobjpriv->pusbdev;

	_func_enter_;
	data = val;
	ret = usb_write_async(udev, addr, &data, 4);
	_func_exit_;

	return ret;
}
#endif /* CONFIG_USB_SUPPORT_ASYNC_VDN_REQ */



/*
	92DU chip needs to remask "value" parameter,  this function only works in 92DU chip.
*/
static inline void usb_value_remask(struct intf_hdl *pintfhdl, u16 *value)
{
}

u8 usb_read8(struct intf_hdl *pintfhdl, u32 addr)
{
	u8 request;
	u8 requesttype;
	u16 wvalue;
	u16 index;
	u16 len;
	u8 data=0;

	_func_enter_;

	request = 0x05;
	requesttype = 0x01;//read_in
	index = 0;//n/a

	wvalue = (u16)(addr&0x0000ffff);
	len = 1;
	usb_value_remask(pintfhdl, &wvalue);
	usbctrl_vendorreq(pintfhdl, request, wvalue, index,
					&data, len, requesttype);

	_func_exit_;

	return data;
}

u16 usb_read16(struct intf_hdl *pintfhdl, u32 addr)
{
	u8 request;
	u8 requesttype;
	u16 wvalue;
	u16 index;
	u16 len;
	u16 data=0;

	_func_enter_;

	request = 0x05;
	requesttype = 0x01;//read_in
	index = 0;//n/a

	wvalue = (u16)(addr&0x0000ffff);
	len = 2;
	usb_value_remask(pintfhdl, &wvalue);
	usbctrl_vendorreq(pintfhdl, request, wvalue, index,
					&data, len, requesttype);

	_func_exit_;

	return data;

}

u32 usb_read32(struct intf_hdl *pintfhdl, u32 addr)
{
	u8 request;
	u8 requesttype;
	u16 wvalue;
	u16 index;
	u16 len;
	u32 data=0;

	_func_enter_;

	request = 0x05;
	requesttype = 0x01;//read_in
	index = 0;//n/a

	wvalue = (u16)(addr&0x0000ffff);
	len = 4;

	usb_value_remask(pintfhdl, &wvalue);
	usbctrl_vendorreq(pintfhdl, request, wvalue, index,
					&data, len, requesttype);

	_func_exit_;

	return data;
}

int usb_write8(struct intf_hdl *pintfhdl, u32 addr, u8 val)
{
	u8 request;
	u8 requesttype;
	u16 wvalue;
	u16 index;
	u16 len;
	u8 data;
	int ret;

	_func_enter_;

	request = 0x05;
	requesttype = 0x00;//write_out
	index = 0;//n/a

	wvalue = (u16)(addr&0x0000ffff);
	len = 1;

	data = val;
	usb_value_remask(pintfhdl, &wvalue);
	ret = usbctrl_vendorreq(pintfhdl, request, wvalue, index,
						&data, len, requesttype);

	_func_exit_;

	return ret;
}

int usb_write16(struct intf_hdl *pintfhdl, u32 addr, u16 val)
{
	u8 request;
	u8 requesttype;
	u16 wvalue;
	u16 index;
	u16 len;
	u16 data;
	int ret;

	_func_enter_;

	request = 0x05;
	requesttype = 0x00;//write_out
	index = 0;//n/a

	wvalue = (u16)(addr&0x0000ffff);
	len = 2;

	data = val;
	usb_value_remask(pintfhdl, &wvalue);
	ret = usbctrl_vendorreq(pintfhdl, request, wvalue, index,
						&data, len, requesttype);

	_func_exit_;

	return ret;

}

int usb_write32(struct intf_hdl *pintfhdl, u32 addr, u32 val)
{
	u8 request;
	u8 requesttype;
	u16 wvalue;
	u16 index;
	u16 len;
	u32 data;
	int ret;

	_func_enter_;

	request = 0x05;
	requesttype = 0x00;//write_out
	index = 0;//n/a

	wvalue = (u16)(addr&0x0000ffff);
	len = 4;
	data =val;
	usb_value_remask(pintfhdl, &wvalue);
	ret = usbctrl_vendorreq(pintfhdl, request, wvalue, index,
						&data, len, requesttype);

	_func_exit_;

	return ret;

}

int usb_writeN(struct intf_hdl *pintfhdl, u32 addr, u32 length, u8 *pdata)
{
	u8 request;
	u8 requesttype;
	u16 wvalue;
	u16 index;
	u16 len;
	u8 buf[VENDOR_CMD_MAX_DATA_LEN]={0};
	int ret;

	_func_enter_;

	request = 0x05;
	requesttype = 0x00;//write_out
	index = 0;//n/a

	wvalue = (u16)(addr&0x0000ffff);
	len = length;
	 _rtw_memcpy(buf, pdata, len );
	usb_value_remask(pintfhdl, &wvalue);
	ret = usbctrl_vendorreq(pintfhdl, request, wvalue, index,
						buf, len, requesttype);

	_func_exit_;

	return ret;

}
