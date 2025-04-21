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

Set-Location ./freetype

& cmake -S . `
    -B build `
    -A x64 `
    -DCMAKE_BUILD_TYPE=Release `
    -DFT_DISABLE_ZLIB=ON  `
    -DFT_DISABLE_BZIP2=ON  `
    -DFT_DISABLE_PNG=ON  `
    -DFT_DISABLE_BROTLI=ON 
Exit-OnFailed "freetype configuration failed"

& cmake --build build --config Release --verbose
Exit-OnFailed "freetype build failed"

& cmake --install build --config Release --prefix build/install --verbose
Exit-OnFailed "freetype install failed"

Write-Host "Gossamer.FreeType" -ForegroundColor Green
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

$SrcDir = Resolve-Path -Path "build/install"
$DstDir = Resolve-Path -Path "../bin"
New-Item -ItemType Directory -Path $DstDir -Force | Out-Null

Write-Host "Copied to $DstDir" -ForegroundColor Green

Get-ChildItem -Path $SrcDir -Filter "*.dll" -Recurse | ForEach-Object {
    Copy-Item -Path $_.FullName -Destination $DstDir -Force
    Write-Host "$($_.Name)" -ForegroundColor Green
}

Remove-Item -Path ./freetype/build -Recurse -Force
Remove-Item -Path ./build -Recurse -Force