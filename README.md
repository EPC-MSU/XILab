# XILab



## How to deploy a XiLab project on a Windows computer

1. Clone the XiLab repository https://github.com/EPC-MSU/XILab to ***C:\Projects\xilab***. Go to the desired branch and the required state in it. Or on https://github.com/EPC-MSU/XILab select the desired state and download the archive. Which to unpack in  ***C:\Projects\xilab***. 

2. Clone the **libximc** repository https://github.com/EPC-MSU/libximc to the **C:\Projects\libximc-win** folder

   - In the repository, install the commit corresponding to the state specified in the XiLab Version file XIMC_VER==
   - Download the collected library archive of the desired version, for example, from the SOFTWARE download page:http://files.xisupport.com/Software.en.html#all-libximc-microsmc-and-ximc-labview-versions
   - Unpack from the downloaded archive the **ximc** folder from it to **C:\projects\libximc-win**
   - Inside the copied **ximc** folder, copy the **ximc.h** file to the **win32** and **win64** folders.

3.  Installing dependencies.

   - Clone the **XILab-dependencies** repository https://github.com/EPC-MSU/XILab-dependencies to the **C:\Projects\dependency_files**
   - Move **Qwt** folder from the repository  on the C: \ drive.
   - Unpack the archive in them **Qwt** - **C:\Qwt\msvc2013\qwt-6.0.0**. 
   - To run XiLab, you also need some system libraries. For this purpose, you can set **vcredist_xXX_2013.exe**  version 12.0.30501, which can be taken from the repository with dependencies, or downloaded from the link https://www.microsoft.com/en-us/download/details.aspx?id=40784 .
   - If you do not want to install the entire package, you can copy the necessary files from the WinXX folders to the assembly directory.  Files msvcm90.dll, msvcp90.dll, msvcr90.dll have a product version 9.00.30729.6161. Files msvcp120.dll, msvcr120.dll have a product version 12.00.21005.1.
   - Download the Qt 4.8.6. Qt no longer supports version 4.8.6, but a saved copy can be downloaded from our server as an archive http://files.xisupport.com/XiLab_dependencies/Qt/msvc2013.7z.
   - Unpack the archive in them **Qt** - **C:\Qt\msvc2013\4.8.6** and **C:\Qt\msvc2013\4.8.6_x64**
   - Set the appropriate environment variables **QTDIR->C:\Qt\msvc2013\4.8.6**
     **QWTDIR->C:\Qwt\msvc2013\qwt-6.0.0**
   - Add to **Path**
     C:\Qwt\msvc2013\qwt-6.0.0\lib32;C:\Qwt\msvc2013\qwt-6.0.0\lib64;C:\Qt\msvc2013\4.8.6\bin;C:\Qt\msvc2013\4.8.6_x64\bin
   - To create a distribution, you need to install **NSIS**. To do this, install the NSIS package from the repository.
   - You must specify the path of the file location **makensis.exe** in PATH.
   



## Editing and debugging

For editing and debugging you need to install development software such as **MSVC2013** and **QtCreator**.

In **QtCreator**, it is convenient to perform visual editing of Windows.

In **MSVC2013**, you can edit and debug code, as well as build it. 



## Build a release using a build script

To build releases for Windows, you can use the **build.bat** build script.

- Run the **build.bat**. 

- Portable software packages for different bit sizes are located in the folder **C:\Projects\xilab\dist_dir**.

- The installer after the build is located in the folder **C:\Projects\xilab**.

  

Building **XiLab** is also possible under **linux** and **Mac**.