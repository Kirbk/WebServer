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
LDLIBS        = -ldl $(subst lib,-l,$(sort $(basename $(notdir $(wildcard /usr/lib/librt.so /lib/librt.so))))) -lpthread
OBJS          = client$(OBJEXT) config$(OBJEXT) host$(OBJEXT) http_header$(OBJEXT) log$(OBJEXT) php_wrapper$(OBJEXT) util$(OBJEXT) get$(OBJEXT) post$(OBJEXT) put$(OBJEXT) delete$(OBJEXT) connect$(OBJEXT) options$(OBJEXT) trace$(OBJEXT) patch$(OBJEXT)
SRC           = client.c config.c host.c http_header.c log.c php_wrapper.c util.c get.c post.c put.c delete.c connect.c options.c trace.c patch.c
LINK.cc       = $(LD) $(LDFLAGS)
EXPORTFLAGS   = 
DEPLIBS       = $(foreach lib, , $(foreach libpath, ., $(wildcard $(libpath)/lib$(lib).a)))

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

php_wrapper$(OBJEXT): php_wrapper.c
	$(COMPILE.c) $(EXPORTFLAGS) $(OUTPUT_OPTION) php_wrapper.c

util$(OBJEXT): util.c
	$(COMPILE.c) $(EXPORTFLAGS) $(OUTPUT_OPTION) util.c

get$(OBJEXT): get.c
	$(COMPILE.c) $(EXPORTFLAGS) $(OUTPUT_OPTION) get.c

post$(OBJEXT): post.c
	$(COMPILE.c) $(EXPORTFLAGS) $(OUTPUT_OPTION) post.c

put$(OBJEXT): put.c
	$(COMPILE.c) $(EXPORTFLAGS) $(OUTPUT_OPTION) put.c

delete$(OBJEXT): delete.c
	$(COMPILE.c) $(EXPORTFLAGS) $(OUTPUT_OPTION) delete.c

connect$(OBJEXT): connect.c
	$(COMPILE.c) $(EXPORTFLAGS) $(OUTPUT_OPTION) connect.c

options$(OBJEXT): options.c
	$(COMPILE.c) $(EXPORTFLAGS) $(OUTPUT_OPTION) options.c

trace$(OBJEXT): trace.c
	$(COMPILE.c) $(EXPORTFLAGS) $(OUTPUT_OPTION) trace.c

patch$(OBJEXT): patch.c
	$(COMPILE.c) $(EXPORTFLAGS) $(OUTPUT_OPTION) patch.c

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
