/**
 * @file pal_3d.h
 * @brief Platform Abstraction Layer for 3D Graphics
 * 
 * This module provides a hardware-independent interface for 3D graphics rendering,
 * abstracting the Nintendo DS G3D system to work with modern graphics APIs.
 * 
 * Key Features:
 * - Multiple backend support (Software, OpenGL ES, Vulkan)
 * - Model loading and management
 * - Matrix transformations (position, rotation, scale)
 * - Camera system
 * - Lighting and materials
 * - Texture management
 * 
 * Backend Priority:
 * 1. Software renderer (Phase 2.5 initial implementation) - Portable but slow
 * 2. OpenGL ES 3.0 (Phase 2.5 future) - Hardware accelerated, widely supported
 * 3. Vulkan (Phase 2.5 optional) - Maximum performance, more complex
 * 
 * @note The initial implementation provides a minimal software renderer for
 *       basic 3D support. Hardware acceleration can be added incrementally.
 */

#ifndef PAL_3D_H
#define PAL_3D_H

#include "platform/platform_types.h"
#include "platform/pal_graphics.h"

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// Type Definitions
// ============================================================================

/**
 * @brief 3D rendering backend type
 */
typedef enum {
    PAL_3D_BACKEND_SOFTWARE,    ///< Software rasterizer (slow, portable)
    PAL_3D_BACKEND_OPENGL,      ///< OpenGL ES 2.0/3.0 (hardware accelerated)
    PAL_3D_BACKEND_VULKAN,      ///< Vulkan (maximum performance)
    PAL_3D_BACKEND_AUTO,        ///< Auto-select best available
} PAL_3D_Backend;

/**
 * @brief 3D vector (fixed-point FX32 on DS, float on SDL)
 */
typedef struct {
    float x, y, z;
} PAL_Vec3;

/**
 * @brief 3D rotation matrix (3x3)
 */
typedef struct {
    float m[3][3];
} PAL_Matrix33;

/**
 * @brief 3D transformation matrix (4x4)
 */
typedef struct {
    float m[4][4];
} PAL_Matrix44;

/**
 * @brief Camera projection type
 */
typedef enum {
    PAL_3D_PROJECTION_PERSPECTIVE,
    PAL_3D_PROJECTION_ORTHOGRAPHIC,
} PAL_3D_ProjectionType;

/**
 * @brief Camera structure
 */
typedef struct {
    PAL_Vec3 position;
    PAL_Vec3 target;
    PAL_Vec3 up;
    float fov;                  ///< Field of view in degrees (perspective only)
    float aspect_ratio;         ///< Width / Height
    float near_clip;
    float far_clip;
    PAL_3D_ProjectionType projection;
} PAL_3D_Camera;

/**
 * @brief Shading mode
 */
typedef enum {
    PAL_3D_SHADING_FLAT,        ///< Flat shading
    PAL_3D_SHADING_TOON,        ///< Toon/cel shading (DS style)
    PAL_3D_SHADING_HIGHLIGHT,   ///< Highlight shading
} PAL_3D_ShadingMode;

/**
 * @brief Light structure
 */
typedef struct {
    PAL_Vec3 direction;         ///< Light direction (normalized)
    u8 r, g, b;                 ///< Light color (0-31 on DS, scaled to 0-255)
    BOOL enabled;
} PAL_3D_Light;

/**
 * @brief Material properties
 */
typedef struct {
    u8 diffuse_r, diffuse_g, diffuse_b;     ///< Diffuse color
    u8 ambient_r, ambient_g, ambient_b;     ///< Ambient color
    u8 specular_r, specular_g, specular_b;  ///< Specular color
    u8 emission_r, emission_g, emission_b;  ///< Emission color
    BOOL use_shininess;                     ///< Enable specular highlights
} PAL_3D_Material;

/**
 * @brief 3D model handle (opaque)
 */
typedef struct PAL_3D_Model PAL_3D_Model;

/**
 * @brief Render object (model instance with transform)
 */
typedef struct PAL_3D_RenderObj PAL_3D_RenderObj;

// ============================================================================
// Initialization and Shutdown
// ============================================================================

/**
 * @brief Initialize 3D graphics system
 * 
 * @param backend Desired rendering backend
 * @param heapID Memory heap for 3D allocations
 * @return TRUE if successful, FALSE otherwise
 */
BOOL PAL_3D_Init(PAL_3D_Backend backend, u32 heapID);

/**
 * @brief Shutdown 3D graphics system
 */
void PAL_3D_Shutdown(void);

/**
 * @brief Get current backend type
 * 
 * @return Active rendering backend
 */
PAL_3D_Backend PAL_3D_GetBackend(void);

// ============================================================================
// Screen and Rendering Control
// ============================================================================

/**
 * @brief Set which screen to render 3D on
 * 
 * @param screen Target screen (DS supports only one 3D screen at a time)
 */
void PAL_3D_SetScreen(PAL_Screen screen);

/**
 * @brief Get current 3D rendering screen
 * 
 * @return Screen currently used for 3D rendering
 */
PAL_Screen PAL_3D_GetScreen(void);

/**
 * @brief Begin 3D frame rendering
 * 
 * Should be called before any 3D drawing operations.
 */
void PAL_3D_BeginFrame(void);

/**
 * @brief End 3D frame rendering
 * 
 * Finalizes 3D rendering for the current frame.
 */
void PAL_3D_EndFrame(void);

/**
 * @brief Clear 3D buffers
 * 
 * @param r Red component (0-255)
 * @param g Green component (0-255)
 * @param b Blue component (0-255)
 * @param clear_depth Whether to clear depth buffer
 */
void PAL_3D_Clear(u8 r, u8 g, u8 b, BOOL clear_depth);

/**
 * @brief Set viewport dimensions
 * 
 * @param x X offset
 * @param y Y offset
 * @param width Viewport width
 * @param height Viewport height
 */
void PAL_3D_SetViewport(int x, int y, int width, int height);

// ============================================================================
// Model Loading and Management
// ============================================================================

/**
 * @brief Load 3D model from file
 * 
 * @param path Path to model file (NSBMD on DS, custom format on SDL)
 * @param heapID Memory heap for allocation
 * @return Model handle, or NULL on failure
 */
PAL_3D_Model* PAL_3D_LoadModel(const char* path, u32 heapID);

/**
 * @brief Load 3D model from memory
 * 
 * @param data Model data in memory
 * @param size Data size in bytes
 * @param heapID Memory heap for allocation
 * @return Model handle, or NULL on failure
 */
PAL_3D_Model* PAL_3D_LoadModelFromMemory(const void* data, u32 size, u32 heapID);

/**
 * @brief Unload 3D model
 * 
 * @param model Model to unload
 */
void PAL_3D_UnloadModel(PAL_3D_Model* model);

/**
 * @brief Create render object from model
 * 
 * A render object is an instance of a model with its own transform.
 * Multiple render objects can share the same model.
 * 
 * @param model Source model
 * @param heapID Memory heap for allocation
 * @return Render object handle, or NULL on failure
 */
PAL_3D_RenderObj* PAL_3D_CreateRenderObj(PAL_3D_Model* model, u32 heapID);

/**
 * @brief Destroy render object
 * 
 * @param obj Render object to destroy
 */
void PAL_3D_DestroyRenderObj(PAL_3D_RenderObj* obj);

// ============================================================================
// Rendering
// ============================================================================

/**
 * @brief Draw render object
 * 
 * @param obj Render object to draw
 * @param position Position in 3D space
 * @param rotation Rotation matrix
 * @param scale Scale factors (x, y, z)
 */
void PAL_3D_DrawRenderObj(PAL_3D_RenderObj* obj, const PAL_Vec3* position,
                         const PAL_Matrix33* rotation, const PAL_Vec3* scale);

/**
 * @brief Draw render object (simple version)
 * 
 * Optimized for single material/shape models.
 * 
 * @param obj Render object to draw
 * @param position Position in 3D space
 * @param rotation Rotation matrix
 * @param scale Scale factors (x, y, z)
 */
void PAL_3D_DrawRenderObjSimple(PAL_3D_RenderObj* obj, const PAL_Vec3* position,
                               const PAL_Matrix33* rotation, const PAL_Vec3* scale);

// ============================================================================
// Camera
// ============================================================================

/**
 * @brief Set active camera
 * 
 * @param camera Camera to make active
 */
void PAL_3D_SetCamera(const PAL_3D_Camera* camera);

/**
 * @brief Get active camera
 * 
 * @param camera Output camera structure
 */
void PAL_3D_GetCamera(PAL_3D_Camera* camera);

/**
 * @brief Create perspective projection camera
 * 
 * @param position Camera position
 * @param target Look-at target
 * @param up Up vector
 * @param fov Field of view in degrees
 * @param aspect Aspect ratio (width/height)
 * @param near_clip Near clipping plane
 * @param far_clip Far clipping plane
 * @param camera Output camera structure
 */
void PAL_3D_CreatePerspectiveCamera(const PAL_Vec3* position, const PAL_Vec3* target,
                                   const PAL_Vec3* up, float fov, float aspect,
                                   float near_clip, float far_clip,
                                   PAL_3D_Camera* camera);

/**
 * @brief Create orthographic projection camera
 * 
 * @param position Camera position
 * @param target Look-at target
 * @param up Up vector
 * @param width View width
 * @param height View height
 * @param near_clip Near clipping plane
 * @param far_clip Far clipping plane
 * @param camera Output camera structure
 */
void PAL_3D_CreateOrthographicCamera(const PAL_Vec3* position, const PAL_Vec3* target,
                                    const PAL_Vec3* up, float width, float height,
                                    float near_clip, float far_clip,
                                    PAL_3D_Camera* camera);

// ============================================================================
// Lighting
// ============================================================================

/**
 * @brief Set directional light
 * 
 * DS supports up to 4 lights (0-3).
 * 
 * @param light_id Light index (0-3)
 * @param light Light parameters
 */
void PAL_3D_SetLight(int light_id, const PAL_3D_Light* light);

/**
 * @brief Get directional light
 * 
 * @param light_id Light index (0-3)
 * @param light Output light parameters
 */
void PAL_3D_GetLight(int light_id, PAL_3D_Light* light);

/**
 * @brief Enable/disable light
 * 
 * @param light_id Light index (0-3)
 * @param enabled TRUE to enable, FALSE to disable
 */
void PAL_3D_EnableLight(int light_id, BOOL enabled);

// ============================================================================
// Materials
// ============================================================================

/**
 * @brief Set global material properties
 * 
 * @param material Material parameters
 */
void PAL_3D_SetMaterial(const PAL_3D_Material* material);

/**
 * @brief Get global material properties
 * 
 * @param material Output material parameters
 */
void PAL_3D_GetMaterial(PAL_3D_Material* material);

// ============================================================================
// Rendering State
// ============================================================================

/**
 * @brief Set shading mode
 * 
 * @param mode Shading mode
 */
void PAL_3D_SetShadingMode(PAL_3D_ShadingMode mode);

/**
 * @brief Enable/disable anti-aliasing
 * 
 * @param enabled TRUE to enable, FALSE to disable
 */
void PAL_3D_SetAntiAlias(BOOL enabled);

/**
 * @brief Enable/disable alpha blending
 * 
 * @param enabled TRUE to enable, FALSE to disable
 */
void PAL_3D_SetAlphaBlend(BOOL enabled);

/**
 * @brief Enable/disable alpha testing
 * 
 * @param enabled TRUE to enable, FALSE to disable
 * @param threshold Alpha threshold (0-255)
 */
void PAL_3D_SetAlphaTest(BOOL enabled, u8 threshold);

/**
 * @brief Enable/disable edge marking (toon outline)
 * 
 * @param enabled TRUE to enable, FALSE to disable
 */
void PAL_3D_SetEdgeMarking(BOOL enabled);

/**
 * @brief Enable/disable fog
 * 
 * @param enabled TRUE to enable, FALSE to disable
 * @param color Fog color (RGB)
 * @param start Fog start distance
 * @param end Fog end distance
 */
void PAL_3D_SetFog(BOOL enabled, u32 color, float start, float end);

// ============================================================================
// Matrix Operations
// ============================================================================

/**
 * @brief Create identity matrix (3x3)
 * 
 * @param mat Output matrix
 */
void PAL_3D_Matrix33Identity(PAL_Matrix33* mat);

/**
 * @brief Create identity matrix (4x4)
 * 
 * @param mat Output matrix
 */
void PAL_3D_Matrix44Identity(PAL_Matrix44* mat);

/**
 * @brief Create rotation matrix from euler angles
 * 
 * @param x X rotation in degrees
 * @param y Y rotation in degrees
 * @param z Z rotation in degrees
 * @param mat Output matrix
 */
void PAL_3D_Matrix33FromEuler(float x, float y, float z, PAL_Matrix33* mat);

/**
 * @brief Multiply two 3x3 matrices
 * 
 * @param a First matrix
 * @param b Second matrix
 * @param result Output matrix (a * b)
 */
void PAL_3D_Matrix33Multiply(const PAL_Matrix33* a, const PAL_Matrix33* b,
                            PAL_Matrix33* result);

/**
 * @brief Transform vector by matrix
 * 
 * @param vec Input vector
 * @param mat Transformation matrix
 * @param result Output vector
 */
void PAL_3D_Matrix33TransformVec(const PAL_Vec3* vec, const PAL_Matrix33* mat,
                                PAL_Vec3* result);

// ============================================================================
// Utility Functions
// ============================================================================

/**
 * @brief Convert DS fixed-point value (FX32) to float
 * 
 * @param fx32 Fixed-point value (12.4 format on DS)
 * @return Float value
 */
float PAL_3D_FX32ToFloat(s32 fx32);

/**
 * @brief Convert float to DS fixed-point value (FX32)
 * 
 * @param f Float value
 * @return Fixed-point value (12.4 format on DS)
 */
s32 PAL_3D_FloatToFX32(float f);

/**
 * @brief Normalize vector
 * 
 * @param vec Vector to normalize (modified in-place)
 */
void PAL_3D_Vec3Normalize(PAL_Vec3* vec);

/**
 * @brief Calculate dot product
 * 
 * @param a First vector
 * @param b Second vector
 * @return Dot product
 */
float PAL_3D_Vec3Dot(const PAL_Vec3* a, const PAL_Vec3* b);

/**
 * @brief Calculate cross product
 * 
 * @param a First vector
 * @param b Second vector
 * @param result Output vector (a × b)
 */
void PAL_3D_Vec3Cross(const PAL_Vec3* a, const PAL_Vec3* b, PAL_Vec3* result);

#ifdef __cplusplus
}
#endif

#endif // PAL_3D_H
