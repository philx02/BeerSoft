set REPODIR=%CD%

set BOOST_ROOT=%BOOST_ROOT_VS2013%

mkdir "%HOMEDRIVE%\%HOMEPATH%\builds\FermentationMonitor"
pushd "%HOMEDRIVE%\%HOMEPATH%\builds\FermentationMonitor"
cmake -G "Visual Studio 12" %REPODIR%
popd

