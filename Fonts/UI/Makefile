# Red Files Font Compiler Makefile
# For GCC/MinGW compilation

CXX = g++
CXXFLAGS = -std=c++17 -O2 -DNDEBUG -Wall
INCLUDES = -Iinclude -Iintegration
LIBS = -lkernel32 -luser32 -lgdi32 -lopengl32
TARGET = build/bin/font_compiler
SRCDIR = tools
OBJDIR = build/obj

# Source files
SOURCES = $(SRCDIR)/font_compiler_main.cpp

# Object files
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

.PHONY: all clean install help

all: $(TARGET)

# Create directories
$(OBJDIR):
	mkdir -p $(OBJDIR)

build/bin:
	mkdir -p build/bin

# Compile object files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Link executable
$(TARGET): $(OBJECTS) | build/bin
	$(CXX) $(CXXFLAGS) $(OBJECTS) $(LIBS) -o $@
	@echo "[OK] C++ font compiler built successfully!"
	@echo "Location: $(TARGET)"

# Clean build files
clean:
	rm -rf build

# Install (copy to system)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

# Show help
help:
	@echo "Red Files Font Compiler Makefile"
	@echo "==============================="
	@echo ""
	@echo "Targets:"
	@echo "  all      - Build the C++ font compiler"
	@echo "  clean    - Remove build files"
	@echo "  install  - Install to system (requires sudo)"
	@echo "  help     - Show this help"
	@echo ""
	@echo "Usage:"
	@echo "  make all"
	@echo "  make clean"
	@echo ""
