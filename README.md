# UWP DLL Injector

<p align="center">
  <img src="https://img.shields.io/github/license/iVyz3r/aegledll" alt="License" />
  <img src="https://img.shields.io/github/stars/iVyz3r/aegledll" alt="Stars" />
  <img src="https://img.shields.io/github/issues/iVyz3r/aegledll" alt="Issues" />
  <img src="https://img.shields.io/github/last-commit/iVyz3r/aegledll" alt="Last Commit" />
  <img src="https://img.shields.io/github/topics/iVyz3r/aegledll?label=topics" alt="Topics" />
</p>

A powerful DLL injector specifically designed for Universal Windows Platform (UWP) applications, featuring a modern ImGui-based user interface. This tool allows safe injection and management of DLLs into UWP processes with advanced monitoring capabilities.

## Features

- **UWP DLL Injection**: Specialized injection for Universal Windows Platform applications
- **Process Selection**: Interactive window to browse and select running processes
- **Module Management**: View and manage loaded DLLs in selected processes
- **Safe DLL Unloading**: Securely unload DLLs without crashing the target process
- **Auto-Detection**: Automatically detects terminated processes and new processes
- **Real-Time Updates**: Process list updates automatically when new processes start
- **Modern UI**: Built with ImGui for an intuitive graphical interface
- **DirectX 9 Support**: Full compatibility with DirectX 9 rendering pipeline
- **Process Monitoring**: Monitor and check process status in real-time
- **Lightweight & Efficient**: Minimal resource footprint with optimal performance

## Quick Start

```bash
# Clone the repository
git clone https://github.com/iVyz3r/vinjector.git
cd vinjector

# Build with PowerShell script
.\build.ps1

# Run the application
.\dist\UWP_Injector.exe
```

For detailed build instructions, see [Building](#building) section below.

## Requirements

- **Operating System**: Windows 10 or later
- **Compiler**: MinGW-w64 GCC (included in MSYS2 or standalone)
- **Dependencies**:
  - DirectX 9 SDK (included in Windows SDK)
  - Windows SDK
  - ImGui framework (included in repository)

## Project Structure

```
├── build.ps1                # PowerShell build script
├── Main.cpp                 # Application entry point with ImGui interface
├── app.manifest             # Application manifest for UWP permissions
├── resources.rc             # Resource file for application metadata
├── ImGui/                   # ImGui framework and backends
│   ├── imgui.*              # Core ImGui headers and implementations
│   ├── imconfig.h           # ImGui configuration
│   └── backend/             # Platform-specific backends
│       ├── imgui_impl_dx9.*      # DirectX 9 implementation
│       └── imgui_impl_win32.*    # Windows implementation
├── Inject/                  # UWP injection mechanism module
│   ├── Inject.cpp
│   └── Inject.hpp
├── ProcessCheck/            # Process monitoring and module management
│   ├── ProcessCheck.cpp
│   └── ProcessCheck.hpp
├── manifest/                # Application manifest files
├── obj/                     # Build object files
└── dist/                    # Output directory for compiled executable
```

## Building

### Using PowerShell Build Script (Recommended)

The project includes a PowerShell script for easy compilation using MinGW-w64 GCC.

#### Prerequisites

- **MinGW-w64 GCC** (add to PATH or use MSYS2)
- **Windows 10 SDK** (for DirectX 9 headers)
- **PowerShell** (included in Windows)

#### Build Steps

1. **Ensure MinGW-w64 is installed** and `gcc` is in your PATH.

2. **Run the build script**:
   ```powershell
   .\build.ps1
   ```

3. The compiled executable will be in `dist\UWP_Injector.exe`

### Manual Build

You can also build manually using GCC:

```bash
g++ -std=c++17 -o dist/UWP_Injector.exe Main.cpp ProcessCheck/ProcessCheck.cpp Inject/Inject.cpp ImGui/*.cpp ImGui/backend/*.cpp -lgdi32 -ld3d9 -lpsapi -lkernel32 -luser32 -lshell32 -lcomdlg32 -mwindows -static-libgcc -static-libstdc++
```

### Alternative: Visual Studio

1. Open Visual Studio 2017 or later
2. Create a new C++ project
3. Add all source files from the project
4. Configure include directories for ImGui and Windows SDK
5. Build the project

## Usage

1. **Run the application**: Execute `UWP_Injector.exe` (requires administrator privileges)

2. **Select a Process**:
   - Click "Select Process" to open the process selection window
   - Browse the list of running processes
   - Use the search box to filter processes by name
   - Select a target UWP process and click to choose it

3. **Manage Modules** (Optional):
   - Click "Show Modules" to view loaded DLLs in the selected process
   - Right-click on any DLL to unload it safely
   - Use "Open Location" to navigate to the DLL file

4. **Inject DLL**:
   - Click "Browse" to select a DLL file to inject
   - Click "Inject DLL" to perform the injection
   - Monitor the status message for success/failure

5. **Auto-Detection Features**:
   - The injector automatically detects if the selected process terminates
   - The process list updates automatically when new processes start (while selection window is open)

## Architecture

### Main Components

- **Inject Module**: Handles UWP-specific DLL injection with permission management
- **ProcessCheck Module**: Monitors processes, enumerates modules, and performs safe DLL unloading
- **ImGui Frontend**: Provides user interface with process selection, module management, and injection controls
- **DirectX 9 Integration**: Enables rendering through DirectX 9 for the GUI

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
