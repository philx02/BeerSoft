set REPODIR=%CD%

set BOOST_LIBRARYDIR=%BOOST_ROOT%\lib32-msvc-14.0

mkdir "%HOMEDRIVE%\%HOMEPATH%\builds\BrewerController"
pushd "%HOMEDRIVE%\%HOMEPATH%\builds\BrewerController"
cmake -G "Visual Studio 14" %REPODIR%
popd

