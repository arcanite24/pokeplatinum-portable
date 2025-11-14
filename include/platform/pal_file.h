#ifndef PAL_FILE_H
#define PAL_FILE_H

/**
 * @file pal_file.h
 * @brief Platform Abstraction Layer - File I/O API
 * 
 * Provides unified file system interface for both Nintendo DS and SDL3 platforms.
 */

#include "platform_config.h"
#include "platform_types.h"

// File and directory handles (opaque)
typedef void* PAL_File;
typedef void* PAL_Dir;

/**
 * Open a file
 * @param path File path
 * @param mode Mode string ("r", "w", "rb", "wb", etc.)
 * @return File handle, or NULL on failure
 */
PAL_File PAL_File_Open(const char* path, const char* mode);

/**
 * Read from a file
 * @param buffer Buffer to read into
 * @param size Size of each element
 * @param count Number of elements to read
 * @param file File handle
 * @return Number of elements read
 */
size_t PAL_File_Read(void* buffer, size_t size, size_t count, PAL_File file);

/**
 * Write to a file
 * @param buffer Buffer to write from
 * @param size Size of each element
 * @param count Number of elements to write
 * @param file File handle
 * @return Number of elements written
 */
size_t PAL_File_Write(const void* buffer, size_t size, size_t count, PAL_File file);

/**
 * Close a file
 * @param file File handle
 */
void PAL_File_Close(PAL_File file);

/**
 * Get file size
 * @param file File handle
 * @return File size in bytes, or -1 on error
 */
long PAL_File_Size(PAL_File file);

/**
 * Seek to position in file
 * @param file File handle
 * @param offset Offset from origin
 * @param origin SEEK_SET, SEEK_CUR, or SEEK_END
 * @return 0 on success, -1 on error
 */
int PAL_File_Seek(PAL_File file, long offset, int origin);

/**
 * Get current file position
 * @param file File handle
 * @return Current position, or -1 on error
 */
long PAL_File_Tell(PAL_File file);

/**
 * Open a directory for reading
 * @param path Directory path
 * @return Directory handle, or NULL on failure
 */
PAL_Dir PAL_Dir_Open(const char* path);

/**
 * Read next entry from directory
 * @param dir Directory handle
 * @return Entry name, or NULL if no more entries
 */
const char* PAL_Dir_ReadNext(PAL_Dir dir);

/**
 * Close a directory
 * @param dir Directory handle
 */
void PAL_Dir_Close(PAL_Dir dir);

/**
 * Combine two path components
 * @param base Base path
 * @param relative Relative path to append
 * @return Combined path (caller must free)
 */
char* PAL_Path_Combine(const char* base, const char* relative);

/**
 * Check if a path exists
 * @param path Path to check
 * @return TRUE if exists, FALSE otherwise
 */
BOOL PAL_Path_Exists(const char* path);

/**
 * Check if a path is a directory
 * @param path Path to check
 * @return TRUE if directory, FALSE otherwise
 */
BOOL PAL_Path_IsDirectory(const char* path);

#endif // PAL_FILE_H
