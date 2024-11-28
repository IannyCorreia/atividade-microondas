#include <xinu.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

/* Estados possíveis do micro-ondas */
#define MICROONDAS_LIVRE 0
#define MICROONDAS_OCUPADO 1
#define MICROONDAS_AQUECENDO 2

int32 estado_microondas;
sid32 mutex;
sid32 comida_inserida;
bool luz_interna = false;

/* Funções principais */
void inserir_comida();
void aquecer_comida();
void remover_comida();
void controlar_microondas();
void emitir_bip();
void controlar_luz(bool estado);
void especificar_ciclo(int32 prato);
void acionar_emergencia();
void exibir_relogio_cortesia();

/* Funções específicas para pratos */
void preparar_carnes();
void preparar_peixe();
void preparar_frango();
void preparar_lasanha();
void preparar_pipoca();

process main(void)
{
    estado_microondas = MICROONDAS_LIVRE;
    mutex = semcreate(1);  // Mutex para controlar o acesso ao micro-ondas
    comida_inserida = semcreate(0); // Semáforo para esperar comida ser inserida

    printf("Iniciando sistema do micro-ondas...\n");

    resume(create(controlar_microondas, 1024, 20, "controlar_microondas", 0));

    int32 opcao = -1;
    while (TRUE) {
        printf("\n=== Menu Micro-ondas ===\n");
        printf("1. Inserir comida\n");
        printf("2. Programar aquecimento\n");
        printf("3. Remover comida\n");
        printf("4. Acionar emergência\n");
        printf("0. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                inserir_comida();
                break;
            case 2:
                aquecer_comida();
                break;
            case 3:
                remover_comida();
                break;
            case 4:
                acionar_emergencia();
                break;
            case 0:
                printf("Desligando o sistema do micro-ondas.\n");
                return OK;
            default:
                printf("Opção inválida. Tente novamente.\n");
                break;
        }
    }

    return OK;
}

void inserir_comida() {
    wait(mutex);
    if (estado_microondas == MICROONDAS_LIVRE) {
        estado_microondas = MICROONDAS_OCUPADO;
        printf("Comida inserida no micro-ondas.\n");
        controlar_luz(true);
        signal(comida_inserida);  // Libera o semáforo para indicar que a comida foi inserida
    } else {
        printf("Micro-ondas já ocupado.\n");
    }
    signal(mutex);
}

void aquecer_comida() {
    int32 prato;
    wait(mutex);
    if (estado_microondas == MICROONDAS_OCUPADO) {
        printf("Selecione o prato:\n");
        printf("1. Carnes\n");
        printf("2. Peixe\n");
        printf("3. Frango\n");
        printf("4. Lasanha\n");
        printf("5. Pipoca\n");
        printf("Escolha: ");
        scanf("%d", &prato);
        especificar_ciclo(prato);
        estado_microondas = MICROONDAS_AQUECENDO;
    } else if (estado_microondas == MICROONDAS_LIVRE) {
        printf("Nenhuma comida foi inserida.\n");
    } else {
        printf("Micro-ondas já está aquecendo.\n");
    }
    signal(mutex);
}

void remover_comida() {
    wait(mutex);
    if (estado_microondas == MICROONDAS_AQUECENDO) {
        estado_microondas = MICROONDAS_LIVRE;
        printf("Comida removida do micro-ondas.\n");
        controlar_luz(false);
    } else if (estado_microondas == MICROONDAS_OCUPADO) {
        printf("A comida ainda não foi aquecida. Removendo...\n");
        estado_microondas = MICROONDAS_LIVRE;
        controlar_luz(false);
    } else {
        printf("O micro-ondas está vazio.\n");
    }
    signal(mutex);
}

void controlar_microondas() {
    while (TRUE) {
        wait(mutex);
        switch (estado_microondas) {
            case MICROONDAS_LIVRE:
                printf("Estado do micro-ondas: LIVRE\n");
                exibir_relogio_cortesia();  // Exibe a hora atual quando livre
                break;
            case MICROONDAS_OCUPADO:
                printf("Estado do micro-ondas: OCUPADO\n");
                break;
            case MICROONDAS_AQUECENDO:
                printf("Estado do micro-ondas: AQUECENDO\n");
                break;
        }
        signal(mutex);
        sleep(5);  // Espera 5 segundos entre atualizações de estado
    }
}

void emitir_bip() {
    printf("BIP! BIP! Aquecimento concluído.\n");
}

void controlar_luz(bool estado) {
    luz_interna = estado;
    if (estado) {
        printf("Luz interna: LIGADA\n");
    } else {
        printf("Luz interna: DESLIGADA\n");
    }
}

void especificar_ciclo(int32 prato) {
    switch (prato) {
        case 1:
            preparar_carnes();
            break;
        case 2:
            preparar_peixe();
            break;
        case 3:
            preparar_frango();
            break;
        case 4:
            preparar_lasanha();
            break;
        case 5:
            preparar_pipoca();
            break;
        default:
            printf("Opção inválida. Escolha um prato válido.\n");
            break;
    }
}

void preparar_carnes() {
    printf("Aquecendo carnes...\n");
    sleep(10);  // Simula o tempo de aquecimento
    emitir_bip();
}

void preparar_peixe() {
    printf("Aquecendo peixe...\n");
    sleep(8);  // Simula o tempo de aquecimento
    emitir_bip();
}

void preparar_frango() {
    printf("Aquecendo frango...\n");
    sleep(12);  // Simula o tempo de aquecimento
    emitir_bip();
}

void preparar_lasanha() {
    printf("Aquecendo lasanha...\n");
    sleep(15);  // Simula o tempo de aquecimento
    emitir_bip();
}

void preparar_pipoca() {
    printf("Preparando pipoca...\n");
    sleep(5);  // Simula o tempo de aquecimento
    emitir_bip();
}

void acionar_emergencia() {
    wait(mutex);
    printf("Emergência ativada! Cancelando operação e abrindo porta.\n");
    estado_microondas = MICROONDAS_LIVRE;
    controlar_luz(false);
    signal(mutex);
}

void exibir_relogio_cortesia() {
    // Exibe a hora atual do sistema
    struct tm *tm_info;
    char buffer[26];
    time_t timer;

    time(&timer);
    tm_info = localtime(&timer);
    strftime(buffer, 26, "Hora atual: %H:%M:%S", tm_info);
    printf("%s\n", buffer);
}
