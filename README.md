# XILab



## How to deploy a XiLab project on a Windows computer

1. Clone the **XiLab** repository https://github.com/EPC-MSU/XILab to ***C:\Projects\xilab***

2. Clone the **libximc** repository https://github.com/EPC-MSU/libximc to the **C:\Projects\libximc-win** folder

   - In the repository, install the commit corresponding to the state specified in the XiLab Version file XIMC_VER==
   - Download the collected library archive of the desired version, for example, from the SOFTWARE download page:[http://files.xisupport.com/Software.en.html#all-libximc-microsmc-and-ximc-labview-versions](http://files.xisupport.com/Software.en.html#all-libximc-microsmc-and-ximc-labview-versions)
   - Copy the archive to ***C:\Projects\xilab***
   - Unpack the **ximc** folder from it to **C:\projects\libximc-win**
   - Inside the copied **ximc** folder, copy the **ximc.h** file to the **win32** and **win64** folders.

3. Clone the **XILab-dependencies** repository https://github.com/EPC-MSU/XILab-dependencies to the **C:\Projects\dependency_files** 

4. Deploying **Qt**

   - Place the **Qt** and **Qwt** folders on the C: \ drive.
   - **Qt** - **C:\Qt\msvc2013\4.8.6** and **C:\Qt\msvc2013\4.8.6_x64**
   - **Qwt** - **C:\Qwt\msvc2013\qwt-6.0.0**
   - Set the appropriate environment variables **QTDIR->C:\Qt\msvc2013\4.8.6**
     **QWTDIR->C:\Qwt\msvc2013\qwt-6.0.0**
   - Add to **Path**
     C:\Qwt\msvc2013\qwt-6.0.0\lib32;C:\Qwt\msvc2013\qwt-6.0.0\lib64;C:\Qt\msvc2013\4.8.6\bin;C:\Qt\msvc2013\4.8.6_x64\bin

   


## Editing and debugging

For editing and debugging you need to install development software such as **MSVC2013** and **QtCreator**.

In **QtCreator**, it is convenient to perform visual editing of Windows.

In **MSVC2013**, you can edit and debug code, as well as build it. 



## Build a release using a build script

To build releases for Windows, you can use the **build.bat** build script.

- To create a distribution, you need to install **NSIS**:

- You must specify the path of the file location **makensis.exe** in PATH.

- Run the **build.bat**. 

- Portable software packages for different bit sizes are located in the folder **C:\Projects\xilab\dist_dir**.

- The installer after the build is located in the folder **C:\Projects\xilab**.

  

Building **XiLab** is also possible under **linux** and **Mac**.