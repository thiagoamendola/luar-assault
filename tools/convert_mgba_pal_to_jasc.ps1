# This was created to convert raw palettes from mGBA into JASC-PAL format, easier to open in Usenti.

param(
    [Parameter(Mandatory = $true, Position = 0)]
    [string] $InputPath,

    [Parameter(Position = 1)]
    [string] $OutputDir,

    [switch] $NoSlotFiles
)

$ErrorActionPreference = 'Stop'

function Read-AsciiString([byte[]] $Bytes, [int] $Offset, [int] $Length)
{
    return [System.Text.Encoding]::ASCII.GetString($Bytes, $Offset, $Length)
}

function Write-JascPalette($Colors, [string] $Path)
{
    $lines = New-Object 'System.Collections.Generic.List[string]'
    $lines.Add('JASC-PAL')
    $lines.Add('0100')
    $lines.Add([string] $Colors.Count)

    foreach($color in $Colors)
    {
        $lines.Add("$($color.R) $($color.G) $($color.B)")
    }

    $lines | Set-Content $Path -Encoding ASCII
}

if(-not (Test-Path $InputPath))
{
    throw "Input palette file not found: $InputPath"
}

$inputFile = Get-Item $InputPath
$bytes = [System.IO.File]::ReadAllBytes($inputFile.FullName)

if($bytes.Length -lt 24 -or (Read-AsciiString $bytes 0 4) -ne 'RIFF' -or (Read-AsciiString $bytes 8 4) -ne 'PAL ')
{
    throw 'Expected a RIFF PAL file, like the BG/OBJ palette files exported by mGBA.'
}

$dataOffset = -1
$dataSize = 0
$chunkOffset = 12

while($chunkOffset + 8 -le $bytes.Length)
{
    $chunkId = Read-AsciiString $bytes $chunkOffset 4
    $chunkSize = [BitConverter]::ToUInt32($bytes, $chunkOffset + 4)
    $chunkDataOffset = $chunkOffset + 8

    if($chunkId -eq 'data')
    {
        $dataOffset = $chunkDataOffset
        $dataSize = [int] $chunkSize
        break
    }

    $chunkOffset = $chunkDataOffset + [int] $chunkSize
    if(($chunkSize % 2) -eq 1)
    {
        ++$chunkOffset
    }
}

if($dataOffset -lt 0)
{
    throw 'RIFF PAL data chunk not found.'
}

if($dataSize -lt 4)
{
    throw 'RIFF PAL data chunk is too small.'
}

$version = [BitConverter]::ToUInt16($bytes, $dataOffset)
$entriesCount = [BitConverter]::ToUInt16($bytes, $dataOffset + 2)

if($version -ne 0x0300)
{
    throw ('Unsupported RIFF PAL version: 0x{0:X4}' -f $version)
}

$entriesOffset = $dataOffset + 4
$requiredSize = 4 + $entriesCount * 4

if($dataSize -lt $requiredSize -or $entriesOffset + $entriesCount * 4 -gt $bytes.Length)
{
    throw "RIFF PAL data chunk does not contain $entriesCount complete palette entries."
}

if([string]::IsNullOrWhiteSpace($OutputDir))
{
    $OutputDir = $inputFile.DirectoryName
}

if(-not (Test-Path $OutputDir))
{
    New-Item -ItemType Directory -Path $OutputDir | Out-Null
}

$colors = New-Object 'System.Collections.Generic.List[object]'

for($index = 0; $index -lt $entriesCount; ++$index)
{
    $entryOffset = $entriesOffset + $index * 4
    $colors.Add([PSCustomObject]@{
        R = [int] $bytes[$entryOffset]
        G = [int] $bytes[$entryOffset + 1]
        B = [int] $bytes[$entryOffset + 2]
    }) | Out-Null
}

$baseName = [System.IO.Path]::GetFileNameWithoutExtension($inputFile.Name)
$fullPalettePath = Join-Path $OutputDir ($baseName + '_jasc.pal')
Write-JascPalette $colors $fullPalettePath

$writtenFiles = New-Object 'System.Collections.Generic.List[string]'
$writtenFiles.Add($fullPalettePath)

if(-not $NoSlotFiles)
{
    $slotCount = [Math]::Floor($entriesCount / 16)

    for($slot = 0; $slot -lt $slotCount; ++$slot)
    {
        $slotColors = New-Object 'System.Collections.Generic.List[object]'

        for($index = 0; $index -lt 16; ++$index)
        {
            $slotColors.Add($colors[$slot * 16 + $index]) | Out-Null
        }

        $slotPath = Join-Path $OutputDir ('{0}_obj_slot_{1:D2}.pal' -f $baseName, $slot)
        Write-JascPalette $slotColors $slotPath
        $writtenFiles.Add($slotPath)
    }
}

Write-Output ('Converted {0} RIFF PAL entries from {1}.' -f $entriesCount, $inputFile.FullName)
Write-Output 'Written files:'

foreach($file in $writtenFiles)
{
    Write-Output ('  {0}' -f $file)
}