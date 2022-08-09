param($sdk,$vs)

$WINSDK_VER = $sdk

$VS2017_X64 = "C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Tools\MSVC\14.16.27023\bin\Hostx64\x64"
$VS2017_X86 = "C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Tools\MSVC\14.16.27023\bin\Hostx64\x86"
$WINSDK_BIN = "C:\Program Files (x86)\Windows Kits\10\bin\$WINSDK_VER\x64"

$VS2022_X64 = "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.31.31103\bin\Hostx64\x64"
$VS2022_X86 = "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.31.31103\bin\Hostx64\x86"

if($vs -eq "VS2017")
{
	$Env:Path += ";$VS2017_X64;$VS2017_X86;"
	$Env:LIB += ";C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Tools\MSVC\14.16.27023\lib\x64;"
	$Env:INCLUDE += "C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Tools\MSVC\14.16.27023\ATLMFC\include;C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Tools\MSVC\14.16.27023\include;"
}
elseif($vs -eq "VS2022")
{
	$Env:Path += ";$VS2022_X64;$VS2022_X86;"
	$Env:LIB += ";C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.31.31103\lib\x64;"
	$Env:INCLUDE += "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.31.31103\include;C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.31.31103\atlmfc\include;"
}

$Env:Path += ";$WINSDK_BIN;"

$Env:LIB += ";C:\Program Files (x86)\Windows Kits\10\Lib\$WINSDK_VER\um\x64;C:\Program Files (x86)\Windows Kits\10\Lib\$WINSDK_VER\ucrt\x64;"

