TARGET = todo

all:
	gcc main.c -o $(TARGET)
	./$(TARGET)