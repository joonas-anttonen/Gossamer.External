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

Set-Location ./libwebp

& cmake -S . `
    -B build `
    -A x64 `
    -DCMAKE_BUILD_TYPE=Release `
    -DWEBP_BUILD_ANIM_UTILS=OFF `
    -DWEBP_BUILD_CWEBP=OFF `
    -DWEBP_BUILD_DWEBP=OFF `
    -DWEBP_BUILD_GIF2WEBP=OFF `
    -DWEBP_BUILD_IMG2WEBP=OFF `
    -DWEBP_BUILD_VWEBP=OFF `
    -DWEBP_BUILD_WEBPINFO=OFF `
    -DWEBP_BUILD_LIBWEBPMUX=OFF `
    -DWEBP_BUILD_WEBPMUX=OFF `
    -DWEBP_BUILD_EXTRAS=OFF `
    -DWEBP_BUILD_WEBP_JS=OFF `
    -DWEBP_BUILD_FUZZTEST=OFF
Exit-OnFailed "libwebp configuration failed"

& cmake --build build --config Release --verbose
Exit-OnFailed "libwebp build failed"

& cmake --install build --config Release --prefix build/install --verbose
Exit-OnFailed "libwebp install failed"

Set-Location ../

& cmake -S . `
    -B build `
    -A x64 `
    -DCMAKE_BUILD_TYPE=Release `
    -DWebP_DIR=libwebp/build/install/share/WebP/cmake
Exit-OnFailed "Gossamer.WebP configuration failed"

& cmake --build build --config Release --verbose
Exit-OnFailed "Gossamer.WebP build failed"

& cmake --install build --config Release --prefix build/install --verbose
Exit-OnFailed "Gossamer.WebP install failed"

$SrcDir = Resolve-Path -Path "build/install"
$DstDir = Resolve-Path -Path "../bin"
New-Item -ItemType Directory -Path $DstDir -Force | Out-Null

Write-Host "Copied to $DstDir" -ForegroundColor Green

Get-ChildItem -Path $SrcDir -Filter "*.dll" -Recurse | ForEach-Object {
    Copy-Item -Path $_.FullName -Destination $DstDir -Force
    Write-Host "$($_.Name)" -ForegroundColor Green
}

Remove-Item -Path ./libwebp/build -Recurse -Force
Remove-Item -Path ./build -Recurse -Force