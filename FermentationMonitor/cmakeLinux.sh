export REPODIR=$PWD

mkdir -p "$HOME/builds/FermentationMonitor"
pushd "$HOME/builds/FermentationMonitor"
cmake -DCMAKE_TOOLCHAIN_FILE=../Toolchain-gcc-x86_64.cmake $REPODIR
popd
