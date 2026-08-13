#include "w5500_idf.h"
int klin_eth_w5500_start(int spi_host, int mosi, int miso, int sclk, int cs,
                         int int_gpio, int rst_gpio, int clock_mhz, int poll_ms)
{
    (void)spi_host; (void)mosi; (void)miso; (void)sclk; (void)cs;
    (void)int_gpio; (void)rst_gpio; (void)clock_mhz; (void)poll_ms;
    return 0;
}
int klin_eth_wait_link(int timeout_ms) { (void)timeout_ms; return 0; }
int klin_eth_link_up(void) { return 1; }
int klin_eth_wait_ip(int timeout_ms) { (void)timeout_ms; return 0; }
uint32_t klin_eth_ip_u32(void) { return 0x0101a8c0u; }
void klin_eth_log_ip(void) {}
void klin_eth_log_mac(void) {}
int klin_eth_stop(void) { return 0; }
