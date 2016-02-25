export REPODIR=$PWD

mkdir -p "$HOME/builds/BrewerMonitor"
pushd "$HOME/builds/BrewerMonitor"
cmake -DCMAKE_TOOLCHAIN_FILE=../Toolchain-gcc-linaro-arm.cmake $REPODIR
popd
