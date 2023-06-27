file(GLOB_RECURSE SOURCES ./src/*.c)
set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

# set(component_srcs "src/rotary_encoder_pcnt_ec11.c")

idf_component_register(
SRCS ${SOURCES} "src/K_Clock.cpp"

INCLUDE_DIRS "include"
PRIV_INCLUDE_DIRS ""
# PRIV_REQUIRES "driver"

REQUIRES
driver
device
app
tcpip_adapter
nvs_flash
esp_http_client
esp-tls
esp_wifi

)

