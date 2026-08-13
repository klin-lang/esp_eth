/* Thin W5500 SPI Ethernet helpers for Klin — ESP-IDF v5.x esp_eth.
 * RMII (internal EMAC) is a later backend in this same package.
 * Heap / netif / event loop are IDF contracts, not Klin magic.
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

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

/** Block until ETH GOT_IP or timeout_ms (-1 = forever). 0 = OK. */
int klin_eth_wait_ip(int timeout_ms);

/** IPv4 as u32 (lwIP order) after wait success; else 0. */
uint32_t klin_eth_ip_u32(void);

void klin_eth_log_ip(void);

int klin_eth_stop(void);

#ifdef __cplusplus
}
#endif
