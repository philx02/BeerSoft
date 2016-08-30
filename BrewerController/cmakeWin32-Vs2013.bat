set REPODIR=%CD%

set BOOST_ROOT=%BOOST_ROOT_VS2013%

mkdir "%HOMEDRIVE%\%HOMEPATH%\builds\BrewerMonitor"
pushd "%HOMEDRIVE%\%HOMEPATH%\builds\BrewerMonitor"
cmake -G "Visual Studio 12" %REPODIR%
popd

