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

typedef struct {
    int tamanho;
    char flag[1];
    int prox;
} Excluido;

void printArquivos(FILE *arq);

void printArquivosMenu();

void abreArquivos();

int contChar(char *ch);

int contRegStr(FILE *arq);

int contISBN(FILE *arq);

void remover();

void inserir();

void printLerRemove();

void compactar();

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
                compactar();
                break;
            case 4: // Print Arquivos
                printArquivosMenu();
                break;
        }
    }

    return 0;
}

char *getISBN(char *ch) {
    int i;
    char *reg = malloc(sizeof(char) * 14);
    for (i = 0; i < 13; i++) {
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

void printBiblioteca(FILE *arq) {
    int tam = 0, end, pos, pos2;
    long ftell(FILE *arq);
    char *ch = malloc(sizeof(char));

    Header header;
    fread(&header, sizeof(Header), 1, arq);
    printf("Header: \n");
    printf("espaco livre: %d  | arquivo inserir: %d  | arquivo remover: %d \n\n", header.pont, header.numInsere,
           header.numRemove);
    fseek(arq, sizeof(Header), 0);
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
        } else if (opcao == 1) {
            printArquivos(insere);
            break;
        } else if (opcao == 2) {
            printArquivos(remove);
            break;
        } else if (opcao == 3) {
            printBiblioteca(biblioteca);
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
    int cont = 0, i = 0;
    for (i = 0; i < strlen(ch); i++) {
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
    int inicio = 0, qtdRemove, qtdInsere = 0, total = 0, cont = 0, tam = 0;
    char *registro, isbn[14], flag = '*', vazio = '$';
    //char *registro, isbn[14], flag = '*', vazio = ' ';
    int ch = 0, n = 0, i = 0, j = 0, k = 0, quant = 0, headerAux = 0, headerRemove = 0, byte = 0;

    fseek(biblioteca, sizeof(int), SEEK_SET); // pega quantidade de reg. inseridos
    qtdInsere = fgetc(biblioteca);

    if (qtdInsere == 0) {
        printf("Arquivo biblioteca esta vazio...\n");
        return;
    }

    fseek(biblioteca, 2 * sizeof(int), SEEK_SET); // pega a quantidade de registros ja removidos (do header)
    inicio = fgetc(biblioteca);

    if (inicio >= contISBN(remove)) {
        printf("Todos os registros do arquivo ja foram removidos...\n");
        return;
    }

    qtdRemove = 0;
    while (1) {
        printf("Numero de registros para remover: ");
        scanf("%d", &qtdRemove); // num de registros a ser lido
        if ((inicio + qtdRemove) > contISBN(remove) || qtdRemove < 0)
            printf("Quantidade invalida\n");
        else if (qtdRemove == 0) {
            printf("Voltando...\n");
            return;
        } else
            break;
    }
	qtdRemove = 1;
	
    fseek(remove, (sizeof(livro.isbn) * inicio), SEEK_SET);
    while (cont < qtdRemove) {
        quant = 0;
        fread(&isbn, sizeof(livro.isbn), 1, remove);
        rewind(biblioteca);
        fseek(biblioteca, sizeof(Header), SEEK_SET);

        for (i = 0; i < (qtdInsere + qtdRemove); i++) {
            n = 0;
            byte = ftell(biblioteca);
            registro = malloc(sizeof(Livro));
            fread(&tam, sizeof(int), 1, biblioteca);

            for (j = 0; j < 3; j++) {
                while ((ch = fgetc(biblioteca)) != '#') {
                    if (ch == EOF) {
                        break;
                    }
                    registro[n++] = (char) ch;
                }
            }

            for (j = 0; j < 4; j++) {
                ch = fgetc(biblioteca);
                registro[n++] = (char) ch;
            }
            quant++;

            if (strcmp(getISBN(registro), isbn) == 0) {
                rewind(biblioteca);
                fread(&headerAux, sizeof(int), 1, biblioteca);
                rewind(biblioteca);
                fwrite(&byte, sizeof(int), 1, biblioteca);

                fseek(biblioteca, byte, SEEK_SET);
                fseek(biblioteca, sizeof(int), SEEK_CUR);
                fwrite(&flag, sizeof(char), 1, biblioteca);
                fwrite(&headerAux, sizeof(int), 1, biblioteca);

                for (k = 0; k < (tam - 5); ++k) {
                    fwrite(&vazio, sizeof(char), 1, biblioteca);
                }

                total = (inicio + qtdRemove); // total = num de registros lidos + num de registros inseridos antes
                fseek(biblioteca, 2 * sizeof(int), SEEK_SET);
                fwrite(&total, sizeof(int), 1, biblioteca);
                break;
            } else if (quant == (qtdInsere + qtdRemove) - 1) {
                printf("Nao achou...\n");
                break;
            }
            free(registro);
        }
        cont++;
    }

    fclose(remove);
    fclose(biblioteca);
}

void inserir() {
    abreArquivos();
    FILE *insere = fopen("insere.bin", "rb");
    FILE *biblioteca = fopen("biblioteca.bin", "r+b");
    Livro livro;
    int inicio = 0, qtd = 0, cont = 0, total = 0, tam = 0, headerAux = 0;

    fread(&headerAux, sizeof(int), 1, biblioteca);

    fseek(biblioteca, sizeof(int), 0);
    inicio = fgetc(biblioteca); // inicio = header.numInsere (segundo byte do arquivo)

    if (headerAux == -1) {
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
            } else
                break;
        }

        fseek(biblioteca, 0, SEEK_END);
        fseek(insere, inicio * sizeof(Livro), SEEK_SET);
        while (cont < qtd) {
            fread(&livro, sizeof(Livro), 1, insere);

            tam = (3 + contChar(livro.isbn) + contChar(livro.titulo) + contChar(livro.autor) +
                   contChar(livro.ano)); // 3 = "#"

            fwrite(&tam, sizeof(int), 1, biblioteca);
            fwrite(&livro.isbn, contChar(livro.isbn), 1, biblioteca);
            fwrite("#", sizeof(char), 1, biblioteca);
            fwrite(&livro.titulo, contChar(livro.titulo), 1, biblioteca);
            fwrite("#", sizeof(char), 1, biblioteca);
            fwrite(&livro.autor, contChar(livro.autor), 1, biblioteca);
            fwrite("#", sizeof(char), 1, biblioteca);
            fwrite(&livro.ano, contChar(livro.ano), 1, biblioteca);
            printf("%d#%s#%s#%s#%s \n", tam, livro.isbn, livro.titulo, livro.autor, livro.ano);

            cont++;
        }

        total = (inicio + qtd); // total = num de registros lidos + num de registros inseridos antes
        fseek(biblioteca, sizeof(int), 0);
        fwrite(&total, sizeof(int), 1, biblioteca);
    } else {
        fseek(biblioteca, headerAux, 0);
        Excluido excluido;
        fread(&excluido.tamanho, sizeof(int), 1, biblioteca);
        fread(&excluido.flag, sizeof(char), 1, biblioteca);
        fread(&excluido.prox, sizeof(int), 1, biblioteca);
        //printf("%d %s %d \n", excluido.tamanho, excluido.flag, excluido.prox);

        fseek(insere, inicio * sizeof(Livro), SEEK_SET);
        fread(&livro, sizeof(Livro), 1, insere);
        tam = (3 + contChar(livro.isbn) + contChar(livro.titulo) + contChar(livro.autor) + contChar(livro.ano));
        printf("%d#%s#%s#%s#%s \n", tam, livro.isbn, livro.titulo, livro.autor, livro.ano);

        if (tam <= excluido.tamanho) {
            fseek(biblioteca, headerAux, 0);
            fwrite(&tam, sizeof(int), 1, biblioteca);
	        fwrite(&livro.isbn, contChar(livro.isbn), 1, biblioteca);
	        fwrite("#", sizeof(char), 1, biblioteca);
	        fwrite(&livro.titulo, contChar(livro.titulo), 1, biblioteca);
	        fwrite("#", sizeof(char), 1, biblioteca);
	        fwrite(&livro.autor, contChar(livro.autor), 1, biblioteca);
	        fwrite("#", sizeof(char), 1, biblioteca);
	        fwrite(&livro.ano, contChar(livro.ano), 1, biblioteca);
	
	        rewind(biblioteca);
	        fwrite(&excluido.prox, sizeof(int), 1, biblioteca);
	        total = (inicio + qtd) + 1; // total = num de registros lidos + num de registros inseridos antes
	        fseek(biblioteca, sizeof(int), 0);
	        fwrite(&total, sizeof(int), 1, biblioteca);
        } else {
            fseek(biblioteca, 0, SEEK_END);
            fwrite(&tam, sizeof(int), 1, biblioteca);
	        fwrite(&livro.isbn, contChar(livro.isbn), 1, biblioteca);
	        fwrite("#", sizeof(char), 1, biblioteca);
	        fwrite(&livro.titulo, contChar(livro.titulo), 1, biblioteca);
	        fwrite("#", sizeof(char), 1, biblioteca);
	        fwrite(&livro.autor, contChar(livro.autor), 1, biblioteca);
	        fwrite("#", sizeof(char), 1, biblioteca);
	        fwrite(&livro.ano, contChar(livro.ano), 1, biblioteca);
	        
	        total = (inicio + qtd) + 1; // total = num de registros lidos + num de registros inseridos antes
	        fseek(biblioteca, sizeof(int), 0);
	        fwrite(&total, sizeof(int), 1, biblioteca);
        }
    }

    fclose(insere);
    fclose(biblioteca);
}

void compactar() {
    abreArquivos();
    FILE *temp = fopen("temp.bin", "w+b");
    FILE *biblioteca = fopen("biblioteca.bin", "r+b");
    int ch = 0, n = 0, i = 0, reset = -1, aux = 0;
    char vazio = '$';
	int booleano = 0;
	Header header;
	Excluido excluido;
	
	rewind(biblioteca);
	rewind(temp);
	
    do {
    	rewind(biblioteca);
    	fread(&header.pont,sizeof(Header), 1, biblioteca);
    	if(header.pont == -1){
    		break;   		
		}
    	fread(&header.numInsere,sizeof(Header), 1, biblioteca);
    	fread(&header.numRemove,sizeof(Header), 1, biblioteca);
    	//printf("pont: %d \n", header.pont);
		fseek(biblioteca, header.pont, 0);
		fread(&excluido.tamanho, sizeof(int), 1, biblioteca);
        fread(&excluido.flag, sizeof(char), 1, biblioteca);
        fread(&excluido.prox, sizeof(int), 1, biblioteca);
		//printf("prox: %d \n", excluido.prox);
		aux = excluido.prox;
		fseek(biblioteca, header.pont, 0);
		for(i = 0; i < 9; i++){
			fwrite(&vazio, sizeof(char), 1, biblioteca);			
		}
    	header.pont = aux;
    	
    	
    	rewind(biblioteca);
        fwrite(&excluido.prox, sizeof(int), 1, biblioteca);     
    	
    	//printf("%d pont:%d %d\n", excluido.prox, header.pont, aux);
    	//getch();
	} while (header.pont != -1);
	
	rewind(biblioteca);
	rewind(temp);
    while ((ch = fgetc(biblioteca)) != EOF) {
		if (ch != '$')
        	fprintf(temp, "%c", ch);	
    }
    
    fclose(biblioteca);
    remove("biblioteca.bin");
    abreArquivos();
    
    rewind(biblioteca);
    rewind(temp);

   FILE *biblioteca2 = fopen("biblioteca.bin", "r+b");
   
	while ((ch = fgetc(temp)) != EOF) {
		if (ch != '*')
        	fprintf(biblioteca2, "%c", ch);
    }
    
    rewind(biblioteca2);
    fwrite(&reset, sizeof(int), 1, biblioteca2);
    
    fclose(temp);
    fclose(biblioteca2);
    remove("temp.bin");
}
