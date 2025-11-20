#ifndef VERSIONS_H
#define VERSIONS_H

// Game version constants for SDL build
// These represent different Pokemon game versions for compatibility

#define VERSION_NONE      0
#define VERSION_RUBY      1  
#define VERSION_SAPPHIRE  2
#define VERSION_EMERALD   3
#define VERSION_FIRERED   4
#define VERSION_LEAFGREEN 5

// Global language setting (stub for SDL build)
extern int gGameLanguage;

// Global ignore cartridge flag
extern int gIgnoreCartridgeForWake;

// Game language constant
#define GAME_LANGUAGE 1

#endif // VERSIONS_H