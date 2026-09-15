/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <wifimg.h>
#include <wifimg_glue.h>
#include <os_net_sync_notify.h>
#include <xrlink_common.h>
#include <wifi_log.h>
#include <expand_cmd.h>
#include <xr_proto.h>

wmg_status_t wmg_xrlink_send_expand_cmd_shell(char *expand_cmd, void *expand_params, void *expand_cb)
{
	system(expand_cmd);
}

static int wmg_xrlink_send_expand_cmd_user_cb(char *data,uint32_t len)
{
	static int xr_frame_count = 0;
	xr_cfg_payload_t *pro_data=  (xr_cfg_payload_t*)data;
	xr_wifi_expand_cmd_data_t *payload = (xr_wifi_expand_cmd_data_t *)(pro_data->param);
	uint8_t *xr_frame;

	WMG_DEBUG("payload->type %d\n", payload->type);

	switch(payload->type) {
		case XR_WIFI_ID_EXPAND_CMD_STATUS:
		{
			uint32_t *cmd_state;

			cmd_state = payload->data;
			WMG_INFO("wifi xrlink expand cmd state:%d\n", *cmd_state);
			break;
		}
		case XR_WIFI_ID_EXPAND_CMD_RAW_DATA_RCV:
		{
			xr_frame = (xr_wifi_expand_cmd_data_t *)payload->data;
			xr_frame_count++;
			WMG_DEBUG("get wifi expand cmd frame data %d\n", xr_frame_count);
			if(xr_frame_count > 1000) {
				WMG_DEBUG("get wifi expand cmd 1000 frame data\n");
				xr_frame_count = 0;
			}
			break;
		}
		case XR_WIFI_ID_EXPAND_CMD_PRINTF:
		{
			char *rsp = (void *)payload->data;

			WMG_INFO("%s", rsp);
			break;
		}
		default:
			WMG_ERROR("unknow event type\n");
			break;
	}
	return WMG_STATUS_SUCCESS;
}

wmg_status_t wmg_xrlink_send_expand_cmd_gmac(char *expand_cmd, void *expand_params, void *expand_cb)
{
	char ifname[10] = {0};
	uint8_t *mac_addr = (uint8_t *)expand_cb;

	strcpy(ifname, expand_cmd);
	if(ifname == NULL) {
		WMG_ERROR("ifname is NULL\n");
		return WMG_STATUS_FAIL;
	}

	xr_wifi_mac_info_t *mac;

	xrlink_send_cmd(0, XR_WIFI_HOST_GET_MAC, NULL);
	mac = (xr_wifi_mac_info_t *)xr_get_dev_event_status(XR_WIFI_DEV_MAC);
	if(mac != NULL) {
		memcpy(mac_addr, mac->mac, 6);
		return WMG_STATUS_SUCCESS;
	}

	return WMG_STATUS_FAIL;
}

wmg_status_t wmg_xrlink_send_expand_cmd_user(char *expand_cmd, void *expand_params, void *expand_cb)
{
	wmg_status_t status = WMG_STATUS_UNHANDLED;

	WMG_DEBUG("cmd = %s\n", expand_cmd);
	xr_wifi_ext_cmd_info_t *e_cmd = (xr_wifi_ext_cmd_info_t *)malloc(sizeof(xr_wifi_ext_cmd_info_t) + strlen(expand_cmd) + 1);
	if(e_cmd == NULL) {
		WMG_ERROR("%s: e_cmd malloc fail\n", expand_cmd);
		return WMG_STATUS_FAIL;
	}

	xr_wifi_register_or_unregister_expand_msg_cb(wmg_xrlink_send_expand_cmd_user_cb);

	WMG_DEBUG("size = %d\n", sizeof(xr_wifi_ext_cmd_info_t) + strlen(expand_cmd) + 1);
	memset(e_cmd, 0, sizeof(xr_wifi_ext_cmd_info_t) + strlen(expand_cmd) + 1);
	e_cmd->len = strlen(expand_cmd) + 1; // include '\0'
	memcpy(e_cmd->ext_cmd_info, expand_cmd, e_cmd->len);
	status = xrlink_send_cmd(sizeof(xr_wifi_ext_cmd_info_t) + strlen(expand_cmd) + 1, XR_WIFI_HOST_EXPEND_CMD, e_cmd);
	free(e_cmd);
	return status;
}

wmg_status_t wmg_xrlink_send_expand_cmd_smac(char *expand_cmd, void *expand_params, void *expand_cb)
{
	wmg_status_t status = WMG_STATUS_UNHANDLED;
	uint8_t mac_addr[6] = {0};
	char ifname[10] = {0};
	char *pch;
	int i;

	pch = strtok(expand_cmd, ":");
	strcpy(ifname, pch);
	pch = strtok(NULL, ":");
	pch++;
	for(i = 0;(pch != NULL) && (i < 6); i++){
		mac_addr[i] = char2uint8(pch);
		pch = strtok(NULL, ":");
	}

	if(i != 6) {
		WMG_ERROR("%s: mac address format is incorrect\n", expand_cmd);
		return WMG_STATUS_FAIL;
	}

	xr_wifi_mac_info_t *mi = (xr_wifi_mac_info_t *)malloc(sizeof(xr_wifi_mac_info_t));

	memset(mi, 0, sizeof(xr_wifi_mac_info_t));
	memcpy(mi->mac, mac_addr, 6);
	memcpy(mi->ifname, ifname, 5);
	status = xrlink_send_cmd(sizeof(xr_wifi_mac_info_t), XR_WIFI_HOST_SET_MAC, mi);
	free(mi);
	return status;
}

wmg_status_t wmg_xrlink_send_expand_cmd_setip(char *expand_cmd, void *expand_params, void *expand_cb)
{
	wmg_status_t status = WMG_STATUS_UNHANDLED;
	uint8_t ip_addr[4] = {0};
	char *pch;
	int i;
	pch = strtok(expand_cmd, ".");
	for(i = 0;(pch != NULL) && (i < 4); i++){
		ip_addr[i] = atoi(pch);
		pch = strtok(NULL, ".");
		WMG_INFO("ip_addr[%d] = %d\n", i, ip_addr[i]);
	}

	if(i != 4) {
		WMG_ERROR("%s: IP address format is incorrect,i=%d\n", expand_cmd, i);
		return WMG_STATUS_FAIL;
	}

	xr_wifi_sta_ip_info_t *cfg = (xr_wifi_sta_ip_info_t *)malloc(sizeof(xr_wifi_sta_ip_info_t));
	memset(cfg, 0, sizeof(xr_wifi_sta_ip_info_t));
	memcpy(cfg->ip_addr, ip_addr, 4);
	memcpy(cfg->gw, cfg->ip_addr, 4);
	cfg->gw[3] = 1;
	memcpy(cfg->dns, cfg->gw, 4);
	cfg->netmask[0] = 255;
	cfg->netmask[1] = 255;
	cfg->netmask[2] = 255;
	cfg->netmask[3] = 0;

	status = xrlink_send_cmd(sizeof(xr_wifi_sta_ip_info_t), XR_WIFI_HOST_SET_STA_IP, cfg);
	free(cfg);
	return status;
}

wmg_status_t wmg_xrlink_send_expand_cmd_gccode(char *expand_cmd, void *expand_params, void *expand_cb)
{
	char gccode[8] = {0};
	xr_wifi_country_code_info_t *country_code = NULL;

	xrlink_send_cmd(0, XR_WIFI_HOST_GET_COUNTRY_CODE, NULL);

	country_code = (xr_wifi_country_code_info_t *)xr_get_dev_event_status(XR_WIFI_DEV_COUNTRY_CODE);

	if(country_code != NULL) {
		switch (country_code->code) {
			case WIFI_COUNTRY_CODE_AU:
				memcpy(gccode, "AU", 2);
				break;
			case WIFI_COUNTRY_CODE_CA:
				memcpy(gccode, "CA", 2);
				break;
			case WIFI_COUNTRY_CODE_CN:
				memcpy(gccode, "CN", 2);
				break;
			case WIFI_COUNTRY_CODE_DE:
				memcpy(gccode, "DE", 2);
				break;
			case WIFI_COUNTRY_CODE_EU:
				memcpy(gccode, "EU", 2);
				break;
			case WIFI_COUNTRY_CODE_FR:
				memcpy(gccode, "FR", 2);
				break;
			case WIFI_COUNTRY_CODE_JP:
				memcpy(gccode, "JP", 2);
				break;
			case WIFI_COUNTRY_CODE_RU:
				memcpy(gccode, "RU", 2);
				break;
			case WIFI_COUNTRY_CODE_SA:
				memcpy(gccode, "SA", 2);
				break;
			case WIFI_COUNTRY_CODE_US:
				memcpy(gccode, "US", 2);
				break;
			case WIFI_COUNTRY_CODE_NONE:
			default:
				memcpy(gccode, "OO", 2);
				break;
		}
		sprintf((char *)expand_cb, "%s", gccode);
		return WMG_STATUS_SUCCESS;
	} else {
		return WMG_STATUS_FAIL;
	}
}

wmg_status_t wmg_xrlink_send_expand_cmd_sccode(char *expand_cmd, void *expand_params, void *expand_cb)
{
	xr_wifi_country_code_info_t country_code;

	if (!strncmp(expand_cmd, "AU", 2)) {
		country_code.code = WIFI_COUNTRY_CODE_AU;
	} else if (!strncmp(expand_cmd, "CA", 2)) {
		country_code.code = WIFI_COUNTRY_CODE_CA;
	} else if (!strncmp(expand_cmd, "CN", 2)) {
		country_code.code = WIFI_COUNTRY_CODE_CN;
	} else if (!strncmp(expand_cmd, "DE", 2)) {
		country_code.code = WIFI_COUNTRY_CODE_DE;
	} else if (!strncmp(expand_cmd, "EU", 2)) {
		country_code.code = WIFI_COUNTRY_CODE_EU;
	} else if (!strncmp(expand_cmd, "FR", 2)) {
		country_code.code = WIFI_COUNTRY_CODE_FR;
	} else if (!strncmp(expand_cmd, "JP", 2)) {
		country_code.code = WIFI_COUNTRY_CODE_JP;
	} else if (!strncmp(expand_cmd, "RU", 2)) {
		country_code.code = WIFI_COUNTRY_CODE_RU;
	} else if (!strncmp(expand_cmd, "SA", 2)) {
		country_code.code = WIFI_COUNTRY_CODE_SA;
	} else if (!strncmp(expand_cmd, "US", 2)) {
		country_code.code = WIFI_COUNTRY_CODE_US;
	} else {
		return WMG_STATUS_FAIL;
	}

	return xrlink_send_cmd(sizeof(xr_wifi_country_code_info_t), XR_WIFI_HOST_SET_COUNTRY_CODE, &country_code);
}

wmg_status_t wmg_xrlink_send_expand_cmd(char *expand_cmd, void *expand_params, void *expand_cb)
{
	WMG_DEBUG("xrlink get exp cmd: %s\n", expand_cmd);
	if(!strncmp(expand_cmd, "shell:", 6)) {
		return wmg_xrlink_send_expand_cmd_shell((expand_cmd + 6), expand_params, expand_cb);
	} else if(!strncmp(expand_cmd, "gmac:", 5)) {
		return wmg_xrlink_send_expand_cmd_gmac((expand_cmd + 5), expand_params, expand_cb);
	} else if(!strncmp(expand_cmd, "smac:", 5)) {
		return wmg_xrlink_send_expand_cmd_smac((expand_cmd + 5), expand_params, expand_cb);
	} else if(!strncmp(expand_cmd, "user:", 5)) {
		return wmg_xrlink_send_expand_cmd_user((expand_cmd + 5), expand_params, expand_cb);
	} else if(!strncmp(expand_cmd, "staip:", 6)) {
		return wmg_xrlink_send_expand_cmd_setip((expand_cmd + 6), expand_params, expand_cb);
	} else if(!strncmp(expand_cmd, "gccode:", 7)) {
		return wmg_xrlink_send_expand_cmd_gccode((expand_cmd + 7), expand_params, expand_cb);
	} else if(!strncmp(expand_cmd, "sccode:", 7)) {
		return wmg_xrlink_send_expand_cmd_sccode((expand_cmd + 7), expand_params, expand_cb);
	} else {
		WMG_ERROR("unspport xrlink expand_cmd: %s\n", expand_cmd);
	}
	return WMG_STATUS_FAIL;
}
