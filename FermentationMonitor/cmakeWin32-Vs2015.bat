set REPODIR=%CD%

set BOOST_LIBRARYDIR=%BOOST_ROOT%\lib32-msvc-14.0

mkdir "%HOMEDRIVE%\%HOMEPATH%\builds\FermentationMonitor"
pushd "%HOMEDRIVE%\%HOMEPATH%\builds\FermentationMonitor"
cmake -G "Visual Studio 14" %REPODIR%
popd

