set(COMPONENT_SRCS "bt_app_core.c"
                   "main.c"
                   "serial_port.c"
                   "ble_task.c"
                   "modbus.c"
                   "modbus-data.c"
                   "modbus-rtu.c"
                   )
set(COMPONENT_ADD_INCLUDEDIRS ".")

register_component()
