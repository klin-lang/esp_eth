# RMII on ESP32-P4

Hardware demo for [`esp_eth`](../../README.md) `@v0.2.0` (internal EMAC).

Defaults in `main.kl` match **ESP32-P4-Function-EV-Board** (on-board **IP101**,
IDF P4 RMII pads, PHY reset GPIO 51, addr 1). Edit pins / `phy_kind` for a
DIY LAN8720 (or other) wiring.

1. Confirm RMII GPIOs against your schematic (P4 data-plane pads are an
   IO_MUX set — not arbitrary GPIO).
2. Optional: before `rmii_start`, call `set_hostname` / `set_static_ip`
   if your LAN has no DHCP.
3. `. $IDF_PATH/export.sh` (ESP-IDF **v5.x** with P4 support)
4. `make emit KLIN=…` / `make build` / `make flash`

Needs `CONFIG_ETH_USE_ESP32_EMAC` (set in `sdkconfig.defaults`).
Does **not** build on S3/C3 (no on-chip EMAC).
