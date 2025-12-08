CUR_DIR  = $(shell pwd)

LLVM_BUILD := $(shell llvm-config --prefix)
LLVM_USED  := $(LLVM_BUILD)

SRC_DIR := ${CURDIR}/src
SRC_BUILD := ${CURDIR}/build

NPROC := ${shell nproc}
LOCAL_OMP_DIR := ${HOME}/my-llvm-omp

# build_src_func = \
# 	(mkdir -p ${2} \
# 		&& cd ${2} \
# 		&& PATH=${LLVM_BUILD}/bin:${PATH}\
# 			CC=clang CXX=clang++ \
# 			cmake ${1} \
# 				-DCMAKE_BUILD_TYPE=Release \
#         		-DCMAKE_CXX_FLAGS_RELEASE="-std=c++17 -I/usr/local/lib/clang/20/include -fno-rtti -fpic -fopenmp -O3" \
# 				-DLLVM_INSTALL_DIR=${LLVM_USED} \
# 		&& make -j${NPROC})

build_src_func = \
    (mkdir -p ${2} \
        && cd ${2} \
        && PATH=${LLVM_BUILD}/bin:${PATH} \
           CC=clang CXX=clang++ \
           cmake ${1} \
                -DCMAKE_BUILD_TYPE=Release \
                -DCMAKE_CXX_STANDARD=17 \
                -DCMAKE_CXX_STANDARD_REQUIRED=ON \
                -DCMAKE_CXX_FLAGS_RELEASE="-fno-rtti -fpic -fopenmp -O3 -I${LOCAL_OMP_DIR}/include" \
                -DCMAKE_EXE_LINKER_FLAGS="-L${LOCAL_OMP_DIR}/lib -Wl,-rpath,${LOCAL_OMP_DIR}/lib" \
                -DCMAKE_SHARED_LINKER_FLAGS="-L${LOCAL_OMP_DIR}/lib -Wl,-rpath,${LOCAL_OMP_DIR}/lib" \
                -DLLVM_INSTALL_DIR=${LLVM_USED} \
        && make -j${NPROC})

all: analyzer

analyzer:
	echo ${LLVM_BUILD}
	$(call build_src_func, ${SRC_DIR}, ${SRC_BUILD})

clean:
	rm -rf ${SRC_BUILD}