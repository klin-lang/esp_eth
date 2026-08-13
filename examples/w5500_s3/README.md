# W5500 on ESP32-S3

Hardware demo for [`esp_eth`](../../README.md) `@v0.1.0` (SPI Ethernet).

1. Wire a W5500 (or use a board that has one) and edit pins in `main.kl`.
2. `. $IDF_PATH/export.sh` (ESP-IDF **v5.x**)
3. `make emit KLIN=…` / `make build` / `make flash`

Needs `CONFIG_ETH_SPI_ETHERNET_W5500` (set in `sdkconfig.defaults`).
