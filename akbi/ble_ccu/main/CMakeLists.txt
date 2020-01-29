
set(COMPONENT_SRCS "bt_app_core.c"
                   "main.c"
                   "ble_apis.c"
                   "uart_async_rxtxtasks_main.c"
                   "akbi_ccu_api.c"
                   )
set(COMPONENT_ADD_INCLUDEDIRS ".")

register_component()