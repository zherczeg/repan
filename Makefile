ifdef CROSS_COMPILER
CC = $(CROSS_COMPILER)
else
ifndef CC
# default compier
CC = gcc
endif
endif
AR = ar

CFLAGS += -Wall -Iheader -std=c99

OUTDIR = out

TARGET = librepan.a
SRCDIR = src

TEST_TARGET = repan-test
TOOLS_SRCDIR = src-tools

OBJS = $(addprefix $(OUTDIR)/, alloc.o literal.o nodes.o opt_uncapture.o \
			opt_flatten.o parser_decode.o parser_javascript.o parser_pcre.o \
			to_string.o to_string_char.o util.o)
TEST_OBJS = $(addprefix $(OUTDIR)/, repan-test.o)

all: $(OUTDIR) $(OUTDIR)/$(TARGET) $(OUTDIR)/$(TEST_TARGET)

$(OUTDIR) :
	mkdir $(OUTDIR)

$(OUTDIR)/%.o : $(SRCDIR)/%.c $(OUTDIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

$(OUTDIR)/%.o : $(TOOLS_SRCDIR)/%.c $(OUTDIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OUTDIR)/*.o
	rm -f $(OUTDIR)/$(TARGET)
	rm -f $(OUTDIR)/$(TEST_TARGET)

$(OUTDIR)/$(TARGET): $(OBJS)
	$(AR) r $@ $?

$(OUTDIR)/$(TEST_TARGET) : $(OUTDIR)/$(TARGET) $(TEST_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(TEST_OBJS) -o $@ -L$(OUTDIR) -lrepan
