param(
    [string[]]$Paths = @(
        "D:\Epic Games\UE_5.8\Engine\Source\Runtime\MassEntity\Public\MassProcessingPhaseManager.h",
        "D:\Epic Games\UE_5.8\Engine\Source\Runtime\MassEntity\Private\MassProcessingPhaseManager.cpp"
    ),
    [string[]]$OperationIds = @(
        "mass-processing-manual-step::add-declarations",
        "mass-processing-manual-step::add-implementations",
        "mass-entity-manager-extended::add-chunk-shared-fragment-values"
    )
)

foreach ($Path in $Paths) {
    if (-not (Test-Path $Path)) {
        Write-Host "Skip missing $Path"
        continue
    }

    $bytes = [System.IO.File]::ReadAllBytes($Path)
    $enc = if ($bytes.Length -ge 2 -and $bytes[0] -eq 0xFF -and $bytes[1] -eq 0xFE) {
        [System.Text.Encoding]::Unicode
    } else {
        [System.Text.Encoding]::UTF8
    }

    $text = $enc.GetString($bytes)
    $original = $text

    foreach ($OpId in $OperationIds) {
        $escaped = [regex]::Escape($OpId)
        $pattern = "(?s)\r?\n// @@PATCH_BEGIN\($escaped\).*?// @@PATCH_END\($escaped\)\r?\n"
        $text = [regex]::Replace($text, $pattern, "`r`n")
    }

    if ($text -eq $original) {
        Write-Host "No orphan patches in $Path"
    } else {
        [System.IO.File]::WriteAllText($Path, $text, $enc)
        Write-Host "Cleaned orphan patches in $Path"
    }
}
