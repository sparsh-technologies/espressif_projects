ESP-IDF MOBI_SMART_BLE_ESP32
======================
This demo showcases APIs to create a GATT service to transmit and receive data between mobile device and esp32.

The BLE GATT SERVER part of demo creates a GATT service and then starts advertising, waiting to be connected by a GATT client. After the program is started, the GATT client can discover a device named "MobiSmart". Once a connection is established, GATT client can read or write data to the device. Or GATT client can receive notification or indication data.
