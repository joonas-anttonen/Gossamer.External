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

Write-Host "Building FreeType ..." -ForegroundColor Green
Set-Location ./freetype

& cmake -S . `
    -B build `
    -A x64 `
    -DCMAKE_BUILD_TYPE=Release `
    -DFT_DISABLE_ZLIB=ON  `
    -DFT_DISABLE_BZIP2=ON  `
    -DFT_DISABLE_PNG=ON  `
    -DFT_DISABLE_BROTLI=ON 
Exit-OnFailed "FreeType configuration failed"

& cmake --build build --config Release --verbose
Exit-OnFailed "FreeType build failed"

& cmake --install build --config Release --prefix build/install --verbose
Exit-OnFailed "FreeType install failed"

Write-Host "Building Gossamer.FreeType ..." -ForegroundColor Green
Set-Location ../

& cmake -S . `
    -B build `
    -A x64 `
    -DCMAKE_BUILD_TYPE=Release `
    -DFREETYPE_LIBRARY="freetype/build/install/lib" `
    -DFREETYPE_INCLUDE_DIRS="freetype/build/install/include/freetype2"
Exit-OnFailed "Gossamer.FreeType configuration failed"

& cmake --build build --config Release --verbose
Exit-OnFailed "Gossamer.FreeType build failed"

& cmake --install build --config Release --prefix build/install --verbose
Exit-OnFailed "Gossamer.FreeType install failed"

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

Remove-Item -Path ./freetype/build -Recurse -Force
Remove-Item -Path ./build -Recurse -Force