CFLAGS = -std=gnu99 -Wall -Wextra -Werror
TARGET = sheet

all: $(TARGET)

$(TARGET): sheet.c
    gcc $(CFLAGS) $^ -o $@

clean:
    rm -f $(TARGET)
