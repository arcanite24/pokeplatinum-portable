#ifndef PAL_AUDIO_H
#define PAL_AUDIO_H

/**
 * @file pal_audio.h
 * @brief Platform Abstraction Layer - Audio API
 * 
 * Provides unified audio interface for both Nintendo DS and SDL3 platforms.
 */

#include "platform_config.h"
#include "platform_types.h"

// Audio types
typedef enum {
    PAL_AUDIO_BGM,      // Background music
    PAL_AUDIO_SFX,      // Sound effects
    PAL_AUDIO_VOICE     // Voice samples
} PAL_AudioType;

// Audio handle (opaque)
typedef void* PAL_AudioHandle;

/**
 * Initialize the audio system
 * @param frequency Sample rate (e.g., 44100)
 * @param channels Number of channels (1=mono, 2=stereo)
 * @return TRUE on success, FALSE on failure
 */
BOOL PAL_Audio_Init(int frequency, int channels);

/**
 * Shutdown the audio system
 */
void PAL_Audio_Shutdown(void);

/**
 * Load music from file
 * @param path File path to music
 * @return Audio handle, or NULL on failure
 */
PAL_AudioHandle PAL_Audio_LoadMusic(const char* path);

/**
 * Play music
 * @param music Music handle
 * @param loop Whether to loop the music
 */
void PAL_Audio_PlayMusic(PAL_AudioHandle music, BOOL loop);

/**
 * Stop music
 * @param music Music handle
 */
void PAL_Audio_StopMusic(PAL_AudioHandle music);

/**
 * Fade music volume
 * @param music Music handle
 * @param ms Fade duration in milliseconds
 * @param target_volume Target volume (0.0 to 1.0)
 */
void PAL_Audio_FadeMusic(PAL_AudioHandle music, int ms, float target_volume);

/**
 * Load sound effect from file
 * @param path File path to sound
 * @return Audio handle, or NULL on failure
 */
PAL_AudioHandle PAL_Audio_LoadSound(const char* path);

/**
 * Play sound effect
 * @param sound Sound handle
 * @param channel Channel number (-1 for auto-select)
 * @param loops Number of loops (-1 for infinite, 0 for once)
 */
void PAL_Audio_PlaySound(PAL_AudioHandle sound, int channel, int loops);

/**
 * Stop sound on channel
 * @param channel Channel number
 */
void PAL_Audio_StopSound(int channel);

/**
 * Set volume for audio type
 * @param type Audio type
 * @param volume Volume (0.0 to 1.0)
 */
void PAL_Audio_SetVolume(PAL_AudioType type, float volume);

/**
 * Get volume for audio type
 * @param type Audio type
 * @return Current volume (0.0 to 1.0)
 */
float PAL_Audio_GetVolume(PAL_AudioType type);

/**
 * Unload audio resource
 * @param handle Audio handle to unload
 */
void PAL_Audio_Unload(PAL_AudioHandle handle);

#endif // PAL_AUDIO_H
