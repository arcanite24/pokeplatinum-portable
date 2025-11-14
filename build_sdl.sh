#!/usr/bin/env zsh
# Build script for Pokemon Platinum SDL3 port

set -e

echo "==================================="
echo "Pokemon Platinum SDL3 Build Script"
echo "==================================="
echo ""

# Check for CMake
if ! command -v cmake &> /dev/null; then
    echo "❌ CMake not found!"
    echo ""
    echo "Please install CMake first:"
    echo "  macOS:  brew install cmake"
    echo "  Linux:  sudo apt install cmake"
    echo ""
    exit 1
fi

CMAKE_VERSION=$(cmake --version | head -n1 | cut -d' ' -f3)
echo "✅ Found CMake version $CMAKE_VERSION"

# Check for SDL3
if ! pkg-config --exists sdl3 2>/dev/null; then
    echo "❌ SDL3 not found!"
    echo ""
    echo "Please install SDL3 first:"
    echo "  macOS:  brew install sdl3"
    echo "  Linux:  Build from source (see src/platform/README.md)"
    echo ""
    exit 1
fi

SDL3_VERSION=$(pkg-config --modversion sdl3)
echo "✅ Found SDL3 version $SDL3_VERSION"
echo ""

# Create build directory
BUILD_DIR="build-sdl"
echo "Creating build directory: $BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with CMake
echo ""
echo "Configuring with CMake..."
cmake .. \
    -DBUILD_SDL_VERSION=ON \
    -DBUILD_DS_VERSION=OFF \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Build
echo ""
echo "Building..."
cmake --build . -j$(sysctl -n hw.ncpu)

# Success message
echo ""
echo "==================================="
echo "✅ Build successful!"
echo "==================================="
echo ""
echo "Run the test application:"
echo "  cd $BUILD_DIR"
echo "  ./pokeplatinum_sdl"
echo ""
echo "Or from repository root:"
echo "  ./$BUILD_DIR/pokeplatinum_sdl"
echo ""
