/* Thin Ethernet helpers for Klin — ESP-IDF v5.x esp_eth.
 * Backends: W5500 SPI (`klin_eth_w5500_start`) and RMII EMAC
 * (`klin_eth_rmii_start`, needs CONFIG_ETH_USE_ESP32_EMAC).
 * Heap / netif / event loop are IDF contracts, not Klin magic.
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Optional static IPv4 (lwIP byte order). Call before `klin_eth_w5500_start`
 * (or after start to re-apply). Disables DHCP client on the ETH netif.
 * Pass 0,0,0 to clear and return to DHCP on next start (before start only).
 */
int klin_eth_set_static_ip(uint32_t ip, uint32_t gw, uint32_t netmask);

/**
 * Optional hostname for the ETH netif (DHCP / mDNS identity).
 * Call before start (stored) or after start (applied immediately).
 * Empty / NULL clears the pending hostname.
 */
int klin_eth_set_hostname(const char *name);

/**
 * NVS (optional reuse) + netif + default event loop + SPI bus + W5500 MAC/PHY
 * + driver install + start. Pins are explicit (prime rule).
 *
 * @param spi_host  SPI2_HOST (typically 1) or SPI3_HOST
 * @param mosi/miso/sclk/cs  SPI GPIO numbers
 * @param int_gpio  interrupt GPIO, or -1 to poll (poll_ms used)
 * @param rst_gpio  PHY reset GPIO, or -1 if hardwired
 * @param clock_mhz SPI clock in MHz (e.g. 20; W5500 max ~33)
 * @param poll_ms   poll period when int_gpio < 0 (e.g. 10)
 */
int klin_eth_w5500_start(int spi_host, int mosi, int miso, int sclk, int cs,
                         int int_gpio, int rst_gpio, int clock_mhz, int poll_ms);

/**
 * Internal EMAC + external PHY (RMII). Pins explicit.
 * Returns ESP_ERR_NOT_SUPPORTED when CONFIG_ETH_USE_ESP32_EMAC is off (S3/C3).
 *
 * @param mdc/mdio     SMI GPIOs
 * @param rst          PHY reset GPIO, or -1 if unused
 * @param phy_addr     MDIO address (board-specific; often 0 or 1)
 * @param phy_kind     0 = LAN87xx, 1 = IP101
 * @param clock_mode   0 = EMAC_CLK_EXT_IN, 1 = EMAC_CLK_OUT
 * @param clock_gpio   REF_CLK GPIO (in or out, matching clock_mode)
 * @param clock_in_gpio P4 loopback input when clock_mode=OUT; else -1
 * @param tx_en/txd0/txd1/crs_dv/rxd0/rxd1  RMII data plane (P4 IO_MUX set)
 */
int klin_eth_rmii_start(int mdc, int mdio, int rst, int phy_addr, int phy_kind,
                        int clock_mode, int clock_gpio, int clock_in_gpio,
                        int tx_en, int txd0, int txd1, int crs_dv, int rxd0,
                        int rxd1);

/** Block until cable link up or timeout_ms (-1 = forever). 0 = OK. */
int klin_eth_wait_link(int timeout_ms);

/** 1 if link up (ETHERNET_EVENT_CONNECTED seen), else 0. */
int klin_eth_link_up(void);

/** Block until ETH GOT_IP or timeout_ms (-1 = forever). 0 = OK. */
int klin_eth_wait_ip(int timeout_ms);

/** IPv4 / gateway / netmask as u32 (lwIP order) after wait success; else 0. */
uint32_t klin_eth_ip_u32(void);
uint32_t klin_eth_gateway_u32(void);
uint32_t klin_eth_netmask_u32(void);

void klin_eth_log_ip(void);

/** Print ip / gateway / netmask. */
void klin_eth_log_ip_info(void);

/** Print MAC via `esp_eth_ioctl(ETH_CMD_G_MAC_ADDR)`. */
void klin_eth_log_mac(void);

int klin_eth_stop(void);

#ifdef __cplusplus
}
#endif
