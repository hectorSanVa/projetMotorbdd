# MiniDB - Checklist del Proyecto

## Estado: EN PROGRESO

---

## ✓ Funcionalidades Implementadas

- [x] Lenguaje propio de comandos
- [x] CRUD completo (agregar, ver, modificar, borrar)
- [x] Transacciones ACID (iniciar, guardar, cancelar)
- [x] Múltiples bases de datos
- [x] Listar tablas
- [x] Info de bases de datos
- [x] Servidor HTTP web
- [x] Sistema de backup manual
- [x] Limpieza de datos
- [x] Manejo de procesos (fork/señales)
- [x] Delimitador `;` para múltiples comandos
- [x] Diccionario de comandos (alias: INSERT, SELECT, UPDATE, DELETE, etc.)
- [x] Diseño web mejorado (estilo oscuro, diccionario visible)
- [x] Limpieza de códigos ANSI para web
- [x] Soporte para valores tipo string/texto
- [x] Comando CREARTABLA para crear tablas

---

## Pendientes / Mejoras

### Media Prioridad

- [ ] Índice de tablas para búsquedas rápidas
- [ ] Exportar/importar datos (JSON/CSV)
- [ ] Historial de comandos
- [ ] Autocomplete en consola

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
- [x] Servidor web para pruebas
- [x] Diccionario de palabras propias

---

## Cómo usar

### Terminal
```bash
make run
# Comandos:
AGREGAR usuarios Juan "Hola mundo"
VER usuarios
MODIFICAR usuarios Juan "Nuevo texto"
BORRAR usuarios Juan
INICIAR; GUARDAR; CANCELAR
CREARTABLA nueva_tabla
```

### Web
```bash
make web
# Abrir: http://localhost:8080
```

---

## Notas

- BD por defecto: `tienda`
- Puerto HTTP: 8080
- Delimitador `;` permite múltiples comandos: `crearbd prueba; usar prueba; agregar datos x 1`
- Soporte para texto con comillas: `AGREGAR tabla clave "texto con espacios"`