cd /D C:\Users\user\Desktop\esp-idf-3\examples\peripherals\uart\uart_echo\build\esp-idf || exit /b
E:\Tools\.espressif\python_env\idf3.3_py3.8_env\Scripts\python.exe C:/Users/user/Desktop/esp-idf-3/tools/kconfig_new/confgen.py --kconfig C:/Users/user/Desktop/esp-idf-3/Kconfig --config C:/Users/user/Desktop/esp-idf-3/examples/peripherals/uart/uart_echo/sdkconfig --env "COMPONENT_KCONFIGS= C:/Users/user/Desktop/esp-idf-3/components/app_trace/Kconfig C:/Users/user/Desktop/esp-idf-3/components/aws_iot/Kconfig C:/Users/user/Desktop/esp-idf-3/components/bt/Kconfig C:/Users/user/Desktop/esp-idf-3/components/driver/Kconfig C:/Users/user/Desktop/esp-idf-3/components/efuse/Kconfig C:/Users/user/Desktop/esp-idf-3/components/esp32/Kconfig C:/Users/user/Desktop/esp-idf-3/components/esp_adc_cal/Kconfig C:/Users/user/Desktop/esp-idf-3/components/esp_event/Kconfig C:/Users/user/Desktop/esp-idf-3/components/esp_http_client/Kconfig C:/Users/user/Desktop/esp-idf-3/components/esp_http_server/Kconfig C:/Users/user/Desktop/esp-idf-3/components/esp_https_ota/Kconfig C:/Users/user/Desktop/esp-idf-3/components/espcoredump/Kconfig C:/Users/user/Desktop/esp-idf-3/components/ethernet/Kconfig C:/Users/user/Desktop/esp-idf-3/components/fatfs/Kconfig C:/Users/user/Desktop/esp-idf-3/components/freemodbus/Kconfig C:/Users/user/Desktop/esp-idf-3/components/freertos/Kconfig C:/Users/user/Desktop/esp-idf-3/components/heap/Kconfig C:/Users/user/Desktop/esp-idf-3/components/libsodium/Kconfig C:/Users/user/Desktop/esp-idf-3/components/log/Kconfig C:/Users/user/Desktop/esp-idf-3/components/lwip/Kconfig C:/Users/user/Desktop/esp-idf-3/components/mbedtls/Kconfig C:/Users/user/Desktop/esp-idf-3/components/mdns/Kconfig C:/Users/user/Desktop/esp-idf-3/components/mqtt/Kconfig C:/Users/user/Desktop/esp-idf-3/components/nvs_flash/Kconfig C:/Users/user/Desktop/esp-idf-3/components/openssl/Kconfig C:/Users/user/Desktop/esp-idf-3/components/pthread/Kconfig C:/Users/user/Desktop/esp-idf-3/components/spi_flash/Kconfig C:/Users/user/Desktop/esp-idf-3/components/spiffs/Kconfig C:/Users/user/Desktop/esp-idf-3/components/tcpip_adapter/Kconfig C:/Users/user/Desktop/esp-idf-3/components/unity/Kconfig C:/Users/user/Desktop/esp-idf-3/components/vfs/Kconfig C:/Users/user/Desktop/esp-idf-3/components/wear_levelling/Kconfig C:/Users/user/Desktop/esp-idf-3/components/wifi_provisioning/Kconfig" --env "COMPONENT_KCONFIGS_PROJBUILD= C:/Users/user/Desktop/esp-idf-3/components/app_update/Kconfig.projbuild C:/Users/user/Desktop/esp-idf-3/components/bootloader/Kconfig.projbuild C:/Users/user/Desktop/esp-idf-3/components/esptool_py/Kconfig.projbuild C:/Users/user/Desktop/esp-idf-3/components/partition_table/Kconfig.projbuild" --env IDF_CMAKE=y --env IDF_TARGET=esp32 --output config C:/Users/user/Desktop/esp-idf-3/examples/peripherals/uart/uart_echo/sdkconfig || exit /b
E:\Tools\.espressif\tools\cmake\3.13.4\bin\cmake.exe -E env "COMPONENT_KCONFIGS= C:/Users/user/Desktop/esp-idf-3/components/app_trace/Kconfig C:/Users/user/Desktop/esp-idf-3/components/aws_iot/Kconfig C:/Users/user/Desktop/esp-idf-3/components/bt/Kconfig C:/Users/user/Desktop/esp-idf-3/components/driver/Kconfig C:/Users/user/Desktop/esp-idf-3/components/efuse/Kconfig C:/Users/user/Desktop/esp-idf-3/components/esp32/Kconfig C:/Users/user/Desktop/esp-idf-3/components/esp_adc_cal/Kconfig C:/Users/user/Desktop/esp-idf-3/components/esp_event/Kconfig C:/Users/user/Desktop/esp-idf-3/components/esp_http_client/Kconfig C:/Users/user/Desktop/esp-idf-3/components/esp_http_server/Kconfig C:/Users/user/Desktop/esp-idf-3/components/esp_https_ota/Kconfig C:/Users/user/Desktop/esp-idf-3/components/espcoredump/Kconfig C:/Users/user/Desktop/esp-idf-3/components/ethernet/Kconfig C:/Users/user/Desktop/esp-idf-3/components/fatfs/Kconfig C:/Users/user/Desktop/esp-idf-3/components/freemodbus/Kconfig C:/Users/user/Desktop/esp-idf-3/components/freertos/Kconfig C:/Users/user/Desktop/esp-idf-3/components/heap/Kconfig C:/Users/user/Desktop/esp-idf-3/components/libsodium/Kconfig C:/Users/user/Desktop/esp-idf-3/components/log/Kconfig C:/Users/user/Desktop/esp-idf-3/components/lwip/Kconfig C:/Users/user/Desktop/esp-idf-3/components/mbedtls/Kconfig C:/Users/user/Desktop/esp-idf-3/components/mdns/Kconfig C:/Users/user/Desktop/esp-idf-3/components/mqtt/Kconfig C:/Users/user/Desktop/esp-idf-3/components/nvs_flash/Kconfig C:/Users/user/Desktop/esp-idf-3/components/openssl/Kconfig C:/Users/user/Desktop/esp-idf-3/components/pthread/Kconfig C:/Users/user/Desktop/esp-idf-3/components/spi_flash/Kconfig C:/Users/user/Desktop/esp-idf-3/components/spiffs/Kconfig C:/Users/user/Desktop/esp-idf-3/components/tcpip_adapter/Kconfig C:/Users/user/Desktop/esp-idf-3/components/unity/Kconfig C:/Users/user/Desktop/esp-idf-3/components/vfs/Kconfig C:/Users/user/Desktop/esp-idf-3/components/wear_levelling/Kconfig C:/Users/user/Desktop/esp-idf-3/components/wifi_provisioning/Kconfig" "COMPONENT_KCONFIGS_PROJBUILD= C:/Users/user/Desktop/esp-idf-3/components/app_update/Kconfig.projbuild C:/Users/user/Desktop/esp-idf-3/components/bootloader/Kconfig.projbuild C:/Users/user/Desktop/esp-idf-3/components/esptool_py/Kconfig.projbuild C:/Users/user/Desktop/esp-idf-3/components/partition_table/Kconfig.projbuild" IDF_CMAKE=y KCONFIG_CONFIG=C:/Users/user/Desktop/esp-idf-3/examples/peripherals/uart/uart_echo/sdkconfig IDF_TARGET=esp32 E:/Tools/.espressif/tools/mconf/v4.6.0.0-idf-20190628/mconf-idf.exe C:/Users/user/Desktop/esp-idf-3/Kconfig || exit /b
