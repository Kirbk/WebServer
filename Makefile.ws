#----------------------------------------------------------------------------
#       Macros
#----------------------------------------------------------------------------
CC            = gcc
CXX           = g++
LD            = $(CXX) $(CCFLAGS) $(CPPFLAGS)
AR            = ar
PICFLAGS      = -fPIC
CPPFLAGS      = $(PICFLAGS) $(GENFLAGS) -g -D_REENTRANT
OBJEXT        = .o
OUTPUT_OPTION = -o "$@"
COMPILE.c     = $(CC) $(CFLAGS) $(CPPFLAGS) -c
COMPILE.cc    = $(CXX) $(CCFLAGS) $(CPPFLAGS) -c
LDFLAGS       = -L"."
CCC           = $(CXX)
MAKEFILE      = Makefile.ws
DEPENDENCIES  = .depend.$(MAKEFILE)
BTARGETDIR    = ./
BIN           = $(BTARGETDIR)ws$(EXESUFFIX)$(EXEEXT)
CAT           = cat
MV            = mv -f
RM            = rm -rf
CP            = cp -p
NUL           = /dev/null
MKDIR         = mkdir -p
TESTDIRSTART  = test -d
TESTDIREND    = ||
TOUCH         = touch
EXEEXT        = 
LIBPREFIX     = lib
LIBSUFFIX     = 
GENFLAGS      = 
LDLIBS        = -l"sqlite3$(LIBSUFFIX)" -ldl $(subst lib,-l,$(sort $(basename $(notdir $(wildcard /usr/lib/librt.so /lib/librt.so))))) -lpthread
OBJS          = client$(OBJEXT) config$(OBJEXT) host$(OBJEXT) http_header$(OBJEXT) log$(OBJEXT)
SRC           = client.c config.c host.c http_header.c log.c
LINK.cc       = $(LD) $(LDFLAGS)
EXPORTFLAGS   = 
DEPLIBS       = $(foreach lib, sqlite3$(LIBSUFFIX) , $(foreach libpath, ., $(wildcard $(libpath)/lib$(lib).a)))

#----------------------------------------------------------------------------
#       Local targets
#----------------------------------------------------------------------------

all: $(BIN)

$(BIN): $(OBJS) $(DEPLIBS)
	@$(TESTDIRSTART) "$(BTARGETDIR)" $(TESTDIREND) $(MKDIR) "$(BTARGETDIR)"
	$(LINK.cc) $(OBJS) $(LDLIBS) $(OUTPUT_OPTION)

generated: $(GENERATED_DIRTY)
	@-:

client$(OBJEXT): client.c
	$(COMPILE.c) $(EXPORTFLAGS) $(OUTPUT_OPTION) client.c

config$(OBJEXT): config.c
	$(COMPILE.c) $(EXPORTFLAGS) $(OUTPUT_OPTION) config.c

host$(OBJEXT): host.c
	$(COMPILE.c) $(EXPORTFLAGS) $(OUTPUT_OPTION) host.c

http_header$(OBJEXT): http_header.c
	$(COMPILE.c) $(EXPORTFLAGS) $(OUTPUT_OPTION) http_header.c

log$(OBJEXT): log.c
	$(COMPILE.c) $(EXPORTFLAGS) $(OUTPUT_OPTION) log.c

clean:
	-$(RM) $(OBJS)

realclean: clean
	-$(RM) $(BIN)

check-syntax:
	$(COMPILE.cc) $(EXPORTFLAGS) -Wall -Wextra -pedantic -fsyntax-only $(CHK_SOURCES)

#----------------------------------------------------------------------------
#       Dependencies
#----------------------------------------------------------------------------


depend:
	-$(MPC_ROOT)/depgen.pl  $(CFLAGS) $(CCFLAGS) $(CPPFLAGS) -f $(DEPENDENCIES) $(SRC) 2> $(NUL)

-include $(DEPENDENCIES)
