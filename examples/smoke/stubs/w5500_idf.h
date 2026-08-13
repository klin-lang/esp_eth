#pragma once
#include <stdint.h>
int klin_eth_w5500_start(int spi_host, int mosi, int miso, int sclk, int cs,
                         int int_gpio, int rst_gpio, int clock_mhz, int poll_ms);
int klin_eth_wait_ip(int timeout_ms);
uint32_t klin_eth_ip_u32(void);
void klin_eth_log_ip(void);
int klin_eth_stop(void);
