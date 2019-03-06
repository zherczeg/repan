ifdef CROSS_COMPILER
CC = $(CROSS_COMPILER)
else
ifndef CC
# default compiler
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

OBJS = $(addprefix $(OUTDIR)/, alloc.o literal.o nodes.o opt_flatten.o \
			opt_merge_alternatives.o opt_uncapture.o parser_decode.o parser_glob.o \
			parser_javascript.o parser_pcre.o parser_posix.o parser_util.o \
			to_string_pcre.o to_string_encode.o unicode_gen.o util.o)
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
