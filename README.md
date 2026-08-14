# esp_eth

Thin **ESP-IDF Ethernet** bindings for [Klin](https://github.com/klin-lang/klin).

One package for ETH backends (like IDF `esp_eth`). **Not** part of
[`machine_esp`](https://github.com/klin-lang/machine_esp). Sibling of
[`esp_wifi`](https://github.com/klin-lang/esp_wifi) — wired vs radio.

Decision: Klin [issue 102](https://github.com/klin-lang/klin/blob/main/issues/102-esp-eth-idf.md)
/ [104 E1](https://github.com/klin-lang/klin/blob/main/issues/104-later-tracks-esp-network.md).

## Status (`@v0.2.0`)

| Backend | Status | Notes |
|---|---|---|
| **W5500** (SPI MAC+PHY) | ✅ | Works on ESP32-S3/C3/… (no on-chip EMAC needed) |
| **RMII** internal EMAC | ✅ | **ESP32-P4** first (`rmii_start`); classic ESP32 same API (fixed pads) |
| DM9051 / KSZ8851 / ENC28J60 | later | Same SPI pattern |

| API | Notes |
|---|---|
| `w5500_start(spi_host, mosi, miso, sclk, cs, int, rst, mhz, poll_ms)` | Pins explicit |
| `rmii_start(mdc, mdio, rst, phy_addr, phy_kind, clk_mode, clk_gpio, clk_in, tx_en, txd0, txd1, crs_dv, rxd0, rxd1)` | Pins / PHY / clock explicit |
| `phy_lan87xx` / `phy_ip101` / `clk_ext_in` / `clk_out` | `phy_kind` / `clock_mode` helpers |
| `set_static_ip` / `ipv4` / `set_hostname` | Optional; prefer before start (DHCP off) |
| `wait_link` / `link_up` | Cable/PHY link (`ETHERNET_EVENT_CONNECTED`) |
| `log_mac` | `ETH_CMD_G_MAC_ADDR` debug print |
| `wait_ip` / `ip_u32` / `gateway_u32` / `netmask_u32` | Shared after GOT_IP |
| `log_ip` / `log_ip_info` / `stop` | Debug + stop |

`version()` → `4` (`@v0.2.0`). One backend at a time (`s_started` is shared).

## Silicon reminder

| SoC | On-chip EMAC (RMII) | SPI ETH (W5500…) |
|---|---|---|
| ESP32 | yes (fixed RMII pads) | yes |
| ESP32-P4 | yes (IO_MUX data-plane set) | yes |
| ESP32-S3 / C3 | **no** (`rmii_start` → `ESP_ERR_NOT_SUPPORTED`) | yes |

P4 RMII data-plane pads (IO_MUX, not arbitrary GPIO):

| Signal | Allowed GPIOs |
|---|---|
| TX_EN | 33, 40, 49 |
| TXD0 | 34, 41 |
| TXD1 | 35, 42 |
| CRS_DV | 28, 45, 51 |
| RXD0 | 29, 46, 52 |
| RXD1 | 30, 47, 53 |
| CLK_EXT_IN | 32, 44, 50 |
| CLK_OUT | 23, 39 (+ loopback on 32/44/50) |

SMI (MDC/MDIO) and PHY reset route via the GPIO matrix (any free pad).

## Requirements

- Klin compiler
- ESP-IDF **v5.x** (`IDF_PATH`)
  - W5500: `CONFIG_ETH_USE_SPI_ETHERNET` + `CONFIG_ETH_SPI_ETHERNET_W5500`
  - RMII: `CONFIG_ETH_USE_ESP32_EMAC` (ESP32 / ESP32-P4)
- Hardware: W5500 module **or** RMII PHY (LAN8720 / IP101 / …)

## Layout

```text
esp_eth/
  version.kl
  w5500.kl              # W5500 + shared wait/ip/stop
  rmii.kl               # rmii_start + PHY/clock helpers
  w5500_idf.c / .h      # IDF glue for both backends (@[link])
examples/w5500_s3/      # ESP32-S3 idf.py (edit pins)
examples/rmii_p4/       # ESP32-P4 idf.py (Function-EV-Board defaults)
examples/smoke/         # emit-c check
```

## Usage — W5500 (DHCP)

```klin
import "github/klin-lang/esp_eth" eth

@[cexport, codename("klin_app_main")]
fn app() {
  let mut e = eth.w5500_start(
    eth.spi2_host(),
    11, 13, 12, 14,   // MOSI MISO SCLK CS
    10,               // INT (−1 = poll)
    9,                // RST (−1 = unused)
    20,               // MHz
    10                // poll_ms if INT unused
  )
  if e != eth.err_ok() {
    return
  }
  eth.log_mac()
  e = eth.wait_link(15000)
  if e != eth.err_ok() {
    return
  }
  e = eth.wait_ip(30000)
  if e != eth.err_ok() {
    return
  }
  eth.log_ip_info()
}
```

## Usage — RMII on ESP32-P4 (DHCP)

Pins below are IDF P4 defaults / **ESP32-P4-Function-EV-Board** (IP101).
Change `phy_kind` / addr / reset for a LAN8720 DIY board.

```klin
import "github/klin-lang/esp_eth" eth

@[cexport, codename("klin_app_main")]
fn app() {
  let mut e = eth.rmii_start(
    31, 52,           // MDC MDIO
    51,               // PHY RST (−1 = unused)
    1,                // PHY addr
    eth.phy_ip101(),  // or eth.phy_lan87xx()
    eth.clk_ext_in(), // external 50 MHz REF_CLK
    50,               // clock GPIO
    -1,               // clock_in loopback (clk_out only)
    49, 34, 35,       // TX_EN TXD0 TXD1
    28, 29, 30        // CRS_DV RXD0 RXD1
  )
  if e != eth.err_ok() {
    return
  }
  eth.log_mac()
  e = eth.wait_link(15000)
  if e != eth.err_ok() {
    return
  }
  e = eth.wait_ip(30000)
  if e != eth.err_ok() {
    return
  }
  eth.log_ip_info()
}
```

## Usage (static IP)

Same as before: `set_hostname` / `set_static_ip` **before** `w5500_start` or
`rmii_start`.

```klin
import "github/klin-lang/esp_eth" eth

@[cexport, codename("klin_app_main")]
fn app() {
  let _h = eth.set_hostname("klin-eth")
  let _s = eth.set_static_ip(
    eth.ipv4(192, 168, 1, 50),
    eth.ipv4(192, 168, 1, 1),
    eth.ipv4(255, 255, 255, 0)
  )
  let mut e = eth.rmii_start(
    31, 52, 51, 1, eth.phy_ip101(),
    eth.clk_ext_in(), 50, -1,
    49, 34, 35, 28, 29, 30
  )
  if e != eth.err_ok() {
    return
  }
  e = eth.wait_link(15000)
  if e != eth.err_ok() {
    return
  }
  e = eth.wait_ip(5000)
  if e != eth.err_ok() {
    return
  }
  eth.log_ip_info()
}
```

```sh
klin get github/klin-lang/esp_eth@v0.2.0
```

## Examples

```sh
cd examples/rmii_p4   # or examples/w5500_s3
# edit main.kl pins
. $IDF_PATH/export.sh
make emit KLIN=/path/to/klin/bin/klin.dart
make build
make flash
```

## Contract

- No Klin GC / hidden heap — SPI or RMII pins, PHY kind, and clock mode are
  arguments.
- IDF netif / event loop / DHCP (or static IP) are IDF contracts.
- Other SPI chips: same package, later tags — do not split repos.

## Links

- Klin issue: https://github.com/klin-lang/klin/blob/main/issues/102-esp-eth-idf.md
- Later ETH tracks: https://github.com/klin-lang/klin/blob/main/issues/104-later-tracks-esp-network.md
- Wi‑Fi sibling: https://github.com/klin-lang/esp_wifi
- Chip MMIO: https://github.com/klin-lang/machine_esp
