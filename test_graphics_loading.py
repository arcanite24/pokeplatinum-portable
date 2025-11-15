#!/usr/bin/env python3
"""
Test graphics loading by creating a simple C program
"""

test_code = '''
#include <stdio.h>
#include <stdlib.h>
#include "platform/pal_file.h"
#include "platform/pal_graphics.h"
#include "platform/pal_background.h"
#include "heap.h"

// Stub for heap functions during testing
void* Heap_Alloc(int heapID, size_t size) {
    return malloc(size);
}

void Heap_Free(void* ptr) {
    free(ptr);
}

int main() {
    printf("Testing Graphics Loading...\\n\\n");
    
    // Test 1: Check if title screen files exist
    printf("Test 1: Checking if title screen assets exist...\\n");
    
    const char* test_files[] = {
        "resources/graphics/title_screen/top_border_tiles.bin",
        "resources/graphics/title_screen/palette_1.pal",
        NULL
    };
    
    for (int i = 0; test_files[i]; i++) {
        PAL_File file = PAL_File_Open(test_files[i], "rb");
        if (file) {
            size_t size = PAL_File_Size(file);
            printf("  ✓ %s (%zu bytes)\\n", test_files[i], size);
            PAL_File_Close(file);
        } else {
            printf("  ✗ %s (NOT FOUND)\\n", test_files[i]);
        }
    }
    
    printf("\\nGraphics loading test complete!\\n");
    return 0;
}
'''

print(test_code)
