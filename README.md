# MiniDB - Motor de Base de Datos Transaccional

Base de datos transaccional desarrollada en C que implementa conceptos fundamentales de sistemas operativos y bases de datos.

## Características

- Lenguaje propio de comandos
- CRUD completo (Agregar, Ver, Modificar, Borrar)
- Transacciones con ACID básico
- Múltiples bases de datos
- Interfaz web (servidor HTTP)
- Sistema de backup manual

## Requisitos

- GCC
- Make

## Compilación

```bash
make
```

## Ejecución

### Modo consola

```bash
./minidb
```

### Modo servidor web

```bash
./minidb --web
```

Luego abrir `http://localhost:8080` en el navegador.

## Comandos disponibles

### Gestión de bases de datos

| Comando | Descripción |
|---|---|
| `CREARBD <nombre>` | Crea una nueva base de datos |
| `MOSTRARBD` | Lista todas las bases de datos |
| `USAR <nombre>` | Cambia a otra base de datos |
| `ELIMINARBD <nombre>` | Elimina una base de datos |
| `INFOBD` | Información de la BD actual |

### Gestión de tablas

| Comando | Descripción |
|---|---|
| `LISTAR` | Lista todas las tablas de la BD actual |

### CRUD

| Comando | Descripción |
|---|---|
| `AGREGAR <tabla> <clave> <valor>` | Agrega un registro |
| `VER <tabla>` | Muestra todos los registros de una tabla |
| `MODIFICAR <tabla> <clave> <nuevo_valor>` | Modifica un registro |
| `BORRAR <tabla> <clave>` | Elimina un registro |

### Transacciones

| Comando | Descripción |
|---|---|
| `INICIAR` | Inicia transacción |
| `GUARDAR` | Confirma cambios (COMMIT) |
| `CANCELAR` | Revierte cambios (ROLLBACK) |

### Otros

| Comando | Descripción |
|---|---|
| `BACKUP` | Crea respaldo manual |
| `LIMPIAR` | Borra todos los datos de la BD actual |
| `AYUDA` | Muestra todos los comandos |
| `SALIR` | Cierra el programa |

## Ejemplos de uso

### Uso básico

```
./minidb
> crearbd empresa
> usar empresa
> agregar empleados ana 30000
> agregar empleados pepito 25000
> ver empleados
> guardar
```

### Transacción con rollback

```
> iniciar
> agregar ventas enero 5000
> agregar ventas febrero 7000
> cancelar    <-- los cambios se descartan
```

## Estructura del proyecto

```
├── src/            # Código fuente en C
├── data/           # Bases de datos (generado al ejecutar)
├── docs/           # Documentación
├── Makefile        # Compilación
└── README.md       # Este archivo
```
