#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>

typedef struct {
    char isbn[14];
    char titulo[50];
    char autor[50];
    char ano[5];
} Livro;

typedef struct {
    int pont;
    int numInsere;
    int numRemove;
} Header;

void printArquivos(FILE *arq);
void printArquivosMenu();
void abreArquivos();
int contChar(char *ch);
int contRegStr(FILE *arq);
int contISBN(FILE *arq);
void remover();
void inserir();
void printLerRemove();

int main() {
    int opcao = -1;

    abreArquivos();

    while (1) {
        printf("---------Menu---------\n");
        printf("1 - Inserir\n");
        printf("2 - Remover\n");
        printf("3 - Compactar\n");
        printf("4 - Print Arquivos\n");
        printf("0 - Sair\n");
        printf("----------------------\n");
        scanf("%d", &opcao);

        switch (opcao) {
        case 0: // Sair
            printf("Saindo...\n");
            exit(0);
            break;
        case 1: // Inserir
            inserir();
            break;
        case 2: // Remover
            remover();
            break;
        case 3: // Compactar
            printf("falta fazer\n");
            break;
        case 4: // Print Arquivos
            printArquivosMenu();
            break;
        }
    }

    return 0;
}

char *getISBN(char *ch) {
    char *reg = malloc(sizeof(char)*14);
    for(int i = 0; i < 13; i++){
        reg[i] = ch[i];
    }
    reg[13] = '\0';

    return reg;
}

void printArquivos(FILE *arq) {
    char *ch = malloc(sizeof(char));

    while (fread(&ch, sizeof(char), 1, arq)) {
        printf("%c", ch);
    }
    printf("\n");
    return;
}

void printArquivosMenu() {
    FILE *insere = fopen("insere.bin", "rb");
    FILE *remove = fopen("remove.bin", "rb");
    FILE *biblioteca = fopen("biblioteca.bin", "rb");
    int opcao = 0;

    while (1) {
        printf("---------Print--------\n");
        printf("1 - Inserir\n");
        printf("2 - Remover\n");
        printf("3 - Biblioteca\n");
        printf("0 - Voltar\n");
        printf("----------------------\n");
        scanf("%d", &opcao);

        if (opcao == 0) {
            printf("Voltando para o menu...\n");
            break;
        }
        else if (opcao == 1) {
            printArquivos(insere);
            break;
        }
        else if (opcao == 2) {
            printArquivos(remove);
            break;
        }
        else if (opcao == 3) {
            printArquivos(biblioteca);
            break;
        }
        printf("Opcao invalida, tente novamente\n");
    }

    fclose(insere);
    fclose(remove);
    fclose(biblioteca);
}

void abreArquivos() {
    FILE *insere;
    FILE *remove;
    FILE *biblioteca;

    if ((insere = fopen("insere.bin", "rb")) == NULL) {
        printf("Nao foi possivel abrir arquivo insere\n");
        getch();
    }
    if ((remove = fopen("remove.bin", "rb")) == NULL) {
        printf("Nao foi possivel abrir arquivo remove\n");
        getch();
    }
    if ((biblioteca = fopen("biblioteca.bin", "r+b")) == NULL) {
        biblioteca = fopen("biblioteca.bin", "w+b");
        Header header;
        header.pont = -1;
        header.numInsere = 0;
        header.numRemove = 0;
        fwrite(&header, sizeof(Header), 1, biblioteca);
        printf("Arquivo biblioteca criado\n");
    }

    fclose(insere);
    fclose(remove);
    fclose(biblioteca);
}

int contChar(char *ch) {
    int cont = 0;
    for (int i = 0; i < strlen(ch); i++) {
        cont++;
    }
    return cont;
}

int contRegStr(FILE *arq) {
    Livro livro;
    int cont = 0;

    rewind(arq);
    while (fread(&livro, sizeof(Livro), 1, arq)) {
        cont++;
    }

    return cont;
}

int contISBN(FILE *arq) {
    Livro livro;
    int cont = 0;

    rewind(arq);
    while (fread(&livro, sizeof(livro.isbn), 1, arq)) {
        cont++;
    }

    return cont;
}

void remover() {
    FILE *remove = fopen("remove.bin", "rb");
    FILE *biblioteca = fopen("biblioteca.bin", "r+b");
    Livro livro;
    int inicio = 0, qtdRemove = 0, qtdInsere = 0, total = 0, cont = 0, tam = 0, byte = 0;
    char *registro, isbn[14];
    int ch = 0, n = 0;

    fseek(biblioteca, sizeof(int), SEEK_SET); // pega quantidade de reg. inseridos
    qtdInsere = fgetc(biblioteca);

    if (qtdInsere == 0) {
        printf("Arquivo biblioteca esta vazio...\n");
        return;
    }

    fseek(biblioteca, 2 * sizeof(int), SEEK_SET); // pega a quantidade de registros já removidos (do header)
    inicio = fgetc(biblioteca);

    if (inicio >= contISBN(remove)) {
        printf("Todos os registros do arquivo ja foram removidos...\n");
        return;
    }

    while (1) {
        printf("Numero de registros para remover: ");
        scanf("%d", &qtdRemove); // num de registros a ser lido
        if ((inicio + qtdRemove) > contISBN(remove) || qtdRemove <= 0)
            printf("Quantidade invalida\n");
        else break;
    }

    fseek(remove, (sizeof(livro.isbn)*inicio), SEEK_SET);
    while(cont < qtdRemove){
        fread(&isbn, sizeof(livro.isbn), 1, remove);
        printf("isbn: %s\n", isbn);
        rewind(biblioteca);
        fseek(biblioteca, sizeof(Header), SEEK_SET);

        for(int i = 0; i < (qtdInsere + qtdRemove); i++) {
            n = 0;
            registro = malloc(sizeof(Livro));
            fread(&tam, sizeof(int), 1, biblioteca);

            for(int j = 0; j < 3; j++){
                while((ch = fgetc(biblioteca)) != '#') {
                    registro[n++] = (char) ch;
                }
            }

            for(int j = 0; j < 4; j++){
                ch = fgetc(biblioteca);
                registro[n++] = (char) ch;
            }

            if(strcmp(getISBN(registro), isbn) == 0){
                printf("Achou!\n");
            }

            free(registro);
        }
        cont++;
    }

    total = (inicio + qtdRemove); // total = num de registros lidos + num de registros inseridos antes
    fseek(biblioteca, 2 * sizeof(int), SEEK_SET);
    fwrite(&total, sizeof(int), 1, biblioteca);

    fclose(remove);
    fclose(biblioteca);
}

void inserir() {
    abreArquivos();
    FILE *insere = fopen("insere.bin", "rb");
    FILE *biblioteca = fopen("biblioteca.bin", "r+b");
    Livro livro;
    int inicio = 0, qtd = 0, cont = 0, total = 0, tam = 0;

    fseek(biblioteca, sizeof(int), 0);
    inicio = fgetc(biblioteca); // inicio = header.numInsere (segundo byte do arquivo)

    if (inicio == contRegStr(insere)) {
        printf("Todos os registros ja foram lidos...\n");
        return;
    }

    while (1) {
        printf("Numero de registros para inserir: ");
        scanf("%d", &qtd); // num de registros a ser lido
        if ((inicio + qtd) > contRegStr(insere) || qtd < 0)
            printf("Quantidade invalida\n");
        else if (qtd == 0) {
            printf("Voltando...\n");
            return;
        }
        else break;
    }

    fseek(biblioteca, 0, SEEK_END);
    fseek(insere, inicio * sizeof(Livro), SEEK_SET);
    while (cont < qtd) {
        fread(&livro, sizeof(Livro), 1, insere);

        tam = (3 + contChar(livro.isbn) + contChar(livro.titulo) + contChar(livro.autor) + contChar(livro.ano)); // 3 = "#"

        fwrite(&tam, sizeof(int), 1, biblioteca);
        fwrite(&livro.isbn, contChar(livro.isbn), 1, biblioteca);
        fwrite("#", sizeof(char), 1, biblioteca);
        fwrite(&livro.titulo, contChar(livro.titulo), 1, biblioteca);
        fwrite("#", sizeof(char), 1, biblioteca);
        fwrite(&livro.autor, contChar(livro.autor), 1, biblioteca);
        fwrite("#", sizeof(char), 1, biblioteca);
        fwrite(&livro.ano, contChar(livro.ano), 1, biblioteca);

        cont++;
    }

    total = (inicio + qtd); // total = num de registros lidos + num de registros inseridos antes
    fseek(biblioteca, sizeof(int), 0);
    fwrite(&total, sizeof(int), 1, biblioteca);

    fclose(insere);
    fclose(biblioteca);
}

