
set(COMPONENT_SRCS "bt_app_core.c"
                   "main.c"
                   "ble_apis.c"
                   "akbi_serial_task.c"
                   "akbi_ccu_api.c"
                   "akbi_ccu_msg_handler.c"
                   "akbi_fsm.c"
                   )
set(COMPONENT_ADD_INCLUDEDIRS ".")

register_component()
