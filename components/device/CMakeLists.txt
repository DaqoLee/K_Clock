 file(GLOB_RECURSE SOURCES ./src/*.c)

# set(component_srcs "src/rotary_encoder_pcnt_ec11.c")

idf_component_register(

SRCS ${SOURCES} "src/DendoStepper.cpp"
INCLUDE_DIRS "include".

REQUIRES
"driver"

)
