# Windows向けMakefile
# コンパイラ
CC		:= GCC
# build対象コード
SRC		= src/WorkingHoursCalculator.c
# build生成物(実行ファイル)
TARGET	= WorkingHoursCalculator.exe

.PHONY:all

all:clean $(TARGET)

clean:
	del $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET)
