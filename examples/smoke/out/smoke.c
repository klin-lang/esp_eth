#include "w5500_idf.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

int main(void);
int32_t esp_eth_version(void);
int32_t klin_eth_w5500_start(int32_t spi_host, int32_t mosi, int32_t miso, int32_t sclk, int32_t cs, int32_t int_gpio, int32_t rst_gpio, int32_t clock_mhz, int32_t poll_ms);
int32_t klin_eth_wait_ip(int32_t timeout_ms);
uint32_t klin_eth_ip_u32(void);
void klin_eth_log_ip(void);
int32_t klin_eth_stop(void);
int32_t esp_eth_err_ok(void);
int32_t esp_eth_spi2_host(void);
int32_t esp_eth_w5500_start(int32_t spi_host, int32_t mosi, int32_t miso, int32_t sclk, int32_t cs, int32_t int_gpio, int32_t rst_gpio, int32_t clock_mhz, int32_t poll_ms);
int32_t esp_eth_wait_ip(int32_t timeout_ms);
uint32_t esp_eth_ip_u32(void);
void esp_eth_log_ip(void);
int32_t esp_eth_stop(void);

#line 4 "/tmp/esp_eth/examples/smoke/smoke.kl"
int main(void) {
#line 5 "/tmp/esp_eth/examples/smoke/smoke.kl"
    int32_t e = esp_eth_w5500_start(esp_eth_spi2_host(), 11, 13, 12, 14, 10, 9, 20, 10);
#line 6 "/tmp/esp_eth/examples/smoke/smoke.kl"
    if ((e != esp_eth_err_ok())) {
#line 7 "/tmp/esp_eth/examples/smoke/smoke.kl"
        return 0;
    }
#line 9 "/tmp/esp_eth/examples/smoke/smoke.kl"
    e = esp_eth_wait_ip(1000);
#line 10 "/tmp/esp_eth/examples/smoke/smoke.kl"
    if ((e != esp_eth_err_ok())) {
#line 11 "/tmp/esp_eth/examples/smoke/smoke.kl"
        return 0;
    }
#line 13 "/tmp/esp_eth/examples/smoke/smoke.kl"
    uint32_t ip = esp_eth_ip_u32();
#line 14 "/tmp/esp_eth/examples/smoke/smoke.kl"
    if ((ip == 0)) {
#line 15 "/tmp/esp_eth/examples/smoke/smoke.kl"
        return 0;
    }
#line 17 "/tmp/esp_eth/examples/smoke/smoke.kl"
    esp_eth_log_ip();
#line 18 "/tmp/esp_eth/examples/smoke/smoke.kl"
    int32_t _v = esp_eth_version();
    return 0;
}

#line 3 "/tmp/esp_eth/examples/smoke/../../esp_eth/version.kl"
int32_t esp_eth_version(void) {
#line 4 "/tmp/esp_eth/examples/smoke/../../esp_eth/version.kl"
    int32_t klin_ret_0 = 1;
    return klin_ret_0;
}

#line 34 "/tmp/esp_eth/examples/smoke/../../esp_eth/w5500.kl"
int32_t esp_eth_err_ok(void) {
#line 35 "/tmp/esp_eth/examples/smoke/../../esp_eth/w5500.kl"
    int32_t klin_ret_0 = 0;
    return klin_ret_0;
}

#line 39 "/tmp/esp_eth/examples/smoke/../../esp_eth/w5500.kl"
int32_t esp_eth_spi2_host(void) {
#line 40 "/tmp/esp_eth/examples/smoke/../../esp_eth/w5500.kl"
    int32_t klin_ret_0 = 1;
    return klin_ret_0;
}

#line 45 "/tmp/esp_eth/examples/smoke/../../esp_eth/w5500.kl"
int32_t esp_eth_w5500_start(int32_t spi_host, int32_t mosi, int32_t miso, int32_t sclk, int32_t cs, int32_t int_gpio, int32_t rst_gpio, int32_t clock_mhz, int32_t poll_ms) {
#line 56 "/tmp/esp_eth/examples/smoke/../../esp_eth/w5500.kl"
    int32_t klin_ret_0 = klin_eth_w5500_start(spi_host, mosi, miso, sclk, cs, int_gpio, rst_gpio, clock_mhz, poll_ms);
    return klin_ret_0;
}

#line 62 "/tmp/esp_eth/examples/smoke/../../esp_eth/w5500.kl"
int32_t esp_eth_wait_ip(int32_t timeout_ms) {
#line 63 "/tmp/esp_eth/examples/smoke/../../esp_eth/w5500.kl"
    int32_t klin_ret_0 = klin_eth_wait_ip(timeout_ms);
    return klin_ret_0;
}

#line 66 "/tmp/esp_eth/examples/smoke/../../esp_eth/w5500.kl"
uint32_t esp_eth_ip_u32(void) {
#line 67 "/tmp/esp_eth/examples/smoke/../../esp_eth/w5500.kl"
    uint32_t klin_ret_0 = klin_eth_ip_u32();
    return klin_ret_0;
}

#line 70 "/tmp/esp_eth/examples/smoke/../../esp_eth/w5500.kl"
void esp_eth_log_ip(void) {
#line 71 "/tmp/esp_eth/examples/smoke/../../esp_eth/w5500.kl"
    klin_eth_log_ip();
}

#line 74 "/tmp/esp_eth/examples/smoke/../../esp_eth/w5500.kl"
int32_t esp_eth_stop(void) {
#line 75 "/tmp/esp_eth/examples/smoke/../../esp_eth/w5500.kl"
    int32_t klin_ret_0 = klin_eth_stop();
    return klin_ret_0;
}

