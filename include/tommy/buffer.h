#ifdef __cplusplus
#pragma once
#endif

#ifndef TOM_BUFFER_H
#define TOM_BUFFER_H

#include "./core.h"

uintptr_t tom_buffer_data(const tom_buffer* buffer);
void* tom_buffer_map(const tom_buffer* buffer);

#endif
