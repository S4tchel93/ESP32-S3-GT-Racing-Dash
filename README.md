| Supported Targets | Waveshare ESP32-S3 4.3'' LCD | Any ESP32-S3 (and maybe other ESP chips) with RGB LCD & Touch (providing your own drivers) |
| ----------------- | ---------------------------- | ------------------------------------------------------------------------------------------ |

# ESP32-S3 GT Racing Dash

This dashboard is heavily inspired in two projects. [ESP-SimHub-ESP32S3-SCREEN](https://github.com/eCrowneEng/ESP-SimHub-ESP32S3-SCREEN) and [Lovely Dashboard](https://github.com/Lovely-Sim-Racing/lovely-dashboard).

This project attempts to simplify what ESP-SimHub-ESP32S3-SCREEN does by removing all the SimHub-Arduino interactions and extra functionalities (like controller, NeoPixels, 16x2 LCD, etc.) by using SimHub's **Custom Serial Device** functionality. Where a Custom Protocol is defined and sent over USB-Serial to the outside world.

On the ESP Side there are some key decisions:
* ESP IDF is used (no Arduino, no PlatformIO), all these include several extra dependencies and/or remove control from the user, although they favor simplicity on the coding side.
* For graphics LVGL is added as a dependency and it's integrated to the project for graphics rendering
* The UI is designed using EEZ-Sudio, an open source drag&drop IDE for GUIs. And integrated seamlessly by adding it as an extra component to the project.

Lastly, the UI is heavily inspired in Lovely Dashboard. No assets were stolen from it, it only came as an inspiration for data/object placement and colors.

## How to use

* Clone the repository using
  git clone
* Open the cloned repository folder with VSCode

### Hardware Required

* An ESP development board, which supports the RGB LCD peripheral and Touch (if you want it)
* A general RGB panel, 16/24 bit-width, with HSYNC, VSYNC and DE signal
* A capacitive/resistive touch panel (if needed)
* An USB cable for power supply/programming/serial Comms

### Software Required
* VSCode
* ESP-IDF Extension
* You might or might not need to have also Git and Python installed (I believe this comes bundled with ESP-IDF Extension, if not, please refer to ESP-IDF install instructions)
* SimHub
* Your SIM of choice, supported by SimHub

### Build and Flash

Run `idf.py -p PORT build flash monitor` to build, flash and monitor the project. A scatter chart will show up on the LCD as expected.

The first time you run `idf.py` for the example will cost extra time as the build system needs to address the component dependencies and downloads the missing components from the ESP Component Registry into `managed_components` folder.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

Alternatively make sure to configure ESP-IDF on the VSCode bottom bar with your ESP Chip, UART/JTAG uploading measure, and your ESP32 COM port on your PC. Then press the build flash and monitor icon (flame).

<img width="865" height="54" alt="image" src="https://github.com/user-attachments/assets/b2a04933-dd11-4147-bb30-5a8210807e1f" />

### Example Output 

If everything goes right, you should see this on your ESP IDF Monitor terminal.

```bash
...
I (889) esp_psram: SPI SRAM memory test OK
ESP-ROM:esp32s3-20210327
Build:Mar 27 2021
rst:0x15 (USB_UART_CHIP_RESET),boot:0x8 (SPI_FAST_FLASH_BOOT)
Saved PC:0x40378e4e
SPIWP:0xee
mode:DIO, clock div:1
load:0x3fce2820,len:0x159c
load:0x403c8700,len:0xd24
load:0x403cb700,len:0x2f48
entry 0x403c8924
I (24) boot: ESP-IDF v5.5 2nd stage bootloader
I (24) boot: compile time Aug 31 2025 14:56:45
I (25) boot: Multicore bootloader
I (25) boot: chip revision: v0.2
I (27) boot: efuse block revision: v1.3
I (31) boot.esp32s3: Boot SPI Speed : 80MHz
I (35) boot.esp32s3: SPI Mode       : DIO
I (39) boot.esp32s3: SPI Flash Size : 16MB
I (42) boot: Enabling RNG early entropy source...
I (47) boot: Partition Table:
I (49) boot: ## Label            Usage          Type ST Offset   Length
I (56) boot:  0 nvs              WiFi data        01 02 00009000 00006000
I (62) boot:  1 phy_init         RF data          01 01 0000f000 00001000
I (69) boot:  2 factory          factory app      00 00 00010000 00300000
I (75) boot: End of partition table
I (79) esp_image: segment 0: paddr=00010020 vaddr=3c060020 size=cc36ch (836460) map
I (233) esp_image: segment 1: paddr=000dc394 vaddr=3fc96100 size=03384h ( 13188) load
I (237) esp_image: segment 2: paddr=000df720 vaddr=40374000 size=008f8h (  2296) load
I (238) esp_image: segment 3: paddr=000e0020 vaddr=42000020 size=54580h (345472) map
I (306) esp_image: segment 4: paddr=001345a8 vaddr=403748f8 size=117d8h ( 71640) load
I (322) esp_image: segment 5: paddr=00145d88 vaddr=600fe000 size=00020h (    32) load
I (329) boot: Loaded app from partition at offset 0x10000
I (329) boot: Disabling RNG early entropy source...
I (340) octal_psram: vendor id    : 0x0d (AP)
I (340) octal_psram: dev id       : 0x02 (generation 3)
I (340) octal_psram: density      : 0x03 (64 Mbit)
I (342) octal_psram: good-die     : 0x01 (Pass)
I (346) octal_psram: Latency      : 0x01 (Fixed)
I (351) octal_psram: VCC          : 0x01 (3V)
I (355) octal_psram: SRF          : 0x01 (Fast Refresh)
I (360) octal_psram: BurstType    : 0x01 (Hybrid Wrap)
I (365) octal_psram: BurstLen     : 0x01 (32 Byte)
I (369) octal_psram: Readlatency  : 0x02 (10 cycles@Fixed)
I (374) octal_psram: DriveStrength: 0x00 (1/1)
I (379) MSPI Timing: PSRAM timing tuning index: 5
I (383) esp_psram: Found 8MB PSRAM device
I (387) esp_psram: Speed: 80MHz
I (474) mmu_psram: Read only data copied and mapped to SPIRAM
I (515) mmu_psram: Instructions copied and mapped to SPIRAM
I (515) cpu_start: Multicore app
I (883) esp_psram: SPI SRAM memory test OK
I (892) cpu_start: Pro cpu start user code
I (892) cpu_start: cpu freq: 160000000 Hz
I (892) app_init: Application information:
I (892) app_init: Project name:     rgb_panel
I (896) app_init: App version:      8d9f6aa-dirty
I (900) app_init: Compile time:     Aug 31 2025 15:10:21
I (905) app_init: ELF file SHA256:  df21f0d9d...
I (910) app_init: ESP-IDF:          v5.5
I (913) efuse_init: Min chip rev:     v0.0
I (917) efuse_init: Max chip rev:     v0.99 
I (921) efuse_init: Chip rev:         v0.2
I (925) heap_init: Initializing. RAM available for dynamic allocation:
I (931) heap_init: At 3FCAA1A0 len 0003F570 (253 KiB): RAM
I (937) heap_init: At 3FCE9710 len 00005724 (21 KiB): RAM
I (942) heap_init: At 3FCF0000 len 00008000 (32 KiB): DRAM
I (947) heap_init: At 600FE020 len 00001FC8 (7 KiB): RTCRAM
I (952) esp_psram: Adding pool of 6976K of PSRAM memory to heap allocator
I (959) esp_psram: Adding pool of 15K of PSRAM memory gap generated due to end address alignment of drom to the heap allocator
I (970) spi_flash: detected chip: generic
I (973) spi_flash: flash io: dio
W (976) i2c: This driver is an old driver, please migrate your application code to adapt `driver/i2c_master.h`
I (986) sleep_gpio: Configure to isolate all GPIO pins in sleep state
I (993) sleep_gpio: Enable automatic switching of GPIO sleep configuration
I (999) main_task: Started on CPU0
I (1009) esp_psram: Reserving pool of 32K of internal memory for DMA/internal allocations
I (1009) main_task: Calling app_main()
I (1009) usb_serial_jtag: USB_SERIAL_JTAG init done
I (1019) SimHub-Dash: Install RGB LCD panel driver
I (1069) ST7262_LCD: Initialize RGB LCD panel
I (1069) SimHub-Dash: Install TOUCH panel driver
I (1069) GT911: Initialize I2C bus
I (1069) GT911: Initialize GPIO
I (1069) GT911: Initialize Touch LCD
I (1479) GT911: Initialize I2C panel IO
I (1479) GT911: Initialize touch controller GT911
I (1479) GT911: I2C address initialization procedure skipped - using default GT9xx setup
I (1479) GT911: TouchPad_ID:0x39,0x31,0x31
I (1489) GT911: TouchPad_Config_Version:67
I (1489) SimHub-Dash: Initialize LVGL library
I (1499) SimHub-Dash: Allocate LVGL draw buffers
I (1499) SimHub-Dash: Register event callbacks
I (1499) SimHub-Dash: Install LVGL tick timer
I (1509) SimHub-Dash: Create Queue for simhub and UI update tasks
I (1509) SimHub-Dash: Create LVGL task
I (1509) LVGL_Port: Starting LVGL task
I (1559) SimHub-Dash: Display LVGL UI
I (1629) SimHub-Dash: Create SimHub task
I (1669) SimHub-Dash: Create UI Update task
I (1669) main_task: Returned from app_main()
...
```

You should also be able to see the following image on your LCD:
<img width="1430" height="958" alt="image" src="https://github.com/user-attachments/assets/f216f41c-b119-4a15-ace4-94be6a285ca9" />


### Connecting it to SimHub

* Ensure your game is detected by SimHub and it's configured
* If not done already, enable Custom Serial Devices in SimHub by clicking on the following button in the lower left corner
  <img width="256" height="93" alt="image" src="https://github.com/user-attachments/assets/7ec2bb09-401e-4285-8141-72bc9b49c84e" />
* Once that's done, copy the contents of "SHCustomProtocol.txt" bundled in this repository to the Update messages section in Custom Serial Devices in SimHub by clicking on Edit
  <img width="1346" height="401" alt="image" src="https://github.com/user-attachments/assets/43917f63-ecd9-4d82-a73b-9cb32e49943f" />
* After that, make sure to enable the update messages, select the frequency in which the messages will go out to the ESP. Configure your comm ports acccordingly and enable the communication.
  <img width="1348" height="395" alt="image" src="https://github.com/user-attachments/assets/3371fb3e-acf1-457f-a61e-65dc5e395747" />

  


