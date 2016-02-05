set REPODIR=%CD%

set BOOST_ROOT=%BOOST_ROOT_VS2013%

mkdir "%HOMEDRIVE%\%HOMEPATH%\builds\KegeratorMonitor"
pushd "%HOMEDRIVE%\%HOMEPATH%\builds\KegeratorMonitor"
cmake -G "Visual Studio 12" %REPODIR%
popd

