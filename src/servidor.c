#include "servidor.h"
#include "parser.h"
#include "gestor_arch.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <ctype.h>
#include <string.h>

#define PUERTO_HTTP 8080
#define BUFFER_HTTP 4096

static char buffer_salida_http[BUFFER_HTTP * 2];

void generar_html_respuesta(char *buffer, const char *resultado);

int iniciar_servidor_http(int puerto) {
    int server_fd, cliente_fd;
    struct sockaddr_in direccion;
    int opt = 1;
    int addrlen = sizeof(direccion);
    
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket error");
        return -1;
    }
    
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    direccion.sin_family = AF_INET;
    direccion.sin_addr.s_addr = INADDR_ANY;
    direccion.sin_port = htons(puerto);
    
    if (bind(server_fd, (struct sockaddr *)&direccion, sizeof(direccion)) < 0) {
        perror("bind error");
        return -1;
    }
    
    if (listen(server_fd, 5) < 0) {
        perror("listen error");
        return -1;
    }
    
    printf("===========================================\n");
    printf("  MiniDB Transaccional - Servidor Web\n");
    printf("===========================================\n");
    printf("  Abre en tu navegador:\n");
    printf("  http://localhost:%d\n", puerto);
    printf("===========================================\n\n");
    
    while (1) {
        cliente_fd = accept(server_fd, (struct sockaddr *)&direccion, (socklen_t*)&addrlen);
        if (cliente_fd < 0) {
            continue;
        }
        
        pthread_t hilo;
        int *fd = malloc(sizeof(int));
        *fd = cliente_fd;
        pthread_create(&hilo, NULL, manejar_cliente, fd);
        pthread_detach(hilo);
    }
    
    close(server_fd);
    return 0;
}

void *manejar_cliente(void *arg) {
    int cliente_fd = *(int*)arg;
    free(arg);
    char buffer[BUFFER_HTTP];
    char respuesta[BUFFER_HTTP * 4];
    char resultado[BUFFER_HTTP * 2];
    
    memset(buffer, 0, BUFFER_HTTP);
    memset(resultado, 0, BUFFER_HTTP * 2);
    read(cliente_fd, buffer, BUFFER_HTTP - 1);
    
    buffer_salida_http[0] = '\0';
    salida_set_buffer(buffer_salida_http, sizeof(buffer_salida_http));
    
    char *query_start = strstr(buffer, "GET /?cmd=");
    if (!query_start) query_start = strstr(buffer, "POST /?cmd=");
    if (!query_start) query_start = strstr(buffer, "cmd=");
    
    if (query_start) {
        char *cmd_start = strstr(query_start, "cmd=");
        if (cmd_start) {
            cmd_start += 4;
            char cmd[512] = {0};
            
            int i = 0;
            while (*cmd_start && *cmd_start != ' ' && *cmd_start != '&' && i < 511) {
                cmd[i++] = *cmd_start++;
            }
            cmd[i] = '\0';
            
            char decoded[512] = {0};
            int j = 0;
            for (int i = 0; cmd[i] && j < 511; i++) {
                if (cmd[i] == '+') {
                    decoded[j++] = ' ';
                } else if (cmd[i] == '%' && cmd[i+1] == '2' && cmd[i+2] == '0') {
                    decoded[j++] = ' ';
                    i += 2;
                } else if (cmd[i] == '%' && cmd[i+1] == '3' && cmd[i+2] == 'B') {
                    decoded[j++] = ';';
                    i += 2;
                } else {
                    decoded[j++] = cmd[i];
                }
            }
            decoded[j] = '\0';
            strcpy(cmd, decoded);
            
            procesar_comando(cmd);
            strcpy(resultado, buffer_salida_http);
        }
    }
    
    if (strlen(resultado) == 0) {
        strcpy(resultado, "Ejecuta un comando...");
    }
    
    generar_html_respuesta(respuesta, resultado);
    
    char http_response[BUFFER_HTTP * 4];
    snprintf(http_response, sizeof(http_response),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=UTF-8\r\n"
        "Content-Length: %lu\r\n"
        "Connection: close\r\n"
        "\r\n%s",
        strlen(respuesta), respuesta);
    
    write(cliente_fd, http_response, strlen(http_response));
    close(cliente_fd);
    
    return NULL;
}

void generar_html_respuesta(char *buffer, const char *resultado) {
    static char ult_cmd[256] = {0};
    
    if (resultado && strlen(resultado) > 0) {
        strncpy(ult_cmd, resultado, 255);
        ult_cmd[255] = '\0';
    }
    
    char resultado_escaped[BUFFER_HTTP * 2];
    if (resultado) {
        strncpy(resultado_escaped, resultado, BUFFER_HTTP * 2 - 1);
        resultado_escaped[BUFFER_HTTP * 2 - 1] = '\0';
    } else {
        resultado_escaped[0] = '\0';
    }
    
    char temp[BUFFER_HTTP * 2];
    int j = 0;
    for (int i = 0; resultado_escaped[i] && j < BUFFER_HTTP * 2 - 2; i++) {
        if (resultado_escaped[i] == '\e' || resultado_escaped[i] == 27) {
            int k = i + 1;
            if (k < BUFFER_HTTP * 2 && resultado_escaped[k] == '[') {
                while (k < BUFFER_HTTP * 2 && resultado_escaped[k] && resultado_escaped[k] != 'm') {
                    k++;
                }
                i = k;
                continue;
            }
        }
        if (resultado_escaped[i] == '\n') {
            temp[j++] = '<';
        } else {
            temp[j++] = resultado_escaped[i];
        }
    }
    temp[j] = '\0';
    strcpy(resultado_escaped, temp);
    
    snprintf(buffer, BUFFER_HTTP * 4,
        "<!DOCTYPE html>"
        "<html><head>"
        "<meta charset='UTF-8'>"
        "<meta name='viewport' content='width=device-width, initial-scale=1'>"
        "<title>MiniDB - Motor de Base de Datos</title>"
        "<style>"
        "* { box-sizing: border-box; margin: 0; padding: 0; }"
        "body { "
        "  font-family: 'Segoe UI', system-ui, sans-serif; "
        "  background: #0f0f1a; "
        "  min-height: 100vh; "
        "  color: #e4e4e7; "
        "}"
        ".container { max-width: 900px; margin: 0 auto; padding: 40px 20px; }"
        "h1 { "
        "  font-size: 2.2em; "
        "  text-align: center; "
        "  margin-bottom: 10px; "
        "  color: #fff; "
        "  letter-spacing: -1px; "
        "}"
        ".subtitle { "
        "  text-align: center; "
        "  color: #71717a; "
        "  margin-bottom: 35px; "
        "  font-size: 0.95em; "
        "}"
        "form { "
        "  display: flex; "
        "  gap: 12px; "
        "  margin-bottom: 30px; "
        "}"
        "input[type=text] { "
        "  flex: 1; "
        "  padding: 14px 18px; "
        "  font-size: 15px; "
        "  background: #1e1e2e; "
        "  color: #fff; "
        "  border: 1px solid #3f3f46; "
        "  border-radius: 8px; "
        "  outline: none; "
        "}"
        "input[type=text]:focus { border-color: #6366f1; }"
        "input[type=submit] { "
        "  padding: 14px 28px; "
        "  font-size: 15px; "
        "  font-weight: 600; "
        "  background: #6366f1; "
        "  color: #fff; "
        "  border: none; "
        "  border-radius: 8px; "
        "  cursor: pointer; "
        "}"
        "input[type=submit]:hover { background: #4f46e5; }"
        ".section { margin-bottom: 25px; }"
        ".section-title { "
        "  font-size: 0.75em; "
        "  text-transform: uppercase; "
        "  letter-spacing: 1.5px; "
        "  color: #a1a1aa; "
        "  margin-bottom: 12px; "
        "  padding-bottom: 8px; "
        "  border-bottom: 1px solid #27272a; "
        "}"
        ".dict-grid { display: grid; grid-template-columns: repeat(auto-fill, minmax(280px, 1fr)); gap: 10px; }"
        ".dict-item { "
        "  background: #18181b; "
        "  padding: 12px 15px; "
        "  border-radius: 8px; "
        "  border: 1px solid #27272a; "
        "}"
        ".dict-cmd { color: #a78bfa; font-weight: 600; font-size: 0.9em; }"
        ".dict-desc { color: #71717a; font-size: 0.8em; margin-top: 4px; }"
        ".dict-alias { color: #4ade80; font-size: 0.75em; margin-top: 6px; }"
        ".result { "
        "  background: #18181b; "
        "  padding: 20px; "
        "  border-radius: 10px; "
        "  border: 1px solid #3f3f46; "
        "  color: #4ade80; "
        "  font-family: 'Consolas', monospace; "
        "  font-size: 0.9em; "
        "  white-space: pre-wrap; "
        "  min-height: 80px; "
        "  line-height: 1.6; "
        "}"
        ".footer { "
        "  text-align: center; "
        "  margin-top: 40px; "
        "  color: #52525b; "
        "  font-size: 0.8em; "
        "}"
        "</style></head><body>"
        "<div class='container'>"
        "<h1>MiniDB</h1>"
        "<p class='subtitle'>Motor de Base de Datos Transaccional - Proyecto Zacarias</p>"
        "<form method='GET' action='/'>"
        "<input type='text' name='cmd' placeholder='Ej: AGREGAR usuarios Juan 20'>"
        "<input type='submit' value='Ejecutar'>"
        "</form>"
        "<div class='section'>"
        "<div class='section-title'>Diccionario de Comandos</div>"
        "<div class='dict-grid'>"
        "<div class='dict-item'><div class='dict-cmd'>CREARBD</div><div class='dict-desc'>Crear base de datos</div><div class='dict-alias'>creardb, newdb</div></div>"
        "<div class='dict-item'><div class='dict-cmd'>USAR</div><div class='dict-desc'>Cambiar base de datos</div><div class='dict-alias'>usedb, use</div></div>"
        "<div class='dict-item'><div class='dict-cmd'>MOSTRARBD</div><div class='dict-desc'>Listar bases de datos</div><div class='dict-alias'>showdb, listadb</div></div>"
        "<div class='dict-item'><div class='dict-cmd'>ELIMINARBD</div><div class='dict-desc'>Eliminar base de datos</div><div class='dict-alias'>dropdb, deletdb</div></div>"
        "<div class='dict-item'><div class='dict-cmd'>AGREGAR</div><div class='dict-desc'>Agregar registro</div><div class='dict-alias'>insert, add</div></div>"
        "<div class='dict-item'><div class='dict-cmd'>VER</div><div class='dict-desc'>Ver registros</div><div class='dict-alias'>select, list, read</div></div>"
        "<div class='dict-item'><div class='dict-cmd'>MODIFICAR</div><div class='dict-desc'>Modificar registro</div><div class='dict-alias'>update, modify</div></div>"
        "<div class='dict-item'><div class='dict-cmd'>BORRAR</div><div class='dict-desc'>Eliminar registro</div><div class='dict-alias'>delete, remove</div></div>"
        "<div class='dict-item'><div class='dict-cmd'>INICIAR</div><div class='dict-desc'>Iniciar transaccion</div><div class='dict-alias'>begin, start, txn</div></div>"
        "<div class='dict-item'><div class='dict-cmd'>GUARDAR</div><div class='dict-desc'>Confirmar transaccion</div><div class='dict-alias'>commit, save</div></div>"
        "<div class='dict-item'><div class='dict-cmd'>CANCELAR</div><div class='dict-desc'>Revertir transaccion</div><div class='dict-alias'>rollback, abort</div></div>"
        "<div class='dict-item'><div class='dict-cmd'>LISTAR</div><div class='dict-desc'>Ver tablas</div><div class='dict-alias'>tables, list</div></div>"
        "<div class='dict-item'><div class='dict-cmd'>INFOBD</div><div class='dict-desc'>Info base de datos</div><div class='dict-alias'>status, info</div></div>"
        "<div class='dict-item'><div class='dict-cmd'>BACKUP</div><div class='dict-desc'>Crear backup</div><div class='dict-alias'>dump, export</div></div>"
        "<div class='dict-item'><div class='dict-cmd'>AYUDA</div><div class='dict-desc'>Mostrar ayuda</div><div class='dict-alias'>help, ?</div></div>"
        "</div></div>"
        "<div class='section'>"
        "<div class='section-title'>Resultado</div>"
        "<div class='result'>%s</div>"
        "</div>"
        "<div class='footer'>"
        "Usa ; para multiples comandos. Ej: crearbd prueba; usar prueba; agregar datos x 1"
        "</div>"
        "</div></body></html>",
        resultado_escaped);
}