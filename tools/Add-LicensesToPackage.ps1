# Copyright (c) 2026 DeadOnKeyboard
# SPDX-License-Identifier: MIT

[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string] $ArchivePath
)

$ErrorActionPreference = 'Stop'
$projectRoot = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..')).Path
$resolvedArchive = (Resolve-Path -LiteralPath $ArchivePath).Path
$temporaryArchive = "$resolvedArchive.codex.tmp"
$docsPrefix = 'Docs/Go to bed'

$documents = [ordered]@{
    'LICENSE-Go-to-bed-MIT.txt' = (Join-Path $projectRoot 'LICENSE')
    'NOTICE.txt' = (Join-Path $projectRoot 'NOTICE.md')
    'THIRD_PARTY_NOTICES.txt' = (Join-Path $projectRoot 'THIRD_PARTY_NOTICES.md')
}

Get-ChildItem -LiteralPath (Join-Path $projectRoot 'licenses') -File |
    Sort-Object Name |
    ForEach-Object { $documents["licenses/$($_.Name)"] = $_.FullName }

foreach ($source in $documents.Values) {
    if (-not (Test-Path -LiteralPath $source -PathType Leaf)) {
        throw "Required license document missing: $source"
    }
}

Add-Type -AssemblyName System.IO.Compression.FileSystem
Copy-Item -LiteralPath $resolvedArchive -Destination $temporaryArchive -Force

try {
    $archive = [System.IO.Compression.ZipFile]::Open(
        $temporaryArchive,
        [System.IO.Compression.ZipArchiveMode]::Update)
    try {
        foreach ($item in $documents.GetEnumerator()) {
            $entryName = "$docsPrefix/$($item.Key)"
            $existing = $archive.GetEntry($entryName)
            if ($existing) {
                $existing.Delete()
            }
            [System.IO.Compression.ZipFileExtensions]::CreateEntryFromFile(
                $archive,
                $item.Value,
                $entryName,
                [System.IO.Compression.CompressionLevel]::Optimal) | Out-Null
        }
    } finally {
        $archive.Dispose()
    }

    $validationArchive = [System.IO.Compression.ZipFile]::OpenRead($temporaryArchive)
    try {
        foreach ($item in $documents.Keys) {
            if (-not $validationArchive.GetEntry("$docsPrefix/$item")) {
                throw "Archive validation failed for: $item"
            }
        }
        if (-not $validationArchive.Entries.Where({ $_.FullName -ieq 'skse/plugins/gotobed.dll' }, 'First')) {
            throw 'Archive validation failed: gotobed.dll is missing.'
        }
    } finally {
        $validationArchive.Dispose()
    }

    Move-Item -LiteralPath $temporaryArchive -Destination $resolvedArchive -Force
} finally {
    if (Test-Path -LiteralPath $temporaryArchive) {
        Remove-Item -LiteralPath $temporaryArchive -Force
    }
}

Write-Output $resolvedArchive
