#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------- struct ---------- */
typedef struct {
    char nome[80];
    char cpf[12];
    int idade;
    char diagnostico[120];
    float peso;
} Paciente;

long tamanho(FILE *fp);
void cadastrar(FILE *fp);
void consultar(FILE *fp);
void limpaBuffer(void);
void ler_string(const char *prompt, char *dest, size_t size);

/* ---------- MAIN ---------- */
int main(void) {
    FILE *fp = fopen("pacientes.bin", "rb+");
    if (fp == NULL) {
        fp = fopen("pacientes.bin", "wb+");
        if (fp == NULL) {
            printf("Erro ao abrir ou criar arquivo.\n");
            return 1;
        }
    }

    int opcao;

    do {
        printf("\n==== Clinica Medica ====\n");
        printf("1 - Cadastrar paciente\n");
        printf("2 - Consultar paciente por indice\n");
        printf("3 - Mostrar quantidade de registros\n");
        printf("0 - Sair\n");
        printf("Escolha: ");

        if (scanf("%d", &opcao) != 1) {
            printf("Entrada invalida.\n");
            limpaBuffer();
            continue;
        }

        limpaBuffer();

        switch (opcao) {
            case 1:
                system("cls || clear");
                cadastrar(fp);
                break;
            case 2:
                system("cls || clear");
                consultar(fp);
                break;
            case 3:
                system("cls || clear");
                printf("Total de registros: %ld\n", tamanho(fp));
                break;
            case 0:
                system("cls || clear");
                printf("Encerrando...\n");
                break;
            default:
                printf("Opcao invalida.\n");
        }

    } while (opcao != 0);

    fclose(fp);
    return 0;
}

/* ---------- FUNCOES ---------- */

/* Retorna quantidade de registros no arquivo */
long tamanho(FILE *fp) {
    if (fp == NULL) return 0;

    long posAtual = ftell(fp);
    fseek(fp, 0L, SEEK_END);
    long bytes = ftell(fp);
    fseek(fp, posAtual, SEEK_SET);

    return bytes / sizeof(Paciente);
}

/* Cadastra e grava struct no final do arquivo */
void cadastrar(FILE *fp) {
    Paciente p;

    ler_string("Nome: ", p.nome, sizeof(p.nome));
    ler_string("CPF: ", p.cpf, sizeof(p.cpf));

    printf("Idade: ");
    while (scanf("%d", &p.idade) != 1) {
        printf("Digite um valor valido para idade: ");
        limpaBuffer();
    }
    limpaBuffer();

    ler_string("Diagnostico: ", p.diagnostico, sizeof(p.diagnostico));

    printf("Peso (ex: 70.5): ");
    while (scanf("%f", &p.peso) != 1) {
        printf("Digite um valor valido para peso: ");
        limpaBuffer();
    }
    limpaBuffer();

    fseek(fp, 0L, SEEK_END);

    if (fwrite(&p, sizeof(Paciente), 1, fp) == 1) {
        printf("Paciente cadastrado com sucesso.\n");
    } else {
        printf("Erro ao gravar registro.\n");
    }

    fflush(fp);
}

/* Le um registro por indice e exibe os dados (com indice iniciando em 1) */
void consultar(FILE *fp) {
    long total = tamanho(fp);

    if (total == 0) {
        printf("Nenhum registro encontrado.\n");
        return;
    }

    long idx;
    printf("Informe o indice (1 ate %ld): ", total);

    while (scanf("%ld", &idx) != 1) {
        printf("Digite um indice valido: ");
        limpaBuffer();
    }
    limpaBuffer();

    if (idx < 1 || idx > total) {
        printf("Indice fora do intervalo.\n");
        return;
    }

    idx -= 1;

    Paciente p;

    fseek(fp, idx * sizeof(Paciente), SEEK_SET);

    if (fread(&p, sizeof(Paciente), 1, fp) != 1) {
        printf("Erro ao ler registro.\n");
        return;
    }

    printf("\n--- Paciente %ld ---\n", idx + 1);
    printf("Nome: %s\n", p.nome);
    printf("CPF: %s\n", p.cpf);
    printf("Idade: %d\n", p.idade);
    printf("Diagnostico: %s\n", p.diagnostico);
    printf("Peso: %.2f\n", p.peso);
}

/* Consome caracteres pendentes do buffer */
void limpaBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

/* Leitura segura de string: fgets + remocao do '\n' */
void ler_string(const char *prompt, char *dest, size_t size) {
    printf("%s", prompt);

    if (fgets(dest, size, stdin) == NULL) {
        dest[0] = '\0';
        return;
    }

    dest[strcspn(dest, "\n")] = '\0';
}
