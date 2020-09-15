idf_build_get_property(project_dir PROJECT_DIR)
#idf_component_register(EMBED_TXTFILES ${project_dir}/server_certs/ca_cert.pem)


set(COMPONENT_SRCS "bt_app_core.c"
                   "main.c"
                   "ble_apis.c"
                   "akbi_serial_task.c"
                   "akbi_ccu_api.c"
                   "akbi_ccu_msg_handler.c"
                   "akbi_fsm.c"
                   "akbi_esp_update.c"
                   )
set(COMPONENT_ADD_INCLUDEDIRS "."
    EMBED_TXTFILES ${project_dir}/server_certs/ca_cert.pem)

register_component()
