param(
    [string]$Path = "D:\Epic Games\UE_5.8\Engine\Source\Runtime\MassEntity\Internal\MassArchetypeData.h"
)

$bytes = [System.IO.File]::ReadAllBytes($Path)
$enc = if ($bytes.Length -ge 2 -and $bytes[0] -eq 0xFF -and $bytes[1] -eq 0xFE) {
    [System.Text.Encoding]::Unicode
} else {
    [System.Text.Encoding]::UTF8
}

$text = $enc.GetString($bytes)
$pattern = '(?s)\r?\n// @@PATCH_BEGIN\(mass-entity-manager-extended::add-chunk-shared-fragment-values\).*?// @@PATCH_END\(mass-entity-manager-extended::add-chunk-shared-fragment-values\)\r?\n'
$newText = [regex]::Replace($text, $pattern, "`r`n")

if ($text -eq $newText) {
    Write-Host "No orphan patch found in $Path"
    exit 0
}

[System.IO.File]::WriteAllText($Path, $newText, $enc)
Write-Host "Removed orphan patch block from $Path"
