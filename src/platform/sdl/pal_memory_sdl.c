/**
 * @file pal_memory_sdl.c
 * @brief SDL3 implementation of memory management abstraction layer
 */

#include "platform/pal_memory.h"

#ifdef PLATFORM_SDL

#include <stdlib.h>
#include <string.h>

// Optional custom allocator
static PAL_Allocator* g_custom_allocator = NULL;

void* PAL_Malloc(size_t size, int heap_id) {
    // On SDL, ignore heap_id and use standard malloc
    // (could be extended to track allocations per heap for debugging)
    if (g_custom_allocator && g_custom_allocator->malloc_func) {
        return g_custom_allocator->malloc_func(size);
    }
    return malloc(size);
}

void PAL_Free(void* ptr) {
    if (g_custom_allocator && g_custom_allocator->free_func) {
        g_custom_allocator->free_func(ptr);
        return;
    }
    free(ptr);
}

void* PAL_Realloc(void* ptr, size_t new_size) {
    if (g_custom_allocator && g_custom_allocator->realloc_func) {
        return g_custom_allocator->realloc_func(ptr, new_size);
    }
    return realloc(ptr, new_size);
}

void* PAL_Calloc(size_t size, int heap_id) {
    void* ptr = PAL_Malloc(size, heap_id);
    if (ptr) {
        memset(ptr, 0, size);
    }
    return ptr;
}

void* PAL_Memcpy(void* dest, const void* src, size_t n) {
    return memcpy(dest, src, n);
}

void* PAL_Memset(void* dest, int value, size_t n) {
    return memset(dest, value, n);
}

void* PAL_Memmove(void* dest, const void* src, size_t n) {
    return memmove(dest, src, n);
}

int PAL_Memcmp(const void* ptr1, const void* ptr2, size_t n) {
    return memcmp(ptr1, ptr2, n);
}

void PAL_Memory_SetAllocator(PAL_Allocator* allocator) {
    g_custom_allocator = allocator;
}

#endif // PLATFORM_SDL
