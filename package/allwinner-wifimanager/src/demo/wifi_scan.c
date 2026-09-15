#include <signal.h>
#include <stdio.h>
#include <string.h>

#include <wifi_log.h>
#include <wifimg.h>

#define SCAN_RESULT_MAX 32

static void wifi_msg_cb(wifi_msg_data_t *msg)
{
	if (msg->id != WIFI_MSG_ID_STA_STATE_CHANGE)
		return;

	switch (msg->data.state) {
	case WIFI_SCAN_STARTED:
		printf("scan started\n");
		break;
	case WIFI_SCAN_SUCCESS:
	case WIFI_SCAN_RESULTS:
		printf("scan completed\n");
		break;
	case WIFI_SCAN_FAILED:
		fprintf(stderr, "scan failed\n");
		break;
	default:
		break;
	}
}

static void print_security(wifi_sec sec)
{
	switch (sec) {
	case WIFI_SEC_NONE:
		printf("none");
		break;
	case WIFI_SEC_WEP:
		printf("wep");
		break;
	case WIFI_SEC_WPA_PSK:
		printf("wpa-psk");
		break;
	case WIFI_SEC_WPA2_PSK:
		printf("wpa2-psk");
		break;
	case WIFI_SEC_WPA3_PSK:
		printf("wpa3-psk");
		break;
	default:
		printf("unknown");
		break;
	}
}

int main(void)
{
	wifi_scan_result_t results[SCAN_RESULT_MAX];
	uint32_t count = SCAN_RESULT_MAX;
	int ret = 1;

	signal(SIGINT, SIG_DFL);
	memset(results, 0, sizeof(results));

	if (wifimanager_init() != WMG_STATUS_SUCCESS) {
		fprintf(stderr, "wifimanager init failed\n");
		return 1;
	}

	wifi_register_msg_cb(wifi_msg_cb, NULL);
	if (wifi_on(WIFI_STATION) != WMG_STATUS_SUCCESS) {
		fprintf(stderr, "wifi on failed\n");
		goto out;
	}

	if (wifi_get_scan_results(results, NULL, &count, SCAN_RESULT_MAX) !=
	    WMG_STATUS_SUCCESS) {
		fprintf(stderr, "wifi scan failed\n");
		wifi_off(WIFI_STATION);
		goto out;
	}

	printf("%-32s %-17s %8s %s\n", "SSID", "BSSID", "RSSI", "SECURITY");
	for (uint32_t i = 0; i < count; i++) {
		printf("%-32s %02x:%02x:%02x:%02x:%02x:%02x %8d ",
		       results[i].ssid,
		       results[i].bssid[0], results[i].bssid[1],
		       results[i].bssid[2], results[i].bssid[3],
		       results[i].bssid[4], results[i].bssid[5],
		       results[i].rssi);
		print_security(results[i].key_mgmt);
		putchar('\n');
	}

	wifi_off(WIFI_STATION);
	ret = 0;

out:
	wifimanager_deinit();
	return ret;
}
