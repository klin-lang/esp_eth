#pragma once
#include <stdint.h>
int klin_eth_set_static_ip(uint32_t ip, uint32_t gw, uint32_t netmask);
int klin_eth_set_hostname(const char *name);
int klin_eth_w5500_start(int spi_host, int mosi, int miso, int sclk, int cs,
                         int int_gpio, int rst_gpio, int clock_mhz, int poll_ms);
int klin_eth_wait_link(int timeout_ms);
int klin_eth_link_up(void);
int klin_eth_wait_ip(int timeout_ms);
uint32_t klin_eth_ip_u32(void);
uint32_t klin_eth_gateway_u32(void);
uint32_t klin_eth_netmask_u32(void);
void klin_eth_log_ip(void);
void klin_eth_log_ip_info(void);
void klin_eth_log_mac(void);
int klin_eth_stop(void);
