/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the people's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>
#include "wifimg.h"
#include <wifi_log.h>

#ifndef UNREGISTER_CB
//Callback functions cannot do high-load and blocking actions
void wifi_msg_cb(wifi_msg_data_t *msg)
{
	if(msg->id == WIFI_MSG_ID_STA_STATE_CHANGE) {
		WMG_DEBUG("get sta state:(%d) ", msg->data.state);
		switch(msg->data.state) {
			case WIFI_STA_CONNECTING:
			WMG_INFO("Connecting......\n");
			break;
			case WIFI_STA_CONNECTED:
			WMG_INFO("Connected to the AP\n");
			break;
			case WIFI_STA_OBTAINING_IP:
			WMG_INFO("Obtaining ip address......\n");
			break;
			case WIFI_STA_NET_CONNECTED:
			WMG_DEBUG("Successful net connected\n");
			break;
			case WIFI_STA_DISCONNECTED:
			WMG_INFO("Disconnected\n");
			break;
		}
	}
}
#endif

void handler(int sig)
{
	WMG_INFO("get a sig, num is %d\n",sig);
	if(sig == 2){
		wifi_off(WIFI_MODE_UNKNOWN);
		wifimanager_deinit();
		WMG_INFO("Exit sta mode simple demo\n");
		exit(0);
	}
}

int main(int argc, char *argv[])
{
	wifi_sta_cn_para_t cn_para;
	int ret_exit = -1;

	if (argc != 3) {
		fprintf(stderr, "Usage: wifi_sta SSID PASSWORD\n");
		return 1;
	}

	signal(SIGINT, handler);

	/* wifimanager init */
	wifimanager_init();
	/* register callback function */
	if(wifi_register_msg_cb(wifi_msg_cb, NULL)) {
		WMG_ERROR("register msg cb failed\n");
	}

	/* open sta mode */
	if(wifi_on(WIFI_STATION)) {
		WMG_ERROR("sta mode simple demo: open sta mode failed, exit now\n");
		goto exit;
	}

	/* connect wifi */
	memset(&cn_para, 0, sizeof(wifi_sta_cn_para_t));
	cn_para.ssid = argv[1];
	cn_para.password = argv[2];
	cn_para.sec = WIFI_SEC_UNKNOWN;
	if(wifi_sta_connect(&cn_para) == 0) {
		WMG_INFO("==Wi-Fi ssid: %s password: %s sec: %d connect successful==\n",
				cn_para.ssid, cn_para.password, cn_para.sec);
	} else {
		WMG_ERROR("==Wi-Fi connect failed==\n");
		goto exit;
	}

	while(1) {
		sleep(1);
	}

	ret_exit = 0;

exit:
	wifimanager_deinit();
	WMG_INFO("Exit sta mode simple demo\n");
	exit(ret_exit);
}
