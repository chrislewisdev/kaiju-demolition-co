#
# A Makefile that compiles all .c and .s files in "src" and "res" 
# subdirectories and places the output in a "obj" subdirectory
#

# If you move this project you can change the directory 
# to match your GBDK root directory (ex: GBDK_HOME = "C:/GBDK/"
GBDK_HOME = /home/clh/gbdk

LCC = $(GBDK_HOME)/bin/lcc
PNG2ASSET = $(GBDK_HOME)/bin/png2asset

# You can set flags for LCC here
# For example, you can uncomment the line below to turn on debug output
LCCFLAGS = -I.

# You can set the name of the .gb ROM file here
PROJECTNAME    = kaiju

SRCDIR      = src
OBJDIR      = build
SPRITEDIR	= sprite
MAPDIR		= tilemap
GENDIR		= gen
BINS	    = $(OBJDIR)/$(PROJECTNAME).gb

PNGSOURCES   = $(foreach dir,$(SPRITEDIR),$(notdir $(wildcard $(dir)/*.png)))
PNG_CSOURCES = $(PNGSOURCES:%.png=gen/%.c)
PNG_CHEADERS = $(PNGSOURCES:%.png=gen/%.h)

MAPSOURCES   = $(foreach dir,$(MAPDIR),$(notdir $(wildcard $(dir)/*.png)))
MAP_CSOURCES = $(MAPSOURCES:%.png=gen/%.c)
MAP_CHEADERS = $(MAPSOURCES:%.png=gen/%.h)

CSOURCES    = $(foreach dir,$(SRCDIR),$(notdir $(wildcard $(dir)/*.c)))
ASMSOURCES  = $(foreach dir,$(SRCDIR),$(notdir $(wildcard $(dir)/*.s)))
OBJS       = $(MAP_CSOURCES:gen/%.c=$(OBJDIR)/%.o) $(PNG_CSOURCES:gen/%.c=$(OBJDIR)/%.o) $(CSOURCES:%.c=$(OBJDIR)/%.o) $(ASMSOURCES:%.s=$(OBJDIR)/%.o)

all:	$(OBJDIR)/ $(BINS)

.PRECIOUS: $(PNG_CSOURCES) $(MAP_CSOURCES)

# Compile .c files in "src/" to .o object files
$(OBJDIR)/%.o:	$(SRCDIR)/%.c $(PNG_CSOURCES)
	$(LCC) $(LCCFLAGS) -c -o $@ $<

$(OBJDIR)/%.o:	$(GENDIR)/%.c
	$(LCC) $(LCCFLAGS) -c -o $@ $<

# Compile .s assembly files in "src/" to .o object files
$(OBJDIR)/%.o:	$(SRCDIR)/%.s
	$(LCC) $(LCCFLAGS) -c -o $@ $<

# If needed, compile .c files in "src/" to .s assembly files
# (not required if .c is compiled directly to .o)
$(OBJDIR)/%.s:	$(SRCDIR)/%.c
	$(LCC) $(LCCFLAGS) -S -o $@ $<

$(GENDIR)/%.c:	$(SPRITEDIR)/%.png | gen/
	$(PNG2ASSET) $< -spr8x8 -keep_palette_order -c $@ 

$(GENDIR)/%.c:	$(MAPDIR)/%.png | gen/
	$(PNG2ASSET) $< -map -keep_palette_order -noflip -c $@ 

# Link the compiled object files into a .gb ROM file
$(BINS):	$(OBJS)
	$(LCC) $(LCCFLAGS) -o $(BINS) $(OBJS)

$(OBJDIR)/:
	mkdir -p $(OBJDIR)

gen/:
	mkdir -p gen

clean:
	rm -rf $(OBJDIR) gen

