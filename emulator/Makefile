EXENAME          = anamatsu

MAINFILES        = src/main.c src/blooper.c src/cpu.c src/framebuffer.c src/keyboard.c src/screen.c
MUSASHIFILES     = musashi/m68kcpu.c musashi/m68kdasm.c musashi/softfloat/softfloat.c
MUSASHIGENCFILES = musashi/m68kops.c
MUSASHIGENHFILES = musashi/m68kops.h
MUSASHIGENERATOR = musashi/m68kmake

# EXE = .exe
# EXEPATH = .\\
EXE =
EXEPATH = ./

.CFILES   = $(MAINFILES) $(MUSASHIFILES) $(MUSASHIGENCFILES)
.OFILES   = $(.CFILES:%.c=%.o)

CC          = gcc
SDL2_CONFIG = sdl2-config
WARNINGS    = -Wall -Wextra `$(SDL2_CONFIG) --cflags --libs`
CFLAGS      = $(WARNINGS)
LFLAGS      = $(WARNINGS)

TARGET = $(EXENAME)$(EXE)

DELETEFILES = $(MUSASHIGENCFILES) $(MUSASHIGENHFILES) $(.OFILES) $(TARGET) $(MUSASHIGENERATOR)$(EXE)


all: $(TARGET)

clean:
	rm -f $(DELETEFILES)

$(TARGET): $(MUSASHIGENHFILES) $(.OFILES) Makefile
	$(CC) -o $@ $(.OFILES) $(LFLAGS) -lm

$(MUSASHIGENCFILES) $(MUSASHIGENHFILES): $(MUSASHIGENERATOR)$(EXE)
	cd musashi; \
	$(EXEPATH)m68kmake

$(MUSASHIGENERATOR)$(EXE):  $(MUSASHIGENERATOR).c
	$(CC) -o  $(MUSASHIGENERATOR)$(EXE)  $(MUSASHIGENERATOR).c
