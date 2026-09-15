param(
    [switch]$Tests,
    [string]$Compiler = 'C:\g++\g++ 14.2\mingw64\bin\g++.exe'
)
$ErrorActionPreference = 'Stop'
Push-Location $PSScriptRoot
try {
    New-Item -ItemType Directory -Force bin | Out-Null
    $common = @('-std=c++17', '-Wall', '-Wextra', '-g', '-I', 'extern/SFML/include')
    $graphics = @('-L', 'extern/SFML/lib', '-lsfml-graphics-d', '-lsfml-window-d', '-lsfml-system-d')
    $audio = @('-L', 'extern/SFML/lib', '-lsfml-audio-d', '-lsfml-system-d')
    & $Compiler @common src/main.cpp @graphics @audio -o bin/pkg.exe
    if ($LASTEXITCODE -ne 0) { throw 'Game build failed' }
    foreach ($library in @('graphics', 'window', 'system', 'audio')) {
        Copy-Item -LiteralPath "extern/SFML/bin/sfml-$library-d-3.dll" -Destination bin
    }
    if ($Tests) {
        New-Item -ItemType Directory -Force tests/visual | Out-Null
        foreach ($name in @('maze', 'game', 'render', 'audio')) {
            $libraries = @()
            if ($name -eq 'render') { $libraries = $graphics }
            if ($name -eq 'audio') { $libraries = $audio }
            & $Compiler @common "tests/${name}_tests.cpp" @libraries -o "bin/${name}_tests.exe"
            if ($LASTEXITCODE -ne 0) { throw "$name test build failed" }
            & "./bin/${name}_tests.exe"
            if ($LASTEXITCODE -ne 0) { throw "$name tests failed" }
        }
    }
    Write-Output 'Pixel Kombat build completed.'
} finally {
    Pop-Location
}
