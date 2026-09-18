param(
    [string]$BuildType = "Release"
)

$ErrorActionPreference = "Stop"

$projectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$msysRoot = "C:\msys64"
$cmakeExe = Join-Path $msysRoot "ucrt64\bin\cmake.exe"
$windeployExe = Join-Path $msysRoot "ucrt64\bin\windeployqt.exe"
$ninjaExe = Join-Path $msysRoot "ucrt64\bin\ninja.exe"

if (-not (Test-Path $cmakeExe)) {
    throw "MSYS2 UCRT64 não encontrado em $msysRoot. Instale o MSYS2 e o pacote 'qt6-base' + 'cmake' + 'libzip' no ambiente ucrt64."
}

if (-not (Test-Path $ninjaExe)) {
    throw "Ninja não encontrado em $ninjaExe. Instale o pacote 'ninja' no MSYS2 UCRT64."
}

if (-not (Test-Path $windeployExe)) {
    throw "windeployqt não foi encontrado em $windeployExe. Instale o Qt no MSYS2 UCRT64 antes de empacotar o executável."
}

$env:PATH = "$msysRoot\ucrt64\bin;$env:PATH"
Set-Location $projectRoot

if (Test-Path "build") {
    Write-Host "Limpando build anterior..."
    Remove-Item "build" -Recurse -Force
}

Write-Host "Configurando projeto em $projectRoot"
& $cmakeExe -S . -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Release

Write-Host "Compilando projeto"
& $cmakeExe --build build

$exePath = Join-Path $projectRoot "build\DescoDeco.exe"
if (Test-Path $exePath) {
    Write-Host "Empacotando dependências do Qt"
    & $windeployExe $exePath --release
} else {
    throw "Executável não encontrado em $exePath após a compilação."
}

Write-Host "Build concluído. O executável e as DLLs estão em: $projectRoot\build"
