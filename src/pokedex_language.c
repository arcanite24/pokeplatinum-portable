#include "pokedex_language.h"

#ifdef PLATFORM_DS
#include <nitro.h>
#else
#include "platform/platform_types.h"
#endif
#include <string.h>

#include "global/pm_version.h"

static const u8 languageArray[NUM_LANGUAGES] = {
    JAPANESE,
    ENGLISH,
    FRENCH,
    GERMAN,
    ITALIAN,
    SPANISH
};

int PokedexLanguage_LanguageToIndex(int language)
{
    int languageIndex;

    for (languageIndex = 0; languageIndex < NUM_LANGUAGES; languageIndex++) {
        if (language == languageArray[languageIndex]) {
            break;
        }
    }

    return languageIndex;
}

int PokedexLanguage_IndexToLanguage(int languageIndex)
{
    GF_ASSERT(languageIndex < NUM_LANGUAGES);
    return languageArray[languageIndex];
}
