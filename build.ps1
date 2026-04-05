[Console]::OutputEncoding = [System.Text.Encoding]::UTF8

$ProjectName = "UWP_Injector"
$BuildDir    = "obj"
$BinDir      = "dist"
$Output      = Join-Path $BinDir "UWP_Injector.exe"

# Lista de fuentes (puedes usar rutas con subcarpetas)
$Sources = "Main.cpp", 
           "ProcessCheck/ProcessCheck.cpp", 
           "Inject/Inject.cpp",
           "ImGui/imgui.cpp",
           "ImGui/imgui_draw.cpp",
           "ImGui/imgui_widgets.cpp",
           "ImGui/imgui_tables.cpp",
           "ImGui/backend/imgui_impl_dx9.cpp",
           "ImGui/backend/imgui_impl_win32.cpp"

# Configuración de compilación
$Flags = "-O3", "-s", "-m64", "-march=x86-64", "-mwindows", "-std=c++17"
$Includes = "-I.", "-I./ImGui", "-I./ImGui/backend"
$Libs  = "-luser32", "-lcomdlg32", "-ladvapi32", "-lole32", "-lcomctl32", "-lgdi32", "-ld3d9", "-ldwmapi"

function Write-Log([string]$Msg, [string]$Type = "Info") {
    $Colors = @{ "Info" = "Cyan"; "Success" = "Green"; "Warn" = "Yellow"; "Error" = "Red"; "Link" = "Magenta" }
    $Color = if ($Colors.ContainsKey($Type)) { $Colors[$Type] } else { "White" }
    Write-Host ("[+] " + $Msg) -ForegroundColor $Color
}

Clear-Host
Write-Log "Starting compilation of $ProjectName..." "Info"

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

# 2. Compilación de Fuentes con creación de carpetas espejo
foreach ($File in $Sources) {
    if (!(Test-Path $File)) {
        Write-Log "File not found: $File" "Error" "no such file"
        continue
    }

    # Limpiar ruta: quitar .\ inicial si existe
    $CleanPath = $File -replace "^\.\\", ""
    
    # Definir ruta del objeto (espejo de la estructura de fuentes)
    $ObjFile = Join-Path $BuildDir ($CleanPath -replace '\.cpp$', '.o')
    
    # CRITICAL: Crear la subcarpeta dentro de 'obj' si no existe
    $TargetSubDir = Split-Path $ObjFile
    if (!(Test-Path $TargetSubDir)) {
        New-Item -ItemType Directory -Path $TargetSubDir | Out-Null
    }

    $ObjectFiles += $ObjFile
    Write-Log "Compiling: $File" "Warn"
    
    # Ejecutar G++
    & g++ -c $File -o $ObjFile $Flags $Includes
    if ($LASTEXITCODE -ne 0) {
        Write-Log "Fatal error in $File" "Error"
        exit 1
    }
}

# 3. Linker (Vinculación)
Write-Log "Vinculando ejecutable final: $Output" "Link"
& g++ -o $Output $ObjectFiles $Flags $Libs

if ($LASTEXITCODE -eq 0) {
    $Size = [Math]::Round((Get-Item $Output).Length / 1KB, 2)
    Write-Log "Compilation successful!" "Success"
    Write-Host "----------------------------------"
    Write-Host "Binary: $Output"
    Write-Host "Size:  $Size KB"
    Write-Host "Privileges: Administrator (via Manifest)"
    Write-Host "----------------------------------"
} else {
    Write-Log "Error generating executable." "Error"
}