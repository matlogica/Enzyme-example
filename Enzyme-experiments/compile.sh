OPT=/root/llvm-project/build/bin/opt
CLANG=/root/llvm-project/build/bin/clang++


$CLANG virtual-function.cpp -S -emit-llvm -o input.ll -O2 -fno-vectorize -fno-slp-vectorize -fno-unroll-loops
$OPT input.ll -load-pass-plugin=/root/Enzyme/enzyme/build/Enzyme/LLVMEnzyme-16.so -passes=enzyme -o output.ll -S

$OPT output.ll -O2 -o output_opt.ll -S

$CLANG output_opt.ll -O3 -o a.out

