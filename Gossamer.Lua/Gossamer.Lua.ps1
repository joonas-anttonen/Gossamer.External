# Enable error handling
$ErrorActionPreference = "Stop"

function Exit-OnFailed {
    param (
        [string]$ErrorMessage
    )

    if ($LASTEXITCODE -ne 0) {
        Write-Host $ErrorMessage -ForegroundColor Red
        exit 1
    }
}

Write-Host "Building Gossamer.Lua ..." -ForegroundColor Green

& cmake -S . `
    -B build `
    -A x64 `
    -DCMAKE_BUILD_TYPE=Release
Exit-OnFailed "Gossamer.Lua configuration failed"

& cmake --build build --config Release --verbose
Exit-OnFailed "Gossamer.Lua build failed"

& cmake --install build --config Release --prefix build/install --verbose
Exit-OnFailed "Gossamer.Lua install failed"

$DstDir = "../bin"
$SrcDir = "build/install"
New-Item -ItemType Directory -Path $DstDir -Force | Out-Null
$DstDir = Resolve-Path -Path $DstDir
$SrcDir = Resolve-Path -Path "build/install"

Write-Host "Copied to $DstDir" -ForegroundColor Green

Get-ChildItem -Path $SrcDir -Include "*.dll", "*.so" -Recurse | ForEach-Object {
    Copy-Item -Path $_.FullName -Destination $DstDir -Force
    Write-Host "$($_.Name)" -ForegroundColor Green
}

Remove-Item -Path ./build -Recurse -Force