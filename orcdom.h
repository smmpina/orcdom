#ifndef _ORCDOM_H
#define _ORCDOM_H

#include "newlib.h"

#define T_TIPO    73
#define T_CONTA    8
#define T_GRUPO   13

#define COR_MSG     66
#define COR_GRUPO    3
#define COR_TIPO     2
#define COR_CONTA    4

struct tipo
{
    int idt;
    wchar_t desc[ 64 ];
    int idg;
    double valor;
};

struct grupo
{
    int idg;
    wchar_t desc[ 25 ];
    double valor;
};

struct conta
{
    int idc;
    wchar_t desc[ 5 ];
    wchar_t descc[ 30 ];
    int cor;
    int ctrl;
    double valor;
};

struct despesa
{
    int id;
    int id_compra;
    time_t data;
    time_t data_reg;
    wchar_t descricao[ 128 ];
    int tipo;
    int conta;
    int parcela;
    int parcelas;
    int apagado;
    int fixa;
    int pago;
    double valor;
};

extern struct conta ct[];
extern struct grupo gp[];
extern struct tipo tp[];
extern wchar_t nome_prog[ 128 ];

void config();
int init_config();

// arquivo manut.c
void manut();
// arquivo fopag.c
void imp_fopag();
// arquivo relat.c
void menu_relat();                     
//arquivo cadastro.c
void cadastra();
//arquivo exclusao.c
void exclui();
int exclui_parcelas( FILE *arq, int n );
//arquivo pesquisa.c
void pesquisa();
//arquivo edicao.c
void edita_desc();
//arquivo pagamento.c
void pagamento();
//arquivo ordenacao.c
void ordenacao();
bool ordena();
int cmp_sort( const void *a, const void *b );

//arquivo impress.c
void impdados( struct despesa *dp );
void mostra_dados( struct despesa *dp );
void impdesp_det( uint8_t lin, uint8_t col, struct despesa dp );

#endif
