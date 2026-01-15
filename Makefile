#---------------------------------------------------------------------------------------------------------------------
# TARGET is the name of the output.
# BUILD is the directory where object files & intermediate files will be placed.
# LIBBUTANO is the main directory of butano library (https://github.com/GValiente/butano).
# PYTHON is the path to the python interpreter.
# SOURCES is a list of directories containing source code.
# INCLUDES is a list of directories containing extra header files.
# DATA is a list of directories containing binary data.
# GRAPHICS is a list of files and directories containing files to be processed by grit.
# AUDIO is a list of files and directories containing files to be processed by mmutil.
# DMGAUDIO is a list of files and directories containing files to be processed by mod2gbt and s3m2gbt.
# FONTS is a list of directories containing font files.
# TEXTS is a list of directories or filenames containing text files.
# ROMTITLE is a uppercase ASCII, max 12 characters text string containing the output ROM title.
# ROMCODE is a uppercase ASCII, max 4 characters text string containing the output ROM code.
# USERFLAGS is a list of additional compiler flags:
#     Pass -flto to enable link-time optimization.
#     Pass -O0 or -Og to try to make debugging work.
# USERCXXFLAGS is a list of additional compiler flags for C++ code only.
# USERASFLAGS is a list of additional assembler flags.
# USERLDFLAGS is a list of additional linker flags:
#     Pass -flto=<number_of_cpu_cores> to enable parallel link-time optimization.
# USERLIBDIRS is a list of additional directories containing libraries.
#     Each libraries directory must contains include and lib subdirectories.
# USERLIBS is a list of additional libraries to link with the project.
# DEFAULTLIBS links standard system libraries when it is not empty.
# STACKTRACE enables stack trace logging when it is not empty.
# USERBUILD is a list of additional directories to remove when cleaning the project.
# EXTTOOL is an optional command executed before processing audio, graphics and code files.
#
# All directories are specified relative to the project directory where the makefile is found.
#---------------------------------------------------------------------------------------------------------------------
TARGET      	:=  luar-assault-v0.5
BUILD       	:=  build
LIBBUTANO   	:=  ../butano/butano
PYTHON      	:=  python
SOURCES     	:=  src src/fr_lib src/enemies src/scenes ../butano/common/src
INCLUDES    	:=  include include/fr_lib include/game_scene_defs include/enemies include/scenes ../butano/common/include $(BUILD)
DATA        	:=
GRAPHICS    	:=  graphics ../butano/common/graphics graphics/shape_group_textures $(BUILD)/fonts
AUDIO       	:=  audio ../butano/common/audio
DMGAUDIO    	:=  dmg_audio ../butano/common/dmg_audio
GBAFONTS		:=  ../gba-free-fonts/fonts
LOCALFONTS		:=  fonts
FONTS     	 	:=  $(GBAFONTS)/SourceHanSans/JP $(GBAFONTS)/SourceHanMono/JP $(LOCALFONTS)/VonwaonBitmap $(LOCALFONTS)/k8x8
TEXTS           :=  src/main.cpp 
ROMTITLE    	:=  LUAR-ASSAULT
ROMCODE     	:=  SBTP
USERFLAGS   	:=  
USERCXXFLAGS	:=  
USERASFLAGS 	:=  
USERLDFLAGS 	:=  
USERLIBDIRS 	:=  
USERLIBS    	:=  
DEFAULTLIBS 	:=  
STACKTRACE		:=	
USERBUILD   	:=  
EXTTOOL     	:=  @$(PYTHON) -B ./tools/precompile.py --build=$(BUILD) --fonts="$(FONTS)" --texts="$(TEXTS)"

# <-- HOW ABOUT TESTING AUDIOBACKEND	:=  maxmod ???

#---------------------------------------------------------------------------------------------------------------------
# Export absolute butano path:
#---------------------------------------------------------------------------------------------------------------------
ifndef LIBBUTANOABS
	export LIBBUTANOABS	:=	$(realpath $(LIBBUTANO))
endif

#---------------------------------------------------------------------------------------------------------------------
# Include main makefile:
#---------------------------------------------------------------------------------------------------------------------
include $(LIBBUTANOABS)/butano.mak
