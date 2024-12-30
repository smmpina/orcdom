#include "colors.h"
#include "newlib.h"
#include "orcdom.h"
#include <stdint.h>
#include "config.h"

int init_config()
{
    FILE *arq;
    struct config cfg;

    cfg.previ = 0.0;
    cfg.inss = 0.0;
    cfg.inpc = 1.0;
    cfg.pdesc_previ = 0.0;
    cfg.pdesc_cassi_pes = 0.0;
    cfg.pdesc_cassi_dep = 0.0;
    cfg.nr_deps = 0;
    cfg.mes1_previ13 = 4;
    cfg.mes2_previ13 = 11;
    cfg.mes1_inss13 = 8;
    cfg.mes2_inss13 = 11;
    cfg.valor_es = 0.0;
    cfg.valor_capec = 0.0;

    arq = fopen( "config.dat", "rb+" );

    if( !arq )
	{
		arq = fopen( "config.dat", "wb+" );

		if( !arq )
		{
            ret_wmsg_form( L"Falha na abertura do arquivo de configuração.", RETMSG_ERRO );
			return 0;
		}

        fseek( arq, 0, SEEK_SET );
        fwrite( &cfg, sizeof( struct config ), 1, arq );
	}
	
	fclose( arq );

    return 1;
}

void config()
{
    FILE *arq;
    struct config cfg;
    struct config aux;
    uint8_t i = 0;
    uint8_t lin = LIN_FORM_APP;
    uint8_t col = COL_FORM_APP;
    int ret;
    wchar_t moddesc[ 128 ];

    wcscpy( moddesc, nome_prog );
    wcscat( moddesc, L" - Configuração" );
    
    arq = fopen( "config.dat", "rb+" );

    init_form( moddesc );

    if( !arq )
    {
        ret_wmsg_form( L"Falha na abertura do arquivo.", RETMSG_ERRO );
        return;
    }

    rewind( arq );

    fread( &cfg, sizeof( struct config ), 1, arq );

    aux = cfg;

    while( true )
    {
        setcolor_on( NGRY, NBLK, 1, NNORM );
        box( BOX, lin, col, lin + 14, col + 57 );
        box( BOX, lin, col + 60, lin + 14, col + 84 );

        lin++;
        col += 2;

        setcurs( lin + i, col );
        setcolor_on( NBLU, NBLK, 1, NBRIG );
        printf( "\xe2\x97\x86" );
        setcolor_on( COR_MSG, NBLK, 1, NBRIG );
        printf( " Valor do beneficio da PREVI (0,00)" );
        
        ret = input_wdouble( lin + i, col + 60, &aux.previ, 12, 2, ENTMODO_EDIT );
    
        if( ret == 0 || ret == -1 ) break;

        setcurs( lin + i, col + 60 );
        setcolor_on( NBLU, NBLK, 1, NBRIG );
        imp_wdoub( fabs( aux.previ ), 12, 2, J_DIR );

        setcurs( lin + ( i += 1 ), col );
        setcolor_on( NBLU, NBLK, 1, NBRIG );
        printf( "\xe2\x97\x86" );
        setcolor_on( COR_MSG, NBLK, 1, NBRIG );
        printf( " Valor do beneficio do INSS (0,00)" );
        
        ret = input_wdouble( lin + i, col + 60, &aux.inss, 12, 2, ENTMODO_EDIT );

        if( ret == 0 || ret == -1 ) break;
        
        setcurs( lin + i, col + 60 );
        setcolor_on( NBLU, NBLK, 1, NBRIG );
        imp_wdoub( fabs( aux.inss ), 12, 2, J_DIR );

        setcurs( lin + ( i += 1 ), col );
        setcolor_on( NGRN, NBLK, 1, NBRIG );
        printf( "\xe2\x97\x86" );
        setcolor_on( COR_MSG, NBLK, 1, NBRIG );
        printf( " INPC (0,000000)" );
        
        ret = input_wdouble( lin + i, col + 60, &aux.inpc, 10, 6, ENTMODO_EDIT );
        /* aux.inpc /= 1e6; */
        
        if( ret == 0 || ret == -1 ) break;
        
        setcurs( lin + i, col + 60 );
        setcolor_on( NGRN, NBLK, 1, NBRIG );
        imp_wdoub( fabs( aux.inpc ), 16, 6, J_DIR );

        setcurs( lin + ( i += 1 ), col );
        setcolor_on( NRED, NBLK, 1, NBRIG );
        printf( "\xe2\x97\x86" );
        setcolor_on( COR_MSG, NBLK, 1, NBRIG );
        printf( " Percentual de desconto da PREVI (0,0000)" );
        
        ret = input_wdouble( lin + i, col + 60, &aux.pdesc_previ, 10, 4, ENTMODO_EDIT );
        /* aux.pdesc_previ /= 1e4; */
        
        if( ret == 0 || ret == -1 ) break;
        
        setcurs( lin + i, col + 60 );
        setcolor_on( NRED, NBLK, 1, NBRIG );
        imp_wdoub( fabs( aux.pdesc_previ ), 14, 4, J_DIR );

        setcurs( lin + ( i += 1 ), col );
        setcolor_on( NRED, NBLK, 1, NBRIG );
        printf( "\xe2\x97\x86" );
        setcolor_on( COR_MSG, NBLK, 1, NBRIG );
        printf( " Percentual de desconto da CASSI pessoal (0,0000)" );
        
        ret = input_wdouble( lin + i, col + 60, &aux.pdesc_cassi_pes, 10, 4, ENTMODO_EDIT );
        
        if( ret == 0 || ret == -1 ) break;
        
        setcurs( lin + i, col + 60 );
        setcolor_on( NRED, NBLK, 1, NBRIG );
        imp_wdoub( fabs( aux.pdesc_cassi_pes ), 14, 4, J_DIR );

        setcurs( lin + ( i += 1 ), col );
        setcolor_on( NRED, NBLK, 1, NBRIG );
        printf( "\xe2\x97\x86" );
        setcolor_on( COR_MSG, NBLK, 1, NBRIG );
        printf( " Percentual de desconto da CASSI dependentes (0,0000)" );
        
        ret = input_wdouble( lin + i, col + 60, &aux.pdesc_cassi_dep, 10, 4, ENTMODO_EDIT );
        
        if( ret == 0 || ret == -1 ) break;
        
        setcurs( lin + i, col + 60 );
        setcolor_on( NRED, NBLK, 1, NBRIG );
        imp_wdoub( fabs( aux.pdesc_cassi_dep ), 14, 4, J_DIR );

        setcurs( lin + ( i += 1 ), col );
        setcolor_on( NWHT, NBLK, 1, NBRIG );
        printf( "\xe2\x97\x86" );
        setcolor_on( COR_MSG, NBLK, 1, NBRIG );
        printf( " Numero de dependentes" );
        
        ret = input_wint( lin + i, col + 60, &aux.nr_deps, 4, ENTMODO_EDIT );
        
        if( ret == 0 || ret == -1 ) break;

        setcurs( lin + i, col + 60 );
        setcolor_on( NWHT, NBLK, 1, NBRIG );
        imp_wint( aux.nr_deps, 9, 0, J_DIR );

        setcurs( lin + ( i += 1 ), col );
        setcolor_on( NWHT, NBLK, 1, NBRIG );
        printf( "\xe2\x97\x86" );
        setcolor_on( COR_MSG, NBLK, 1, NBRIG );
        printf( " Mes da antecipacao do decimo terceiro da PREVI" );

        ret = input_wint( lin + i, col + 60, &aux.mes1_previ13, 4, ENTMODO_EDIT );
        
        if( ret == 0 || ret == -1 ) break;
        
        setcurs( lin + i, col + 60 );
        setcolor_on( NWHT, NBLK, 1, NBRIG );
        imp_wint( aux.mes1_previ13, 9, 0, J_DIR );

        setcurs( lin + ( i += 1 ), col );
        setcolor_on( NWHT, NBLK, 1, NBRIG );
        printf( "\xe2\x97\x86" );
        setcolor_on( COR_MSG, NBLK, 1, NBRIG );
        printf( " Mes da antecipacao do decimo terceiro do INSS" );
        
        ret = input_wint( lin + i, col + 60, &aux.mes1_inss13, 4, ENTMODO_EDIT );
        
        if( ret == 0 || ret == -1 ) break;
        
        setcurs( lin + i, col + 60 );
        setcolor_on( NWHT, NBLK, 1, NBRIG );
        imp_wint( aux.mes1_inss13, 9, 0, J_DIR );

        setcurs( lin + ( i += 1 ), col );
        setcolor_on( NWHT, NBLK, 1, NBRIG );
        printf( "\xe2\x97\x86" );
        setcolor_on( COR_MSG, NBLK, 1, NBRIG );
        printf( " Mes da segunda parcela do decimo terceiro da PREVI" );
        
        ret = input_wint( lin + i, col + 60, &aux.mes2_previ13, 4, ENTMODO_EDIT );
        
        if( ret == 0 || ret == -1 ) break;
        
        setcurs( lin + i, col + 60 );
        setcolor_on( NWHT, NBLK, 1, NBRIG );
        imp_wint( aux.mes2_previ13, 9, 0, J_DIR );

        setcurs( lin + ( i += 1 ), col );
        setcolor_on( NWHT, NBLK, 1, NBRIG );
        printf( "\xe2\x97\x86" );
        setcolor_on( COR_MSG, NBLK, 1, NBRIG );
        printf( " Mes da segunda parcela do decimo terceiro do INSS" );
        
        ret = input_wint( lin + i, col + 60, &aux.mes2_inss13, 4, ENTMODO_EDIT );
        
        if( ret == 0 || ret == -1 ) break;
        
        setcurs( lin + i, col + 60 );
        setcolor_on( NWHT, NBLK, 1, NBRIG );
        imp_wint( aux.mes2_inss13, 9, 0, J_DIR );

        setcurs( lin + ( i += 1 ), col );
        setcolor_on( NRED, NBLK, 1, NBRIG );
        printf( "\xe2\x97\x86" );
        setcolor_on( COR_MSG, NBLK, 1, NBRIG );
        printf( " Valor do Emprestimo simples da PREVI (0,00)" );
        
        ret = input_wdouble( lin + i, col + 60, &aux.valor_es, 12, 2, ENTMODO_EDIT );
        
        if( ret == 0 || ret == -1 ) break;
        
        setcurs( lin + i, col + 60 );
        setcolor_on( NRED, NBLK, 1, NBRIG );
        imp_wdoub( fabs( aux.valor_es ), 12, 2, J_DIR );

        setcurs( lin + ( i += 1 ), col );
        setcolor_on( NRED, NBLK, 1, NBRIG );
        printf( "\xe2\x97\x86" );
        setcolor_on( COR_MSG, NBLK, 1, NBRIG );
        printf( " Valor da CAPEC (0,00)" );
        
        ret = input_wdouble( lin + i, col + 60, &aux.valor_capec, 12, 2, ENTMODO_EDIT );
        
        if( ret == 0 || ret == -1 ) break;
        
        setcurs( lin + i, col + 60 );
        setcolor_on( NRED, NBLK, 1, NBRIG );
        imp_wdoub( fabs( aux.valor_capec ), 12, 2, J_DIR );

        /* i += 3; */
    
        if( hdialog_button( L"Confirma?", CONFIRM_DEFAULT_Y ) != 1 ) 
        {
            ret = -1;
            break;
        }

        cfg = aux;

        fseek( arq, 0, SEEK_SET );
        ret = fwrite( &cfg, sizeof( struct config ), 1, arq );
        
        break;
    }

    fclose( arq );

    setcolor_off();

    if( ret == 0 ) ret_wmsg_form( L"Valor inválido.", RETMSG_ERRO );
    if( ret == -1 ) ret_wmsg_form( L"Cancelado pelo operador.", RETMSG_ERRO );
    if( ret == 1 ) ret_wmsg_form( L"Valores gravados com sucesso.", RETMSG_NO_ERRO );

    return;
}
