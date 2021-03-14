#include <tommy/internal/buffer.hpp>
#include <tommy/buffer.h>

//extern "C" {

uintptr_t tom_buffer_data(const tom_buffer* buffer) { 
    return buffer->handle;
};

void* tom_buffer_map(const tom_buffer* buffer) {
    
};

//};
