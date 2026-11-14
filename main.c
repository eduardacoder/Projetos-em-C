#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INVENTORY_FILE "inventory.txt"
#define TEMP_FILE "inventory.txt.tmp"
#define NAME_SIZE 256

typedef struct {
    char name[NAME_SIZE];
    long quantity;
} Item;

typedef struct {
    Item *items;
    size_t count;
} Inventory;

static int readLine(FILE *input, char *line, size_t size) {
    if (fgets(line, (int)size, input) == NULL) {
        return 0;
    }

    size_t length = strlen(line);
    if (length > 0 && line[length - 1] == '\n') {
        line[--length] = '\0';
    } else if (!feof(input)) {
        int character = fgetc(input);
        if (character != '\n' && character != EOF) {
            while (character != '\n' && character != EOF) {
                character = fgetc(input);
            }
            return -1;
        }
    }

    if (length > 0 && line[length - 1] == '\r') {
        line[length - 1] = '\0';
    }
    return 1;
}

static void trim(char *text) {
    char *start = text;
    while (isspace((unsigned char)*start)) {
        start++;
    }

    if (start != text) {
        memmove(text, start, strlen(start) + 1);
    }

    size_t length = strlen(text);
    while (length > 0 && isspace((unsigned char)text[length - 1])) {
        text[--length] = '\0';
    }
}

static int parsePositiveInteger(const char *text, long *value) {
    char *end;
    errno = 0;
    long parsed = strtol(text, &end, 10);
    while (isspace((unsigned char)*end)) {
        end++;
    }

    if (text == end || *end != '\0' || errno == ERANGE || parsed <= 0) {
        return 0;
    }
    *value = parsed;
    return 1;
}

static void showMenu(void) {
    printf("==================================\n");
    printf("   Controle de Estoque\n");
    printf("==================================\n");
    printf("Selecione uma opção:\n");
    printf("1. Adicionar Item\n");
    printf("2. Remover Item\n");
    printf("3. Listar Estoque\n");
    printf("4. Sair\n");
}

static int readOption(int *option) {
    char line[NAME_SIZE];
    while (1) {
        char *end;
        printf("Opção: ");
        fflush(stdout);

        int status = readLine(stdin, line, sizeof line);
        if (status == 0) {
            return 0;
        }

        errno = 0;
        long parsed = strtol(line, &end, 10);
        while (isspace((unsigned char)*end)) {
            end++;
        }
        if (status < 0 || line == end || *end != '\0'
                || errno == ERANGE || parsed < 1 || parsed > 4) {
            printf("Opção inválida! Tente novamente.\n");
        } else {
            *option = (int)parsed;
            return 1;
        }
    }
}

static int readPositiveQuantity(const char *prompt, long *quantity) {
    char line[NAME_SIZE];
    while (1) {
        printf("%s", prompt);
        fflush(stdout);
        int status = readLine(stdin, line, sizeof line);
        if (status == 0) {
            return 0;
        }
        if (status < 0 || !parsePositiveInteger(line, quantity)) {
            printf("Quantidade inválida. Digite um inteiro positivo.\n");
            continue;
        }
        return 1;
    }
}

static int loadInventory(Inventory *inventory) {
    FILE *file = fopen(INVENTORY_FILE, "r");
    if (file == NULL && errno == ENOENT) {
        file = fopen(INVENTORY_FILE, "w");
        if (file != NULL) {
            if (fclose(file) != 0) {
                perror("Não foi possível inicializar o arquivo de estoque");
                return 0;
            }
            return 1;
        }
    }
    if (file == NULL) {
        perror("Não foi possível abrir o arquivo de estoque");
        return 0;
    }

    char name[NAME_SIZE];
    char quantityLine[NAME_SIZE];
    int status;
    while ((status = readLine(file, name, sizeof name)) == 1) {
        trim(name);
        if (name[0] == '\0') {
            continue;
        }

        status = readLine(file, quantityLine, sizeof quantityLine);
        long quantity;
        if (status != 1 || !parsePositiveInteger(quantityLine, &quantity)) {
            fprintf(stderr, "Erro: formato inválido no arquivo %s.\n", INVENTORY_FILE);
            fclose(file);
            return 0;
        }

        Item *items = realloc(inventory->items,
                (inventory->count + 1) * sizeof *inventory->items);
        if (items == NULL) {
            perror("Não foi possível alocar memória para o estoque");
            fclose(file);
            return 0;
        }
        inventory->items = items;
        strcpy(inventory->items[inventory->count].name, name);
        inventory->items[inventory->count].quantity = quantity;
        inventory->count++;
    }

    if (status < 0 || ferror(file)) {
        fprintf(stderr, "Erro ao ler o arquivo %s.\n", INVENTORY_FILE);
        fclose(file);
        return 0;
    }
    if (fclose(file) != 0) {
        perror("Erro ao fechar o arquivo de estoque");
        return 0;
    }
    return 1;
}

static int saveInventory(const Inventory *inventory) {
    FILE *file = fopen(TEMP_FILE, "w");
    if (file == NULL) {
        perror("Não foi possível salvar o estoque");
        return 0;
    }

    int success = 1;
    for (size_t i = 0; i < inventory->count; i++) {
        if (fprintf(file, "%s\n%ld\n", inventory->items[i].name,
                    inventory->items[i].quantity) < 0) {
            success = 0;
            break;
        }
    }
    if (fclose(file) != 0) {
        success = 0;
    }
    if (!success) {
        perror("Erro ao gravar o arquivo de estoque");
        remove(TEMP_FILE);
        return 0;
    }
    if (rename(TEMP_FILE, INVENTORY_FILE) != 0) {
        perror("Não foi possível atualizar o arquivo de estoque");
        remove(TEMP_FILE);
        return 0;
    }
    return 1;
}

static int readItemName(char *name) {
    printf("Digite o nome do item: ");
    fflush(stdout);
    int status = readLine(stdin, name, NAME_SIZE);
    if (status <= 0) {
        return 0;
    }
    trim(name);
    if (name[0] == '\0') {
        printf("O nome do item não pode ficar vazio.\n");
        return -1;
    }
    return 1;
}

static void addItem(Inventory *inventory) {
    char name[NAME_SIZE];
    long quantity;
    int nameStatus = readItemName(name);
    if (nameStatus == 0) {
        return;
    }
    if (nameStatus < 0) {
        return;
    }
    if (!readPositiveQuantity("Digite a quantidade: ", &quantity)) {
        return;
    }

    for (size_t i = 0; i < inventory->count; i++) {
        if (strcmp(inventory->items[i].name, name) == 0) {
            if (inventory->items[i].quantity > LONG_MAX - quantity) {
                printf("Erro: quantidade total excede o limite permitido.\n");
                return;
            }
            long previousQuantity = inventory->items[i].quantity;
            inventory->items[i].quantity += quantity;
            if (saveInventory(inventory)) {
                printf("Item adicionado com sucesso!\n");
            } else {
                inventory->items[i].quantity = previousQuantity;
            }
            return;
        }
    }

    Item *items = realloc(inventory->items,
            (inventory->count + 1) * sizeof *inventory->items);
    if (items == NULL) {
        perror("Não foi possível adicionar o item");
        return;
    }
    inventory->items = items;
    strcpy(inventory->items[inventory->count].name, name);
    inventory->items[inventory->count].quantity = quantity;
    inventory->count++;

    if (saveInventory(inventory)) {
        printf("Item adicionado com sucesso!\n");
    } else {
        inventory->count--;
    }
}

static void removeItem(Inventory *inventory) {
    char name[NAME_SIZE];
    long quantity;
    int nameStatus = readItemName(name);
    if (nameStatus == 0) {
        return;
    }
    if (nameStatus < 0) {
        return;
    }
    if (!readPositiveQuantity("Digite a quantidade a ser removida: ", &quantity)) {
        return;
    }

    size_t index = 0;
    while (index < inventory->count
            && strcmp(inventory->items[index].name, name) != 0) {
        index++;
    }
    if (index == inventory->count) {
        printf("Item não encontrado.\n");
        return;
    }

    long available = inventory->items[index].quantity;
    if (quantity > available) {
        printf("Estoque insuficiente. Quantidade disponível: %ld\n", available);
        return;
    }

    Item removed = inventory->items[index];
    if (quantity == available) {
        memmove(&inventory->items[index], &inventory->items[index + 1],
                (inventory->count - index - 1) * sizeof *inventory->items);
        inventory->count--;
        if (saveInventory(inventory)) {
            printf("Item removido do estoque!\n");
        } else {
            memmove(&inventory->items[index + 1], &inventory->items[index],
                    (inventory->count - index) * sizeof *inventory->items);
            inventory->items[index] = removed;
            inventory->count++;
        }
        return;
    }

    inventory->items[index].quantity -= quantity;
    if (saveInventory(inventory)) {
        printf("Quantidade atualizada com sucesso!\n");
    } else {
        inventory->items[index].quantity = available;
    }
}

static void listInventory(const Inventory *inventory) {
    printf("=============================\n");
    printf("        Estoque Atual\n");
    printf("=============================\n");
    if (inventory->count == 0) {
        printf("O estoque está vazio.\n");
        return;
    }

    for (size_t i = 0; i < inventory->count; i++) {
        printf("Nome: %s\n", inventory->items[i].name);
        printf("Quantidade: %ld\n\n", inventory->items[i].quantity);
    }
}

static int shouldContinue(void) {
    char answer[NAME_SIZE];
    printf("Deseja realizar outra operação? (s/n): ");
    fflush(stdout);
    if (readLine(stdin, answer, sizeof answer) != 1) {
        return 0;
    }
    trim(answer);
    return answer[0] == 's' || answer[0] == 'S';
}

int main(void) {
    Inventory inventory = {NULL, 0};
    int option;
    int running = 1;

    if (!loadInventory(&inventory)) {
        free(inventory.items);
        return 1;
    }

    while (running) {
        showMenu();
        if (!readOption(&option)) {
            break;
        }

        switch (option) {
            case 1:
                addItem(&inventory);
                break;
            case 2:
                removeItem(&inventory);
                break;
            case 3:
                listInventory(&inventory);
                break;
            case 4:
                running = 0;
                break;
        }

        if (option == 4 || !running) {
            break;
        }
        running = shouldContinue();
    }

    printf("Obrigado por usar o Controle de Estoques! Até a próxima.\n");
    free(inventory.items);
    return 0;
}
