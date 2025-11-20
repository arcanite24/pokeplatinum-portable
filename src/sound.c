#include "sound.h"

#ifdef PLATFORM_DS
#include <nitro.h>
#else
#include "platform/platform_types.h"
#endif
#include <string.h>

#include "constants/species.h"

#include "global/utility.h"

#include "communication_system.h"
#include "heap.h"
#include "sound_playback.h"
#include "sound_system.h"

#define BGM_PLAYER_NORMAL_CHANNELS 0x7FF
#define BGM_PLAYER_EXTRA_CHANNELS  0x7FFF

#define WAVEFORM_SAMPLE_WINDOW 100 // Used to generate amplitude graphs from waveform data
#define WAVEFORM_MAX_AMPLITUDE 9

static void Sound_LoadSoundEffectsForSceneWithState(u8 scene);
static void Sound_Impl_PlayFieldBGM(u16 bgmID, int unused);
static void Sound_Impl_ResumeAndSwitchFieldBGM(u16 bgmID, u16 unused);
static void Sound_Impl_PlayBattleBGM(u16 bgmID, int unused);
static void sub_020048AC(u16 bgmID, int unused);
static void Sound_Impl_PlayContestBGM(u16 bgmID, int unused);
static void sub_020048F0(u16 bgmID, int unused);
static void Sound_Impl_LoadSubSceneSoundData(u8 scene);
static void Sound_Impl_PlayCutsceneBGM(u8 scene, u16 bgmID, int unused);
static void Sound_Impl_ReverseBuffer(u8 *buffer, u32 size);
static void Sound_Impl_FadeToBGM(u8 unused1, u16 bgmID, int fadeOutFrames, int fadeInFrames, u8 bankState, void *unused2);
static void Sound_SetBGMAllocatableChannels(u16 channels);
static void Sound_Impl_PauseOrStopFieldBGM(void);
static void Sound_Impl_FilterCallback(void *bufferL, void *bufferR, u32 length, NNSSndCaptureFormat format, void *arg);
static const SNDWaveData *Sound_Impl_GetWaveDataForSpecies(int species);

static s8 sWaveBuffer[SOUND_WAVE_BUFFER_SIZE] ATTRIBUTE_ALIGN(32);
static int sPlaybackMode;

void Sound_SetBGMFixed(u8 fixed)
{
    u8 *param = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_BGM_FIXED);
    *param = fixed;
}

u8 Sound_IsBGMFixed(void)
{
    u8 *param = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_BGM_FIXED);
    return *param;
}

void Sound_SetCurrentBGM(u16 bgmID)
{
    u16 *param = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_CURRENT_BGM);
    *param = bgmID;

    Sound_SetNextBGM(SEQ_NONE);
}

u16 Sound_Impl_GetCurrentBGM(void)
{
    u16 *param = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_CURRENT_BGM);
    return *param;
}

void Sound_SetNextBGM(u16 bgmID)
{
    u16 *param = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_NEXT_BGM);
    *param = bgmID;
}

u16 Sound_GetNextBGM(void)
{
    u16 *param = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_NEXT_BGM);
    return *param;
}

void Sound_SetFieldBGM(u16 bgmID)
{
    u16 *param = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_FIELD_BGM);
    *param = bgmID;
}

void Sound_SetScene(u8 scene)
{
    u8 *mainScene = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_MAIN_SCENE);
    u8 *subScene = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_SUB_SCENE);

    if (scene < SOUND_SCENE_MAX) {
        *mainScene = scene;
        *subScene = SOUND_SCENE_NONE;
    } else {
        *subScene = scene;
    }
}

void Sound_SetSubScene(u8 scene)
{
    u8 *mainScene = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_MAIN_SCENE); // Required to match
    u8 *subScene = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_SUB_SCENE);
    *subScene = scene;
}

int Sound_LoadSoundEffectsForScene(u8 scene)
{
    int result;

    switch (scene) {
    case 1:
    case 9:
    case 10:
    case 17:
    case 20:
    case 23:
        result = SoundSystem_LoadSoundGroup(GROUP_SE_FIELD);
        break;
    case 19:
        result = SoundSystem_LoadSoundGroup(GROUP_SE_FIELD);

        #ifdef PLATFORM_DS
        SoundSystem_LoadSequenceEx(SEQ_SE_PL_BALLOON02, NNS_SND_ARC_LOAD_SEQ);
        #else
        // TODO: Port NNS_SND_ARC_LOAD_SEQ to PAL
        #endif
        #ifdef PLATFORM_DS
        SoundSystem_LoadSequenceEx(SEQ_SE_PL_BALLOON03_2, NNS_SND_ARC_LOAD_SEQ);
        #else
        // TODO: Port NNS_SND_ARC_LOAD_SEQ to PAL
        #endif
        #ifdef PLATFORM_DS
        SoundSystem_LoadSequenceEx(SEQ_SE_PL_BALLOON05, NNS_SND_ARC_LOAD_SEQ);
        #else
        // TODO: Port NNS_SND_ARC_LOAD_SEQ to PAL
        #endif

        #ifdef PLATFORM_DS
        SoundSystem_LoadSequenceEx(SEQ_SE_PL_BALLOON01, NNS_SND_ARC_LOAD_SEQ);
        #else
        // TODO: Port NNS_SND_ARC_LOAD_SEQ to PAL
        #endif
        #ifdef PLATFORM_DS
        SoundSystem_LoadSequenceEx(SEQ_SE_PL_BALLOON07, NNS_SND_ARC_LOAD_SEQ);
        #else
        // TODO: Port NNS_SND_ARC_LOAD_SEQ to PAL
        #endif
        #ifdef PLATFORM_DS
        SoundSystem_LoadSequenceEx(SEQ_SE_PL_ALERT4, NNS_SND_ARC_LOAD_SEQ);
        #else
        // TODO: Port NNS_SND_ARC_LOAD_SEQ to PAL
        #endif

        #ifdef PLATFORM_DS
        SoundSystem_LoadSequenceEx(SEQ_SE_DP_FW104, NNS_SND_ARC_LOAD_SEQ);
        #else
        // TODO: Port NNS_SND_ARC_LOAD_SEQ to PAL
        #endif
        #ifdef PLATFORM_DS
        SoundSystem_LoadSequenceEx(SEQ_SE_PL_NOMI02, NNS_SND_ARC_LOAD_SEQ);
        #else
        // TODO: Port NNS_SND_ARC_LOAD_SEQ to PAL
        #endif
        #ifdef PLATFORM_DS
        SoundSystem_LoadSequenceEx(SEQ_SE_DP_023, NNS_SND_ARC_LOAD_SEQ);
        #else
        // TODO: Port NNS_SND_ARC_LOAD_SEQ to PAL
        #endif

        #ifdef PLATFORM_DS
        SoundSystem_LoadSequenceEx(SEQ_SE_PL_POINT1, NNS_SND_ARC_LOAD_SEQ);
        #else
        // TODO: Port NNS_SND_ARC_LOAD_SEQ to PAL
        #endif
        #ifdef PLATFORM_DS
        SoundSystem_LoadSequenceEx(SEQ_SE_PL_POINT2, NNS_SND_ARC_LOAD_SEQ);
        #else
        // TODO: Port NNS_SND_ARC_LOAD_SEQ to PAL
        #endif
        #ifdef PLATFORM_DS
        SoundSystem_LoadSequenceEx(SEQ_SE_PL_POINT3, NNS_SND_ARC_LOAD_SEQ);
        #else
        // TODO: Port NNS_SND_ARC_LOAD_SEQ to PAL
        #endif
        #ifdef PLATFORM_DS
        SoundSystem_LoadSequenceEx(SEQ_SE_PL_BALLOON05_2, NNS_SND_ARC_LOAD_SEQ);
        #else
        // TODO: Port NNS_SND_ARC_LOAD_SEQ to PAL
        #endif

        #ifdef PLATFORM_DS
        SoundSystem_LoadSequenceEx(SEQ_SE_DP_HAMARU, NNS_SND_ARC_LOAD_SEQ);
        #else
        // TODO: Port NNS_SND_ARC_LOAD_SEQ to PAL
        #endif
        #ifdef PLATFORM_DS
        SoundSystem_LoadSequenceEx(SEQ_SE_DP_CON_016, NNS_SND_ARC_LOAD_SEQ);
        #else
        // TODO: Port NNS_SND_ARC_LOAD_SEQ to PAL
        #endif
        #ifdef PLATFORM_DS
        SoundSystem_LoadSequenceEx(SEQ_SE_PL_KIRAKIRA, NNS_SND_ARC_LOAD_SEQ);
        #else
        // TODO: Port NNS_SND_ARC_LOAD_SEQ to PAL
        #endif
        #ifdef PLATFORM_DS
        SoundSystem_LoadSequenceEx(SEQ_SE_PL_FCALL, NNS_SND_ARC_LOAD_SEQ);
        #else
        // TODO: Port NNS_SND_ARC_LOAD_SEQ to PAL
        #endif
        break;
    case 14:
        result = SoundSystem_LoadSoundGroup(GROUP_SE_NUTMIXER);
        break;
    case 2:
    case 13:
        result = SoundSystem_LoadSoundGroup(GROUP_SE_BATTLE);
        break;
    case 18:
        result = SoundSystem_LoadSoundGroup(GROUP_SE_BATTLE);
        break;
    case 21:
        result = SoundSystem_LoadBank(BANK_SE_HIROBA);
        result = SoundSystem_LoadWaveArc(WAVE_ARC_SE_HIROBA);
        break;
    case 3:
        result = SoundSystem_LoadSoundGroup(GROUP_SE_TRADE);
        break;
    case SOUND_SCENE_FIELD:
    case 22:
        result = SoundSystem_LoadSoundGroup(GROUP_SE_FIELD);
        break;
    case SOUND_SCENE_BATTLE:
        result = SoundSystem_LoadSoundGroup(GROUP_SE_BATTLE);
        break;
    case 11:
        result = SoundSystem_LoadSoundGroup(GROUP_SE_FIELD);
        break;
    case SOUND_SCENE_CONTEST:
        result = SoundSystem_LoadSoundGroup(GROUP_SE_CONTEST);
        break;
    case 8:
        result = SoundSystem_LoadSoundGroup(GROUP_SE_FIELD);
        break;
    case 12:
        result = SoundSystem_LoadSoundGroup(GROUP_SE_NUTMIXER);
        break;
    case 16:
        result = SoundSystem_LoadSoundGroup(GROUP_SE_FIELD);
        result = SoundSystem_LoadSoundGroup(GROUP_SE_DIG);
        break;
    case 15:
        result = SoundSystem_LoadSoundGroup(GROUP_SE_FIELD);
        break;
    case SOUND_SCENE_SUB_BAG:
        result = SoundSystem_LoadSoundGroup(GROUP_SE_BAG);
        break;
    case 66:
        result = SoundSystem_LoadSoundGroup(GROUP_SE_SLOT);
        break;
    case 52:
        result = SoundSystem_LoadSoundGroup(GROUP_SE_NAMEIN);
        break;
    case 7:
    case 53:
        result = SoundSystem_LoadSoundGroup(GROUP_SE_IMAGE);
        break;
    case 54:
        result = SoundSystem_LoadSoundGroup(GROUP_SE_ZUKAN);
        break;
    case 55:
    case 67:
        result = SoundSystem_LoadBank(BANK_SE_TOWNMAP);
        result = SoundSystem_LoadWaveArc(WAVE_ARC_SE_TOWNMAP);
        break;
    case 56:
        result = SoundSystem_LoadSoundGroup(GROUP_SE_TRCARD);
        break;
    case 57:
        result = SoundSystem_LoadSoundGroup(GROUP_SE_POKELIST);
        break;
    case 58:
        result = SoundSystem_LoadSoundGroup(GROUP_SE_DIG);
        break;
    case 59:
        result = SoundSystem_LoadSoundGroup(GROUP_SE_CUSTOM);
        break;
    case 60:
        result = SoundSystem_LoadSoundGroup(GROUP_SE_BAG);
        break;
    case 61:
        result = SoundSystem_LoadSoundGroup(GROUP_SE_NAMEIN);
        break;
    case 62:
        result = SoundSystem_LoadSoundGroup(GROUP_SE_CUSTOM);
        break;
    case 63:
        result = SoundSystem_LoadSoundGroup(GROUP_SE_CLIMAX);
        break;
    case 64:
        result = SoundSystem_LoadBank(BANK_SE_SHIP);
        result = SoundSystem_LoadWaveArc(WAVE_ARC_SE_SHIP);
        break;
    case 65:
        result = SoundSystem_LoadBank(BANK_SE_TRAIN);
        result = SoundSystem_LoadWaveArc(WAVE_ARC_SE_TRAIN);
        break;
    case 68:
        result = SoundSystem_LoadBank(BANK_SE_SCRATCH);
        result = SoundSystem_LoadWaveArc(WAVE_ARC_SE_SCRATCH);
        break;
    }

    return result;
}

BOOL Sound_SetSceneAndPlayBGM(u8 scene, u16 bgmID, int unused)
{
    u8 *mainScene = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_MAIN_SCENE);
    u8 *subScene = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_SUB_SCENE);
    u16 *fanfareDelay = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_FANFARE_DELAY);

    if (scene < SOUND_SCENE_MAX) {
        if (*mainScene == scene) {
            return FALSE;
        }
    } else {
        if (*subScene == scene) {
            return FALSE;
        }
    }

    Sound_SetScene(scene);

    switch (scene) {
    case SOUND_SCENE_FIELD:
        Sound_ConfigureBGMChannelsAndReverb(SOUND_CHANNEL_CONFIG_DEFAULT);
        Sound_Impl_PlayFieldBGM(bgmID, unused);
        *fanfareDelay = 0;
        break;
    case SOUND_SCENE_BATTLE:
        Sound_Impl_PlayBattleBGM(bgmID, unused);
        break;
    case 11:
        sub_020048AC(bgmID, unused);
        break;
    case SOUND_SCENE_CONTEST:
        Sound_Impl_PlayContestBGM(bgmID, unused);
        break;
    case SOUND_SCENE_7:
        sub_020048F0(bgmID, unused);
        break;
    case SOUND_SCENE_SUB_BAG:
    case 52:
    case 53:
    case 54:
    case 55:
    case 56:
    case 57:
    case 58:
    case 59:
    case 60:
    case 61:
    case 62:
    case 63:
    case 64:
    case 65:
    case 66:
    case 67:
    case 68:
        Sound_Impl_LoadSubSceneSoundData(scene);
        break;
    case SOUND_SCENE_TITLE_SCREEN:
        Sound_ConfigureBGMChannelsAndReverb(SOUND_CHANNEL_CONFIG_TITLE);
        Sound_Impl_PlayCutsceneBGM(scene, bgmID, unused);
        break;
    case SOUND_SCENE_14:
        Sound_ConfigureBGMChannelsAndReverb(SOUND_CHANNEL_CONFIG_ENDING);
        Sound_Impl_PlayCutsceneBGM(scene, bgmID, unused);
        break;
    case SOUND_SCENE_2:
        Sound_ConfigureBGMChannelsAndReverb(SOUND_CHANNEL_CONFIG_DEFAULT);
        Sound_Impl_PlayCutsceneBGM(scene, bgmID, unused);
        break;
    case SOUND_SCENE_3:
    case SOUND_SCENE_8:
    case SOUND_SCENE_9:
    case SOUND_SCENE_10:
    case SOUND_SCENE_12:
    case SOUND_SCENE_13:
    case SOUND_SCENE_15:
    case SOUND_SCENE_16:
    case SOUND_SCENE_17:
    case SOUND_SCENE_18:
    case SOUND_SCENE_19:
    case SOUND_SCENE_20:
    case SOUND_SCENE_21:
    case SOUND_SCENE_23:
        Sound_Impl_PlayCutsceneBGM(scene, bgmID, unused);
        break;
    case SOUND_SCENE_22:
        Sound_Impl_PlayCutsceneBGM(scene, bgmID, unused);
        break;
    }

    return TRUE;
}

static void Sound_LoadSoundEffectsForSceneWithState(u8 scene)
{
    int *persistentState = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_PERSISTENT);

    SoundSystem_LoadHeapState(*persistentState);
    SoundSystem_SaveHeapState(SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_BGM_BANK));
    Sound_LoadSoundEffectsForScene(scene);
    SoundSystem_SaveHeapState(SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_SFX));
}

static void Sound_Impl_PlayFieldBGM(u16 bgmID, int unused)
{
    u8 *fieldBGMPaused = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_FIELD_BGM_PAUSED);
    UNUSED(SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_PERSISTENT));
    UNUSED(SoundSystem_GetParam(SOUND_SYSTEM_PARAM_FIELD_BGM));

    int currentFieldBGM = Sound_GetSequenceIDFromSoundHandle(SoundSystem_GetSoundHandle(SOUND_HANDLE_TYPE_FIELD_BGM));

    if (*fieldBGMPaused == FALSE) {
        if (currentFieldBGM == bgmID) {
            if (Sound_GetNextBGM() != SEQ_BICYCLE) {
                return;
            }
        }
    }

    Sound_SetFieldBGMBankState(FIELD_BGM_BANK_STATE_SWITCH);
    Sound_StopAll();

    if (currentFieldBGM != bgmID) {
        Sound_SetBGMPlayerPaused(PLAYER_FIELD, FALSE);
        Sound_StopWaveOutAndSequences();
    }

    if (*fieldBGMPaused == TRUE) {
        SoundSystem_LoadHeapState(Sound_GetHeapState(SOUND_HEAP_STATE_BGM_BANK));
        Sound_LoadSoundEffectsForScene(SOUND_SCENE_FIELD);
        SoundSystem_SaveHeapState(SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_SFX));

        if (currentFieldBGM != bgmID) {
            Sound_SetBGMPlayerPaused(PLAYER_FIELD, FALSE);
        }

        Sound_Impl_ResumeAndSwitchFieldBGM(bgmID, currentFieldBGM);
        return;
    }

    Sound_PlayBGM(bgmID);
}

static void Sound_Impl_ResumeAndSwitchFieldBGM(u16 bgmID, u16 unused)
{
    u16 *currentBGM = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_FIELD_BGM);
    u16 currentBankID = Sound_GetBankIDFromSequenceID(*currentBGM);

    if (currentBankID != BANK_BGM_FIELD && currentBankID != BANK_BGM_DUNGEON) {
        #ifdef PLATFORM_DS
        SoundSystem_LoadSequenceEx(bgmID, NNS_SND_ARC_LOAD_WAVE);
        #else
        // TODO: Port NNS_SND_ARC_LOAD_WAVE to PAL
        #endif
        GF_ASSERT(FALSE);
    } else {
        #ifdef PLATFORM_DS
        SoundSystem_LoadSequenceEx(*currentBGM, NNS_SND_ARC_LOAD_WAVE | NNS_SND_ARC_LOAD_BANK);
        #else
        // TODO: Port NNS_SND_ARC_LOAD_BANK to PAL
        #endif
    }

    SoundSystem_SaveHeapState(SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_BGM));
    Sound_SetBGMPlayerPaused(PLAYER_FIELD, FALSE);
    Sound_FadeInBGM(SOUND_VOLUME_MAX, 40, BGM_FADE_IN_TYPE_FROM_ZERO);
    Sound_SetFieldBGMBankState(FIELD_BGM_BANK_STATE_IDLE);
}

void Sound_LoadSoundDataForFieldBGM(u16 seqID, u16 currentBankID)
{
    u8 *bankState = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_FIELD_BGM_BANK_STATE);
    u16 *newFieldBGM = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_FIELD_BGM);

    if (*bankState == FIELD_BGM_BANK_STATE_SWITCH || currentBankID == 0) {
        SoundSystem_LoadHeapState(Sound_GetHeapState(SOUND_HEAP_STATE_PERSISTENT));
        Sound_SetSubScene(0);
        #ifdef PLATFORM_DS
        SoundSystem_LoadSequenceEx(*newFieldBGM, NNS_SND_ARC_LOAD_BANK);
        #else
        // TODO: Port NNS_SND_ARC_LOAD_BANK to PAL
        #endif
        SoundSystem_SaveHeapState(SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_BGM_BANK));
        Sound_LoadSoundEffectsForScene(SOUND_SCENE_FIELD);
        SoundSystem_SaveHeapState(SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_SFX));

        u16 newBankID = Sound_GetBankIDFromSequenceID(*newFieldBGM);
        if (newBankID != BANK_BGM_FIELD && newBankID != BANK_BGM_DUNGEON) {
            #ifdef PLATFORM_DS
            SoundSystem_LoadSequenceEx(seqID, NNS_SND_ARC_LOAD_WAVE);
            #else
            // TODO: Port NNS_SND_ARC_LOAD_WAVE to PAL
            #endif
            GF_ASSERT(FALSE);
        } else {
            #ifdef PLATFORM_DS
            SoundSystem_LoadSequenceEx(*newFieldBGM, NNS_SND_ARC_LOAD_WAVE);
            #else
            // TODO: Port NNS_SND_ARC_LOAD_WAVE to PAL
            #endif
        }

        SoundSystem_SaveHeapState(SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_BGM));
    }
}

static void Sound_Impl_PlayBattleBGM(u16 bgmID, int unused)
{
    UNUSED(SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_PERSISTENT));

    Sound_Impl_PauseOrStopFieldBGM();
    SoundSystem_LoadHeapState(Sound_GetHeapState(SOUND_HEAP_STATE_BGM_BANK));
    Sound_LoadSoundEffectsForScene(SOUND_SCENE_BATTLE);
    SoundSystem_SaveHeapState(SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_SFX));
    Sound_SetFieldBGMBankState(FIELD_BGM_BANK_STATE_SWITCH);
    Sound_PlayBGM(bgmID);
}

static void sub_020048AC(u16 param0, int param1)
{
    UNUSED(SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_PERSISTENT));

    Sound_StopWaveOutAndSequences();
    Sound_ClearBGMPauseFlags();
    Sound_LoadSoundEffectsForSceneWithState(SOUND_SCENE_FIELD);
    Sound_PlayBGM(param0);
}

void Sound_Impl_PlayContestBGM(u16 bgmID, int unused)
{
    UNUSED(SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_PERSISTENT));

    Sound_StopWaveOutAndSequences();
    Sound_LoadSoundEffectsForSceneWithState(SOUND_SCENE_CONTEST);
    Sound_SetFieldBGMBankState(FIELD_BGM_BANK_STATE_SWITCH);
    Sound_PlayBGM(bgmID);
}

void sub_020048F0(u16 bgmID, int unused)
{
    UNUSED(SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_PERSISTENT));

    Sound_StopWaveOutAndSequences();
    Sound_LoadSoundEffectsForSceneWithState(7);
    Sound_SetFieldBGMBankState(FIELD_BGM_BANK_STATE_SWITCH);
    Sound_PlayBGM(bgmID);
}

static void Sound_Impl_LoadSubSceneSoundData(u8 scene)
{
    Sound_LoadHeapStateBGM();
    Sound_LoadSoundEffectsForScene(scene);
    SoundSystem_SaveHeapState(SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_SUB_SFX));
}

static void Sound_Impl_PlayCutsceneBGM(u8 scene, u16 bgmID, int unused)
{
    UNUSED(SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_PERSISTENT));

    Sound_StopWaveOutAndSequences();
    Sound_LoadSoundEffectsForSceneWithState(scene);
    Sound_PlayBGM(bgmID);
}

void Sound_SwapBGM(u16 bgmID)
{
    BOOL v0;
    SoundSystem *v1 = SoundSystem_Get();

    Sound_Impl_PauseOrStopFieldBGM();

    v0 = Sound_PlayBGM(bgmID);
    return;
}

void Sound_LoadHeapStateBGM(void)
{
    SoundSystem_LoadHeapState(Sound_GetHeapState(SOUND_HEAP_STATE_BGM));
}

int Sound_GetHeapState(enum SoundHeapState state)
{
    int *param;
    SoundSystem *soundSys = SoundSystem_Get();

    if (state >= SOUND_HEAP_STATE_COUNT) {
        GF_ASSERT(FALSE);
        param = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_BGM);
        return *param;
    }

    switch (state) {
    case SOUND_HEAP_STATE_EMPTY:
        param = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_EMPTY);
        break;
    case SOUND_HEAP_STATE_PERSISTENT:
        param = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_PERSISTENT);
        break;
    case SOUND_HEAP_STATE_BGM_BANK:
        param = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_BGM_BANK);
        break;
    case SOUND_HEAP_STATE_SFX:
        param = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_SFX);
        break;
    case SOUND_HEAP_STATE_BGM:
        param = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_BGM);
        break;
    case SOUND_HEAP_STATE_SUB_SFX:
        param = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_SUB_SFX);
        break;
    case SOUND_HEAP_STATE_FANFARE:
        param = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_FANFARE);
        break;
    }

    return *param;
}

void Sound_SetBGMPlayerPaused(u8 playerID, BOOL paused)
{
    u8 handleType;
    u8 *playerPaused;

    if (playerID == PLAYER_FIELD) {
        playerPaused = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_FIELD_BGM_PAUSED);
        handleType = SOUND_HANDLE_TYPE_FIELD_BGM;
    } else if (playerID == PLAYER_BGM) {
        playerPaused = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_BGM_PAUSED);
        handleType = SOUND_HANDLE_TYPE_BGM;
    } else {
        return;
    }

    if (paused == FALSE) {
        Sound_SetCurrentBGM(Sound_GetSequenceIDFromSoundHandle(SoundSystem_GetSoundHandle(handleType)));
    }

    #ifdef PLATFORM_DS
    NNS_SndPlayerPause(SoundSystem_GetSoundHandle(handleType), paused);
    #else
    // TODO: Port NNS_SndPlayerPause to PAL
    #endif
    *playerPaused = paused;
}

void Sound_ClearBGMPauseFlags(void)
{
    u8 *fieldBGMPaused = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_FIELD_BGM_PAUSED);
    u8 *bgmPaused = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_BGM_PAUSED);

    *fieldBGMPaused = FALSE;
    *bgmPaused = FALSE;
}

void Sound_FadeVolumeForHandle(enum SoundHandleType handleType, int targetVolume, int frames)
{
    #ifdef PLATFORM_DS
    NNS_SndPlayerMoveVolume(SoundSystem_GetSoundHandle(handleType), targetVolume, frames);
    #else
    // TODO: Port NNS_SndPlayerMoveVolume to PAL
    #endif
}

void Sound_SetInitialVolumeForHandle(enum SoundHandleType handleType, int volume)
{
    if (volume < SOUND_VOLUME_MIN) {
        volume = SOUND_VOLUME_MIN;
    }

    if (volume > SOUND_VOLUME_MAX) {
        volume = SOUND_VOLUME_MAX;
    }

    #ifdef PLATFORM_DS
    NNS_SndPlayerSetInitialVolume(SoundSystem_GetSoundHandle(handleType), volume);
    #else
    // TODO: Port NNS_SndPlayerSetInitialVolume to PAL
    #endif
}

void Sound_AdjustVolumeForVoiceChat(int seqID)
{
    u8 playerID = Sound_GetPlayerForSequence(seqID);
    enum SoundHandleType handleType = SoundSystem_GetSoundHandleTypeFromPlayerID(playerID);

    Sound_AdjustVolumeForVoiceChatEx(seqID, handleType);
}

void Sound_AdjustVolumeForVoiceChatEx(int seqID, enum SoundHandleType handleType)
{
    u8 volume;
    #ifdef PLATFORM_DS
    const NNSSndSeqParam *param = NNS_SndArcGetSeqParam(seqID);
    #else
    // TODO: Port NNS_SndArcGetSeqParam to PAL
    #endif

    switch (handleType) {
    case SOUND_HANDLE_TYPE_POKEMON_CRY:
    case SOUND_HANDLE_TYPE_ECHO:
        volume = SOUND_VOLUME_MAX;
        break;

    default:
        if (param == NULL) {
            return;
        }

        volume = param->volume;
        break;
    }

    if (sub_02036314() == TRUE) {
        Sound_SetInitialVolumeForHandle(handleType, (volume / 5));
    }
}

void Sound_SetInitialVolumeForSequence(u16 seqID, int volume)
{
    u8 playerID = Sound_GetPlayerForSequence(seqID);
    enum SoundHandleType handleType = SoundSystem_GetSoundHandleTypeFromPlayerID(playerID);

    Sound_SetInitialVolumeForHandle(handleType, volume);
}

BOOL Sound_PlaySequenceWithPlayer(enum SoundHandleType handleType, int playerID, u16 seqID)
{
    #ifdef PLATFORM_DS
    return NNS_SndArcPlayerStartSeqEx(SoundSystem_GetSoundHandle(handleType), playerID, -1, -1, seqID);
    #else
    // TODO: Port NNS_SndArcPlayerStartSeqEx to PAL
    #endif
}

int Sound_GetNumberOfPlayingSequencesForPlayer(int playerID)
{
    if (playerID < PLAYER_PV) {
        GF_ASSERT(FALSE);
    }

    #ifdef PLATFORM_DS
    return NNS_SndPlayerCountPlayingSeqByPlayerNo(playerID);
    #else
    // TODO: Port NNS_SndPlayerCountPlayingSeqByPlayerNo to PAL
    #endif
}

u8 Sound_GetPlayerForSequence(u16 seqID)
{
    if (seqID == 0) {
        return 0xFF;
    }

    #ifdef PLATFORM_DS
    const NNSSndSeqParam *param = NNS_SndArcGetSeqParam(seqID);
    #else
    // TODO: Port NNS_SndArcGetSeqParam to PAL
    #endif
    if (param == NULL) {
        return 0xff;
    }

    return param->playerNo;
}

int Sound_GetSequenceIDFromSoundHandle(NNSSndHandle *handle)
{
    #ifdef PLATFORM_DS
    return NNS_SndPlayerGetSeqNo(handle);
    #else
    // TODO: Port NNS_SndPlayerGetSeqNo to PAL
    #endif
}

const NNSSndArcBankInfo *Sound_GetBankInfoForSequence(int seqID)
{
    #ifdef PLATFORM_DS
    return NNS_SndArcGetBankInfo(Sound_GetBankIDFromSequenceID(seqID));
    #else
    // TODO: Port NNS_SndArcGetBankInfo to PAL
    #endif
}

u16 Sound_GetBankIDFromSequenceID(int seqID)
{
    #ifdef PLATFORM_DS
    const NNSSndSeqParam *param = NNS_SndArcGetSeqParam(seqID);
    #else
    // TODO: Port NNS_SndArcGetSeqParam to PAL
    #endif
    if (param == NULL) {
        return 0;
    }

    return param->bankNo;
}

MICResult Sound_StartMicAutoSampling(MICAutoParam *param)
{
    return MIC_StartAutoSampling(param);
}

MICResult Sound_StopMicAutoSampling(void)
{
    UNUSED(SoundSystem_Get());
    return MIC_StopAutoSampling();
}

MICResult Sound_StartMicManualSampling(MICSamplingType type, void *buffer, MICCallback callback, void *param)
{
    return MIC_DoSamplingAsync(type, buffer, callback, param);
}

NNSSndWaveOutHandle *Sound_GetWaveOutHandle(enum WaveOutChannel channel)
{
    UNUSED(SoundSystem_Get());
    u8 *primaryAllocated = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_PRIMARY_ALLOCATED);
    u8 *secondaryAllocated = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_SECONDARY_ALLOCATED);

    if (channel != WAVE_OUT_CHANNEL_PRIMARY && channel != WAVE_OUT_CHANNEL_SECONDARY) {
        GF_ASSERT(FALSE);
    }

    if (channel == WAVE_OUT_CHANNEL_PRIMARY && *primaryAllocated == FALSE) {
        GF_ASSERT(FALSE);
    }

    if (channel == WAVE_OUT_CHANNEL_SECONDARY && *secondaryAllocated == FALSE) {
        GF_ASSERT(FALSE);
    }

    if (channel == WAVE_OUT_CHANNEL_PRIMARY) {
        return SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_PRIMARY_HANDLE);
    } else {
        return SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_SECONDARY_HANDLE);
    }
}

BOOL Sound_AllocateWaveOutChannel(enum WaveOutChannel channel)
{
    NNSSndWaveOutHandle *handle;

    UNUSED(SoundSystem_Get());
    u8 *primaryAllocated = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_PRIMARY_ALLOCATED);
    u8 *secondaryAllocated = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_SECONDARY_ALLOCATED);

    if (channel != WAVE_OUT_CHANNEL_PRIMARY && channel != WAVE_OUT_CHANNEL_SECONDARY) {
        GF_ASSERT(FALSE);
    }

    if (channel == WAVE_OUT_CHANNEL_PRIMARY) {
        if (*primaryAllocated == FALSE) {
            handle = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_PRIMARY_HANDLE);
            #ifdef PLATFORM_DS
            *handle = NNS_SndWaveOutAllocChannel(channel);
            #else
            // TODO: Port NNS_SndWaveOutAllocChannel to PAL
            #endif

            #ifdef PLATFORM_DS
            if (*handle == NNS_SND_WAVEOUT_INVALID_HANDLE) {
            #else
            // TODO: Port NNS_SND_WAVEOUT_INVALID_HANDLE to PAL
            #endif
                return FALSE;
            }

            *primaryAllocated = TRUE;
        } else {
            GF_ASSERT(FALSE);
        }
    } else {
        if (*secondaryAllocated == FALSE) {
            handle = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_SECONDARY_HANDLE);
            #ifdef PLATFORM_DS
            *handle = NNS_SndWaveOutAllocChannel(channel);
            #else
            // TODO: Port NNS_SndWaveOutAllocChannel to PAL
            #endif

            #ifdef PLATFORM_DS
            if (*handle == NNS_SND_WAVEOUT_INVALID_HANDLE) {
            #else
            // TODO: Port NNS_SND_WAVEOUT_INVALID_HANDLE to PAL
            #endif
                return FALSE;
            }

            *secondaryAllocated = TRUE;
        } else {
            GF_ASSERT(FALSE);
        }
    }

    return TRUE;
}

void Sound_FreeWaveOutChannel(enum WaveOutChannel channel)
{
    UNUSED(SoundSystem_Get());

    u8 *primaryAllocated = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_PRIMARY_ALLOCATED);
    u8 *secondaryAllocated = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_SECONDARY_ALLOCATED);

    if (channel != WAVE_OUT_CHANNEL_PRIMARY && channel != WAVE_OUT_CHANNEL_SECONDARY) {
        GF_ASSERT(FALSE);
        return;
    }

    if (channel == WAVE_OUT_CHANNEL_PRIMARY) {
        if (*primaryAllocated == TRUE) {
            #ifdef PLATFORM_DS
            NNS_SndWaveOutFreeChannel(*Sound_GetWaveOutHandle(channel));
            #else
            // TODO: Port NNS_SndWaveOutFreeChannel to PAL
            #endif
            *primaryAllocated = FALSE;
        } else {
            GF_ASSERT(FALSE);
        }
    } else {
        if (*secondaryAllocated == TRUE) {
            #ifdef PLATFORM_DS
            NNS_SndWaveOutFreeChannel(*Sound_GetWaveOutHandle(channel));
            #else
            // TODO: Port NNS_SndWaveOutFreeChannel to PAL
            #endif
            *secondaryAllocated = FALSE;
        } else {
            GF_ASSERT(FALSE);
        }
    }
}

BOOL Sound_PlayWaveOut(WaveOutParam *param, enum WaveOutChannel channel)
{
    #ifdef PLATFORM_DS
    BOOL success = NNS_SndWaveOutStart(
    #else
    // TODO: Port NNS_SndWaveOutStart to PAL
    #endif
        *param->handle,
        param->format,
        param->data,
        param->loop,
        param->loopStartSample,
        param->samples,
        param->sampleRate,
        param->volume,
        param->speed,
        param->pan);

    if (success == FALSE) {
        Sound_FreeWaveOutChannel(channel);
    }

    return success;
}

void Sound_StopWaveOut(enum WaveOutChannel channel)
{
    #ifdef PLATFORM_DS
    NNS_SndWaveOutStop(*Sound_GetWaveOutHandle(channel));
    #else
    // TODO: Port NNS_SndWaveOutStop to PAL
    #endif
}

BOOL Sound_IsWaveOutPlaying(enum WaveOutChannel channel)
{
    #ifdef PLATFORM_DS
    return NNS_SndWaveOutIsPlaying(*Sound_GetWaveOutHandle(channel));
    #else
    // TODO: Port NNS_SndWaveOutIsPlaying to PAL
    #endif
}

void Sound_SetWaveOutPan(enum WaveOutChannel channel, u8 pan)
{
    u8 clampedPan;
    if (pan > 127) {
        clampedPan = 127;
    } else {
        clampedPan = pan;
    }

    #ifdef PLATFORM_DS
    NNS_SndWaveOutSetPan(*Sound_GetWaveOutHandle(channel), clampedPan);
    #else
    // TODO: Port NNS_SndWaveOutSetPan to PAL
    #endif
}

void Sound_SetWaveOutSpeed(enum WaveOutChannel channel, u32 speed)
{
    #ifdef PLATFORM_DS
    NNS_SndWaveOutSetSpeed(*Sound_GetWaveOutHandle(channel), speed);
    #else
    // TODO: Port NNS_SndWaveOutSetSpeed to PAL
    #endif
}

void Sound_SetWaveOutVolume(enum WaveOutChannel channel, int volume)
{
    if (sub_02036314() == 1) {
        #ifdef PLATFORM_DS
        NNS_SndWaveOutSetVolume(*Sound_GetWaveOutHandle(channel), volume / 5);
        #else
        // TODO: Port NNS_SndWaveOutSetVolume to PAL
        #endif
    } else {
        #ifdef PLATFORM_DS
        NNS_SndWaveOutSetVolume(*Sound_GetWaveOutHandle(channel), volume);
        #else
        // TODO: Port NNS_SndWaveOutSetVolume to PAL
        #endif
    }
}

BOOL Sound_PlayWaveOutReversed(u16 waveArcID, int volume, int pan, enum WaveOutChannel channel, int heapID)
{
    UNUSED(SoundSystem_Get());
    void **reverseBuffer = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_REVERSE_BUFFER);

    if (channel != WAVE_OUT_CHANNEL_PRIMARY && channel != WAVE_OUT_CHANNEL_SECONDARY) {
        GF_ASSERT(FALSE);
    }

    #ifdef PLATFORM_DS
    const NNSSndArcWaveArcInfo *info = NNS_SndArcGetWaveArcInfo(waveArcID);
    #else
    // TODO: Port NNS_SndArcGetWaveArcInfo to PAL
    #endif
    if (info == NULL) {
        GF_ASSERT(FALSE);
        return FALSE;
    }

    #ifdef PLATFORM_DS
    u32 fileSize = NNS_SndArcGetFileSize(info->fileId);
    #else
    // TODO: Port NNS_SndArcGetFileSize to PAL
    #endif
    if (fileSize == 0) {
        GF_ASSERT(FALSE);
        return FALSE;
    }

    if (channel == WAVE_OUT_CHANNEL_PRIMARY) {
        *reverseBuffer = Heap_Alloc(heapID, fileSize);
        if (*reverseBuffer == NULL) {
            GF_ASSERT(FALSE);
            return FALSE;
        }

        memset(*reverseBuffer, 0, fileSize);

        #ifdef PLATFORM_DS
        if (NNS_SndArcReadFile(info->fileId, *reverseBuffer, fileSize, 0) == -1) {
        #else
        // TODO: Port NNS_SndArcReadFile to PAL
        #endif
            GF_ASSERT(FALSE);
            return FALSE;
        }

        Sound_Impl_ReverseBuffer(*reverseBuffer, fileSize);
    }

    WaveOutParam param;
    param.handle = Sound_GetWaveOutHandle(channel);
    #ifdef PLATFORM_DS
    param.format = NNS_SND_WAVE_FORMAT_PCM8;
    #else
    // TODO: Port NNS_SND_WAVE_FORMAT_PCM8 to PAL
    #endif
    param.data = *reverseBuffer;
    param.loop = FALSE;
    param.loopStartSample = 0;
    param.samples = fileSize;
    param.sampleRate = 13379;
    param.volume = volume;
    param.speed = WAVE_OUT_SPEED(0.75);
    param.pan = pan;

    BOOL result = Sound_PlayWaveOut(&param, channel);

    Sound_SetWaveOutVolume(channel, volume);

    u8 *reversedPlayback = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_REVERSED_PLAYBACK);
    *reversedPlayback = TRUE;

    return result;
}

static void Sound_Impl_ReverseBuffer(u8 *buffer, u32 size)
{
    for (int i = 0; i < (size / 2); i++) {
        u8 tmp = buffer[i];
        buffer[i] = buffer[size - 1 - i];
        buffer[size - 1 - i] = tmp;
    }
}

void Sound_StopWaveOutReversed(enum WaveOutChannel channel)
{
    UNUSED(SoundSystem_Get());
    u8 *reversedPlaybackEnabled = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_REVERSED_PLAYBACK);
    void **reverseBuffer = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_REVERSE_BUFFER);

    if (channel != WAVE_OUT_CHANNEL_PRIMARY && channel != WAVE_OUT_CHANNEL_SECONDARY) {
        GF_ASSERT(FALSE);
    }

    Sound_StopWaveOut(channel);

    if (*reversedPlaybackEnabled == TRUE) {
        *reversedPlaybackEnabled = FALSE;
        Heap_Free(*reverseBuffer);
    }
}

BOOL Sound_IsCaptureActive(void)
{
    #ifdef PLATFORM_DS
    return NNS_SndCaptureIsActive();
    #else
    // TODO: Port NNS_SndCaptureIsActive to PAL
    #endif
}

BOOL Sound_StartReverb(int volume)
{
    UNUSED(SoundSystem_Get());
    void *buffer = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_CAPTURE_BUFFER);
    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port NNS_SndCaptureStartReverb to PAL
    #endif
    return NNS_SndCaptureStartReverb(buffer, 0x1000, (NNS_SND_CAPTURE_FORMAT_PCM16), 16000, volume);
    #else
    // TODO: Port NNS_SND_CAPTURE_FORMAT_PCM16 to PAL
    #endif
}

void Sound_StopReverb(int frames)
{
    #ifdef PLATFORM_DS
    NNS_SndCaptureStopReverb(frames);
    #else
    // TODO: Port NNS_SndCaptureStopReverb to PAL
    #endif
}

void Sound_SetReverbVolume(int targetVolume, int frames)
{
    #ifdef PLATFORM_DS
    NNS_SndCaptureSetReverbVolume(targetVolume, frames);
    #else
    // TODO: Port NNS_SndCaptureSetReverbVolume to PAL
    #endif
}

BOOL Sound_StartFilter(void)
{
    UNUSED(SoundSystem_Get());

    MI_CpuClear8(SoundSystem_GetParam(SOUND_SYSTEM_PARAM_FILTER_CALLBACK_PARAM), sizeof(SoundFilterCallbackParam));
    #ifdef PLATFORM_DS
    return NNS_SndCaptureStartEffect(
    #else
    // TODO: Port NNS_SndCaptureStartEffect to PAL
    #endif
        SoundSystem_GetParam(SOUND_SYSTEM_PARAM_CAPTURE_BUFFER),
        SOUND_SYSTEM_CAPTURE_BUFFER_SIZE,
        #ifdef PLATFORM_DS
        NNS_SND_CAPTURE_FORMAT_PCM16,
        #else
        // TODO: Port NNS_SND_CAPTURE_FORMAT_PCM16 to PAL
        #endif
        SOUND_FILTER_SAMPLE_RATE,
        SOUND_FILTER_INTERVAL,
        Sound_Impl_FilterCallback,
        SoundSystem_GetParam(SOUND_SYSTEM_PARAM_FILTER_CALLBACK_PARAM));
}

void Sound_StopFilter(void)
{
    #ifdef PLATFORM_DS
    NNS_SndCaptureStopEffect();
    #else
    // TODO: Port NNS_SndCaptureStopEffect to PAL
    #endif
}

void Sound_SetFilterSize(int size)
{
    u8 *filterSize = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_FILTER_SIZE);

    if (size > SOUND_FILTER_MAX_SIZE) {
        size = SOUND_FILTER_MAX_SIZE;
    } else if (size < 0) {
        size = 0;
    }

    *filterSize = size;
}

void Sound_SetPitchForHandle(enum SoundHandleType handleType, u16 tracks, int pitch)
{
    #ifdef PLATFORM_DS
    NNS_SndPlayerSetTrackPitch(SoundSystem_GetSoundHandle(handleType), tracks, pitch);
    #else
    // TODO: Port NNS_SndPlayerSetTrackPitch to PAL
    #endif
}

void Sound_SetPitchForSequence(u16 seqID, u16 tracks, int pitch)
{
    u8 playerID = Sound_GetPlayerForSequence(seqID);
    enum SoundHandleType handleType = SoundSystem_GetSoundHandleTypeFromPlayerID(playerID);

    Sound_SetPitchForHandle(handleType, tracks, pitch);
}

void Sound_SetPanForHandle(enum SoundHandleType handleType, u16 tracks, int pan)
{
    #ifdef PLATFORM_DS
    NNS_SndPlayerSetTrackPan(SoundSystem_GetSoundHandle(handleType), tracks, pan);
    #else
    // TODO: Port NNS_SndPlayerSetTrackPan to PAL
    #endif
}

void Sound_SetTempoRatioForHandle(enum SoundHandleType handleType, int tempoRatio)
{
    #ifdef PLATFORM_DS
    NNS_SndPlayerSetTempoRatio(SoundSystem_GetSoundHandle(handleType), tempoRatio);
    #else
    // TODO: Port NNS_SndPlayerSetTempoRatio to PAL
    #endif
}

void Sound_SetPlaybackMode(int mode)
{
    #ifdef PLATFORM_DS
    NNS_SndSetMonoFlag(mode);
    #else
    // TODO: Port NNS_SndSetMonoFlag to PAL
    #endif
    sPlaybackMode = mode;
}

void Sound_SetFadeCounter(int frames)
{
    int *param = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_FADE_COUNTER);
    *param = frames;
}

void Sound_SetFollowUpWaitFrames(int frames)
{
    int *param = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_FOLLOW_UP_WAIT_FRAMES);
    *param = frames;
}

BOOL Sound_UpdateFollowUpWaitFrames()
{
    u16 *waitFrames = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_FOLLOW_UP_WAIT_FRAMES);
    if (*waitFrames <= 0) {
        *waitFrames = 0;
        return FALSE;
    }

    (*waitFrames)--;
    return *waitFrames;
}

void Sound_SetMasterVolume(int volume)
{
    #ifdef PLATFORM_DS
    NNS_SndSetMasterVolume(volume);
    #else
    // TODO: Port NNS_SndSetMasterVolume to PAL
    #endif
}

void *Sound_GetWaveBuffer(void)
{
    return sWaveBuffer;
}

// Needs to be set to FIELD_BGM_BANK_STATE_SWITCH before calling
// Sound_PlayBGM, if the bank needs to be switched
void Sound_SetFieldBGMBankState(int state)
{
    u8 *param = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_FIELD_BGM_BANK_STATE);
    *param = state;
}

BOOL Sound_FadeOutAndPlayBGM(u8 unused1, u16 bgmID, int fadeOutFrames, int waitFrames, u8 bankState, void *unused2)
{
    u8 *subScene = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_SUB_SCENE);

    Sound_Impl_FadeToBGM(unused1, bgmID, fadeOutFrames, waitFrames, bankState, unused2);
    *subScene = SOUND_SCENE_NONE;
    SoundSystem_SetState(SOUND_SYSTEM_STATE_FADE_OUT_PLAY);

    return TRUE;
}

BOOL Sound_FadeToBGM(u8 unused1, u16 bgmID, int fadeOutFrames, int waitFrames, int fadeInFrames, u8 bankState, void *unused2)
{
    int *fadeFrames = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_FOLLOW_UP_FADE_FRAMES);

    Sound_Impl_FadeToBGM(unused1, bgmID, fadeOutFrames, waitFrames, bankState, unused2);
    *fadeFrames = fadeInFrames;
    SoundSystem_SetState(SOUND_SYSTEM_STATE_FADE_OUT_FADE_IN);

    return TRUE;
}

static void Sound_Impl_FadeToBGM(u8 unused1, u16 bgmID, int fadeOutFrames, int fadeInFrames, u8 bankState, void *unused2)
{
    const NNSSndArcBankInfo **currentBankInfo = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_CURRENT_BANK_INFO);

    Sound_FadeOutBGM(SOUND_VOLUME_MIN, fadeOutFrames);
    Sound_SetCurrentBGM(SEQ_NONE);

    Sound_SetNextBGM(bgmID);
    Sound_SetFollowUpWaitFrames(fadeInFrames);

    *currentBankInfo = Sound_GetBankInfoForSequence(bgmID);
    Sound_SetFieldBGMBankState(bankState);
}

const u8 *Sound_WaveData_GetSamples(const SNDWaveData *data)
{
    if (data == NULL) {
        return NULL;
    }

    return &data->samples[0];
}

const u32 Sound_WaveData_GetLoopLength(const SNDWaveData *data)
{
    if (data == NULL) {
        return 0;
    }

    return data->param.looplen;
}

const SNDWaveData *Sound_LoadPokedexDataForSpecies(int species)
{
    SoundSystem_LoadHeapState(Sound_GetHeapState(SOUND_HEAP_STATE_SUB_SFX));

    u16 waveArcID = species;

    if (waveArcID != SPECIES_CHATOT) {
        if (species > MAX_SPECIES || species == SPECIES_NONE) {
            waveArcID = SPECIES_BULBASAUR;
        }
    }

    SoundSystem_LoadWaveArc(waveArcID);

    return Sound_Impl_GetWaveDataForSpecies(waveArcID);
}

static const SNDWaveData *Sound_Impl_GetWaveDataForSpecies(int species)
{
    const SNDWaveData **waveData = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_CURRENT_WAVE_DATA);

    u16 waveArcID = species;
    if (waveArcID != SPECIES_CHATOT) {
        if (species > MAX_SPECIES || species == SPECIES_NONE) {
            waveArcID = SPECIES_BULBASAUR;
        }
    }

    #ifdef PLATFORM_DS
    const NNSSndArcWaveArcInfo *info = NNS_SndArcGetWaveArcInfo(waveArcID);
    #else
    // TODO: Port NNS_SndArcGetWaveArcInfo to PAL
    #endif
    if (info == NULL) {
        GF_ASSERT(FALSE);
        return NULL;
    }

    #ifdef PLATFORM_DS
    SNDWaveArc *waveArc = NNS_SndArcGetFileAddress(info->fileId);
    #else
    // TODO: Port NNS_SndArcGetFileAddress to PAL
    #endif
    if (waveArc == NULL) {
        GF_ASSERT(FALSE);
        return NULL;
    }

    #ifdef PLATFORM_DS
    *waveData = SND_GetWaveDataAddress(waveArc, 0);
    #else
    // TODO: Port SND_GetWaveDataAddress to PAL
    #endif
    return *waveData;
}

u32 Sound_GetNumberOfPlayedCrySamples(int unused, const SNDWaveData *data, int pitch)
{
    u32 ticks = Sound_GetTicksForHandle(SOUND_HANDLE_TYPE_POKEMON_CRY);
    u32 maxSamples = data->param.looplen;

    // Convert pitch to octaves
    int scale = pitch / SOUND_SEMITONES(12);
    scale = scale * 2;

    if (scale < 0) {
        scale = 32 / scale * -1;
    } else if (scale == 0) {
        scale = 32;
    } else {
        scale = 32 * scale;
    }

    u32 samples = (ticks * scale);
    if (samples >= maxSamples) {
        samples = 0;
    }

    return samples;
}

u32 Sound_GetTicksForHandle(enum SoundHandleType handleType)
{
    #ifdef PLATFORM_DS
    return NNS_SndPlayerGetTick(SoundSystem_GetSoundHandle(handleType));
    #else
    // TODO: Port NNS_SndPlayerGetTick to PAL
    #endif
}

// Maps the number of times the sample midpoint (128) is crossed in a certain window
// (the values) to an amplitude from 0-8 (the indices)
static const u8 sCrossingsToAmplitudeMapping[WAVEFORM_MAX_AMPLITUDE] = { 2, 3, 4, 5, 6, 7, 8, 9, 10 };

// Stores the most frequent amplitude levels of the given waveform data
// within the last WAVEFORM_SAMPLE_WINDOW samples in `amplitudes`
// Used exclusively to generate the bar graphs for pokemon cries in the pokedex
void Sound_WaveData_AccumulateAmplitudes(const SNDWaveData *data, u8 *amplitudes, int numAmplitudes, int pitch)
{
    enum {
        SAMPLE_MIDPOINT = 128,
        ABOVE_MIDPOINT = 1,
        BELOW_MIDPOINT = -1
    };

    // All of these are required to match
    int startSample;
    s8 level;
    int i, j, amplitude, numCrossings;

    u32 samples = Sound_GetNumberOfPlayedCrySamples(1, data, pitch);
    startSample = samples - WAVEFORM_SAMPLE_WINDOW;
    if (startSample < 0) {
        startSample = 0;
    }

    numCrossings = 0;
    amplitude = 0;
    level = (data->samples[startSample] < SAMPLE_MIDPOINT) ? ABOVE_MIDPOINT : BELOW_MIDPOINT;

    for (i = startSample; i < samples; i += 2) {
        BOOL crossedMidpoint = FALSE;

        if (level > 0) {
            if (data->samples[i] > SAMPLE_MIDPOINT) {
                crossedMidpoint = TRUE;
            } else {
                numCrossings++;
            }
        } else {
            if (data->samples[i] < SAMPLE_MIDPOINT) {
                crossedMidpoint = TRUE;
            } else {
                numCrossings++;
            }
        }

        if (crossedMidpoint == TRUE) {
            for (j = 0; j < WAVEFORM_MAX_AMPLITUDE; j++) {
                if (numCrossings < sCrossingsToAmplitudeMapping[j]) {
                    amplitude = j;
                    break;
                }
            }

            if (amplitude >= numAmplitudes) {
                amplitude = numAmplitudes - 1;
            }

            if (numCrossings != 0) {
                amplitudes[numAmplitudes - 1 - amplitude]++;
            }

            numCrossings = 0;
            crossedMidpoint = 0;
            level = (data->samples[i] < SAMPLE_MIDPOINT) ? ABOVE_MIDPOINT : BELOW_MIDPOINT;
        }
    }

    // Cap amplitudes at 9
    for (i = 0; i < numAmplitudes; i++) {
        if (amplitudes[i] >= 10) {
            amplitudes[i] = 9;
        }
    }

    // Apply smoothing
    for (i = 0; i < numAmplitudes - 1; i++) {
        if (amplitudes[i] == 0 && amplitudes[i + 1] != 0) {
            amplitudes[i] = amplitudes[i + 1] / 2;
            break;
        }
    }
}

static void Sound_Impl_FilterCallback(void *bufferL, void *bufferR, u32 length, NNSSndCaptureFormat format, void *arg)
{
    enum Channel {
        L,
        R
    };

    s16 originalSamples[SOUND_FILTER_MAX_SIZE][2];
    s16 *bufL = (s16 *)bufferL;
    s16 *bufR = (s16 *)bufferR;
    SoundFilterCallbackParam *param = (SoundFilterCallbackParam *)arg;
    u8 *filterSize = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_FILTER_SIZE);
    int total, i, j; // Need to be declared here to match

    #ifdef PLATFORM_DS
    int sampleCount = (format == NNS_SND_CAPTURE_FORMAT_PCM8) ? length : (length >> 1);
    #else
    // TODO: Port NNS_SND_CAPTURE_FORMAT_PCM8 to PAL
    #endif

    if (*filterSize == 0) {
        return;
    }

    for (i = 0; i < *filterSize; i++) {
        originalSamples[i][L] = bufL[sampleCount - *filterSize + i];
        originalSamples[i][R] = bufR[sampleCount - *filterSize + i];
    }

    // The following applies a simple moving average low-pass filter to the audio
    for (i = sampleCount - 1; i >= *filterSize - 1; i--) {
        total = 0;
        for (j = 0; j < *filterSize; j++) {
            total += bufL[i - j];
        }

        bufL[i] = (s16)(total / *filterSize);

        total = 0;
        for (j = 0; j < *filterSize; j++) {
            total += bufR[i - j];
        }

        bufR[i] = (s16)(total / (*filterSize));
    }

    for (i = 0; i < *filterSize; i++) {
        param->samples[i][L] = originalSamples[i][L];
        param->samples[i][R] = originalSamples[i][R];
    }

    DC_FlushRange(bufferL, sampleCount);
    DC_FlushRange(bufferR, sampleCount);
}

static void Sound_SetBGMAllocatableChannels(u16 channels)
{
    #ifdef PLATFORM_DS
    NNS_SndPlayerSetAllocatableChannel(PLAYER_BGM, channels);
    #else
    // TODO: Port NNS_SndPlayerSetAllocatableChannel to PAL
    #endif
}

void Sound_ConfigureBGMChannelsAndReverb(enum SoundChannelConfig config)
{
    if (config == SOUND_CHANNEL_CONFIG_DEFAULT) {
        Sound_SetBGMAllocatableChannels(BGM_PLAYER_NORMAL_CHANNELS);
        Sound_StopReverb(0);
    } else if (config == SOUND_CHANNEL_CONFIG_TITLE) {
        Sound_SetBGMAllocatableChannels(BGM_PLAYER_EXTRA_CHANNELS);
        Sound_StartReverb(30);
    } else { // SOUND_CHANNEL_CONFIG_ENDING or anything else
        Sound_SetBGMAllocatableChannels(BGM_PLAYER_EXTRA_CHANNELS);
        Sound_StartReverb(15);
    }

    UNUSED(Sound_IsCaptureActive());
}

static void Sound_Impl_PauseOrStopFieldBGM(void)
{
    if ((Sound_IsFadeActive() == FALSE) && (Sound_GetSequenceIDFromSoundHandle(SoundSystem_GetSoundHandle(SOUND_HANDLE_TYPE_FIELD_BGM)) != -1) && (Sound_GetCurrentBGM() != SEQ_POKERADAR)) {
        Sound_StopAll();
        Sound_SetBGMPlayerPaused(PLAYER_FIELD, TRUE);
    } else {
        Sound_StopWaveOutAndSequences();
    }
}

void Sound_SetPlayerVolume(int playerID, int volume)
{
    #ifdef PLATFORM_DS
    NNS_SndPlayerSetPlayerVolume(playerID, volume);
    #else
    // TODO: Port NNS_SndPlayerSetPlayerVolume to PAL
    #endif
}

void Sound_Set2PokemonCriesAllowed(BOOL allowed)
{
    u8 *param = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_ALLOW_2_POKEMON_CRIES);
    *param = allowed;
}

void sub_02005464(BOOL param0)
{
    u8 *v0 = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_54);

    *v0 = param0;
    return;
}
