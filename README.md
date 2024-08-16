# Compile & Debug ELF for Android

## Compile

Download NDK:

```
wget https://dl.google.com/android/repository/android-ndk-r27-linux.zip
unzip android-ndk-r27-linux.zip # use unzip instead of ark
```

Prepare a CMakeLists.txt (look at the [example](./example/CMakeLists.txt)).
Compile with:

```sh
cd example
mkdir build
cd build

export NDK=$PWD/android-ndk-r27
cmake -DCMAKE_TOOLCHAIN_FILE=$NDK/build/cmake/android.toolchain.cmake -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=31 ..
make
```

Push on device:

```sh
adb push example /data/local/tmp
adb shell
cd /data/local/tmp
./example
```

If the program requires shared libraries you can upload them in /data/local/tmp/lib
and launch it with:

```
LD_LIBRARY_PATH=/data/local/tmp/lib /data/local/tmp/example
```

### ASAN

Add into the CMakeLists.txt the following lines and recompile:

```
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address")
```

If on the android system libclang_rt.asan-aarch64-android.so is not already
present, then you can push your own version with:

```
# note that the clang version number depends on the ndk version
adb push $NDK/toolchains/llvm/prebuilt/linux-x86_64/lib64/clang/14.0.7/lib/linux/libclang_rt.asan-aarch64-android.so /data/local/tmp/lib
```

And then launch the example with:

```sh
LD_LIBRARY_PATH=/data/local/tmp/lib /data/local/tmp/example
```

## Debug

I prefer to use gdb with gdb-multiarch because I can use pwndbg with it.

### GDB

Gdbserver is available only in older versions of NDK <= android-ndk-r21e.

```sh
curl https://dl.google.com/android/repository/android-ndk-r21e-linux-x86_64.zip --output ndk.zip
unzip ndk.zip
```

```sh
adb forward tcp:5039 tcp:5039
adb push $NDK/prebuilt/android-arm64/gdbserver/gdbserver
adb shell
cd /data/local/tmp
chmod +x gdbserver
./gdbserver :5039 ./example
```

And on another terminal:

```
gdb-multiarch -ex "target remote :5039" -ex "b main" \
-ex "set auto-solib-add off" -ex "set solib-absolute-prefix /invalid/path" -ex "set solib-search-path /invalid/path" \
-ex "c" \
-ex "set auto-solib-add on" -ex "set solib-search-path ./local_lib/"
```

This command specifies that gdb-multiarch should look for symbols in
shared libraries only in the local directory local_lib (not android), otherwise it
takes several minutes to look for symbols in all shared libraries. The trick is
to let the program continue until main, and then when the libraries are mapped in memory
re-enable the solib option. This is particular useful if you are debugging an
harness for a shared library.

### LLDB

LLDB is the default debugger for android. It can be used in platform or gdbserver
mode.

#### Platform

```sh
adb forward tcp:5039 tcp:5039
adb push  $NDK/toolchains/llvm/prebuilt/linux-x86_64/lib64/clang/14.0.7/lib/linux/aarch64/lldb-server /data/local/tmp
adb shell
cd /data/local/tmp
./lldb-server platform --listen '*:5039' --server
```

And on another terminal shell:

```
$NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/lldb.sh -o 'platform select remote-android' \
          -o 'platform connect connect://:5039' \
          -o 'platform shell cd /data/local/tmp' \
          -o 'target create /data/local/tmp/example' \
          -o 'br s -n main' \
          -o 'process launch --environment LD_LIBRARY_PATH=/data/local/tmp/lib -- /data/local/tmp/example'
```

#### Gdbserver

```sh
adb forward tcp:5039 tcp:5039
adb push  $NDK/toolchains/llvm/prebuilt/linux-x86_64/lib64/clang/14.0.7/lib/linux/aarch64/lldb-server /data/local/tmp
export LD_LIBRARY_PATH=/data/local/tmp/lib
cd /data/local/tmp
./lldb-server g :5039 -- ./example
```

And on another terminal/shell:

```
lldb
gdb-remote localhost:5039
c
```
