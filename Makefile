CC = g++
CFLAGS = -std=c++20 -Wall -Weffc++ -Wextra -Wconversion -Wsign-conversion -Werror -pedantic-errors

SRCDIR = src
TGTDIR = target
BINDIR = target/bin

SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(TGTDIR)/%.o,$(SRCS))
TARGET = $(BINDIR)/webserver

all: $(TARGET)

# Build Target - Link Objects
$(TARGET): $(OBJS) | $(BINDIR)
	@echo "Linking: $@"
	$(CC) $(CFLAGS) $^ -o $@

# Compile Sources
$(TGTDIR)/%.o: $(SRCDIR)/%.cpp | $(TGTDIR)
	@echo "Compiling: $<"
	$(CC) $(CFLAGS) -MMD -c $< -o $@

# Create target directory
$(TGTDIR):
	mkdir -p $(TGTDIR)

# Create bin directory
$(BINDIR):
	mkdir -p $(BINDIR)

clean:
	rm -rf $(TGTDIR)

