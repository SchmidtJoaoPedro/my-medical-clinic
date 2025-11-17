#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define CLEAR "cls"
#else
#endif

typedef struct {
    char nome[80];
    char cpf[20];
    int idade;
    char diagnostico[120];
    float peso;
} Paciente;

long tamanho(FILE *fp);
void cadastrar(FILE *fp);
void consultar(FILE *fp);
void listar(FILE *fp);
void editar(FILE *fp);
void excluir(FILE *fp);
int validarCPF(const char *cpf);
void ordenarPorNome(FILE *fp);
void limpaBuffer(void);
void ler_string(const char *prompt, char *dest, size_t size);

/* MAIN */
int main(void) {
    FILE *fp = fopen("pacientes.bin", "rb+");
    int opcao;

    if (fp == NULL) {
        fp = fopen("pacientes.bin", "wb+");
        if (fp == NULL) {
            printf("Erro ao abrir/criar arquivo.\n");
            return 1;
        }
    }

    do {
        printf("\n==== Clinica Medica ====\n");
        printf("1 - Cadastrar paciente\n");
        printf("2 - Consultar paciente\n");
        printf("3 - Mostrar quantidade de registros\n");
        printf("4 - Listar pacientes\n");
        printf("5 - Editar paciente\n");
        printf("6 - Excluir paciente\n");
        printf("7 - Ordenar por nome\n");
        printf("0 - Sair\n");
        printf("Escolha: ");

        if (scanf("%d", &opcao) != 1) {
            printf("Entrada invalida.\n");
            limpaBuffer();
            continue;
        }

        limpaBuffer();
        system(CLEAR);

        switch (opcao) {
            case 1: cadastrar(fp); break;
            case 2: consultar(fp); break;
            case 3: printf("Total de registros: %ld\n", tamanho(fp)); break;
            case 4: listar(fp); break;
            case 5: editar(fp); break;
            case 6: excluir(fp); break;
            case 7: ordenarPorNome(fp); break;
            case 0: printf("Encerrando...\n"); break;
            default: printf("Opcao invalida.\n");
        }

    } while (opcao != 0);

    fclose(fp);
    return 0;
}

/* quantidade de registros */
long tamanho(FILE *fp) {
    long posAtual, bytes;
    posAtual = ftell(fp);
    fseek(fp, 0L, SEEK_END);
    bytes = ftell(fp);
    fseek(fp, posAtual, SEEK_SET);
    return bytes / sizeof(Paciente);
}

/* cadastro */
void cadastrar(FILE *fp) {
    Paciente p;

    ler_string("Nome: ", p.nome, sizeof(p.nome));

    do {
        ler_string("CPF (11 digitos): ", p.cpf, sizeof(p.cpf));
        if (!validarCPF(p.cpf))
            printf("CPF invalido.\n");
    } while (!validarCPF(p.cpf));

    printf("Idade: ");
    while (scanf("%d", &p.idade) != 1) {
        printf("Digite idade valida: ");
        limpaBuffer();
    }
    limpaBuffer();

    ler_string("Diagnostico: ", p.diagnostico, sizeof(p.diagnostico));

    printf("Peso: ");
    while (scanf("%f", &p.peso) != 1) {
        printf("Digite peso valido: ");
        limpaBuffer();
    }
    limpaBuffer();

    fseek(fp, 0L, SEEK_END);

    if (fwrite(&p, sizeof(Paciente), 1, fp) == 1)
        printf("Paciente cadastrado.\n");
    else
        printf("Erro ao gravar.\n");

    fflush(fp);
}

/* lista os pacientes */
void listar(FILE *fp) {
    long total = tamanho(fp);
    Paciente p;
    long i;

    if (total == 0) {
        printf("Nenhum registro.\n");
        return;
    }

    printf("\n--- Lista de Pacientes ---\n");

    fseek(fp, 0L, SEEK_SET);
    for (i = 0; i < total; i++) {
        fread(&p, sizeof(Paciente), 1, fp);
        printf("%ld - %s (CPF: %s)\n", i + 1, p.nome, p.cpf);
    }
}

/* consulta */
void consultar(FILE *fp) {
    long total = tamanho(fp);
    long idx;
    Paciente p;

    if (total == 0) {
        printf("Nenhum registro.\n");
        return;
    }

    listar(fp);

    printf("Escolha o indice: ");
    while (scanf("%ld", &idx) != 1) {
        printf("Indice invalido: ");
        limpaBuffer();
    }
    limpaBuffer();

    if (idx < 1 || idx > total) {
        printf("Indice fora do intervalo.\n");
        return;
    }

    fseek(fp, (idx - 1) * sizeof(Paciente), SEEK_SET);
    fread(&p, sizeof(Paciente), 1, fp);

    printf("\nNome: %s\n", p.nome);
    printf("CPF: %s\n", p.cpf);
    printf("Idade: %d\n", p.idade);
    printf("Diagnostico: %s\n", p.diagnostico);
    printf("Peso: %.2f\n", p.peso);
}

/* editar */
void editar(FILE *fp) {
    long total = tamanho(fp);
    long idx;
    Paciente p;

    if (total == 0) {
        printf("Nenhum registro.\n");
        return;
    }

    listar(fp);

    printf("Indice para editar: ");
    while (scanf("%ld", &idx) != 1) {
        printf("Valor invalido: ");
        limpaBuffer();
    }
    limpaBuffer();

    if (idx < 1 || idx > total) {
        printf("Indice invalido.\n");
        return;
    }

    fseek(fp, (idx - 1) * sizeof(Paciente), SEEK_SET);
    fread(&p, sizeof(Paciente), 1, fp);

    ler_string("Novo nome: ", p.nome, sizeof(p.nome));

    do {
        ler_string("Novo CPF: ", p.cpf, sizeof(p.cpf));
        if (!validarCPF(p.cpf))
            printf("CPF invalido.\n");
    } while (!validarCPF(p.cpf));

    printf("Nova idade: ");
    while (scanf("%d", &p.idade) != 1) {
        printf("Valor invalido: ");
        limpaBuffer();
    }
    limpaBuffer();

    ler_string("Novo diagnostico: ", p.diagnostico, sizeof(p.diagnostico));

    printf("Novo peso: ");
    while (scanf("%f", &p.peso) != 1) {
        printf("Valor invalido: ");
        limpaBuffer();
    }
    limpaBuffer();

    fseek(fp, (idx - 1) * sizeof(Paciente), SEEK_SET);
    fwrite(&p, sizeof(Paciente), 1, fp);
    fflush(fp);

    printf("Registro atualizado.\n");
}

/* excluir paciente */
void excluir(FILE *fp) {
    long total = tamanho(fp);
    long idx;
    long i;
    Paciente *vet;

    if (total == 0) {
        printf("Nenhum registro.\n");
        return;
    }

    listar(fp);

    printf("Indice para excluir: ");
    while (scanf("%ld", &idx) != 1) {
        printf("Valor invalido: ");
        limpaBuffer();
    }
    limpaBuffer();

    if (idx < 1 || idx > total) {
        printf("Indice invalido.\n");
        return;
    }

    vet = (Paciente*) malloc(total * sizeof(Paciente));
    if (vet == NULL) {
        printf("Erro de memoria.\n");
        return;
    }

    fseek(fp, 0L, SEEK_SET);
    fread(vet, sizeof(Paciente), total, fp);

    for (i = idx - 1; i < total - 1; i++)
        vet[i] = vet[i + 1];

    fp = freopen("pacientes.bin", "wb+", fp);
    fwrite(vet, sizeof(Paciente), total - 1, fp);
    fflush(fp);

    free(vet);

    printf("Registro excluido.\n");
}

/* ordenar por nome */
void ordenarPorNome(FILE *fp) {
    long total = tamanho(fp);
    long i, j;
    Paciente *vet;
    Paciente temp;

    if (total < 2) {
        printf("Poucos registros para ordenar.\n");
        return;
    }

    vet = (Paciente*) malloc(total * sizeof(Paciente));
    if (vet == NULL) {
        printf("Erro de memoria.\n");
        return;
    }

    fseek(fp, 0L, SEEK_SET);
    fread(vet, sizeof(Paciente), total, fp);

    for (i = 0; i < total - 1; i++) {
        for (j = i + 1; j < total; j++) {
            if (strcmp(vet[i].nome, vet[j].nome) > 0) {
                temp = vet[i];
                vet[i] = vet[j];
                vet[j] = temp;
            }
        }
    }

    fp = freopen("pacientes.bin", "wb+", fp);
    fwrite(vet, sizeof(Paciente), total, fp);
    fflush(fp);

    free(vet);

    printf("Registros ordenados por nome.\n");
}

/* CPF simples */
int validarCPF(const char *cpf) {
    int i;
    if (strlen(cpf) != 11) return 0;
    for (i = 0; i < 11; i++)
        if (cpf[i] < '0' || cpf[i] > '9')
            return 0;
    return 1;
}

/* buffer */
void limpaBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

/* fgets sem \n */
void ler_string(const char *prompt, char *dest, size_t size) {
    printf("%s", prompt);
    if (fgets(dest, size, stdin) == NULL) {
        dest[0] = '\0';
        return;
    }
    dest[strcspn(dest, "\n")] = '\0';
}
