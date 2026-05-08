#include "transaccion.h"
#include "gestor_arch.h"
#include "parser.h"
#include <string.h>
#include <stdlib.h>

BufferTransaccion txn_buffer = {0, 0, {}, 0, "", 0};

static BufferTransaccion* obtener_txn_actual() {
    return &txn_buffer;
}

void txn_iniciar() {
    BufferTransaccion *txn = obtener_txn_actual();
    
    if (txn->activa) {
        out("[WARN] Transaccion ya activa (#%d)\n", txn->numero);
        return;
    }
    
    const char *ruta = obtener_ruta_bd();
    FILE *fp = fopen(ruta, "r");
    txn->backup_size = 0;
    if (fp) {
        txn->backup_size = fread(txn->backup_contenido, 1, sizeof(txn->backup_contenido) - 1, fp);
        txn->backup_contenido[txn->backup_size] = '\0';
        fclose(fp);
    }
    
    txn->activa = 1;
    txn->numero++;
    txn->num_cambios = 0;
    
    out("[TXN] Transaccion #%d iniciada. Backup en memoria.\n", txn->numero);
}

void txn_agregar_cambio(char op, const char *tabla, const char *clave, int valor) {
    BufferTransaccion *txn = obtener_txn_actual();
    txn_agregar_cambio_sesion(txn, op, tabla, clave, valor);
}

void txn_agregar_cambio_sesion(BufferTransaccion *txn, char op, const char *tabla, const char *clave, int valor) {
    if (!txn->activa) {
        return;
    }
    
    if (txn->num_cambios >= MAX_BUFFER) {
        out("[ERROR] Buffer de transacciones lleno.\n");
        return;
    }
    
    Cambio *c = &txn->cambios[txn->num_cambios];
    c->operacion = op;
    strncpy(c->tabla, tabla, MAX_CAMPO - 1);
    strncpy(c->clave, clave, MAX_CAMPO - 1);
    c->valor = valor;
    txn->num_cambios++;
    
    out("[TXN] Cambio registrado en buffer: %c %s:%s\n", op, tabla, clave);
}

int txn_guardar() {
    BufferTransaccion *txn = obtener_txn_actual();
    
    if (!txn->activa) {
        out("[WARN] No hay transaccion activa para GUARDAR.\n");
        return 0;
    }
    
    int num = txn->num_cambios;
    txn->activa = 0;
    
    for (int i = 0; i < num; i++) {
        Cambio *c = &txn->cambios[i];
        char val_str[MAX_VALOR];
        snprintf(val_str, sizeof(val_str), "%d", c->valor);
        if (c->operacion == 'A') {
            escribir_registro(c->tabla, c->clave, val_str);
        } else if (c->operacion == 'M') {
            modificar_registro(c->tabla, c->clave, val_str);
        } else if (c->operacion == 'B') {
            borrar_registro(c->tabla, c->clave);
        }
    }
    
    txn->num_cambios = 0;
    txn->backup_size = 0;
    
    out("[TXN] Transaccion #%d guardada (COMMIT). %d cambios persistidos.\n", 
        txn->numero, num);
    txn->numero++;
    
    return 1;
}

int txn_cancelar() {
    BufferTransaccion *txn = obtener_txn_actual();
    
    if (!txn->activa) {
        out("[WARN] No hay transaccion activa para CANCELAR.\n");
        return 0;
    }
    
    const char *ruta = obtener_ruta_bd();
    FILE *fp = fopen(ruta, "w");
    if (fp && txn->backup_size > 0) {
        fwrite(txn->backup_contenido, 1, txn->backup_size, fp);
        fclose(fp);
        out("[TXN] Transaccion #%d cancelada (ROLLBACK). Restaurado backup.\n", txn->numero);
    } else {
        if (fp) fclose(fp);
        fp = fopen(ruta, "w");
        if (fp) fclose(fp);
        out("[TXN] Transaccion #%d cancelada (ROLLBACK). Archivo vaciado.\n", txn->numero);
    }
    
    txn->activa = 0;
    txn->num_cambios = 0;
    txn->backup_size = 0;
    txn->numero++;
    
    return 1;
}

int txn_activa() {
    BufferTransaccion *txn = obtener_txn_actual();
    return txn->activa;
}
