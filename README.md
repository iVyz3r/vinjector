# vinjector
![License](https://img.shields.io/github/license/iVyz3r/vinjector)
![Stars](https://img.shields.io/github/stars/iVyz3r/vinjector)
![Topics](https://img.shields.io/github/topics/iVyz3r/vinjector?label=topics&logo=github)

A powerful and flexible process injector with a modern ImGui-based user interface. This project provides a robust solution for process injection with DirectX 9 integration.

## Features

- **Process Injection**: Inject code and libraries into running processes
- **Modern UI**: Built with ImGui for an intuitive graphical interface
- **DirectX 9 Support**: Full compatibility with DirectX 9 rendering pipeline
- **Process Monitoring**: Monitor and check process status in real-time
- **Lightweight & Efficient**: Minimal resource footprint with optimal performance

## Quick Start

```bash
# Clone the repository
git clone https://github.com/iVyz3r/vinjector.git
cd vinjector

# Build with CMake
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

# Run the application
./build/bin/vinjector.exe
```

For detailed build instructions, see [Building](#building) section below.

## Requirements

- **Operating System**: Windows 7 or later
- **Build System**: CMake 3.20+ (Recommended) or Visual Studio 2017+
- **Development Environment**: Visual Studio 2017, MinGW64, or Clang-CL
- **Dependencies**:
  - DirectX 9 SDK (Windows 10 SDK includes this)
  - Windows SDK
  - ImGui framework (included in repository)

## Project Structure

```
├── CMakeLists.txt           # CMake build configuration (recommended)
├── Main.cpp                 # Application entry point
├── app.manifest             # Application manifest configuration
├── resources.rc             # Resource file for application metadata
├── build.ps1                # Alternative PowerShell build script
├── ImGui/                   # ImGui framework and backends
│   ├── imgui.*              # Core ImGui headers and implementations
│   ├── imconfig.h           # ImGui configuration
│   └── backend/             # Platform-specific backends
│       ├── imgui_impl_dx9.*      # DirectX 9 implementation
│       └── imgui_impl_win32.*    # Windows implementation
├── Inject/                  # Injection mechanism module
│   ├── Inject.cpp
│   └── Inject.hpp
└── ProcessCheck/            # Process monitoring module
    ├── ProcessCheck.cpp
    └── ProcessCheck.hpp
```

## Building

### Using CMake (Recommended)

The project uses CMake for cross-platform compilation support.

#### Prerequisites

- **CMake 3.20+**
- **Visual Studio 2017+** (or MinGW64 / Clang-CL)
- **DirectX 9 SDK** (or Windows 10 SDK which includes DirectX 9)
- **Windows 10 or later**

#### Build Steps

1. **Clone the repository** and navigate to the project directory:
   ```bash
   git clone https://github.com/iVyz3r/vinjector.git
   cd vinjector
   ```

2. **Configure the build**:
   ```bash
   cmake -B build -DCMAKE_BUILD_TYPE=Release
   ```

3. **Build the executable**:
   ```bash
   cmake --build build --config Release
   ```

4. The compiled executable will be in `build/bin/vinjector.exe`

#### CMake Build Options

```bash
# Enable/disable DirectX 9 support (enabled by default)
cmake -B build -DENABLE_DX9=ON

# Use static runtime linking
cmake -B build -DUSE_STATIC_RUNTIME=ON

# Strip debug symbols from binary (reduces file size)
cmake -B build -DSTRIP_BINARY=ON
```

### Alternative: PowerShell Build Script

For quick builds without CMake setup, use the PowerShell script:

```powershell
.\build.ps1
```

### Manual Build with Visual Studio

You can also build using Visual Studio directly:
1. Open Visual Studio 2017 or later
2. Create a new CMake project from the vinjector directory
3. Configure the build and run

## Usage

1. Run the application executable
2. Use the ImGui interface to select target processes
3. Configure injection parameters
4. Initiate the injection process
5. Monitor status through the real-time interface

## Architecture

### Main Components

- **Inject Module**: Handles the core injection logic and payload delivery
- **ProcessCheck Module**: Monitors and validates process status
- **ImGui Frontend**: Provides user interface and event handling
- **DirectX 9 Integration**: Enables rendering through DirectX 9

## License

This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.

### MIT License Summary

You are free to:
- Use the software for any purpose
- Copy, modify, and distribute the software
- Include the software in proprietary applications

The only requirement is that the above copyright notice and license appear in all copies or substantial portions of the software.

## Security

For security concerns and vulnerability reporting, please refer to [SECURITY.md](SECURITY.md).

## Contributing

We welcome contributions! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines on how to contribute to this project.

## Author

- **GitHub**: [iVyz3r](https://github.com/iVyz3r)
- **Discord**: notvyzer

## Support & Contact

For questions, suggestions, or issues:
- 📝 Report bugs on [GitHub Issues](https://github.com/iVyz3r/vinjector/issues)
- 💬 Reach out on Discord: **notvyzer**
- 🔐 Report security issues on [GitHub Security Advisories](https://github.com/iVyz3r/vinjector/security/advisories)

## Repository

- **GitHub**: https://github.com/iVyz3r/vinjector

## Disclaimer

This tool is provided for educational and authorized use only. Unauthorized process injection may violate laws and terms of service. Users are responsible for ensuring their use complies with applicable laws and regulations.

---

*Last updated: April 2026*
