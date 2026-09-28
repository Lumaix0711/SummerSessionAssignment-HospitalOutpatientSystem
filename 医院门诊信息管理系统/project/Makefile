#编译脚本
#编译方式:
#WSL2 交叉编译:make
#Windows MinGW:mingw32-make

#编译器自动检测环境
#WSL2中使用mingw-w64交叉编译器
#Windows中直接使用mingw32-gcc
CC := $(shell if command -v x86_64-w64-mingw32-gcc >/dev/null 2>&1; then \
    echo x86_64-w64-mingw32-gcc; \
  elif [ -f /mnt/c/mingw64/bin/x86_64-w64-mingw32-gcc.exe ]; then \
    echo /mnt/c/mingw64/bin/x86_64-w64-mingw32-gcc.exe; \
  else \
    echo gcc; \
  fi)

#编译选项
CFLAGS = -Wall -Wextra -finput-charset=UTF-8 -fexec-charset=GBK -static

#目标程序名
TARGET = hospital.exe

#源文件列表
SRCS = main.c utils.c password.c doctor_base.c doctor_duty.c \
       registration.c query.c statistics.c report.c

#头文件列表
HDRS = types.h utils.h password.h doctor_base.h doctor_duty.h \
       registration.h query.h statistics.h report.h

#目标文件.o列表
OBJS = $(SRCS:.c=.o)

#编译生成可执行文件
all: $(TARGET)

#链接所有.o文件生成可执行文件
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)
	@echo "————————————————————————————————————————————"
	@echo "  编译成功! 在Windows中双击$(TARGET)运行"
	@echo "————————————————————————————————————————————"

#编译规则:.c->.o
%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

#清理编译产物
clean:
	rm -f $(OBJS) $(TARGET)
	@echo "清理完成"

#运行程序WSL2中调用Windows可执行文件
run: $(TARGET)
	./$(TARGET)

#声明伪目标
.PHONY: all clean run
