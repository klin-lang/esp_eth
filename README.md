# esp_eth

Thin **ESP-IDF Ethernet** bindings for [Klin](https://github.com/klin-lang/klin).

One package for ETH backends (like IDF `esp_eth`). **Not** part of
[`machine_esp`](https://github.com/klin-lang/machine_esp). Sibling of
[`esp_wifi`](https://github.com/klin-lang/esp_wifi) — wired vs radio.

Decision: Klin [issue 102](https://github.com/klin-lang/klin/blob/main/issues/102-esp-eth-idf.md).

## Status (`@v0.1.1`)

| Backend | Status | Notes |
|---|---|---|
| **W5500** (SPI MAC+PHY) | ✅ | Works on ESP32-S3/C3/… (no on-chip EMAC needed) |
| RMII internal EMAC | later | Classic **ESP32** / **ESP32-P4** + external PHY (LAN8720…) |
| DM9051 / KSZ8851 / ENC28J60 | later | Same SPI pattern |

| API | Notes |
|---|---|
| `w5500_start(spi_host, mosi, miso, sclk, cs, int, rst, mhz, poll_ms)` | Pins explicit |
| `wait_link` / `link_up` | Cable/PHY link (`ETHERNET_EVENT_CONNECTED`) |
| `log_mac` | `ETH_CMD_G_MAC_ADDR` debug print |
| `wait_ip` / `ip_u32` / `log_ip` / `stop` | Shared after start |

`version()` → `2` (`@v0.1.1`).

## Silicon reminder

| SoC | On-chip EMAC (RMII) | SPI ETH (W5500…) |
|---|---|---|
| ESP32 | yes | yes |
| ESP32-P4 | yes | yes |
| ESP32-S3 / C3 | **no** | yes |

## Requirements

- Klin compiler
- ESP-IDF **v5.x** (`IDF_PATH`) with `CONFIG_ETH_USE_SPI_ETHERNET` +
  `CONFIG_ETH_SPI_ETHERNET_W5500`
- Hardware: W5500 module + SPI wiring (or a board that has it)

## Layout

```text
esp_eth/
  version.kl
  w5500.kl              # Klin API
  w5500_idf.c / .h      # IDF glue (@[link])
examples/w5500_s3/      # ESP32-S3 idf.py (edit pins)
examples/smoke/         # emit-c check
```

## Usage

```klin
import "github/klin-lang/esp_eth" eth

@[cexport, codename("klin_app_main")]
fn app() {
  // Example pins — change to match your board / W5500 wiring.
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
  eth.log_ip()
}
```

```sh
klin get github/klin-lang/esp_eth@v0.1.1
```

## Example

```sh
cd examples/w5500_s3
# edit main.kl pins
. $IDF_PATH/export.sh
make emit KLIN=/path/to/klin/bin/klin.dart
make build
make flash
```

## Contract

- No Klin GC / hidden heap — SPI pins and clock are arguments.
- IDF netif / event loop / DHCP are IDF contracts (documented here).
- RMII and other SPI chips: same package, later tags — do not split repos.

## Links

- Klin issue: https://github.com/klin-lang/klin/blob/main/issues/102-esp-eth-idf.md
- Wi‑Fi sibling: https://github.com/klin-lang/esp_wifi
- Chip MMIO: https://github.com/klin-lang/machine_esp
