This is the Compiler for SysY language.

```sh
mkdir build 

cd build 

cmake -DCMAKE_BUILD_TYPE=Debug ..

make 

./compiler input.txt

```

generate llvm ir representation from *.c file

```sh

clang main.c -emit-llvm -S -c -o main.ll

```

generate *.ll to *.o file

```sh

llc -filetype=obj main.ll -o main.o

```