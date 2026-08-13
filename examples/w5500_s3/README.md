# W5500 on ESP32-S3

Hardware demo for [`esp_eth`](../../README.md) `@v0.1.2` (SPI Ethernet).

1. Wire a W5500 (or use a board that has one) and edit pins in `main.kl`.
2. Optional: before `w5500_start`, call `set_hostname` / `set_static_ip`
   (see package README) if your LAN has no DHCP.
3. `. $IDF_PATH/export.sh` (ESP-IDF **v5.x**)
4. `make emit KLIN=…` / `make build` / `make flash`

Needs `CONFIG_ETH_SPI_ETHERNET_W5500` (set in `sdkconfig.defaults`).
