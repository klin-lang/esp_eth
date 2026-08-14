set(KLIN_C "${CMAKE_SOURCE_DIR}/out/main.c")
set(ETH_C "${CMAKE_SOURCE_DIR}/../../esp_eth/w5500_idf.c")

if(NOT EXISTS "${KLIN_C}")
    message(FATAL_ERROR
        "Missing ${KLIN_C}. From examples/rmii_p4 run: make emit KLIN=...")
endif()

idf_component_register(
    SRCS "app_main.c" "${KLIN_C}" "${ETH_C}"
    INCLUDE_DIRS "." "${CMAKE_SOURCE_DIR}/out" "${CMAKE_SOURCE_DIR}/../../esp_eth"
    REQUIRES nvs_flash esp_eth esp_netif esp_event driver
)
