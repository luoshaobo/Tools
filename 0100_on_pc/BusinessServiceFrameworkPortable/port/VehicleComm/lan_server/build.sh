#!/bin/bash
#export CC=/usr/local/src/gcc4.8/bin/gcc
#export CXX=/usr/local/src/gcc4.8/bin/g++
mkdir -p `pwd`/build
HOME=`pwd`/build
LIBBOOST=$HOME/../../../../../package/opensource/boost/
LIBCMAKE=$HOME/../../../../../package/opensource/cmake/
#CMAKE=$HOME/build_cmake/inst/bin/cmake
CMAKE=$HOME/../../../../../release/fs/devel/usr/bin/cmake
VSOMEIP=$HOME/../../../../../package/opensource/vSomeIP/
SERVER=$HOME/../


build_libboost() {
    # BUILD LIBBOOST for X86 and put output include/lib into local directories
    cd $HOME
    sudo rm -rf build_libboost
    mkdir -p build_libboost/lib
    mkdir -p build_libboost/include
    cd $LIBBOOST
    ./b2 --clean
    rm -rf 'bin.v2/project-cache-jam'
    ./bootstrap.sh --with-libraries=system,filesystem,thread,log --libdir=$HOME/build_libboost/lib --includedir=$HOME/build_libboost/include
    ./b2 -a
    sudo ./b2 install
    ./b2 --clean
}

build_cmake(){
    cd $HOME
    sudo rm -rf build_cmake
    mkdir -p build_cmake/inst
    mkdir -p build_cmake/data
    mkdir -p build_cmake/doc
    mkdir -p build_cmake/man
    cd $LIBCMAKE
    ./configure --prefix=$HOME/build_cmake/inst --datadir=$HOME/build_cmake/data --docdir=$HOME/build_cmake/doc --mandir=$HOME/build_cmake/man
    make
    make install
}

build_vsomeip() {
    # BUILD vSomeIP using our local libboost libs
    cd $HOME
    rm -rf build_vsomeip
    mkdir build_vsomeip
    cd build_vsomeip
    $CMAKE -D CMAKE_INCLUDE_PATH=$HOME/build_libboost/include -D CMAKE_LIBRARY_PATH=$HOME/build_libboost/lib -DENABLE_SIGNAL_HANDLING=1 $VSOMEIP
    make
    make doc
    cd $HOME
}

build_lan_server() {
    #BUILD VGMServer
    cd $HOME
    rm -rf build_lan_server
    mkdir build_lan_server
    cd build_lan_server
    $CMAKE -D CMAKE_INCLUDE_PATH=$HOME/build_libboost/include -D CMAKE_LIBRARY_PATH=$HOME/build_libboost/lib $SERVER
    make
    cd $HOME
}

if [ "$1" == "--help" ]; then
    echo "./build.sh --patch   - Patches vSomeIP for gcc 4.6.3"
    echo "./build.sh --client  - To build the client for x86 (run plain ./build.sh before)"
    echo "./build.sh --clean   - Remove everything that is built"
    echo "./build.sh           - Builds vSomeIP and server component"
    exit 0;
fi;

if [ "$1" == "--clean" ]; then
    echo "Cleaning: sudo rm -rf $HOME";
    sudo rm -rf $HOME;
    exit 0;
fi;

if [ "$1" == "--patch" ]; then
    echo "Patching vSomeIP to support gcc 4.6.3 (ubuntu 12.04 default)";
    cd $VSOMEIP
    patch -p1 < $HOME/../lan_server/gcc4_6_3.patch
    exit 0;
fi

# Build Boost
if [ ! -d "$HOME/build_libboost" ]; then
    echo "build_libboost/ is missing, building it";
    build_libboost;
fi

# Build_cmake
#if [ ! -d "$HOME/build_cmake" ]; then
#    echo "build_cmake/ is missing, building it";
#    build_cmake;
#fi

# Build vSomeIP
if [ ! -d "$HOME/build_vsomeip" ]; then
    echo "build_vsomeip is missing, building it";
    build_vsomeip;
else
    cd $HOME
    cd build_vsomeip
    make 
fi

if [ ! -d "$HOME/build_lan_server" ]; then
    echo "build_lan_server is missing, building it";
    build_lan_server;
else
    cd $HOME
    cd build_lan_server
    make
fi
