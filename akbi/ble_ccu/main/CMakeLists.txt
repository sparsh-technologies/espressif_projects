
set(COMPONENT_SRCS "bt_app_core.c"
                   "main.c"
                   "ble_apis.c"
                   "akbi_serial_task.c"
                   "akbi_ccu_api.c"
                   )
set(COMPONENT_ADD_INCLUDEDIRS ".")

register_component()