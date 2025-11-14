/**
 * @file pal_file_sdl.c
 * @brief SDL3 implementation of file I/O abstraction layer
 */

#include "platform/pal_file.h"

#ifdef PLATFORM_SDL

#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
    #include <windows.h>
    #include <direct.h>
    #define PATH_SEPARATOR '\\'
#else
    #include <dirent.h>
    #include <unistd.h>
    #define PATH_SEPARATOR '/'
#endif

PAL_File PAL_File_Open(const char* path, const char* mode) {
    return (PAL_File)fopen(path, mode);
}

size_t PAL_File_Read(void* buffer, size_t size, size_t count, PAL_File file) {
    return fread(buffer, size, count, (FILE*)file);
}

size_t PAL_File_Write(const void* buffer, size_t size, size_t count, PAL_File file) {
    return fwrite(buffer, size, count, (FILE*)file);
}

void PAL_File_Close(PAL_File file) {
    if (file) {
        fclose((FILE*)file);
    }
}

long PAL_File_Size(PAL_File file) {
    if (!file) {
        return -1;
    }
    
    FILE* f = (FILE*)file;
    long current = ftell(f);
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, current, SEEK_SET);
    return size;
}

int PAL_File_Seek(PAL_File file, long offset, int origin) {
    return fseek((FILE*)file, offset, origin);
}

long PAL_File_Tell(PAL_File file) {
    return ftell((FILE*)file);
}

#ifdef _WIN32
// Windows directory implementation
struct PAL_Dir_Internal {
    HANDLE handle;
    WIN32_FIND_DATAA find_data;
    BOOL first;
    char current_name[MAX_PATH];
};

PAL_Dir PAL_Dir_Open(const char* path) {
    struct PAL_Dir_Internal* dir = malloc(sizeof(struct PAL_Dir_Internal));
    if (!dir) {
        return NULL;
    }
    
    char search_path[MAX_PATH];
    snprintf(search_path, sizeof(search_path), "%s\\*", path);
    
    dir->handle = FindFirstFileA(search_path, &dir->find_data);
    if (dir->handle == INVALID_HANDLE_VALUE) {
        free(dir);
        return NULL;
    }
    
    dir->first = TRUE;
    return (PAL_Dir)dir;
}

const char* PAL_Dir_ReadNext(PAL_Dir dir) {
    struct PAL_Dir_Internal* d = (struct PAL_Dir_Internal*)dir;
    if (!d) {
        return NULL;
    }
    
    if (d->first) {
        d->first = FALSE;
        strncpy(d->current_name, d->find_data.cFileName, MAX_PATH);
        return d->current_name;
    }
    
    if (!FindNextFileA(d->handle, &d->find_data)) {
        return NULL;
    }
    
    strncpy(d->current_name, d->find_data.cFileName, MAX_PATH);
    return d->current_name;
}

void PAL_Dir_Close(PAL_Dir dir) {
    struct PAL_Dir_Internal* d = (struct PAL_Dir_Internal*)dir;
    if (d) {
        FindClose(d->handle);
        free(d);
    }
}
#else
// POSIX directory implementation
struct PAL_Dir_Internal {
    DIR* handle;
    char current_name[256];
};

PAL_Dir PAL_Dir_Open(const char* path) {
    struct PAL_Dir_Internal* dir = malloc(sizeof(struct PAL_Dir_Internal));
    if (!dir) {
        return NULL;
    }
    
    dir->handle = opendir(path);
    if (!dir->handle) {
        free(dir);
        return NULL;
    }
    
    return (PAL_Dir)dir;
}

const char* PAL_Dir_ReadNext(PAL_Dir dir) {
    struct PAL_Dir_Internal* d = (struct PAL_Dir_Internal*)dir;
    if (!d || !d->handle) {
        return NULL;
    }
    
    struct dirent* entry = readdir(d->handle);
    if (!entry) {
        return NULL;
    }
    
    strncpy(d->current_name, entry->d_name, sizeof(d->current_name) - 1);
    d->current_name[sizeof(d->current_name) - 1] = '\0';
    return d->current_name;
}

void PAL_Dir_Close(PAL_Dir dir) {
    struct PAL_Dir_Internal* d = (struct PAL_Dir_Internal*)dir;
    if (d) {
        if (d->handle) {
            closedir(d->handle);
        }
        free(d);
    }
}
#endif

char* PAL_Path_Combine(const char* base, const char* relative) {
    size_t base_len = strlen(base);
    size_t rel_len = strlen(relative);
    size_t total_len = base_len + rel_len + 2;  // +2 for separator and null
    
    char* result = malloc(total_len);
    if (!result) {
        return NULL;
    }
    
    snprintf(result, total_len, "%s%c%s", base, PATH_SEPARATOR, relative);
    return result;
}

BOOL PAL_Path_Exists(const char* path) {
#ifdef _WIN32
    DWORD attrib = GetFileAttributesA(path);
    return (attrib != INVALID_FILE_ATTRIBUTES);
#else
    struct stat st;
    return (stat(path, &st) == 0);
#endif
}

BOOL PAL_Path_IsDirectory(const char* path) {
#ifdef _WIN32
    DWORD attrib = GetFileAttributesA(path);
    return (attrib != INVALID_FILE_ATTRIBUTES && 
            (attrib & FILE_ATTRIBUTE_DIRECTORY));
#else
    struct stat st;
    if (stat(path, &st) != 0) {
        return FALSE;
    }
    return S_ISDIR(st.st_mode);
#endif
}

#endif // PLATFORM_SDL
