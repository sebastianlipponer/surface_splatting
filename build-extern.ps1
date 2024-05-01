$ErrorActionPreference = "Stop"

# Find CMake.
$cmake = 'cmake.exe'

try
{
    Get-Command $cmake | Out-Null
}
catch
{
    $key = 'HKLM:\SOFTWARE\Kitware\CMake'

    try
    {
        $cmake_installdir = Get-ItemProperty -Path $key -Name InstallDir
        $global:cmake = Join-Path -Path $cmake_installdir.InstallDir `
                                  -ChildPath 'bin\cmake.exe'
    }
    catch
    {
        throw 'Failed to find cmake.'
    }
}

# Find the Microsoft Visual Studio install path.
pushd $PSScriptRoot
New-Item -ItemType Directory -Force -Path '.extern/tool/vswhere' | Out-Null
Set-Location -Path '.extern/tool/vswhere'

$vswhere = 'vswhere.exe'

if (-Not (Test-Path $vswhere))
{
    $uri = 'https://github.com/microsoft/vswhere/releases/download/2.7.1'
    Invoke-WebRequest -Uri "$uri/$vswhere" -OutFile $vswhere
}

$msvc = & ./$vswhere -legacy -latest -format json | ConvertFrom-Json
popd

if ($msvc)
{
    $path = $msvc.installationPath
    $version = [Version]$msvc.installationVersion

    if (($version.Major -eq 17) -or ($version.Major -eq 16) `
                                -or ($version.Major -eq 15))
    {
        $global:msvc = Join-Path -Path $msvc.installationPath `
                                 -ChildPath 'VC\Auxiliary\Build'
    }
    elseif ($version.Major -eq 14)
    {
        $global:msvc = Join-Path -Path $msvc.installationPath `
                                 -ChildPath 'bin\amd64'
    }
    elseif ($version.Major -eq 12)
    {
        $global:msvc = Join-Path -Path $msvc.installationPath `
                                 -ChildPath 'VC\bin\amd64'
    }
    else
    {
        throw 'Microsoft Visual Studio 2013 or above required.'
    }
}
else
{
    throw 'Failed to find Microsoft Visual Studio.'
}

# Put MSVC compiler in the search path.
pushd $msvc
cmd /c 'vcvars64.bat&set' |
foreach {
  if ($_ -match '=') {
    $v = $_.split('='); set-item -force -path "ENV:\$($v[0])" `
                                        -value "$($v[1])"
  }
}
popd

# Download Ninja.
pushd $PSScriptRoot
Set-Location -Path '.extern/tool'

if (-Not (Test-Path 'ninja-win/ninja.exe'))
{
    $uri = 'https://github.com/ninja-build/ninja/releases/download/v1.9.0/ninja-win.zip'
    Invoke-WebRequest -Uri $uri -OutFile ninja-win.zip

    Expand-Archive ninja-win.zip
    Remove-item 'ninja-win.zip'
}

$ninja = Resolve-Path -Path 'ninja-win'
$env:path = "$ninja;" + $env:path
popd

# Create the build directory.
pushd $PSScriptRoot
New-Item -ItemType Directory -Force -Path '.extern/build' | Out-Null
Set-Location -Path '.extern/build'

# Run Cmake and perform the build.
& "$cmake" "-G" "`"Ninja`"" "-DCMAKE_INSTALL_PREFIX=../install" "../../cmake/extern"
& "$cmake" "--build" "."
popd
