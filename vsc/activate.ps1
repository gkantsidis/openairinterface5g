[CmdletBinding()]
param (
)

$pythonroot = Join-Path -Path $PSScriptRoot -ChildPath packages | Join-Path -ChildPath python | Join-Path -ChildPath tools

if (-not (Test-Path -Path $pythonroot -PathType Container)) {
    throw "Cannot find python in $pythonroot"
}

$paths = @(
    $pythonroot
    Join-Path -Path $pythonroot -ChildPath Scripts
)

$paths | ForEach-Object -Process {
    $p = $_
    if (-not (Test-Path -Path $p -PathType Container)) {
        Write-Warning -Message "Cannot find path $p"
    }

    $envpath = $Env:Path
    if ($envpath.Contains($p, [System.StringComparison]::InvariantCultureIgnoreCase)) {
        Write-Verbose -Message "Directory $p already in path; ignoring"
    } else {
        Write-Verbose -Message "Adding $p to path"
        $Env:PATH = "{0};{1}" -f $p,$Env:Path
    }
}

python -m pip install --upgrade pip
pip3 install virtualenv

$vpython = Join-Path -Path $PSScriptRoot -ChildPath .python
if (-not (Test-Path -Path $vpython)) {
    virtualenv $vpython
}

$activate = Join-Path -Path $vpython -ChildPath Scripts | Join-Path -ChildPath "activate.ps1"
. $activate

$pipini = Join-Path -Path $Env:VIRTUAL_ENV -ChildPath "pip.ini"
$pipiniorig = Join-Path -Path $PSScriptRoot -ChildPath "PythonFiles" | Join-Path -ChildPath "pip.ini"
if (-not (Test-Path -Path $pipini)) {
    Write-Verbose -Message "Copying pip.ini"

    Copy-Item -Path $pipiniorig -Destination $pipini
}