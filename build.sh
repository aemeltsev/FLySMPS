#!/bin/bash
# ███████╗██╗  ██╗   ██╗███████╗███╗   ███╗██████╗ ███████╗
# ██╔════╝██║  ╚██╗ ██╔╝██╔════╝████╗ ████║██╔══██╗██╔════╝
# █████╗  ██║   ╚████╔╝ ███████╗██╔████╔██║██████╔╝███████╗
# ██╔══╝  ██║    ╚██╔╝  ╚════██║██║╚██╔╝██║██╔═══╝ ╚════██║
# ██║     ███████╗██║   ███████║██║ ╚═╝ ██║██║     ███████║
# ╚═╝     ╚══════╝╚═╝   ╚══════╝╚═╝     ╚═╝╚═╝     ╚══════╝
# Qt Project Build Script (qmake/CMake)
# Usage: ./build.sh [debug|release] [clean]

set -e  # Exit on error

# --- Config ---
QT_PATH="Qt5.12.12/5.12.12/gcc_64"         # Path to installed Qt (replace by your path)
QT_DIR="$HOME/$QT_PATH"                    # Path to Qt directory
PROJECT_DIR="$(pwd)"                       # Current directory as default
BUILD_TYPE="Debug"                         # Default build type
CLEAN_BUILD=false                          # Clean build flag
USE_CMAKE=false                            # Use CMake instead of qmake
TARGET="FLySMPS"                           # The name of target project

# --- Colors for output ---
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# --- Function to print colored output ---
info() { echo -e "${BLUE}[INFO]${NC} $1"; }
success() { echo -e "${GREEN}[SUCCESS]${NC} $1"; }
warning() { echo -e "${YELLOW}[WARNING]${NC} $1"; }
error() { echo -e "${RED}[ERROR]${NC} $1"; exit 1; }

# --- Parse Arguments ---
for arg in "$@"; do
    case $arg in
        debug|Debug)
            BUILD_TYPE="Debug" 
            ;;
        release|Release)
            BUILD_TYPE="Release"
            ;;
        clean)
            CLEAN_BUILD=true
            ;;
        cmake)
            USE_CMAKE=true
            ;;
        *) 
            echo "Usage: $0 [debug|release] [clean] [cmake]"
            echo "Examples:"
            echo " $0 debug                 # Debug build (incremental)"
            echo " $0 release               # Release build (incremental)"
            echo " $0 debug clean           # Debug build (clean)"
            echo " $0 clean debug           # Debug build (clean)"
            echo " $0 debug clean cmake     # Build with cmake build system"
            exit 1
            ;;
    esac
done

# --- Check Project Directory ---
if [ ! -d "$PROJECT_DIR" ]; then
    error "Project directory not found: $PROJECT_DIR"
fi

# --- Find .pro or CMakeLists.txt ---
PROJECT_FILE=""
if [ -f "$PROJECT_DIR/CMakeLists.txt" ]; then
    PROJECT_FILE="CMakeLists.txt"
    USE_CMAKE=true
elif [ -f "$PROJECT_DIR/$TARGET.pro" ]; then
    PROJECT_FILE="$TARGET.pro"
else
    error "No project file found! Expected $TARGET.pro or CMakeLists.txt"
fi

# --- Current information output
info "Build Type: $BUILD_TYPE"

if [ "$CLEAN_BUILD" = true ]; then
    info "Clean Build: Yes"
else
    info "Clean Build: No"
fi
if [ "$USE_CMAKE" = true ]; then
    info "Build System: Cmake"
else
    info "Build System: Qmake"
fi

info "Project File: $PROJECT_FILE"
info "Target Name: $TARGET"

# --- Install Dependencies ---
install_dependencies() {
    info "Installing dependencies..."
    
    sudo apt update
    sudo apt install -y \
        build-essential \
        qt5-default \
        qtbase5-dev \
        qtdeclarative5-dev \
        qml-module-qtquick2 \
        qml-module-qtquick-controls2 \
        libqt5charts5-dev \
        libqt5serialport5-dev \
        qttools5-dev-tools \
        qtcreator \
        cmake \
        ninja-build
    success "Dependencies installed"
}

# --- Check Qt5 installation ---
# --- Setup Environment ---
setup_environment() {
    info "Setting up environment..."
    if [ ! -d "$QT_DIR" ]; then
        error "Qt directory not found: $QT_DIR"
    fi
    export PATH="$QT_DIR/bin:$PATH"
    export LD_LIBRARY_PATH="$QT_DIR/lib:$LD_LIBRARY_PATH"
    source "$QT_DIR/bin/qtenv.sh" 2>/dev/null || warning "qtenv.sh not found"
    success "Environment set up"
}

# --- Run script to create/update databases
make_database() {
    local SCRIPT_PATH="$1" # Путь к Python-скрипту для работы с базой данных
    local DB_CONFIG="${@:2}" # Конфигурация базы данных (опционально)
    
    info "Running database maker script: $SCRIPT_PATH"
    
    # Проверка существования Python-скрипта
    if [ ! -f "$SCRIPT_PATH" ]; then
        error "Database maker script not found: $SCRIPT_PATH"
    fi

    # Проверка наличия Python 3
    if ! command -v python3 &> /dev/null; then
        error "Python 3 is not installed. Install it with: sudo apt install python3"
    fi

    # Проверка наличия необходимых Python-пакетов для работы с базой данных
    if ! python3 -c "import sqlite3" &> /dev/null; then
        error "Python SQLite3 module not found. Install it with: sudo apt install python3-sqlite3"
    fi

    # Запуск Python-скрипта для работы с базой данных
    if python3 "$SCRIPT_PATH" "$DB_CONFIG"; then
        success "Database maker script executed successfully"
    else
        error "Failed to execute database maker script: $SCRIPT_PATH"
    fi
}

# --- Set up build environment ---
configure_build() {
    info "Configuring build..."

    # Create build directories
    BUILD_DIR="$PROJECT_DIR/build_${BUILD_TYPE,,}"
    if [ "$CLEAN_BUILD" = true ]; then
        warning "Clean build requested - removing $BUILD_DIR"
        rm -rf "$BUILD_DIR"
    fi
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR" || error "Failed to enter build directory"
    success "Build directory ready: $BUILD_DIR"
    
    if [ "$USE_CMAKE" = true ]; then
        info "Configuring with CMake..."
        cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
              -DCMAKE_PREFIX_PATH="$QT_DIR" \
              -G Ninja \
              "$PROJECT_DIR" || error "CMake configuration failed"
    else
        info "Configuring with qmake..."
        qmake "$PROJECT_DIR/$PROJECT_FILE" \
              CONFIG+=$BUILD_TYPE \
              PREFIX="$QT_DIR" || error "qmake configuration failed"
    fi
    success "Configuration completed"
}

# --- Build Project ---
build_project() {
    info "Building FLySMPS..."
    CORES=$(nproc)
    info "Building with $CORES cores..."
    if [ "$USE_CMAKE" = true ]; then
        cmake --build . --config $BUILD_TYPE --parallel $(nproc) || error "CMake build failed"
    else
        make -j$(nproc) || error "Make build failed"
    fi
    success "Build completed successfully"
}

# --- Package Project ---
package_project() {
    info "Packaging project for $BUILD_TYPE..."

    # Директория для упаковки
    local PACKAGE_DIR="$PROJECT_DIR/package_${BUILD_TYPE,,}"
    mkdir -p "$PACKAGE_DIR"

    # 1. Копирование исполняемого файла
    if [ -f "$BUILD_DIR/$TARGET" ]; then
        cp "$BUILD_DIR/$TARGET" "$PACKAGE_DIR/"
        success "Executable copied to $PACKAGE_DIR/"
    else
        error "Executable not found: $BUILD_DIR/$TARGET"
        return 1
    fi

    # 2. Копирование зависимостей с помощью linuxdeployqt
    if command -v linuxdeployqt &> /dev/null; then
        info "Copying dependencies with linuxdeployqt..."
        linuxdeployqt "$PACKAGE_DIR/$TARGET" -appimage \
            -qmldir="$PROJECT_DIR/qml" \
            -executable="$PACKAGE_DIR/$TARGET" \
            -unsupported-allow-new-glibc
        success "Dependencies copied."
    else
        error "linuxdeployqt not found. Install it with:"
        echo "  wget -c https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage"
        echo "  chmod +x linuxdeployqt-continuous-x86_64.AppImage"
        echo "  sudo mv linuxdeployqt-continuous-x86_64.AppImage /usr/local/bin/linuxdeployqt"
    fi

    # 3. Копирование дополнительных файлов (конфиги, ресурсы, иконки)
    if [ -d "$PROJECT_DIR/resources" ]; then
        info "Copying resources..."
        cp -r "$PROJECT_DIR/resources" "$PACKAGE_DIR/"
        success "Resources copied."
    fi

    # 4. Копирование библиотек Qt (если linuxdeployqt не справился)
    local QT_LIB_DIR="$QT_DIR/lib"
    if [ -d "$QT_LIB_DIR" ]; then
        info "Copying Qt libraries..."
        mkdir -p "$PACKAGE_DIR/lib"
        ldd "$PACKAGE_DIR/$TARGET" | grep "qt" | awk '{print $3}' | xargs -I '{}' cp -v '{}' "$PACKAGE_DIR/lib/"
        success "Qt libraries copied."
    fi

    # 5. Создание AppImage (опционально)
    if command -v linuxdeployqt &> /dev/null; then
        info "Creating AppImage..."
        linuxdeployqt "$PACKAGE_DIR/$TARGET" -appimage
        success "AppImage created: $PACKAGE_DIR/$TARGET.AppImage"
    fi

    # 6. Архивирование (опционально)
    info "Creating archive..."
    tar -czvf "$PROJECT_DIR/${TARGET}_${BUILD_TYPE,,}.tar.gz" -C "$PACKAGE_DIR" .
    success "Archive created: $PROJECT_DIR/${TARGET}_${BUILD_TYPE,,}.tar.gz"
}

# --- Вывод информации о сборке ---
show_build_summary() {
    info "Build Summary"
    echo "=============="
    echo "Executable: $(pwd)/$PROJECT_NAME"
    echo "Build type: $BUILD_TYPE"
    echo "Qt5 version: $(qmake -version | grep Qt | cut -d' ' -f4)"
    echo ""
    if [ -f "$PROJECT_NAME" ]; then
        success "$PROJECT_NAME executable created successfully!"
        echo ""
        info "To test the application:"
        echo "cd $(pwd)"
        echo "./$PROJECT_NAME"
    else
        error "$PROJECT_NAME executable not found!"
    fi
}

# --- Main ---
main() {
    #install_dependencies
    setup_environment
    #configure_build
    #build_project
    #package_project
    #show_build_summary
    success "Build script completed successfully!"
    info "Executable: $BUILD_DIR/$TARGET"
}

main
