# MiniDB - Simulación ACID

## Atomicidad (Atomicity)
- Las transacciones se ejecutan como una unidad atómica
- INICIAR: Se crea backup del archivo actual en memoria
- GUARDAR: Se escriben todos los cambios al disco
- CANCELAR: Se restaura el backup, descartando todos los cambios

## Consistencia (Consistency)
- Validación de datos antes de agregar/modificar
- Verificación de registros existentes para evitar duplicados
- El sistema siempre mantiene un estado válido

## Aislamiento (Isolation)
- Variable global `txn_buffer.activa` controla acceso
- Solo una transacción puede estar activa a la vez
- Los cambios dentro de una transacción no son visibles hasta el COMMIT

## Durabilidad (Durability)
- Los datos solo se escriben en disco con GUARDAR (COMMIT)
- Archivo `tienda.db` persiste localmente
- Una vez completado el COMMIT, los datos sobreviven al reinicio

---

## Transacciones: INICIAR / GUARDAR / CANCELAR

```
INICIAR         -> Inicia transacción, guarda backup
AGREGAR ...     -> Registra en buffer (no toca disco)
MODIFICAR ...   -> Registra en buffer  
BORRAR ...      -> Registra en buffer
GUARDAR         -> COMMIT: escribe todo al disco
CANCELAR        -> ROLLBACK: restaura backup
```

## Ejemplo práctico

```bash
# Sin transacción - se guarda inmediatamente
AGREGAR usuarios Juan 20

# Con transacción
INICIAR
AGREGAR usuarios Maria 25
MODIFICAR usuarios Juan 30
GUARDAR    # Se escriben ambos cambios

# Cancelar
INICIAR
AGREGAR productos lapiz 10
CANCELAR   # Se descarta el cambio
```