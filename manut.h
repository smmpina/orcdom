#ifndef _MANUT_H
#define _MANUT_H

#include "orcdom.h"
#include "menuh.h"
#include "brws.h"
#include "ordenacao.h"
#include <stdint.h>

struct pesquisa
{
    int periodo;
    int estado;
    int situacao;
    int tipo;
    int parcela;
    int ordem;
    int num_compra;
    int parc_min;
    int parc_max;
    time_t segs_ini;
    time_t segs_fin;
    wchar_t descricao[ 128 ];
};

bool carrega_dados( BR *b );
int cmpInvertWchar( const void *a, const void *b );

LN *cad( BR *b, LN *l );
void cadastra();
int cad_parcs( FILE *arq, struct despesa *dp );
int entdados( struct despesa *dp );

LN *exclui_manut( BR *b, LN *l );
int exc_parcs( FILE *arq, int num_compra );

LN *edita_manut( BR *b, LN *l );

LN *dispcompra_manut( BR *b, LN *l );
LN *edita_parcela( BR *b, LN *l );
LN *paga_parcela( BR *b, LN *l );
bool dispcompra_dados( BR *b, int num );
void resumo_compra( uint8_t lin, uint8_t col, int num_compra ); 

LN *con_descricao( BR *b, LN *l );
LN *consulta_tipo( BR *b, LN *l );

LN *filtro_manut( BR *b, LN *l );
int filtra_conta( int lin, int col );
int filtra_estado( int lin, int col );
int filtra_situacao( int lin, int col );
int filtra_parcelas( int lin, int col, int *min_parc, int *max_parc );
int filtra_ordenacao( int lin, int col );
int filtra_tipo( int lin, int col );

void mostra_filtros( int lin, int col );

int menu_tipo( uint8_t lin, uint8_t col, uint8_t to );
int16_t menuh_est( uint8_t lin, uint8_t col );
int16_t menuh_lco( uint8_t lin, uint8_t col );
int16_t menuh_cta( uint8_t lin, uint8_t col );

#endif
