#ifdef __cplusplus
#pragma once
#endif

#ifndef TOM_CORE_H
#define TOM_CORE_H

#include <vulkan/vulkan.h>
#include <stdio.h>

#ifdef __cplusplus
struct tom_instance { uintptr_t handle = 0ull; };
struct tom_device { uintptr_t handle = 0ull; };
struct tom_buffer { uintptr_t handle = 0ull; };
struct tom_compute { uintptr_t handle = 0ull; };
#else
struct tom_instance { uintptr_t handle; };
struct tom_device { uintptr_t handle; };
struct tom_buffer { uintptr_t handle; };
struct tom_compute { uintptr_t handle; };
#endif


#ifdef __cplusplus
extern "C" {
#endif

tom_instance tom_init_instance() { return tom_instance{ 0ull }; };
tom_device tom_init_device() { return tom_device{ 0ull }; };
tom_buffer tom_init_buffer() { return tom_buffer{ 0ull }; };
tom_compute tom_init_compute() { return tom_compute{ 0ull }; };

#ifdef __cplusplus
};
#endif


#endif
