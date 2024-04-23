CC = g++
CFLAGS = -std=c++20 -Wall -Weffc++ -Wextra -Wconversion -Wsign-conversion -Werror -pedantic-errors

SRCDIR = src
TSTDIR = test
TGTDIR = target
BINDIR = target/bin

SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(TGTDIR)/%.o,$(SRCS))
TARGET = $(BINDIR)/webserver

TEST_SRCS = $(wildcard $(TSTDIR)/*.cpp)
TEST_OBJS = $(patsubst $(TSTDIR)/%.cpp,$(TGTDIR)/%.o,$(TEST_SRCS))
TEST_TARGET = $(BINDIR)/test

all: $(TARGET) $(TEST_TARGET)

##### WEBSERVER #####
webserver: $(TARGET)

# Build Target - Link Objects
$(TARGET): $(OBJS) | $(BINDIR)
	@echo "Linking: $@"
	$(CC) $(CFLAGS) $^ -o $@

# Compile Sources
$(TGTDIR)/%.o: $(SRCDIR)/%.cpp | $(TGTDIR)
	@echo "Compiling: $<"
	$(CC) $(CFLAGS) -MMD -c $< -o $@

##### UNIT TESTS #####
test: $(TEST_TARGET)

# Build Test Target - Link Test Objects
$(TEST_TARGET): $(TEST_OBJS) $(filter-out $(TGTDIR)/main.o, $(OBJS)) | $(BINDIR)
	@echo "Linking Test: $@"
	@echo "Linking Files: $^"
	$(CC) $(CFLAGS) $^ -o $@

# Compile Test Sources
$(TGTDIR)/%.o: $(TSTDIR)/%.cpp | $(TGTDIR)
	@echo "Compiling Test: $<"
	$(CC) $(CFLAGS) -MMD -c $< -o $@

##### GENERAL #####
# Create target directory
$(TGTDIR):
	mkdir -p $(TGTDIR)

# Create bin directory
$(BINDIR):
	mkdir -p $(BINDIR)

clean:
	rm -rf $(TGTDIR)

details:
	@echo "SRCS: $(SRCS)"
	@echo "OBJS: $(OBJS)"
	@echo "TEST_SRCS: $(TEST_SRCS)"
	@echo "TEST_OBJS: $(TEST_OBJS)"
