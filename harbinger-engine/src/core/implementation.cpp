#include "core/memory/allocators.hpp"

#define STBI_MALLOC(size)               har::internal::MainDynamicAllocator.allocate(har::memory_tag::assets, size)
#define STBI_REALLOC(address, size)     har::internal::MainDynamicAllocator.reallocate(har::memory_tag::assets, address, size)
#define STBI_FREE(address)              har::internal::MainDynamicAllocator.deallocate(har::memory_tag::assets, address)

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define NK_IMPLEMENTATION
#include <nuklear.h>