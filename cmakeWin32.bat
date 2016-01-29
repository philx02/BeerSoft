set REPODIR=%CD%

set BOOST_ROOT=%BOOST_ROOT_VS2013%

mkdir "%HOMEDRIVE%\%HOMEPATH%\builds\BeerSoft"
pushd "%HOMEDRIVE%\%HOMEPATH%\builds\BeerSoft"
cmake -G "Visual Studio 12" %REPODIR%
popd

