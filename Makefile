CC = gcc
CFLAGS = -Wall -Wextra -g -pthread
LDFLAGS = -pthread

TARGET = minidb
SRC = src/main.c src/parser.c src/gestor_arch.c src/servidor.c src/transaccion.c src/procesos.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

web: $(TARGET)
	./$(TARGET) --web

test: $(TARGET)
	@echo "=== Probando CLI ==="
	@echo "AYUDA" | ./$(TARGET)
	@echo "VER usuarios" | ./$(TARGET)
	@echo "SALIR" | ./$(TARGET)

help:
	@echo "Uso: make run    - Modo terminal"
	@echo "     make web    - Modo servidor web (http://localhost:8080)"