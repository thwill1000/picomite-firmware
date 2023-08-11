#!/bin/bash

cp gpio.c ${PICO_SDK_PATH}/src/rp2_common/hardware_gpio/gpio.c
dos2unix ${PICO_SDK_PATH}/src/rp2_common/hardware_gpio/gpio.c
cp spi.h ${PICO_SDK_PATH}/src/rp2_common/hardware_spi/include/hardware/spi.h
dos2unix ${PICO_SDK_PATH}/src/rp2_common/hardware_spi/include/hardware/spi.h
