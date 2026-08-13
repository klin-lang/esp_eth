/* W5500 SPI Ethernet bring-up for Klin under ESP-IDF v5.x.
 * Requires sdkconfig: CONFIG_ETH_USE_SPI_ETHERNET + CONFIG_ETH_SPI_ETHERNET_W5500.
 */
#include "w5500_idf.h"

#include <stdio.h>
#include <string.h>

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_eth.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "nvs_flash.h"

#define KLIN_ETH_GOT_IP_BIT BIT0
#define KLIN_ETH_LINK_BIT   BIT1

static EventGroupHandle_t s_eth_event_group;
static esp_eth_handle_t s_eth_handle;
static esp_netif_t *s_eth_netif;
static esp_eth_netif_glue_handle_t s_eth_glue;
static uint32_t s_ip_u32;
static int s_started;
static int s_link_up;
static spi_device_interface_config_t s_spi_devcfg;

static void klin_eth_event_handler(void *arg, esp_event_base_t event_base,
                                   int32_t event_id, void *event_data)
{
    (void)arg;
    (void)event_data;
    if (event_base == ETH_EVENT) {
        if (event_id == ETHERNET_EVENT_CONNECTED) {
            s_link_up = 1;
            xEventGroupSetBits(s_eth_event_group, KLIN_ETH_LINK_BIT);
        } else if (event_id == ETHERNET_EVENT_DISCONNECTED) {
            s_link_up = 0;
            xEventGroupClearBits(s_eth_event_group, KLIN_ETH_LINK_BIT);
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_ETH_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        s_ip_u32 = (uint32_t)event->ip_info.ip.addr;
        xEventGroupSetBits(s_eth_event_group, KLIN_ETH_GOT_IP_BIT);
    }
}

int klin_eth_w5500_start(int spi_host, int mosi, int miso, int sclk, int cs,
                         int int_gpio, int rst_gpio, int clock_mhz, int poll_ms)
{
    esp_err_t err;
    eth_mac_config_t mac_config;
    eth_phy_config_t phy_config;
    eth_w5500_config_t w5500_config;
    esp_eth_mac_t *mac;
    esp_eth_phy_t *phy;
    esp_eth_config_t eth_config;
    esp_netif_config_t netif_cfg;
    spi_bus_config_t buscfg;

    if (s_started) {
        return (int)ESP_OK;
    }
    if (clock_mhz <= 0) {
        clock_mhz = 20;
    }
    if (poll_ms < 0) {
        poll_ms = 10;
    }

    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        err = nvs_flash_erase();
        if (err != ESP_OK) {
            return (int)err;
        }
        err = nvs_flash_init();
    }
    if (err != ESP_OK) {
        return (int)err;
    }

    err = esp_netif_init();
    if (err != ESP_OK) {
        return (int)err;
    }

    err = esp_event_loop_create_default();
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        return (int)err;
    }

    s_eth_event_group = xEventGroupCreate();
    if (s_eth_event_group == NULL) {
        return (int)ESP_ERR_NO_MEM;
    }

    if (int_gpio >= 0) {
        err = gpio_install_isr_service(0);
        if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
            return (int)err;
        }
    }

    memset(&buscfg, 0, sizeof(buscfg));
    buscfg.miso_io_num = miso;
    buscfg.mosi_io_num = mosi;
    buscfg.sclk_io_num = sclk;
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;
    err = spi_bus_initialize((spi_host_device_t)spi_host, &buscfg, SPI_DMA_CH_AUTO);
    if (err != ESP_OK) {
        return (int)err;
    }

    mac_config = (eth_mac_config_t)ETH_MAC_DEFAULT_CONFIG();
    phy_config = (eth_phy_config_t)ETH_PHY_DEFAULT_CONFIG();
    phy_config.phy_addr = 1;
    phy_config.reset_gpio_num = rst_gpio;

    memset(&s_spi_devcfg, 0, sizeof(s_spi_devcfg));
    s_spi_devcfg.mode = 0;
    s_spi_devcfg.clock_speed_hz = clock_mhz * 1000 * 1000;
    s_spi_devcfg.queue_size = 20;
    s_spi_devcfg.spics_io_num = cs;

    w5500_config = (eth_w5500_config_t)ETH_W5500_DEFAULT_CONFIG(
        (spi_host_device_t)spi_host, &s_spi_devcfg);
    w5500_config.int_gpio_num = int_gpio;
    w5500_config.poll_period_ms = (int_gpio < 0) ? (uint32_t)poll_ms : 0;

    mac = esp_eth_mac_new_w5500(&w5500_config, &mac_config);
    phy = esp_eth_phy_new_w5500(&phy_config);
    if (mac == NULL || phy == NULL) {
        return (int)ESP_FAIL;
    }

    eth_config = (esp_eth_config_t)ETH_DEFAULT_CONFIG(mac, phy);
    err = esp_eth_driver_install(&eth_config, &s_eth_handle);
    if (err != ESP_OK) {
        return (int)err;
    }

    netif_cfg = (esp_netif_config_t)ESP_NETIF_DEFAULT_ETH();
    s_eth_netif = esp_netif_new(&netif_cfg);
    if (s_eth_netif == NULL) {
        return (int)ESP_FAIL;
    }
    s_eth_glue = esp_eth_new_netif_glue(s_eth_handle);
    err = esp_netif_attach(s_eth_netif, s_eth_glue);
    if (err != ESP_OK) {
        return (int)err;
    }

    err = esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID,
                                     &klin_eth_event_handler, NULL);
    if (err != ESP_OK) {
        return (int)err;
    }
    err = esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP,
                                     &klin_eth_event_handler, NULL);
    if (err != ESP_OK) {
        return (int)err;
    }

    s_ip_u32 = 0;
    s_link_up = 0;
    xEventGroupClearBits(s_eth_event_group,
                         KLIN_ETH_GOT_IP_BIT | KLIN_ETH_LINK_BIT);

    err = esp_eth_start(s_eth_handle);
    if (err != ESP_OK) {
        return (int)err;
    }

    s_started = 1;
    return (int)ESP_OK;
}

int klin_eth_wait_link(int timeout_ms)
{
    TickType_t ticks;
    EventBits_t bits;

    if (!s_started || s_eth_event_group == NULL) {
        return (int)ESP_ERR_INVALID_STATE;
    }

    if (timeout_ms < 0) {
        ticks = portMAX_DELAY;
    } else {
        ticks = pdMS_TO_TICKS((uint32_t)timeout_ms);
    }

    bits = xEventGroupWaitBits(s_eth_event_group, KLIN_ETH_LINK_BIT, pdFALSE,
                               pdFALSE, ticks);
    if (bits & KLIN_ETH_LINK_BIT) {
        return (int)ESP_OK;
    }
    return (int)ESP_ERR_TIMEOUT;
}

int klin_eth_link_up(void)
{
    return s_link_up ? 1 : 0;
}

int klin_eth_wait_ip(int timeout_ms)
{
    TickType_t ticks;
    EventBits_t bits;

    if (!s_started || s_eth_event_group == NULL) {
        return (int)ESP_ERR_INVALID_STATE;
    }

    if (timeout_ms < 0) {
        ticks = portMAX_DELAY;
    } else {
        ticks = pdMS_TO_TICKS((uint32_t)timeout_ms);
    }

    bits = xEventGroupWaitBits(s_eth_event_group, KLIN_ETH_GOT_IP_BIT, pdFALSE,
                               pdFALSE, ticks);
    if (bits & KLIN_ETH_GOT_IP_BIT) {
        return (int)ESP_OK;
    }
    return (int)ESP_ERR_TIMEOUT;
}

uint32_t klin_eth_ip_u32(void)
{
    return s_ip_u32;
}

void klin_eth_log_ip(void)
{
    uint32_t a = s_ip_u32;
    printf("klin_eth: ip %u.%u.%u.%u\n", (unsigned)(a & 0xffu),
           (unsigned)((a >> 8) & 0xffu), (unsigned)((a >> 16) & 0xffu),
           (unsigned)((a >> 24) & 0xffu));
}

void klin_eth_log_mac(void)
{
    uint8_t mac[6];

    if (!s_started || s_eth_handle == NULL) {
        printf("klin_eth: mac (not started)\n");
        return;
    }
    if (esp_eth_ioctl(s_eth_handle, ETH_CMD_G_MAC_ADDR, mac) != ESP_OK) {
        printf("klin_eth: mac ioctl failed\n");
        return;
    }
    printf("klin_eth: mac %02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1],
           mac[2], mac[3], mac[4], mac[5]);
}

int klin_eth_stop(void)
{
    if (!s_started || s_eth_handle == NULL) {
        return (int)ESP_ERR_INVALID_STATE;
    }
    return (int)esp_eth_stop(s_eth_handle);
}
