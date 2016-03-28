Notes for Creating IDE project files with Microsoft DevStudio
-------------------------------------------------------------

The DMAKE makefile system used in SciTech SNAP and MGL SDKs has the benefits of offering independence from particular host platform compilers and target platform systems. As with any tool, a developer may need to spend some time getting familiar with its usage and all its options.

In contrast IDE project files are compiler-specific, and hence are more difficult to support and maintain compared to DMAKE makefiles. For this reason SciTech SDKs don't ordinarily include IDE project files.

Acknowledging that IDEs may be more convenient to use for development nonetheless, the following is a guide for creating an IDE project file with Microsoft Visual C DevStudio for a typical SciTech MGL SDK example.

(1) Create an empty project for either Windows Console or GUI application. Many of the SNAP and MGL SDK examples are consoles apps. Only the examples which include .DEF (or .REF) definition files and include the alternate WinMain entry point to the program are intended to be built as non-console GUI apps with DMAKE.

(2) Add the source files specified in the corresponding DMAKE makefile. In this example, the specific mgldemo.c file and the common mglsamp.c and cmdline.c files have been added. If compiling as a GUI app, add the mglsamp.rc resource file too.

(3) Modify the project's C compile options. If compiling as a console app, add the __CONSOLE__ preprocessor definition. If compiling as a GUI app, add the __WINDOWS__ preprocessor definition instead.

(4) Modify the project's Link options. Typically MGL Windows apps need only to link with the core Win32 libraries KERNEL32.LIB, USER32.LIB, GDI32.LIB, and ADVAPI32.LIB, so others may be removed. Then add MGL.LIB, PM.LIB, and WINMM.LIB to the libraries to be linked. Add the link option "/NODEFAULTLIB:LIBCMT.LIB" to avoid linker warnings.

(5) Add the SciTech SDK INCLUDE and LIB paths to the project. In the example IDE project file, the INCLUDE path was explicitly added as "/I$(SCITECH)/include" to the C compile options, and the LIB path was added as either "/LIBPATH:$(SCITECH)/lib/debug/win32/vc6" or "/LIBPATH:$(SCITECH)/lib/release/win32/vc6" to the Link options for the respective debug or release builds. You should have the SCITECH variable defined in your environment to point to the SDK installation root path (such as "C:\scitech"), or you will have to use that path explicitly. Note that you might find it more convenient to add these paths persistently to your DevStudio configuration under the Tools menu Directory options, which is saved independently from specific project files.

(6) Now you should be able to build and run the MGL demo. If you encounter any warnings or errors, please check that you have setup the project options as described above. Note that since DevStudio typically puts binaries in separate /Release and /Debug subdirectories, you may have to change the MGL root path passed to MGL_init() to the an explicit path (such as "C:\scitech"), or set the MGL_ROOT environment variable prior to launching DevStudio.

Further projects may be created following these steps as a guideline. Note that you might want to examine the base DMAKE makefile definitions for Visual C located at $(SCITECH)\makedefs\vc32.mk for more specific compiler and linker options to be added to your project settings.

For more information, visit SciTech Software at
http://www.scitechsoft.com
