CC = gcc
CFLAGS = -Wall -Wextra -g -pthread
LDFLAGS = -pthread

TARGET = minidb
SRC = src/main.c src/parser.c src/gestor_arch.c src/transaccion.c src/procesos.c src/indice.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

test: $(TARGET)
	@echo "=== Probando CLI ==="
	@echo "AYUDA" | ./$(TARGET)
	@echo "VER usuarios" | ./$(TARGET)
	@echo "SALIR" | ./$(TARGET)

help:
	@echo "Uso: make run    - Modo terminal (CLI)"