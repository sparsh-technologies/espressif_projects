set(COMPONENT_SRCS "bt_app_core.c"
                   "main_task.c"
                   "serial_task.c"
                   "platform_task.c"
                   "icom_modbus_task.c"
                   "ble_task.c"
                   "modbus.c"
                   "modbus-data.c"
                   "modbus-rtu.c"
                   "icom_config_storage.c"
                   "icom_cloud_task.c"
                   "icom_wifi.c"
                   "icom_ipc.c"
                   "icom_mqtt.c"
                   "icom_gpio.c"
                   "icom_timer_api.c"
                   "icom_bt.c"
                   "bt_app_core.c"
                   )

set(COMPONENT_ADD_INCLUDEDIRS "."
                              "include"
    )

register_component()
