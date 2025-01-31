# Lifting

Activate the environment and go into this folder:

```sh
source llvm-env
cd exercises/3_lifting
```

## Simple example

To get familiar with remill's output we will use a very simple example:

```c
uint64_t lift0(uint64_t rdi)
{
    return rdi;
}
```

In assembly:

```x86
mov rax, rdi
ret
```

Now lift it to bitcode:

```sh
remill-lift -bytes "4889f8c3" -address 0x1000 -arch amd64 -ir_pre_out lift0-pre.ll -ir_out lift0.ll
```

Look at `lift0.ll` and use the `bc-demangle` tool you developed to read the `sub_1000` function.

## Recovering the calling convention

First we need to copy the helpers for our architecture to the current directory:

```sh
cp ../../build/helpers/x86_64/RemillHelpers.ll ./helpers.ll
```

Then link the helpers and optimize:


```sh
llvm-link lift0.ll helpers.ll -o lift0-linked.ll -S
opt "-passes=default<O3>" lift0-linked.ll -o lift0-recovered.ll -S
```

_Note_: You can completely customize the passes here (try the `strip` pass to clean things up).

## More complex example

Compile `lift1.cpp` to `x86_64` and obtain the hexdump of the function:

```sh
clang-15 -c -target x86_64-linux-gnu -O1 lift1.cpp -o lift1.o
llvm-objcopy -O binary --only-section=.text lift1.o lift1.bin
echo $(hexdump -ve '1/1 "%02x"' lift1.bin)
```

Also output the clean bitcode for comparison purposes:

```sh
clang-15 -c lift1.cpp -emit-llvm -S -o lift1-clean.ll -O3
```

```llvm
define dso_local i64 @test1_linear_flow(i64 noundef %n) local_unnamed_addr #0 {
entry:
  %shr = lshr i64 %n, 3
  %mul = mul nuw nsw i64 %shr, 3
  %add = add nuw nsw i64 %mul, 1337
  ret i64 %add
}
```

Now lift the function:

```sh
remill-lift -bytes "48c1ef03488d047f480539050000c3" -address 0x1000 -arch amd64 -ir_out lift1.ll
```

Exercise: clean up the function and reconstruct the LLVM IR

## Recovering the calling convention

The custom implementation of `remill-lift` shows how you might get a similar result using the C++ API:

```sh
remill-lift -bytes "48c1ef03488d047f480539050000c3" -address 0x1000 -arch amd64 -ir_out lift1.ll -call_inputs RDI -call_output RAX
llvm-link lift1.ll helpers.ll -o lift1-linked.ll -S
opt "-passes=default<O3>,strip" lift1-linked.ll -o lift1-recovered.ll -S
```

Exercise: reconstruct the function in lift3.cpp
