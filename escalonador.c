//Carlos Chagas Oliveira Filho
//Caroline Bernat Rebelo

#include <stdio.h>
#include <stdlib.h>

// #define AUTO

#define LIMITE 5
#define TIMESLICE 3
#define TRUE 1
#define FALSE 0
#define TIPOS char *tiposIO[] = {"Disco", "Fita", "Impressora", "Sem IO"}
#define ESTADOS char *estados[] = {"Nao Iniciado", "Pronto", "Em Execucao", "Em IO", "Finalizado"}
#define TIPOSFILA char *tiposFila[] = {"Baixa Prioridade", "Alta Prioridade"}

// Tempos de cada IO

#define ioIMPRESSORA 9
#define ioFITA 6
#define ioDISCO 7

// Contador de processos em IO

int countIO;

// Cores no console

#define RESET           "\x1b[0m"
#define RED             "\x1b[31m"
#define GREEN           "\x1b[32m"
#define YELLOW          "\x1b[33m"
#define BLUE            "\x1b[34m"
#define MAGENTA         "\x1b[35m"
#define CYAN            "\x1b[36m"
#define LIGHT_GREEN     "\x1b[92m"
#define LIGHT_BLUE      "\x1b[94m"
#define LIGHT_MAGENTA   "\x1b[95m"

// Estruturas

typedef struct PROCESS {
    int pID;
    int tempoChegada;
    int duracaoProcesso;
    int tempoExecutado;
    int inicioIO;
    int duracaoIO;
    int tipoIO;
    int tempoExecutadoIO;
    int estado;     // 0 = Não Iniciado, 1 = Pronto, 2 = Em execução, 3 = Em IO, 4 = Finalizado
    struct PROCESS *prox;
} PROCESSO;

typedef struct QUEUE {
    int qtdProcessos;
    PROCESSO *head;
    PROCESSO *final;
    int identificacao;  // 0 = baixa, 1 = alta, 5 = disco, 6 = fita, 7 = impressora
} FILA;


// Filas de IO globais

FILA disco;
FILA fita;
FILA impressora;


void limparConsole() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

//Cria filas e utiliza o parâmetro da prioridade como identificação
FILA criarFila(int PRIO);

//Cria os processos a partir das entradas
PROCESSO *criarProcessos(int CHEGADA, int DURACAO, int TIPO, int pID, int inicioIO);

//Aloca o processo passado na fila passada
void alocarProcesso(FILA *f, PROCESSO *p);

//Remove um processo da fila passada como parâmetro
void removerProcesso(FILA *f);

//Le o arquivo passado na main ignorando a primeira linha do txt para criar os processos
int lerArquivo(FILE *arq, PROCESSO *p);

//Imprime a fila passada por parametro
void imprimirFila(FILA *f);

//Imprime as informações de todos os processos no sistema no instante de tempo passado como parâmetro
void imprimirProcessos(PROCESSO *array, int qtdProcessos, int tempoAtual);

//Avança no tempo e administra a lógica do escalonador, utilizando funções auxiliares
void avancarTimeSlice(FILA *altaPrioridade, FILA *baixaPrioridade, int qtdProcessos, PROCESSO *arrayProcessos, int *tAtual);

//Verifica se um processo chegou no sistema
void verificarInicioProcesso(PROCESSO *p, int tAtual, FILA *altaPrioridade);

//Função que "executa" um processo em uma unidade de tempo
void executarProcesso(FILA *f, int tempoAtual);

//Remove um processo que estourou o quantum da execução
void preemptaProcesso(FILA *f, FILA *q);

//Verifica se um processo terminou sua execução
int verificarFimProcesso(FILA *f, int tempoAtual);

//Verifica se a execução de um processo alcançou o instante em que ele deve ir pra fila de IO
int verificarInicioIO(PROCESSO *p, int tAtual, FILA *f);

//Processa uma fila de IO (passa uma unidade de tempo na execução do primeiro processo da fila)
int processarIO(FILA *IO, FILA *f, int tAtual);

//Mostra a situação das filas de IO e quais processos estão fazendo operação de IO e quanto falta para eles acabarem
void mostrarSituacaoIO(FILA *IO);

//Imprime o menu com os comandos do programa
void menu();

int main(int argc, char *argv[]) {

    FILE *arq;
    
    if (argc == 2) {
        arq = fopen(argv[1], "r");
    } else {
        arq = fopen("input.txt", "r");
    }

    // Filas

    FILA altaPrioridade = criarFila(1);
    FILA baixaPrioridade = criarFila(0);

    // Filas IO
    
    disco = criarFila(5);
    fita = criarFila(6);
    impressora = criarFila(7);

    countIO = 0;

    // Processos

    PROCESSO arrayProcessos[LIMITE];
    int qtdProcessos = lerArquivo(arq, arrayProcessos);

    fclose(arq);

    int tempoAtual = 0;

    int comando;

    printf("===============================================================\n");
    printf("\t\tCarlos Chagas - 122178456\n\t\tCaroline Rebelo - 118046249\n");
    printf("===============================================================\n");
    printf("\n\t\tEscalonador RoundRobin com Feedback\t\t\n\n");

    #ifdef AUTO
    while(1) {
        avancarTimeSlice(&altaPrioridade, &baixaPrioridade, qtdProcessos, arrayProcessos, &tempoAtual);
        imprimirProcessos(arrayProcessos, qtdProcessos, tempoAtual);
        imprimirFila(&altaPrioridade);
        imprimirFila(&baixaPrioridade);
        imprimirFila(&disco);
        imprimirFila(&fita);
        imprimirFila(&impressora);
    }
    #endif

    while(1) {
        menu();
        printf("Comando: ");

        scanf("%d", &comando);
        printf("===============================================================\n");
        switch (comando){
    
            case 1:
                imprimirProcessos(arrayProcessos, qtdProcessos, tempoAtual);
                imprimirFila(&altaPrioridade);
                imprimirFila(&baixaPrioridade);
                imprimirFila(&disco);
                imprimirFila(&fita);
                imprimirFila(&impressora);
                break;

            case 2:
                avancarTimeSlice(&altaPrioridade, &baixaPrioridade, qtdProcessos, arrayProcessos, &tempoAtual);
                break;
            
            case 3:
                printf("Situacao das filas de IO no tempo atual: %d\n", tempoAtual);
                mostrarSituacaoIO(&disco);
                mostrarSituacaoIO(&fita);
                mostrarSituacaoIO(&impressora);
                break;
            case 4:
                printf("\nPressione Enter para limpar");
                getchar(); 
                getchar();
                limparConsole();
                break;
                    
            case 0:
                exit(0);

            default:
                break;

        }

    }

    return 0;
}


FILA criarFila(int PRIO) {
    FILA f;
    f.head = NULL;
    f.final = NULL;
    f.qtdProcessos = 0;
    f.identificacao = PRIO;
    return f;
}

PROCESSO *criarProcessos(int CHEGADA, int DURACAO, int TIPO, int pID, int inicioIO) {
    PROCESSO *p = (PROCESSO*) malloc(sizeof(PROCESSO));

    if (p == NULL) {
        printf(RED"Erro ao alocar memoria para o processo\n"RESET);
        exit(-1);
    }

    if (inicioIO >= DURACAO) {      // o instante de início de IO não pode ser maior que o tempo de serviço do processo
        printf(RED"Entrada invalida. O instante de inicio de IO deve ser menor que o tempo de servico do processo.\n"RESET);
        exit(-1);
    }

    p->pID = pID;
    p->tempoChegada = CHEGADA;
    p->tipoIO = TIPO;
    p->duracaoProcesso = DURACAO;
    p->inicioIO = inicioIO;
    p->tempoExecutado = 0;
    p->tempoExecutadoIO = -1;

    if (p->tipoIO == 0) {
        p->duracaoIO = ioDISCO;
    } else if (p->tipoIO == 1) {
        p->duracaoIO = ioFITA;
    } else if (p->tipoIO == 2) {
        p->duracaoIO = ioIMPRESSORA;
    } else if (p->tipoIO == -1) {   // nao tem IO
        p->tipoIO = 3;  // corresponde ao "Sem IO" do array tiposIO
    }

    p->estado = 0;
    p->prox = NULL;

    return p;
}

void alocarProcesso(FILA *f, PROCESSO *p) {

    if (f->head == NULL) {  // fila vazia
        f->head = p;
        f->final = p;
        p->prox = p;
    } else {    // fila não vazia
        f->final->prox = p;
        f->final = p;
        f->final->prox = f->head;
    }

    f->qtdProcessos++;
}

void removerProcesso(FILA *f) {

    if (f->qtdProcessos == 0) {
        printf("A fila ja esta vazia.\n");
        return;
    }

    PROCESSO *aux = f->head;

    if (f->qtdProcessos == 1) { // Removendo o unico elemento da fila
        f->head = NULL;
        f->final = NULL;
    } else {
        f->head = aux->prox;
        f->final->prox = f->head;
    }

    f->qtdProcessos--;
}

int lerArquivo(FILE *arq, PROCESSO *array) {
    static int linha = 0;   // qtd de processos criados

    if (arq == NULL) {
        printf(RED"Erro ao abrir arquivo\n"RESET);
        exit(-1);
    }

    char buffer[256];

    // fgets só pra pular a primeira linha
    fgets(buffer, sizeof(buffer), arq);

    while (fgets(buffer, sizeof(buffer), arq) != NULL) {
        linha++;
        if (linha > LIMITE) {
            printf(RED"ERRO\t Voce esta tentando criar mais processos que o permitido\n"RESET);
            exit(-1);
        }

        int entrada, servico, tipo, inicioIO;

        if (sscanf(buffer, "%d %d %d %d", &entrada, &servico, &tipo, &inicioIO) == 4) {
            // Conseguiu criar o processo do arquivo
            array[linha-1] = *criarProcessos(entrada, servico, tipo, linha, inicioIO);
        }
    }

    return linha;
}

void imprimirProcessos(PROCESSO *array, int qtdProcessos, int tempoAtual) {
    
    printf("\nProcessos:\tTempo atual: %du - QUANTUM [%d]\n\n", tempoAtual, TIMESLICE);

    TIPOS;
    ESTADOS;

    for (int i = 0; i < qtdProcessos; i++) {
        PROCESSO *p = &array[i];

        printf("[pID = %d]\tEstado = %-10s\tChegada = %du\tServico restante = %du\tInicio IO = %du\tTipo IO = %s\n", p->pID, estados[p->estado], p->tempoChegada, p->duracaoProcesso - p->tempoExecutado, p->inicioIO, tiposIO[p->tipoIO]);
    }
}

void imprimirFila(FILA *f) {
    switch(f->identificacao) {
        case 0:
            printf("\nFILA BAIXA PRIORIDADE:");
            break;

        case 1:
            printf("\nFILA ALTA PRIORIDADE:");
            break;

        case 5:
            printf("\nFILA IO DISCO:");
            break;

        case 6:
            printf("\nFILA IO FITA MAGNETICA:");
            break;

        case 7:
            printf("\nFILA IO IMPRESSORA:");
            break;

        default:
            break;
    }

    if (f->head == NULL) {  // fila vazia
        printf("A fila esta vazia\n\n");
        return;
    }

    PROCESSO *aux = f->head;

    do {
        printf("[%d] -> ", aux->pID);
        aux = aux->prox;
    } while (aux != f->head);

    printf("FIM\n\n");
}

void avancarTimeSlice(FILA *altaPrioridade, FILA *baixaPrioridade, int qtdProcessos, PROCESSO *arrayProcessos, int *tAtual) {
    int tAntes = *tAtual;
    *tAtual += TIMESLICE;

    TIPOSFILA;

    printf("\n\nTimeSlice Atual: \t%du ====> %du\n\n", tAntes, *tAtual);


    for (int t = tAntes; t < *tAtual; t++) {
        // verifica a chegada de novos processos
        for (int i = 0; i < qtdProcessos; i++) {
            PROCESSO *p = &arrayProcessos[i];
            verificarInicioProcesso(p, t, altaPrioridade);
        }

        
        // Processa as filas de IO
        if (processarIO(&disco, baixaPrioridade, t))   // se uma operacao de IO terminou, já começa o proximo no mesmo instante
            processarIO(&disco, baixaPrioridade, t);
        
        if (processarIO(&fita, altaPrioridade, t))
            processarIO(&fita, altaPrioridade, t);

        if (processarIO(&impressora, altaPrioridade, t))
            processarIO(&impressora, altaPrioridade, t);

        // verifica se há processos na fila de alta prioridade
        if (altaPrioridade->head != NULL) {
            // verifica se o processo vai iniciar IO
            if (verificarInicioIO(altaPrioridade->head, t+1, altaPrioridade)) { // se for início de IO, retorna para que o usuário possa avançar o timeslice a partir do instante que o processo foi para IO
                *tAtual = t+1;
                return;
            }
            
            if (t == tAntes)  // printa só na primeira iteração
                printf(LIGHT_GREEN"[EXECUCAO]%-4sO processo [pID = %d] da fila de %s comeca a ser executado no instante %du\n" RESET, "", altaPrioridade->head->pID, tiposFila[altaPrioridade->identificacao], t);
            
            // executa o processo
            executarProcesso(altaPrioridade, t);

            // verifica novamente se o processo vai pra IO (agora no instante final da execução dele). 
            if (verificarInicioIO(altaPrioridade->head, t+1, altaPrioridade)) {
                *tAtual = t+1;
                return;
            }

            // verifica o fim do processo
            if (verificarFimProcesso(altaPrioridade, t+1)) { // se for fim de processo, retorna para que o usuário possa avançar o timeslice a partir do instante que o processo finalizou
                *tAtual = t+1;
                return;
            } else if (t == (*tAtual) - 1) { // é a última iteração, se o processo não finalizou, ele é preemptado (neste caso, sai da alta e vai pro final da fila de baixa)
                printf(CYAN"[PREEMPCAO]%-3sO processo [pID = %d] foi preemptado no instante %du. Movido para a fila de Baixa Prioridade.\n"RESET, "", altaPrioridade->head->pID, t+1);  
                preemptaProcesso(altaPrioridade, baixaPrioridade);
                imprimirFila(altaPrioridade);
                imprimirFila(baixaPrioridade);
            }
            

        // aqui segue a mesma lógica da altaPrioridade, só muda as filas que vão ser passadas como parâmetro
        } else if (baixaPrioridade->head != NULL) {
            // verifica se o processo vai iniciar IO
            if (verificarInicioIO(baixaPrioridade->head, t+1, baixaPrioridade)) {
                *tAtual = t+1;
                return;
            }
            
            if (t == tAntes)  // printa só na primeira iteração
                printf(LIGHT_GREEN"[EXECUCAO]%-4sO processo [pID = %d] comeca a ser executado na fila de %s no instante %du\n" RESET, "", baixaPrioridade->head->pID, tiposFila[baixaPrioridade->identificacao], t);
            
            // executa o processo
            executarProcesso(baixaPrioridade, t);

            // verifica novamente se o processo vai pra IO (agora no instante final da execução dele). 
            if (verificarInicioIO(baixaPrioridade->head, t+1, baixaPrioridade)) {
                *tAtual = t+1;
                return;
            }

            // verifica o fim do processo
            if (verificarFimProcesso(baixaPrioridade, t+1)) {
                *tAtual = t+1;
                return;
            } else if (t == (*tAtual) - 1) { // é a última iteração, se o processo não finalizou, ele é preemptado (neste caso, sai da baixa e vai pro final da fila de baixa)
                printf(CYAN"[PREEMPCAO]%-3sO processo [pID = %d] foi preemptado no instante %du. Vai para o fim da fila de Baixa Prioridade.\n"RESET, "", baixaPrioridade->head->pID, t+1);  
                preemptaProcesso(baixaPrioridade, baixaPrioridade);
                imprimirFila(baixaPrioridade);
            }
        }
        
        else if (countIO != 0) { // se as duas filas de execução estão vazias, verifica se ainda tem processos em IO
            continue;
        } 

        else {
                printf("Fim. \n");
                imprimirProcessos(arrayProcessos, qtdProcessos, t);
                exit(0); 
        }
    }
}

void verificarInicioProcesso(PROCESSO *p, int tAtual, FILA *altaPrioridade) {
    if (p->tempoChegada == tAtual) {
        printf(YELLOW"[NEW]%-9sO processo [pID = %d] entra na fila de Alta Prioridade no instante %du\n" RESET, "", p->pID, tAtual);

        p->estado = 1; 
        alocarProcesso(altaPrioridade, p);
        imprimirFila(altaPrioridade);
    }
}

void executarProcesso(FILA *f, int tempoAtual) {

    PROCESSO *p = f->head;

    if (p == NULL) {
        printf("Fila vazia!\n");
        return;
    }

    p->estado = 2;
    p->tempoExecutado++;

}

void preemptaProcesso(FILA *f, FILA *q) {
    PROCESSO *p = f->head;

    removerProcesso(f);
    alocarProcesso(q, p);

    p->estado = 1;
}

int verificarFimProcesso(FILA *f, int tempoAtual) {
    
    PROCESSO *p = f->head;

    if (p->tempoExecutado == p->duracaoProcesso) { // FIM DO PROCESSO
    
        printf(GREEN"[FIM]%-9sO processo [pID = %d] finalizou no instante %du\n"RESET, "", p->pID, tempoAtual);
        removerProcesso(f);
        p->estado = 4;

        return TRUE;
    }

    return FALSE;
}

int verificarInicioIO(PROCESSO *p, int tAtual, FILA *f) {
    
    TIPOS;

    if (p->tempoExecutado == p->inicioIO) {
        removerProcesso(f);
        p->estado = 3;
        countIO++;

        switch (p->tipoIO) {
            case 0:
                alocarProcesso(&disco, p);
                break;
            case 1:
                alocarProcesso(&fita, p);
                break;
            case 2:
                alocarProcesso(&impressora, p);
                break;
            default:
                break;
        }

        printf(MAGENTA"[IO]%-10sO processo [pID = %d] entra fila de IO de tipo %s no instante %du\n" RESET, "", p->pID, tiposIO[p->tipoIO], tAtual);
        imprimirFila(f);
        return TRUE;
    }

    return FALSE;
}

int processarIO(FILA *IO, FILA *f, int tAtual) {

    TIPOS;
    TIPOSFILA;

    PROCESSO *p = IO->head;

    if (p == NULL) { // FILA DE IO VAZIA
        return FALSE;
    }

    p->tempoExecutadoIO++;

    if (p->tempoExecutadoIO == 0) {
        printf(LIGHT_BLUE"[IO]%-10sO processo [pID = %d] comecou a operacao de IO de tipo %s no instante %du\n"RESET,"", p->pID, tiposIO[p->tipoIO], tAtual);
    }
    

    // verifica o fim de IO
    if (p->tempoExecutadoIO == p->duracaoIO) {
        p->tempoExecutadoIO = 0;
        p->estado = 1;

        countIO--;
        p->inicioIO = -1;       // para não avaliar de novo que o tempo executado era igual ao inicio de IO e ir pra IO de novo

        removerProcesso(IO);
        alocarProcesso(f, p);
        
        printf(LIGHT_MAGENTA"[IO]%-10sO processo %d terminou IO no instante %du. Movido para fila de %s\n"RESET, "", p->pID, tAtual, tiposFila[f->identificacao]);
        imprimirFila(f);
        
        return TRUE;
    }

    return FALSE;
}

void mostrarSituacaoIO(FILA *IO) {

    TIPOS;
    
    imprimirFila(IO);

    PROCESSO *p = IO->head;

    if (p == NULL) {
        return;
    }

    printf("Em operacao de IO do tipo %s:\n[pID = %d]\tTempo restante de IO: %d\n", tiposIO[p->tipoIO], p->pID, p->duracaoIO - p->tempoExecutadoIO-1);
}

void menu(){
    printf("===============================================================\n");
    printf("1. Mostrar Processos e mostrar fila\n");
    printf("2. Avancar timeslice\n");
    printf("3. Mostrar situacao das filas de IO\n");
    printf("4. Limpar console\n");
    printf("0. Encerrar programa\n");
}
