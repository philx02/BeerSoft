export REPODIR=$PWD

mkdir -p "$HOME/builds/BrewerController"
pushd "$HOME/builds/BrewerController"
cmake -DCMAKE_TOOLCHAIN_FILE=../Toolchain-gcc-linaro-arm.cmake $REPODIR
popd
