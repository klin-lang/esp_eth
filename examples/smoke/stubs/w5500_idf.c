#include "w5500_idf.h"
int klin_eth_set_static_ip(uint32_t ip, uint32_t gw, uint32_t netmask)
{
    (void)ip;
    (void)gw;
    (void)netmask;
    return 0;
}
int klin_eth_set_hostname(const char *name)
{
    (void)name;
    return 0;
}
int klin_eth_w5500_start(int spi_host, int mosi, int miso, int sclk, int cs,
                         int int_gpio, int rst_gpio, int clock_mhz, int poll_ms)
{
    (void)spi_host; (void)mosi; (void)miso; (void)sclk; (void)cs;
    (void)int_gpio; (void)rst_gpio; (void)clock_mhz; (void)poll_ms;
    return 0;
}
int klin_eth_rmii_start(int mdc, int mdio, int rst, int phy_addr, int phy_kind,
                        int clock_mode, int clock_gpio, int clock_in_gpio,
                        int tx_en, int txd0, int txd1, int crs_dv, int rxd0,
                        int rxd1)
{
    (void)mdc; (void)mdio; (void)rst; (void)phy_addr; (void)phy_kind;
    (void)clock_mode; (void)clock_gpio; (void)clock_in_gpio;
    (void)tx_en; (void)txd0; (void)txd1; (void)crs_dv; (void)rxd0; (void)rxd1;
    return 0;
}
int klin_eth_wait_link(int timeout_ms) { (void)timeout_ms; return 0; }
int klin_eth_link_up(void) { return 1; }
int klin_eth_wait_ip(int timeout_ms) { (void)timeout_ms; return 0; }
uint32_t klin_eth_ip_u32(void) { return 0x0101a8c0u; }
uint32_t klin_eth_gateway_u32(void) { return 0x0100a8c0u; }
uint32_t klin_eth_netmask_u32(void) { return 0x00ffffffu; }
void klin_eth_log_ip(void) {}
void klin_eth_log_ip_info(void) {}
void klin_eth_log_mac(void) {}
int klin_eth_stop(void) { return 0; }
