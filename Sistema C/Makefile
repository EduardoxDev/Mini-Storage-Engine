# Mini Storage Engine Makefile
# Professional build system with multiple targets

CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11 -pedantic
CFLAGS_DEBUG = -Wall -Wextra -g -O0 -std=c11 -DDEBUG -fsanitize=address
CFLAGS_RELEASE = -Wall -Wextra -O3 -std=c11 -DNDEBUG -march=native -flto
LDFLAGS = 
TARGET = minidb
SRC_DIR = src
OBJ_DIR = obj
TEST_DIR = tests

SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
HEADERS = $(wildcard $(SRC_DIR)/*.h)

# Colors for output
RED = \033[0;31m
GREEN = \033[0;32m
YELLOW = \033[0;33m
BLUE = \033[0;34m
NC = \033[0m # No Color

# Default target
all: $(TARGET)
	@echo "$(GREEN)✓ Build completo: ./$(TARGET)$(NC)"

# Standard build
$(TARGET): $(OBJECTS)
	@echo "$(BLUE)Linking $(TARGET)...$(NC)"
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS) | $(OBJ_DIR)
	@echo "$(BLUE)Compiling $<...$(NC)"
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

# Debug build with sanitizers
debug: CFLAGS = $(CFLAGS_DEBUG)
debug: clean $(TARGET)
	@echo "$(YELLOW)✓ Debug build completo com AddressSanitizer$(NC)"

# Release build with optimizations
release: CFLAGS = $(CFLAGS_RELEASE)
release: clean $(TARGET)
	@echo "$(GREEN)✓ Release build completo com otimizações$(NC)"
	@strip $(TARGET)
	@echo "$(GREEN)✓ Binary stripped$(NC)"

# Run the database
run: $(TARGET)
	@echo "$(BLUE)Iniciando $(TARGET)...$(NC)"
	./$(TARGET) test.db

# Quick test
test: $(TARGET)
	@echo "$(BLUE)Executando testes básicos...$(NC)"
	@echo "INSERT test1 value1" | ./$(TARGET) test_db.db > /dev/null
	@echo "SELECT test1" | ./$(TARGET) test_db.db | grep -q "value1" && echo "$(GREEN)✓ INSERT/SELECT funcionando$(NC)" || echo "$(RED)✗ Teste falhou$(NC)"
	@rm -f test_db.db test_db.db.wal

# Performance benchmark
benchmark: release
	@echo "$(BLUE)Executando benchmark...$(NC)"
	@echo "Inserindo 10000 registros..."
	@time (for i in $$(seq 1 10000); do echo "INSERT key$$i value$$i"; done | ./$(TARGET) bench.db > /dev/null)
	@rm -f bench.db bench.db.wal
	@echo "$(GREEN)✓ Benchmark completo$(NC)"

# Memory leak check (requires valgrind)
valgrind: debug
	@echo "$(BLUE)Verificando memory leaks com Valgrind...$(NC)"
	@echo "INSERT test1 value1\nSELECT test1\nEXIT" | valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET) valgrind_test.db
	@rm -f valgrind_test.db valgrind_test.db.wal

# Code coverage (requires gcov)
coverage: CFLAGS += -fprofile-arcs -ftest-coverage
coverage: LDFLAGS += -lgcov --coverage
coverage: clean $(TARGET)
	@echo "$(BLUE)Gerando relatório de cobertura...$(NC)"
	@echo "INSERT test1 value1\nSELECT test1\nEXIT" | ./$(TARGET) coverage_test.db
	@gcov $(SOURCES)
	@echo "$(GREEN)✓ Arquivos .gcov gerados$(NC)"
	@rm -f coverage_test.db coverage_test.db.wal

# Static analysis (requires cppcheck)
analyze:
	@echo "$(BLUE)Executando análise estática...$(NC)"
	@cppcheck --enable=all --suppress=missingIncludeSystem $(SRC_DIR)/*.c
	@echo "$(GREEN)✓ Análise completa$(NC)"

# Format code (requires clang-format)
format:
	@echo "$(BLUE)Formatando código...$(NC)"
	@clang-format -i $(SRC_DIR)/*.c $(SRC_DIR)/*.h
	@echo "$(GREEN)✓ Código formatado$(NC)"

# Generate documentation (requires doxygen)
docs:
	@echo "$(BLUE)Gerando documentação...$(NC)"
	@doxygen Doxyfile 2>/dev/null || echo "$(YELLOW)Doxygen não configurado$(NC)"

# Install to system (requires sudo)
install: release
	@echo "$(BLUE)Instalando $(TARGET)...$(NC)"
	@install -m 755 $(TARGET) /usr/local/bin/
	@echo "$(GREEN)✓ Instalado em /usr/local/bin/$(TARGET)$(NC)"

# Uninstall from system
uninstall:
	@echo "$(BLUE)Removendo $(TARGET)...$(NC)"
	@rm -f /usr/local/bin/$(TARGET)
	@echo "$(GREEN)✓ Desinstalado$(NC)"

# Clean build artifacts
clean:
	@echo "$(BLUE)Limpando arquivos de build...$(NC)"
	@rm -rf $(OBJ_DIR) $(TARGET) *.db *.wal *.gcov *.gcda *.gcno
	@echo "$(GREEN)✓ Limpeza completa$(NC)"

# Deep clean (including documentation)
distclean: clean
	@echo "$(BLUE)Limpeza profunda...$(NC)"
	@rm -rf docs/ html/ latex/
	@echo "$(GREEN)✓ Limpeza profunda completa$(NC)"

# Show help
help:
	@echo "$(BLUE)Mini Storage Engine - Targets disponíveis:$(NC)"
	@echo ""
	@echo "  $(GREEN)make$(NC)              - Build padrão (otimizado)"
	@echo "  $(GREEN)make debug$(NC)        - Build com símbolos de debug e sanitizers"
	@echo "  $(GREEN)make release$(NC)      - Build otimizado para produção"
	@echo "  $(GREEN)make run$(NC)          - Executa o banco de dados"
	@echo "  $(GREEN)make test$(NC)         - Executa testes básicos"
	@echo "  $(GREEN)make benchmark$(NC)    - Executa benchmark de performance"
	@echo "  $(GREEN)make valgrind$(NC)     - Verifica memory leaks"
	@echo "  $(GREEN)make coverage$(NC)     - Gera relatório de cobertura de código"
	@echo "  $(GREEN)make analyze$(NC)      - Análise estática com cppcheck"
	@echo "  $(GREEN)make format$(NC)       - Formata código com clang-format"
	@echo "  $(GREEN)make docs$(NC)         - Gera documentação"
	@echo "  $(GREEN)make install$(NC)      - Instala no sistema"
	@echo "  $(GREEN)make uninstall$(NC)    - Remove do sistema"
	@echo "  $(GREEN)make clean$(NC)        - Remove arquivos de build"
	@echo "  $(GREEN)make distclean$(NC)    - Limpeza completa"
	@echo "  $(GREEN)make help$(NC)         - Mostra esta mensagem"
	@echo ""

# Print build info
info:
	@echo "$(BLUE)Informações do Build:$(NC)"
	@echo "  Compiler: $(CC)"
	@echo "  CFLAGS: $(CFLAGS)"
	@echo "  Sources: $(SOURCES)"
	@echo "  Objects: $(OBJECTS)"
	@echo "  Target: $(TARGET)"

.PHONY: all debug release run test benchmark valgrind coverage analyze format docs install uninstall clean distclean help info
