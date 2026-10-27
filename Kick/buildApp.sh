set -x
toolChainfile=${TOOLCHAIN_FILE}
mkdir -p Build/$1
echo $toolChainfile
cd Build/$1 
cmake -GNinja -DCMAKE_BUILD_TYPE=$1 -DCMAKE_TOOLCHAIN_FILE=${toolChainfile}  ../../
ninja -j20
cp KT_Kick ../../
cd -
