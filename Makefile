# Название исполняемого файла
TARGET = Prefetcher

# Компилятор и флаги
CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -Iinclude -Isrc

# Директории
SRC_DIR = src
BUILD_DIR = build
PREFETCHER_DIR = $(SRC_DIR)/Prefetchers
CACHE_DIR = $(SRC_DIR)/Cache

# Исходники
MAIN = $(SRC_DIR)/main.cpp
PREFETCHERS = $(wildcard $(PREFETCHER_DIR)/*.cpp)
CACHE = $(wildcard $(CACHE_DIR)/*.cpp)

SRCS = $(MAIN) $(PREFETCHERS) $(CACHE)

# Получаем имена объектных файлов в build/
OBJS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SRCS))

# Цель по умолчанию
all: $(TARGET)

# Линковка
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Правило компиляции .cpp → build/*.o
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Очистка
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean
