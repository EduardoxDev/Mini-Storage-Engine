#include "storage.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_banner() {
    printf("╔════════════════════════════════════════╗\n");
    printf("║     Mini Storage Engine v1.0           ║\n");
    printf("║  B-Tree + WAL + Cache LRU + Paging     ║\n");
    printf("╚════════════════════════════════════════╝\n\n");
}

void print_help() {
    printf("Comandos disponíveis:\n");
    printf("  INSERT <key> <value>  - Insere um registro\n");
    printf("  SELECT <key>          - Busca um registro\n");
    printf("  UPDATE <key> <value>  - Atualiza um registro\n");
    printf("  DELETE <key>          - Remove um registro\n");
    printf("  SCAN                  - Lista todos os registros\n");
    printf("  EXIT                  - Sai do programa\n\n");
}

int main(int argc, char* argv[]) {
    const char* db_file = "database.db";
    
    if (argc > 1) {
        db_file = argv[1];
    }
    
    print_banner();
    printf("Abrindo banco de dados: %s\n\n", db_file);
    
    StorageEngine* engine = storage_open(db_file);
    if (!engine) {
        fprintf(stderr, "Erro ao abrir banco de dados\n");
        return 1;
    }
    
    print_help();
    
    char input[2048];
    while (1) {
        printf("minidb> ");
        fflush(stdout);
        
        if (!fgets(input, sizeof(input), stdin)) {
            break;
        }
        
        Command cmd = parse_command(input);
        
        switch (cmd.type) {
            case CMD_INSERT: {
                if (storage_insert(engine, cmd.key, cmd.value)) {
                    printf("✓ Registro inserido: %s\n", cmd.key);
                } else {
                    printf("✗ Erro: chave já existe\n");
                }
                break;
            }
            
            case CMD_SELECT: {
                char value[1024];
                if (storage_select(engine, cmd.key, value)) {
                    printf("✓ Valor: %s\n", value);
                } else {
                    printf("✗ Registro não encontrado\n");
                }
                break;
            }
            
            case CMD_UPDATE: {
                if (storage_update(engine, cmd.key, cmd.value)) {
                    printf("✓ Registro atualizado: %s\n", cmd.key);
                } else {
                    printf("✗ Registro não encontrado\n");
                }
                break;
            }
            
            case CMD_DELETE: {
                if (storage_delete(engine, cmd.key)) {
                    printf("✓ Registro removido: %s\n", cmd.key);
                } else {
                    printf("✗ Registro não encontrado\n");
                }
                break;
            }
            
            case CMD_SCAN: {
                printf("Listando registros:\n");
                storage_scan(engine);
                break;
            }
            
            case CMD_EXIT: {
                printf("Fechando banco de dados...\n");
                storage_close(engine);
                return 0;
            }
            
            default: {
                printf("✗ Comando desconhecido. Digite HELP para ajuda.\n");
                break;
            }
        }
        
        printf("\n");
    }
    
    storage_close(engine);
    return 0;
}
