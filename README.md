# XILab



## How to deploy a XiLab project on a Windows computer

1. Clone the **XiLab** repository https://github.com/EPC-MSU/XILab to ***C:\Projects\xilab***

2. Clone the **libximc** repository https://github.com/EPC-MSU/libximc to the **C:\Projects\libximc-win** folder

   - In the repository, install the commit corresponding to the state specified in the XiLab Version file XIMC_VER==
   - Download the collected library archive of the desired version, for example, from the SOFTWARE download page:[http://files.xisupport.com/Software.en.html#all-libximc-microsmc-and-ximc-labview-versions](http://files.xisupport.com/Software.en.html#all-libximc-microsmc-and-ximc-labview-versions)
   - Copy the archive to ***C:\Projects\xilab***
   - Unpack the **ximc** folder from it to **C:\projects\libximc-win**
   - Inside the copied **ximc** folder, copy the **ximc.h** file to the **win32** and **win64** folders.

3. Deploying **Qt**

   - Place the **Qt** and **Qwt** folders on the C: \ drive.
   - **Qt** - **C:\Qt\msvc2013\4.8.6** and **C:\Qt\msvc2013\4.8.6_x64**
   - **Qwt** - **C:\Qwt\msvc2013\qwt-6.0.0**
   - Set the appropriate environment variables **QTDIR->C:\Qt\msvc2013\4.8.6**
     **QWTDIR->C:\Qwt\msvc2013\qwt-6.0.0**
   - Add to **Path**
     C:\Qwt\msvc2013\qwt-6.0.0\lib32;C:\Qwt\msvc2013\qwt-6.0.0\lib64;C:\Qt\msvc2013\4.8.6\bin;C:\Qt\msvc2013\4.8.6_x64\bin

4. Additional dependencies

   Create a folder C:\dependency_files It is necessary to place

| dependency_files          | win64        | win32        |
| :------------------------ | ------------ | ------------ |
| win32      ->             | ->           | msvcm90.dll  |
| win64      ->             | msvcm90.dll  | msvcp90.dll  |
| cfgmgr32.lib              | msvcp90.dll  | msvcp120.dll |
| setupapi.lib              | msvcp120.dll | msvcr90.dll  |
| SignTool.exe              | msvcr90.dll  | msvcr90.dll  |
| vcredist_x64_2008_SP1.exe | msvcr120.dll |              |
| vcredist_x64_2013.exe     |              |              |
| vcredist_x86_2008_SP1.exe |              |              |
| vcredist_x86_2013.exe     |              |              |
| x64_cfgmgr32.lib          |              |              |
| x64_setupapi.lib          |              |              |



## Editing and debugging

