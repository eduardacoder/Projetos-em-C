#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int lerLinha(char *entrada, size_t tamanho) {
    if (fgets(entrada, tamanho, stdin) == NULL) {
        return 0;
    }

    size_t comprimento = strlen(entrada);
    if (comprimento > 0 && entrada[comprimento - 1] == '\n') {
        return 1;
    }

    if (!feof(stdin)) {
        int caractere;
        while ((caractere = getchar()) != '\n' && caractere != EOF) {
        }
        return -1;
    }

    return 1;
}

static void exibirMenu(void) {
    printf("===============================\n");
    printf("   Calculadora Simples\n");
    printf("===============================\n");
    printf("Selecione uma operação:\n");
    printf("1. Adição\n");
    printf("2. Subtração\n");
    printf("3. Multiplicação\n");
    printf("4. Divisão\n");
    printf("5. Sair\n");
}

static int lerOpcao(int *opcao) {
    char entrada[128];

    while (1) {
        char *fim;
        long valor;

        printf("Opção: ");
        fflush(stdout);
        int estado = lerLinha(entrada, sizeof entrada);
        if (estado == 0) {
            return 0;
        }
        if (estado < 0) {
            printf("Erro: Entrada inválida. Digite um número inteiro entre 1 e 5.\n");
            continue;
        }

        errno = 0;
        valor = strtol(entrada, &fim, 10);
        while (isspace((unsigned char)*fim)) {
            fim++;
        }

        if (fim == entrada || *fim != '\0' || errno == ERANGE) {
            printf("Erro: Entrada inválida. Digite um número inteiro entre 1 e 5.\n");
        } else if (valor < 1 || valor > 5) {
            printf("Erro: Opção fora do intervalo. Escolha entre 1 e 5.\n");
        } else {
            *opcao = (int)valor;
            return 1;
        }
    }
}

static int lerNumero(const char *mensagem, double *numero) {
    char entrada[128];

    while (1) {
        char *fim;

        printf("%s", mensagem);
        fflush(stdout);
        int estado = lerLinha(entrada, sizeof entrada);
        if (estado == 0) {
            return 0;
        }
        if (estado < 0) {
            printf("Erro: Entrada inválida. Por favor, digite um número válido.\n");
            continue;
        }

        errno = 0;
        *numero = strtod(entrada, &fim);
        while (isspace((unsigned char)*fim)) {
            fim++;
        }

        if (fim == entrada || *fim != '\0' || errno == ERANGE) {
            printf("Erro: Entrada inválida. Por favor, digite um número válido.\n");
        } else {
            return 1;
        }
    }
}

static int lerContinuacao(int *continuar) {
    char entrada[128];

    while (1) {
        printf("Deseja realizar outra operação? (s/n): ");
        fflush(stdout);
        int estado = lerLinha(entrada, sizeof entrada);
        if (estado == 0) {
            return 0;
        }
        if (estado < 0) {
            printf("Erro: Responda com 's' para sim ou 'n' para não.\n");
            continue;
        }

        char *inicio = entrada;
        while (isspace((unsigned char)*inicio)) {
            inicio++;
        }

        char resposta = *inicio++;
        while (isspace((unsigned char)*inicio)) {
            inicio++;
        }

        if (*inicio != '\0' || (resposta != 's' && resposta != 'S'
                && resposta != 'n' && resposta != 'N')) {
            printf("Erro: Responda com 's' para sim ou 'n' para não.\n");
        } else {
            *continuar = resposta == 's' || resposta == 'S';
            return 1;
        }
    }
}

int main(void) {
    int opcao;
    int continuar = 1;
    double num1;
    double num2;

    while (continuar) {
        exibirMenu();
        if (!lerOpcao(&opcao)) {
            break;
        }

        if (opcao == 5) {
            printf("Obrigado por usar a calculadora! Até a próxima.\n");
            break;
        }

        if (!lerNumero("Digite o primeiro número: ", &num1)
                || !lerNumero("Digite o segundo número: ", &num2)) {
            break;
        }

        switch (opcao) {
            case 1:
                printf("Resultado: %.6g + %.6g = %.6g\n", num1, num2, num1 + num2);
                break;
            case 2:
                printf("Resultado: %.6g - %.6g = %.6g\n", num1, num2, num1 - num2);
                break;
            case 3:
                printf("Resultado: %.6g * %.6g = %.6g\n", num1, num2, num1 * num2);
                break;
            case 4:
                if (num2 == 0) {
                    printf("Erro: Divisão por zero não é permitida.\n");
                } else {
                    printf("Resultado: %.6g / %.6g = %.6g\n", num1, num2, num1 / num2);
                }
                break;
        }

        if (!lerContinuacao(&continuar)) {
            break;
        }
        if (!continuar) {
            printf("Obrigado por usar a calculadora! Até a próxima.\n");
            break;
        }
    }

    return 0;
}
