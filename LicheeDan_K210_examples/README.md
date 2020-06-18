LicheeDan_K210_examples
======
Use the Kendryte K210 standalone SDK commitid d13b38098a3598b0018c38590387e43953efd140  
and should use gcc-8.2. This can be download at https://kendryte.com/downloads/]

Kendryte official standalone sdk demo
======
You can find the official standalone sdk demo at https://github.com/kendryte/kendryte-standalone-demo.git  
It has not been tested on Sipeed board. so you must test it by yourself. 

Demo instruction
======

The following demos have been tested:

##### hello_world -- uart output hello_world
##### lcd -- lcd display
##### dvp_ov -- dvp output to lcd
##### gpiohs_led -- press key boot change led blue
##### ai_dmeo_sim -- k210 ai demo sim
##### lvgl -- lvgl demo on maix go
##### mic_play -- mic record, speak play
##### wav_test -- test wav decode, speak play
##### kpu_conv -- kpu demo
##### dace_detect_kmodel_flash -- load kmodel from flash

Kendryte K210 standalone SDK
======

This SDK is for Kendryte K210 without OS support.
If you have any questions, please be free to contact us.

# Usage

If you want to start a new project, for instance, `hello_world`, you only need to:

`mkdir` your project in `src/`, `cd src && mkdir hello_world`, then put your codes in it, and build it.

```shell
mkdir build && cd build
cmake .. -DPROJ=<ProjectName> -DTOOLCHAIN=/opt/riscv-toolchain/bin && make
```

You will get 2 key files, `hello_world` and `hello_world.bin`.

1. If you are using JLink to run or debug your program, use `hello_world`
2. If you want to flash it in UOG, using `hello_world.bin`, then using flash-tool(s) burn <ProjectName>.bin to your flash.

This is very important, don't make a mistake in files.
