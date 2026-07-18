# Sentinel v1.0 firmware

Sentinel continuously monitors three-phase AC voltage and true internet
availability. It is an ESP32-S3 PlatformIO/Arduino project designed for the
provided controller PCB: ADS1115, DS3231, 128x64 SSD1306 OLED, microSD, and an
optional SHT31.

## Features

- 1-second RMS windows for R, Y, and B phase measurements
- Phase loss, under-voltage, over-voltage, and phase-imbalance events
- Internet reachability test using asynchronous TCP to a public endpoint
- Automatic Wi-Fi recovery without blocking `loop()`
- DS3231 ISO-8601 timestamps
- `events.csv` and `readings.csv` on a FAT32 microSD card
- OLED current voltage, connectivity, and fault status

## Project layout

```text
include/    Public module interfaces and configuration
src/        Module implementations and application composition root
lib/        Reserved for local libraries
docs/       Reserved for commissioning and operating documentation
hardware/   Reserved for released hardware assets
```

## Configure and flash

1. Install PlatformIO and open this directory.
2. Set `WIFI_SSID` and `WIFI_PASSWORD` in [include/config.h](include/config.h).
3. Verify `PHASE_SCALE`, phase-loss, under/over-voltage and imbalance limits
   against the installation requirements. Each ZMPT101B channel must be
   calibrated with an isolated, trusted reference meter.
4. Insert a FAT32 microSD card and upload:

   ```powershell
   pio run --target upload
   pio device monitor --baud 115200
   ```

## Connectivity behavior

The device distinguishes internet availability from mere association with a
Wi-Fi access point. It makes a non-blocking HTTP request to `1.1.1.1` every
minute. A 2xx response marks the internet `ONLINE`; connection failure or a
timeout marks it `OFFLINE`. Wi-Fi retry timing and endpoint configuration are
in `config.h`. No router monitoring, cloud connection, MQTT, OTA, dashboard,
or mobile functionality is included.

## Pin map

| Function | ESP32-S3 GPIO |
| --- | ---: |
| Status LED / Wi-Fi LED | 4 / 5 |
| I2C SDA / SCL | 8 / 9 |
| microSD CS / MOSI / SCK / MISO | 10 / 11 / 12 / 13 |

## Safety

This product interfaces with mains voltage. Commission only with suitable
isolation, enclosure, fusing and a qualified person. Default calibration is
`1.0` and is not a valid mains-voltage reading.
