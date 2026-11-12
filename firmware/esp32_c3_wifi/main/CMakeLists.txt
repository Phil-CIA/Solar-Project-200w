idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES esp_driver_gpio esp_driver_usb_serial_jtag esp_event esp_http_server esp_netif esp_wifi nvs_flash
)
