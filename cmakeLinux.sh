export REPODIR=$PWD

mkdir -p "$HOME/builds/BeerSoft"
pushd "$HOME/builds/BeerSoft"
cmake -DCMAKE_TOOLCHAIN_FILE=Toolchain-gcc-linaro-arm.cmake $REPODIR
popd
