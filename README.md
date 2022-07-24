# FBX SDK Wrapper

A wrapper library of FBX SDK to help reading fbx file.

Test on win 10 with VS2019 and FBX SDK version 2020.2.1.

## Build guide

1. download fbx sdk from official site: https://www.autodesk.com/developer-network/platform-technologies/fbx-sdk-2020-2-1
2. install fbx sdk.
3. copy all files from `<sdk_install_path>/include` to `./fbxsdk`.
4. copy static library files (*-mt.lib and libfbxsdk.lib) from `<sdk_install_path>/lib/vs2019/x64` to `./lib`.
5. run cmake: `cmake . -B build`
6. open `build/fbx_sdk_wrapper.sln` and build project in VS.

## Thrid party

FBX SDK: https://damassets.autodesk.net/content/dam/autodesk/www/adn/fbx/2020-0/About_FBX_SDK_2020.0.pdf
