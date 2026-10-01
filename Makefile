# ===========================================================================
#  comm-in-cpp / Makefile
#  依順序編譯 .o，每一階段 print 進度
#  
#    make          依序編譯 + 連結
#    make run      編譯後執行
#    make clean    清除 obj/ 與 bin/
#  
#  Author: Opencode Zen (Big Pickle)
# ===========================================================================

CXX      := g++
# -std=gnu++17 is for POSIX use of M_PI
CXXFLAGS := -std=gnu++17 -O2 -Wall -Wextra

# --- 外部依賴 -------------------------------------------------------------
FFTW_LIB ?= -lfftw3
EIGEN_DIR ?= ../../eigen
INC := $(if $(strip $(EIGEN_DIR)),-I$(strip $(EIGEN_DIR)),)

LDFLAGS := $(FFTW_LIB)

# --- 路徑 -----------------------------------------------------------------
OBJ_DIR := obj
BIN_DIR := bin
TARGET  := $(BIN_DIR)/test_plot.exe

OBJS := \
	$(OBJ_DIR)/lib/Mod/qammap.o \
	$(OBJ_DIR)/lib/Utils/functions.o \
	$(OBJ_DIR)/lib/Base/transmitter.o \
	$(OBJ_DIR)/lib/Utils/convolution.o \
	$(OBJ_DIR)/lib/Base/channel.o \
	$(OBJ_DIR)/lib/Base/receiver.o \
	$(OBJ_DIR)/lib/Mod/OFDM_Tx.o \
	$(OBJ_DIR)/lib/Mod/OFDM_Rx.o \
	$(OBJ_DIR)/lib/Mod/OTFS_Tx.o \
	$(OBJ_DIR)/lib/Mod/OTFS_Rx.o \
	$(OBJ_DIR)/lib/test_plot.o

DEPS := $(OBJS:.o=.d)

STEPS := 12

.NOTPARALLEL:

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	@echo ">>> [12/12]  linking   -> $(TARGET)"
	@$(CXX) $(CXXFLAGS) $(INC) $^ -o $@ $(LDFLAGS)

# ---------------------------------------------------------------------------
#  編譯規則：依 可編譯的最後依賴 排序
#    qammap.h        <- 無
#    functions.h     <- qammap.h
#    transmitter.h   <- qammap.h
#    convolution.h   <- transmitter.h (ComplexVec)
#    channel.h       <- transmitter.h + functions.h + convolution.h
#    receiver.h      <- transmitter.h + channel.h
#    Mod/OFDM.h      <- Base/*.h
#    Mod/OTFS.h      <- Base/*.h + functions.h
#    test_plot.cpp   <- 以上全部
#  -MMD -MP 產生 .d 依賴檔，header 變更時自動重編
# ---------------------------------------------------------------------------

$(OBJ_DIR)/lib/Mod/qammap.o: lib/Mod/qammap.cpp
	@mkdir -p $(dir $@)
	@echo ">>> [ 1/12]  compiling  $<"
	@$(CXX) $(CXXFLAGS) $(INC) -MMD -MP -c $< -o $@

$(OBJ_DIR)/lib/Utils/functions.o: lib/Utils/functions.cpp
	@mkdir -p $(dir $@)
	@echo ">>> [ 2/12]  compiling  $<"
	@$(CXX) $(CXXFLAGS) $(INC) -MMD -MP -c $< -o $@

$(OBJ_DIR)/lib/Base/transmitter.o: lib/Base/transmitter.cpp
	@mkdir -p $(dir $@)
	@echo ">>> [ 3/12]  compiling  $<"
	@$(CXX) $(CXXFLAGS) $(INC) -MMD -MP -c $< -o $@

$(OBJ_DIR)/lib/Utils/convolution.o: lib/Utils/convolution.cpp
	@mkdir -p $(dir $@)
	@echo ">>> [ 4/12]  compiling  $<"
	@$(CXX) $(CXXFLAGS) $(INC) -MMD -MP -c $< -o $@

$(OBJ_DIR)/lib/Base/channel.o: lib/Base/channel.cpp
	@mkdir -p $(dir $@)
	@echo ">>> [ 5/12]  compiling  $<"
	@$(CXX) $(CXXFLAGS) $(INC) -MMD -MP -c $< -o $@

$(OBJ_DIR)/lib/Base/receiver.o: lib/Base/receiver.cpp
	@mkdir -p $(dir $@)
	@echo ">>> [ 6/12]  compiling  $<"
	@$(CXX) $(CXXFLAGS) $(INC) -MMD -MP -c $< -o $@

$(OBJ_DIR)/lib/Mod/OFDM_Tx.o: lib/Mod/OFDM_Tx.cpp
	@mkdir -p $(dir $@)
	@echo ">>> [ 7/12]  compiling  $<"
	@$(CXX) $(CXXFLAGS) $(INC) -MMD -MP -c $< -o $@

$(OBJ_DIR)/lib/Mod/OFDM_Rx.o: lib/Mod/OFDM_Rx.cpp
	@mkdir -p $(dir $@)
	@echo ">>> [ 8/12]  compiling  $<"
	@$(CXX) $(CXXFLAGS) $(INC) -MMD -MP -c $< -o $@

$(OBJ_DIR)/lib/Mod/OTFS_Tx.o: lib/Mod/OTFS_Tx.cpp
	@mkdir -p $(dir $@)
	@echo ">>> [ 9/12]  compiling  $<"
	@$(CXX) $(CXXFLAGS) $(INC) -MMD -MP -c $< -o $@

$(OBJ_DIR)/lib/Mod/OTFS_Rx.o: lib/Mod/OTFS_Rx.cpp
	@mkdir -p $(dir $@)
	@echo ">>> [10/12]  compiling  $<"
	@$(CXX) $(CXXFLAGS) $(INC) -MMD -MP -c $< -o $@

$(OBJ_DIR)/lib/test_plot.o: lib/test_plot.cpp
	@mkdir -p $(dir $@)
	@echo ">>> [11/12]  compiling  $<"
	@$(CXX) $(CXXFLAGS) $(INC) -MMD -MP -c $< -o $@

# ---------------------------------------------------------------------------

.PHONY: run
run: all
	@echo ">>>  running  ./$(TARGET)"
	@./$(TARGET)

.PHONY: clean
clean:
	@echo ">>>  cleaning obj/ bin/"
	@rm -rf $(OBJ_DIR) $(BIN_DIR)

-include $(DEPS)
