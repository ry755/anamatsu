#pragma once

#include <stdint.h>

void blooper_command_write(uint8_t command);
void blooper_data_write(uint8_t data);
uint8_t blooper_data_read();
