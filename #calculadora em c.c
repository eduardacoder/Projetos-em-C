#calculadora em c

void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void exibirMenu() {
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

int lerOpcao() {
    int opcao;
    while (1) {
        printf("Opção: ");
        if (scanf("%d", &opcao) != 1) {
            printf("Error: Entrada inválida. Digite um número inteiro entre 1 e 5.\n");
            limparBuffer();
        } else if (opcao < 1 || opcao > 5) {
            printf("Error: Opção fora do intervalo. Escolha entre 1 e 5.\n");
            limparBuffer();
        } else {
            limparBuffer();
            return opcao;
        }
    }
}

double lerNumero(const char *mensagem) {
    double num;
    while (1) {
        printf("%s", mensagem);
        if (scanf("%lf", &num) != 1) {
            printf("Error: Entrada inválida. Por favor, digite um número válido.\n");
            limparBuffer();
        } else {
            limparBuffer();
            return num;
        }
    }
}

int main() {
    int opcao;
    char continuar;

    do {
        exibirMenu();
        opcao = lerOpcao();

        if (opcao == 5) {
            printf("Obrigado por usar a calculadora! Até a próxima.\n");
            break;
        }

        double num1 = lerNumero("Digite o primeiro número: ");
        double num2 = lerNumero("Digite o segundo número: ");

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

        printf("Deseja realizar outra operação? (s/n): ");
        scanf(" %c", &continuar);
        limparBuffer();

        if (continuar != 's' && continuar != 'S') {
            printf("Obrigado por usar a calculadora! Até a próxima.\n");
            break;
        }

    } while (1);

    return 0;
}