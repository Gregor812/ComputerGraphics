TARGET = App

DEBUG = 1
OPT = -O0

BUILD_DIR = build

SOURCES = \
main.cpp \
TgaImage/TgaImage.cpp

CXX = g++
SZ = size

INCLUDES = \
-ITgaImage

CXXFLAGS = -Wall -Wextra -pedantic -std=c++14 $(INCLUDES) $(OPT)

ifeq ($(DEBUG), 1)
CXXFLAGS += -g -gdwarf-2
endif

LIBS = -lm
LDFLAGS = $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

all: $(BUILD_DIR)/$(TARGET)

OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(SOURCES:.cpp=.o)))
vpath %.c $(sort $(dir $(SOURCES)))

$(BUILD_DIR)/%.o: %.cpp Makefile | $(BUILD_DIR)
	$(CXX) -c $(CXXFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.cpp=.lst)) $< -o $@

$(BUILD_DIR)/$(TARGET): $(OBJECTS) Makefile
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR):
	mkdir $@

clean:
	rm -rf $(BUILD_DIR)
