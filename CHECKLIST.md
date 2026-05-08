# MiniDB - Checklist del Proyecto

## Estado: COMPLETADO ✅

---

## ✓ Funcionalidades Implementadas

### Gestión de Bases de Datos
- [x] Crear bases de datos (`CREARBD`)
- [x] Eliminar bases de datos (`ELIMINARBD`)
- [x] Cambiar entre BDs (`USAR`)
- [x] Listar todas las BDs (`MOSTRARBD`)
- [x] Info de BD actual (`INFOBD`)

### Gestión de Tablas
- [x] Crear tablas (`CREARTABLA`)
- [x] Listar tablas (`LISTAR`)
- [x] Múltiples tablas por BD

### CRUD Completo
- [x] Agregar registros (`AGREGAR`)
- [x] Ver registros (`VER`)
- [x] Modificar registros (`MODIFICAR`)
- [x] Borrar registros (`BORRAR`)
- [x] Limpieza de datos (`LIMPIAR`)

### Transacciones ACID
- [x] Iniciar transacción (`INICIAR`)
- [x] Confirmar cambios (`GUARDAR` / COMMIT)
- [x] Cancelar cambios (`CANCELAR` / ROLLBACK)

### Sistema de Índices (Solo Terminal C)
- [x] Hash index (256 buckets con chaining)
- [x] Archivo `.idx` por base de datos
- [x] Reconstrucción automática al cambiar BD
- [x] Actualización en tiempo real (INSERT/DELETE)

### Interfaz Web (JavaScript)
- [x] Motor MiniDB completo en JavaScript
- [x] Interfaz tipo terminal (fondo blanco, texto negro)
- [x] Input siempre visible arriba
- [x] Prompt con BD actual: `MiniDB[bd]$`
- [x] Historial de comandos
- [x] Colores para mensajes ([OK], [ERROR], [TXN])
- [x] Despliegue en Vercel/Netlify

### Utilidades
- [x] Sistema de backup manual (`BACKUP`) - Solo Terminal
- [x] Manejo de procesos (`fork()` y señales) - Solo Terminal
- [x] Señales SIGINT/SIGTERM - Solo Terminal
- [x] Delimitador `;` para múltiples comandos
- [x] Diccionario de comandos (alias en español e inglés)

### Diccionario Completo (Igual en Terminal y Web)
| Propio | Alias SQL/EN | Función |
|---|---|---|
| CREARBD | creardb, newdb | Crear base de datos |
| USAR | usedb | Cambiar base de datos |
| MOSTRARBD | showdb, listadb | Listar BDs |
| ELIMINARBD | dropdb, deletedb | Eliminar BD |
| CREARTABLA | newtable | Crear tabla |
| LISTAR | tables | Ver tablas |
| AGREGAR | insert, add | Agregar registro |
| VER | select, read | Ver registros |
| MODIFICAR | update, modify | Modificar registro |
| BORRAR | delete, remove | Borrar registro |
| INICIAR | begin, start, txn | Iniciar transacción |
| GUARDAR | commit, save | Confirmar transacción |
| CANCELAR | rollback, abort | Cancelar transacción |
| BACKUP | dump, export | Crear backup (Terminal) |
| LIMPIAR | clear, reset | Limpiar BD |
| AYUDA | help, ? | Mostrar ayuda |
| SALIR | - | Salir del programa |

---

## Pendientes / Mejoras

### Media Prioridad
- [ ] Exportar/importar datos (JSON/CSV)
- [ ] Persistencia en web (localStorage)
- [ ] Autocomplete en terminal

### Baja Prioridad
- [ ] Logs del sistema
- [ ] Compresión de archivos de BD
- [ ] Límite de tamaño por base de datos
- [ ] Modo verbose/debug
- [ ] Tests unitarios

---

## Requisitos del Profesor

- [x] Motor propio de base de datos
- [x] Transacciones ACID
- [x] Propias instrucciones (no SQL estándar)
- [x] Realizado en C
- [x] Manejo de interrupciones y procesos
- [x] Interfaz web funcional
- [x] Diccionario de palabras propias

---

## Estructura del Proyecto

```
ProjectBdd/
├── src/
│   ├── definiciones.h    # Tipos, constantes, estructuras
│   ├── main.c            # Entrada principal (CLI)
│   ├── parser.c/h        # Parser de comandos
│   ├── gestor_arch.c/h   # CRUD en archivos, tablas, registros
│   ├── transaccion.c/h   # ACID: BEGIN/COMMIT/ROLLBACK
│   ├── indice.c/h        # Hash index (256 buckets)
│   └── procesos.c/h      # fork(), señales SIGINT/SIGTERM, backup
├── data/                 # Archivos .db y .idx (solo Terminal)
├── docs/
│   └── ACID.md           # Documentación ACID
├── index.html            # Interfaz web (JavaScript)
├── vercel.json           # Configuración para Vercel/Netlify
├── Makefile              # Compilación y targets
├── README.md             # Manual de usuario
├── INSTRUCCIONES.txt      # Manual completo
└── CHECKLIST.md          # Este archivo
```

---

## Cómo usar

### Terminal (C)
```bash
./minidb
# o
make run

# Comandos:
CREARBD empresa
USAR empresa
CREARTABLA empleados
AGREGAR empleados pepito 50000
MODIFICAR empleados pepito 55000
BORRAR empleados pepito
INICIAR; GUARDAR; CANCELAR
LISTAR
MOSTRARBD
INFOBD
BACKUP
SALIR
```

### Web (JavaScript - Navegador)
```bash
# Abrir directamente en navegador
xdg-open index.html

# O con servidor local
python3 -m http.server 8080
# Luego abrir: http://localhost:8080
```

### Despliegue en Vercel/Netlify
```bash
# Subir a GitHub y conectar en Vercel/Netlify
# El archivo index.html se servirá automáticamente
```

### Delimitador
```bash
# Múltiples comandos en una línea
crearbd prueba; usar prueba; agregar datos x 1; ver datos
```

---

## Diferencias: Terminal vs Web

| Característica | Terminal (C) | Web (JS) |
|---|---|---|
| Persistencia | Archivos .db | Memoria (se pierde al recargar) |
| Índices | Hash index256 buckets | No aplica |
| Señales | SIGINT/SIGTERM | No aplica |
| Backup | Archivo físico | No aplica |
| forks() | Sí | No aplica |

**Funcionalidad de comandos: IDÉNTICA en ambos.**

---

## Notas Técnicas

- BD por defecto: `tienda`
- Terminal: usa archivos en `data/*.db`
- Web: datos en memoria del navegador
- Formato de datos (Terminal): `tabla:clave:i:valor` (entero) o `tabla:clave:t:valor` (texto)
- Líneas `#` son metadatos (marcas de tabla creadas por CREARTABLA)
- Índice (Terminal): `data/<bd>.idx`
- Compilación: `gcc -Wall -Wextra -g -pthread`
- Sin librerías externas (solo libc y pthread)