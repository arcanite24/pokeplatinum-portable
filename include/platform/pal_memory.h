#ifndef PAL_MEMORY_H
#define PAL_MEMORY_H

/**
 * @file pal_memory.h
 * @brief Platform Abstraction Layer - Memory Management API
 * 
 * Provides unified memory management interface for both Nintendo DS and SDL3 platforms.
 */

#include "platform_config.h"
#include "platform_types.h"

/**
 * Allocate memory from a specific heap
 * @param size Size in bytes
 * @param heap_id Heap identifier (from constants/heap.h)
 * @return Pointer to allocated memory, or NULL on failure
 */
void* PAL_Malloc(size_t size, int heap_id);

/**
 * Free memory
 * @param ptr Pointer to free
 */
void PAL_Free(void* ptr);

/**
 * Reallocate memory
 * @param ptr Existing pointer (or NULL)
 * @param new_size New size in bytes
 * @return Pointer to reallocated memory, or NULL on failure
 */
void* PAL_Realloc(void* ptr, size_t new_size);

/**
 * Allocate and zero-initialize memory
 * @param size Size in bytes
 * @param heap_id Heap identifier
 * @return Pointer to allocated memory, or NULL on failure
 */
void* PAL_Calloc(size_t size, int heap_id);

/**
 * Copy memory
 * @param dest Destination
 * @param src Source
 * @param n Number of bytes to copy
 * @return Destination pointer
 */
void* PAL_Memcpy(void* dest, const void* src, size_t n);

/**
 * Set memory to a value
 * @param dest Destination
 * @param value Value to set
 * @param n Number of bytes to set
 * @return Destination pointer
 */
void* PAL_Memset(void* dest, int value, size_t n);

/**
 * Move memory (handles overlapping regions)
 * @param dest Destination
 * @param src Source
 * @param n Number of bytes to move
 * @return Destination pointer
 */
void* PAL_Memmove(void* dest, const void* src, size_t n);

/**
 * Compare memory
 * @param ptr1 First pointer
 * @param ptr2 Second pointer
 * @param n Number of bytes to compare
 * @return 0 if equal, non-zero otherwise
 */
int PAL_Memcmp(const void* ptr1, const void* ptr2, size_t n);

#ifdef PLATFORM_SDL
/**
 * Custom allocator structure (SDL only)
 */
typedef struct PAL_Allocator {
    void* (*malloc_func)(size_t);
    void (*free_func)(void*);
    void* (*realloc_func)(void*, size_t);
} PAL_Allocator;

/**
 * Set custom allocator (SDL only)
 * @param allocator Custom allocator structure
 */
void PAL_Memory_SetAllocator(PAL_Allocator* allocator);
#endif

#endif // PAL_MEMORY_H
