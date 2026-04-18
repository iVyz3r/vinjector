[Console]::OutputEncoding = [System.Text.Encoding]::UTF8

$ProjectName = "UWP_Injector"
$BuildDir    = "obj"
$BinDir      = "dist"
$Output      = Join-Path $BinDir "UWP_Injector.exe"

# Lista de fuentes
$Sources = "Main.cpp", 
           "ProcessCheck/ProcessCheck.cpp", 
           "Inject/Inject.cpp",
           "ImGui/imgui.cpp",
           "ImGui/imgui_draw.cpp",
           "ImGui/imgui_widgets.cpp",
           "ImGui/imgui_tables.cpp",
           "ImGui/backend/imgui_impl_dx9.cpp",
           "ImGui/backend/imgui_impl_win32.cpp"

# --- CAMBIO AQUÍ ---
# Se agregan -static-libgcc -static-libstdc++ y -static para evitar dependencias de DLLs externas de MinGW
$Flags = "-O3", "-s", "-m64", "-march=x86-64", "-mwindows", "-std=c++17", "-static-libgcc", "-static-libstdc++", "-static"

$Includes = "-I.", "-I./ImGui", "-I./ImGui/backend"
$Libs  = "-luser32", "-lcomdlg32", "-ladvapi32", "-lole32", "-lcomctl32", "-lgdi32", "-ld3d9", "-ldwmapi"

function Write-Log([string]$Msg, [string]$Type = "Info") {
    $Colors = @{ "Info" = "Cyan"; "Success" = "Green"; "Warn" = "Yellow"; "Error" = "Red"; "Link" = "Magenta" }
    $Color = if ($Colors.ContainsKey($Type)) { $Colors[$Type] } else { "White" }
    Write-Host ("[+] " + $Msg) -ForegroundColor $Color
}

Clear-Host
Write-Log "Starting compilation of $ProjectName (Static Build)..." "Info"

# Crear directorios base
if (!(Test-Path $BinDir)) { New-Item -ItemType Directory -Path $BinDir | Out-Null }
if (!(Test-Path $BuildDir)) { New-Item -ItemType Directory -Path $BuildDir | Out-Null }

$ObjectFiles = @()

# 1. Manejo del Manifiesto / Recursos
if (Test-Path "manifest/resources.rc") {
    Write-Log "Compiling resources..." "Warn"
    & windres manifest/resources.rc -O coff -o "$BuildDir/resources.res"
    $ObjectFiles += "$BuildDir/resources.res"
}

# 2. Compilación de Fuentes
foreach ($File in $Sources) {
    if (!(Test-Path $File)) {
        Write-Log "File not found: $File" "Error"
        continue
    }

    $CleanPath = $File -replace "^\.\\", ""
    $ObjFile = Join-Path $BuildDir ($CleanPath -replace '\.cpp$', '.o')
    
    $TargetSubDir = Split-Path $ObjFile
    if (!(Test-Path $TargetSubDir)) {
        New-Item -ItemType Directory -Path $TargetSubDir | Out-Null
    }

    $ObjectFiles += $ObjFile
    Write-Log "Compiling: $File" "Warn"
    
    & g++ -c $File -o $ObjFile $Flags $Includes
    if ($LASTEXITCODE -ne 0) {
        Write-Log "Fatal error in $File" "Error"
        exit 1
    }
}

# 3. Linker
Write-Log "Vinculando ejecutable final (Static): $Output" "Link"
& g++ -o $Output $ObjectFiles $Flags $Libs

if ($LASTEXITCODE -eq 0) {
    $Size = [Math]::Round((Get-Item $Output).Length / 1KB, 2)
    Write-Log "Compilation successful!" "Success"
    Write-Host "----------------------------------"
    Write-Host "Binary: $Output"
    Write-Host "Size:  $Size KB"
    Write-Host "Note: All GCC/C++ dependencies are now embedded."
    Write-Host "----------------------------------"
} else {
    Write-Log "Error generating executable." "Error"
}
