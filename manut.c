#include "manut.h"
#include "brws.h"
#include "colors.h"
#include "newlib.h"
#include "menuh.h"
#include "orcdom.h"
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <wchar.h>

struct pesquisa pesq;
double totalvalor = 0.0;
bool refresh_data = true;

void manut()
{
    int8_t ret = -1;
    BR *browse = NULL;
    TB *tb = NULL;
    SB *sb = NULL;
    TC *tc = NULL;
    LN * ( *fcad )( BR *, LN * );
    LN * ( *fdel )( BR *, LN * );
    LN * ( *fcon )( BR *, LN * );
    LN * ( *fedit )( BR *, LN * );
    LN * ( *ftab )( BR *, LN * );
    LN * ( *ffiltro )( BR *, LN * );
    uint8_t lin = LIN_FORM_APP;
    uint8_t col = COL_FORM_APP;
    wchar_t moddesc[ 128 ];
    wchar_t buffer[ 128 ];
    wchar_t buf_val[ 128 ];
    wchar_t *statusbar1[] = { L"[ESC]", L"[HOME]", L"[END]", L"[F2]",
                              L"[F3]", L"[TAB]", L"[INS]", L"[ENTER]", L"[DEL]" };
    wchar_t *statusbar2[] = { L"Sai", L"Inicio", L"Fim", L"Pesquisa",
                              L"Filtra", L"Detalha", L"Cadastra", L"Edita", L"Exclui" };
    wchar_t *titlecols[] = { L"Descrição", L"#", L"Tipo", L"Conta", L"Id", L"Idc",
                             L"Parc", L"Parcs", L"Data", L"Val Parc" };
    uint8_t titlecols2[] = { J_ESQ, J_ESQ, J_CEN, J_ESQ, J_DIR, J_DIR, J_DIR, J_DIR, J_ESQ, J_DIR };

    fcad = cad;
    fdel = exclui_manut;
    fedit = edita_manut;
    fcon = con_descricao;
    ftab = dispcompra_manut;
    ffiltro = filtro_manut;

    wcscpy( moddesc, nome_prog );
    wcscat( moddesc, L" - Manutenção" );

    browse = create_bw();

    setbox_bw( browse, BOX_BROWSE_BW );
    press_keys_bw( browse, INS, fcad );
    press_keys_bw( browse, ENTER, fedit );
    press_keys_bw( browse, F3, ffiltro );
    press_keys_bw( browse, DEL, fdel );
    press_keys_bw( browse, F2, fcon );
    press_keys_bw( browse, TAB, ftab );

    setcolor_bw( browse, COLOR_FOCUS_BW, NWHT, 238, 1, NNORM );
    
    for( uint8_t inc = 0; inc < 10; inc++ )
    {
        tc = addtitlecol_bw( browse, titlecols[ inc ] );
        setattrtitlecol_bw( tc, titlecols2[ inc ], MAIUSC );
        /* setcolortitlecol_bw( tc, NBLK, 67, 2, NNORM, NITAL ); */
        setcolortitlecol_bw( tc, NWHT, 238, 2, NNORM, NITAL );
    }

    for( uint8_t inc = 0; inc < 9; inc++ )
    {
        sb = addstatusbar_bw( browse, statusbar1[ inc ] );
        setcolorstatusbar_bw( sb, NGRY, NBLK, 2, NNORM, NREVE );
        
        sb = addstatusbar_bw( browse, statusbar2[ inc ] );
        setcolorstatusbar_bw( sb, NLCYN, NWHT, 3, NNORM, NITAL, NREVE );
    
        if( inc < 8 ) addstatusbar_bw( browse, L" " );
    }

    for( uint8_t inc = 0; inc < T_CONTA; inc++ ) ct[ inc ].ctrl = 1;

    init_form( moddesc );

    pesq.periodo = input_daterange( lin, col + 1, &pesq.segs_ini, &pesq.segs_fin );

    if( pesq.periodo == -1 )
    {
        ret_wmsg_form( L"Cancelado pelo operador.", RETMSG_ERRO );
        return;
    }

    if( pesq.periodo == 2 )
    {
        int mes, ano, mes_prox, ano_prox;
        
        mes = decomp_date( pesq.segs_ini, 2 );
        ano = decomp_date( pesq.segs_ini, 3 );

        pesq.segs_ini = date_segs( 20, mes, ano );

        mes_prox = mes + 1;
        ano_prox = ano;

        if( mes_prox > 12 )
        {
            mes_prox = 1;
            ano_prox = ano + 1;
        }

        pesq.segs_fin = date_segs( 19, mes_prox, ano_prox );
    }

    pesq.estado = 0;
    pesq.situacao = 0;
    pesq.tipo = 0;
    pesq.parcela = 0;
    pesq.ordem = 0;
    pesq.num_compra = 0;
    pesq.parc_min = 1;
    pesq.parc_max = 99;

    wcscpy( pesq.descricao, L"" );
        
    do
    {
        if( refresh_data )
        {
            refresh_data = false;
            if( !carrega_dados( browse ) ) return;
        }

        init_form( moddesc );
        
        mostra_filtros( lin, col );

        wcscpy( buffer, L" Despesas - " );
        wcscat( buffer, doub_to_wstr( fabs( totalvalor ), 2, buf_val ) );
        wcscat( buffer, L" " );
    
        rmtitlebar_bw( browse, 0 );
        
        tb = addtitlebar_bw( browse, buffer );
        setcolortitlebar_bw( tb, NGRY, NBLK, 2, NBRIG, NREVE );
        
        setcoord_bw( browse, lin + 11, col, getsize_bw( browse ) > 17 ? 17 : getsize_bw( browse ) );

        init_bw( browse );

        ret = exec_bw( browse );
    }
    while( ret != -1 );

    refresh_data = true;

    free_bw( browse );

    return;
}

bool carrega_dados( BR *b )
{
    FILE *arq;
	struct despesa dp;
    bool vazio_arq = true;
    bool erro_arq = false;
    time_t segs_hoje;
    time_t tnum;
    uint8_t cor_c;
    wchar_t dwchar[ 128 ];
    wchar_t buffer[ 128 ];
    LN *l = NULL;
    CO *c = NULL;
    bool critpesq;

    freelines_bw( b );

    segs_hoje = now_segs();

    arq = fopen( "despesas.dat", "rb" );

    if( !arq )
	{
        ret_wmsg_form( L"Falha na abertura do arquivo", RETMSG_ERRO );
        return false;
	}

    rewind( arq );
            
    totalvalor = 0.0;

    while( fread( &dp, sizeof( struct despesa ), 1, arq ) )
    {
        critpesq = wcsstr( dp.descricao, pesq.descricao ) &&
                   !( dp.apagado ) &&
                   ( pesq.periodo ? ( dp.data >= pesq.segs_ini && dp.data <= pesq.segs_fin ) : 1 ) && 
                   ( pesq.num_compra ? dp.id_compra == pesq.num_compra : 1 ) &&
                   ( ct[ dp.conta - 1 ].ctrl ) &&
                   ( pesq.estado ? ( dp.fixa == ( pesq.estado == 1 ? 1 : 0 ) ) : 1 ) && 
                   ( pesq.situacao ? ( dp.pago == ( pesq.situacao == 1 ? 0 : 1 ) ) : 1 ) &&
                   ( pesq.tipo ? dp.tipo == pesq.tipo : 1 ) &&
                   ( pesq.parcela ? ( dp.parcelas >= pesq.parc_min && dp.parcelas <= pesq.parc_max ) : 1 ) &&
                   ( pesq.parcela == 3 ? ( dp.parcela == dp.parcelas && dp.parcelas > 1 ) : 1 ) &&
                   ( pesq.parcela == 4 ? ( dp.parcelas == pesq.parc_max ) : 1 );

        if( critpesq )
        {
            switch( pesq.ordem )
	        {
                case 0:
                    setdatatypekey_bw( b, sizeof( time_t ) );
                    
                    tnum = ( time_t )( dp.id_compra + dp.data );
                    /* defDadoChav_brws( chave, &dp.data ); */
                    
                    setfunckey_bw( b, cmpdateline_bw );
                    
                    l = addline_bw( b, &tnum, ( uint32_t ) dp.id, NULL ); 
                    break;    
                case 1:
                    setdatatypekey_bw( b, sizeof( int ) );
                    setfunckey_bw( b, cmpintline_bw );
                    
                    l = addline_bw( b, &dp.id, ( uint32_t ) dp.id, NULL ); 
			        break;
		        case 2:
                    setdatatypekey_bw( b, sizeof( dwchar ) );
                    setfunckey_bw( b, cmpwcharline_bw );
                    
                    l = addline_bw( b, dp.descricao, ( uint32_t ) dp.id, NULL ); 
			        break;
                case 3:
                    setdatatypekey_bw( b, sizeof( double ) );
                    setfunckey_bw( b, cmpfloatline_bw );
                    
                    l = addline_bw( b, &dp.valor, ( uint32_t ) dp.id, NULL ); 
                    break;
                case 4:
                    setdatatypekey_bw( b, sizeof( time_t ) );
                    setfunckey_bw( b, cmpdateline_bw );

                    //tnum = ( time_t )( fabs( dp.valor ) * 100 ) + dp.data; // por data e valor
                    tnum = ( time_t )( dp.valor * 100 ) + dp.data; // por data e valor

                    l = addline_bw( b, &tnum, ( uint32_t ) dp.id, NULL ); 
                    break;    
                case 5:
                    setdatatypekey_bw( b, sizeof( time_t ) );
                    setfunckey_bw( b, cmpdateline_bw );
                    
                    l = addline_bw( b, &dp.data_reg, ( uint32_t ) dp.id, NULL ); 
                    break;    
	        }
            
            c = addnewcol_bw( b, l, dp.descricao, DWCHAR ); // 0
            setattrcol_bw( c, 40, 0, J_ESQ, MAIUSC );
            setcolorcol_bw( c, NLYEL, NBLK, 1, NNORM );

            if( dp.pago )
            {
                cor_c = NLBLU;
                wcscpy( buffer, L"L" );
            }
            else
            {
                if( dp.data >= segs_hoje )
                {
                    cor_c = NLGRN;
                    wcscpy( buffer, L"S" );
                }
                else
                {
                    cor_c = NRED;
                    wcscpy( buffer, L"V" );
                }
            }

            c = addnewcol_bw( b, l, buffer, DWCHAR ); // 1
            setattrcol_bw( c, 1, 0, J_ESQ, MAIUSC );
            setcolorcol_bw( c, cor_c, NBLK, 1, NNORM );

            if( dp.fixa )
            {
                cor_c = NMAG;
                wcscpy( buffer, L"Fixa" );
            }
            else
            {
                cor_c = NCYN;
                wcscpy( buffer, L"Vari" );
            }

            c = addnewcol_bw( b, l, buffer, DWCHAR ); // 2
            setattrcol_bw( c, 4, 0, J_CEN, MAIUSC );
            setcolorcol_bw( c, cor_c, NBLK, 1, NNORM );

            c = addnewcol_bw( b, l, ct[ dp.conta - 1 ].desc, DWCHAR ); // 3
            setattrcol_bw( c, 6, 0, J_CEN, MINUSC );
            setcolorcol_bw( c, ct[ dp.conta - 1 ].cor, NBLK, 1, NNORM );

            c = addnewcol_bw( b, l, &dp.id, DINT ); // 4
            setattrcol_bw( c, 6, 0, J_DIR, MINUSC );
            setcolorcol_bw( c, NGRY, NBLK, 1, NNORM );

            c = addnewcol_bw( b, l, &dp.id_compra, DINT ); // 5
            setattrcol_bw( c, 5, 0, J_DIR, MINUSC );
            setcolorcol_bw( c, NGRN, NBLK, 1, NNORM );

            c = addnewcol_bw( b, l, &dp.parcela, DINT ); // 6
            setattrcol_bw( c, 5, 0, J_DIR, MINUSC );
            setcolorcol_bw( c, NWHT, NBLK, 1, NNORM );

            c = addnewcol_bw( b, l, &dp.parcelas, DINT ); // 7
            setattrcol_bw( c, 5, 0, J_DIR, MINUSC );
            setcolorcol_bw( c, NWHT, NBLK, 1, NNORM );

            c = addnewcol_bw( b, l, &dp.data, DDATE ); // 8
            setattrcol_bw( c, 15, 0, J_CEN, MINUSC );
            setcolorcol_bw( c, NGRY, NBLK, 1, NNORM );

            c = addnewcol_bw( b, l, &dp.valor, DFLOAT ); // 9
            setattrcol_bw( c, 10, 2, J_DIR, MINUSC );
            setcolorcol_bw( c, dp.valor < 0 ? NBLU : NLRED, NBLK, 1, NNORM );
                
            totalvalor += dp.valor;
            vazio_arq = false;
        }
    }

    if( ferror( arq ) ) erro_arq = true;

    fclose( arq );

    if( erro_arq )
    {
        ret_wmsg_form( L"Erro de acesso ao arquivo", RETMSG_ERRO );
        freelines_bw( b );
        return false;
    }

    if( vazio_arq )
    {
        ret_wmsg_form( L"arquivo vazio", RETMSG_ERRO );
        freelines_bw( b );
        return false;
    }

    return true;
}

int cmpInvertWchar( const void *a, const void *b )
{
    return wcscoll( ( wchar_t * )b, ( wchar_t * )a ) > 0;
}

LN *cad( BR *b, LN *l )
{
    cadastra();

    return l;
}

void cadastra()
{
    FILE *arq;
	struct despesa dp;
    int opc;
    wchar_t moddesc[ 128 ];

    wcscpy( moddesc, nome_prog );
    wcscat( moddesc, L" - Cadastro" );
    
    arq = fopen( "despesas.dat", "rb+" );
    
    init_form( moddesc );

    if( !arq )
	{
        ret_wmsg_form( L"Falha na abertura do arquivo.", RETMSG_ERRO );
		return;
	}

    if( entdados( &dp ) )
    {
        opc = hdialog_button( L"Confirma?", CONFIRM_DEFAULT_Y );

        switch( opc )
        {
            case 1:
                if( !cad_parcs( arq, &dp ) )
                {
                    ret_wmsg_form( L"Erro no cadastro.", RETMSG_ERRO );
                }

                refresh_data = true;

                break;
            case 0:
            case -1:
                ret_wmsg_form( L"Cancelado pelo operador.", RETMSG_ERRO );
                break;
        }
    }
    
    fclose( arq );
}

int cad_parcs( FILE *arq, struct despesa *dp )
{
    struct despesa aux;
    long int reg;
    size_t cad = 0;
    int tdia;
    int apagado;
    int id_c = 0;
    int id_p = 0;
    int dia = decomp_date( dp->data, 1 );
    int mes = decomp_date( dp->data, 2 );
    int ano = decomp_date( dp->data, 3 );
    int dias_mes[ 12 ] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    
    rewind( arq );

    while( fread( &aux, sizeof( struct despesa ), 1, arq ) )
    {
        if( !aux.apagado ) id_c = aux.id_compra > id_c ? aux.id_compra : id_c;
    }

    if( ferror( arq ) ) return 0;

    id_c++;
    cad = dp->parcela;

    do
    {
        apagado = 0;
        reg = 0;

        rewind( arq );

        while( fread( &aux, sizeof( struct despesa ), 1, arq ) )
        {
            if( aux.id > id_p ) id_p = aux.id;
            
            if( aux.apagado )
            {
                id_p = aux.id;
                apagado = 1;
                break;
            }
        
            reg = ftell( arq );
        }

        if( ferror( arq ) ) return 0;

        if( !apagado ) id_p++;

        reg = apagado ? reg : 0;

        if( cad > dp->parcela )
        {
            mes++;

            if( mes > 12 )
            {
                mes = 1;
                ano++;
            }

            tdia = mes == 2 ? ( valdt_n( 29, 2, ano ) ? 29 : 28 ) : dias_mes[ mes - 1 ];

            dp->data = date_segs( dia > tdia ? tdia : dia, mes, ano );
            dp->parcela++;
        }

        dp->id = id_p;
        dp->id_compra = id_c;
        dp->apagado = 0;
        dp->pago = 0;
                    
        fseek( arq, reg, apagado ? SEEK_SET : SEEK_END );
        cad += fwrite( dp, sizeof( struct despesa ), 1, arq );
    }
    while( cad <= dp->parcelas );

    return 1;
}

int entdados( struct despesa *dp )
{
    int estado;
    int tipo;
    int conta;
    int lco;
    int ret = 0;
    uint8_t lin = LIN_FORM_APP;
    uint8_t col = COL_FORM_APP + 1;
    time_t segs_comp;
    time_t segs_venc;
    time_t segs_hoje;
    bool ( *fi ) ( wint_t );
    wchar_t buffer[ 64 ];
    wchar_t moddesc[ 128 ];

    wcscpy( moddesc, nome_prog );
    wcscat( moddesc, L" - Cadastro" );
    
    fi = filter_input_no;

    segs_hoje = now_segs();

    wcscpy( buffer, L"Data da compra:        | " ); 
        
    segs_comp = input_date( buffer, lin, col );

    if( segs_comp > segs_hoje )
    {
        ret_wmsg_form( L"Data superior a data de hoje.", RETMSG_ERRO );
        return 0;
    }

    if( !segs_comp )
    {
        ret_wmsg_form( L"Cancelado pelo operador.", RETMSG_ERRO );
        return 0;
    }

    wcscpy( buffer, L"Data do vencimento:    | " ); 

    segs_venc = input_date( buffer, lin + 1, col );

    if( !segs_venc )
    {
        ret_wmsg_form( L"Cancelado pelo operador.", RETMSG_ERRO );
        return 0;
    }

    init_form( moddesc );
    
    setcurs( lin, col );
    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
    printf( "Data da compra:        | " );
    setcolor_off();
    imp_colordate( segs_comp, 15, NGRY, NBLK, 1, NBRIG );

    setcurs( lin + 1, col );
    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
    printf( "Data do vencimento:    | " );
    setcolor_off();
    imp_colordate( segs_venc, 15, NGRY, NBLK, 1, NBRIG );

    estado = menuh_est( lin + 2, col );

    if( estado == -1 )
    {
        ret_wmsg_form( L"Cancelado pelo operador.", RETMSG_ERRO );
        return 0;
    }

    dp->data_reg = segs_comp;
    dp->data = segs_venc;
    dp->fixa = estado; 

    while( true )
    {
        cleanarea( lin + 3, col, lin + 3, col + 25 + 42 );
        
        setcurs( lin + 3, col );
        setcolor_on( COR_MSG, NBLK, 1, NBRIG );
        printf( "Descrição:             | " );
        setcolor_off();

        wcscpy( dp->descricao, L"" );

        ret = input_wstr( lin + 3, col + 25, dp->descricao, 42, ENTMODO_CAD, fi );
    
        if( ret == -1 )
        {
            ret_wmsg_form( L"Cancelado pelo operador.", RETMSG_ERRO );
            return 0;
        }

        if( !wcslen( dp->descricao ) )
            ret_wmsg_form( L"descrição não pode ser vazia.", RETMSG_ERRO );
        else
            break;
    }

    setcurs( lin + 3, col );

    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
    printf( "Descrição:             | " );
    setcolor_off();
   
    setcolor_on( NLRED, NBLK, 1, NBRIG );
    imp_wstr( dp->descricao, 42, 1, 1 );
    setcolor_off();

    tipo = menu_tipo( lin + 4, col, 10 );

    if( tipo == -1 )
    {
        ret_wmsg_form( L"Cancelado pelo operador.", RETMSG_ERRO );
        return 0;
    }

    dp->tipo = tipo;

    conta = menuh_cta( lin + 5, col );

    if( conta == -1 )
    {
        ret_wmsg_form( L"Cancelado pelo operador.", RETMSG_ERRO );
        return 0;
    }

    dp->conta = conta;

    while( true )
    {
        cleanarea( lin + 6, col, lin + 6, col + 25 + 10 );
        
        setcurs( lin + 6, col );

        dp->parcela = 1;

	    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
        printf( "Parcela:               | " );
        setcolor_off();
    
        ret = input_wint( lin + 6, col + 25, &dp->parcela, 10, ENTMODO_CAD );

        if( ret == -1 )
        {
            ret_wmsg_form( L"Cancelado pelo operador.", RETMSG_ERRO );
            return 0;
        }
    
        if( ret == 0 )
            ret_wmsg_form( L"Número inválido.", RETMSG_ERRO );
        else if( dp->parcela < 1 )
            ret_wmsg_form( L"Número da parcela inicial menor que 1.", RETMSG_ERRO );
        else break;
    }
    
    setcurs( lin + 6, col );

    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
    printf( "Parcela:               | " );

    setcolor_on( NLRED, NBLK, 1, NBRIG );
    printf( "%02d", dp->parcela );
    setcolor_off();

    while( true )
    {
        cleanarea( lin + 7, col, lin + 7, col + 25 + 10 );

        setcurs( lin + 7, col );

        dp->parcelas = dp->parcela;

	    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
        printf( "Parcelas:              | " );
        setcolor_off();
        ret = input_wint( lin + 7, col + 25, &dp->parcelas, 10, ENTMODO_CAD );

        if( ret == -1 )
        {
            ret_wmsg_form( L"Cancelado pelo operador.", RETMSG_ERRO );
            return 0;
        }

        if( ret == 0 )
            ret_wmsg_form( L"Número inválido.", RETMSG_ERRO );
        else if( dp->parcelas < dp->parcela )
            ret_wmsg_form( L"número de parcelas menor que parcela inicial.", RETMSG_ERRO );
        else break;
    }

    setcurs( lin + 7, col );

    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
    printf( "Parcelas:              | " );

    setcolor_on( NLRED, NBLK, 1, NBRIG );
    printf( "%02d", dp->parcelas );
    setcolor_off();

    lco = menuh_lco( lin + 8, col );

    if( lco == -1 )
    {
        ret_wmsg_form( L"Cancelado pelo operador.", RETMSG_ERRO );
        return 0;
    }

    while( true )
    {
        cleanarea( lin + 9, col, lin + 9, col + 25 + 12 );
        setcurs( lin + 9, col );

        dp->valor = 0.00;

	    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
        printf( "Valor:                 | " );
        setcolor_off();
        ret = input_wdouble( lin + 9, col + 25, &dp->valor, 12, 2, ENTMODO_CAD );

        if( ret == -1 )
        {
            ret_wmsg_form( L"cancelado pelo operador.", RETMSG_ERRO );
            return 0;
        }

        if( ret == 0 )
            ret_wmsg_form( L"Número inválido.", RETMSG_ERRO );
        else if( dp->valor == 0.00 )
            ret_wmsg_form( L"valor não pode ser zero.", RETMSG_ERRO );
        else break;
    }

    if( lco ) dp->valor = dp->valor * ( -1.00 );

    setcurs( lin + 9, col );

    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
    printf( "Valor:                 | " );

    setcolor_on( dp->valor < 0 ? NBLU : NRED, NBLK, 1, NBRIG );
    imp_wdoub( fabs( dp->valor ), 12, 2, J_DIR );
    setcolor_off();
        
    return 1;
}

LN *exclui_manut( BR *b, LN *l )
{
    int *p_num_compra;
    int opc;
    int ret;
	struct despesa dp;
    FILE *arq;
    uint8_t lin = LIN_FORM_APP;
    uint8_t col = COL_FORM_APP;
    wchar_t moddesc[ 128 ];

    wcscpy( moddesc, nome_prog );
    wcscat( moddesc, L" - Exclusão" );

    p_num_compra = ( int * ) getlinedatacol_bw( l, 5 );
    
    setcurline_bw( b, l );

    arq = fopen( "despesas.dat", "rb+" );
    
    init_form( moddesc );

    if( !arq )
	{
        ret_wmsg_form( L"Falha na abertura do arquivo", RETMSG_ERRO );
		return l;
	}    

    rewind( arq );

    while( fread( &dp, sizeof( struct despesa ), 1, arq ) )
    {
        if( dp.id_compra == *p_num_compra && !dp.apagado )
        {
            resumo_compra( lin, col, *p_num_compra );
            break;
        }
    }

    opc = hdialog_button( L"Confirma a exclusão?", CONFIRM_DEFAULT_N );

    switch( opc )
    {
        case 1:
            ret = exc_parcs( arq, *p_num_compra );

            if( ret == 1 )
            {
                ret_wmsg_form( L"Exclusão realizada com sucesso", RETMSG_NO_ERRO );
                refresh_data = true;
            }
            else
                ret_wmsg_form( L"Erro na exclusão.", RETMSG_ERRO );
            
            break;
        case 0:
        case -1:
            ret_wmsg_form( L"Não confirmado pelo operador.", RETMSG_ERRO );
            break;
    }
    
    fclose( arq );

    return l;
}

int exc_parcs( FILE *arq, int num_compra )
{
    struct despesa dp;
    long int reg = 0;
    size_t cad = 0;
    int inc = 0;

    rewind( arq );

    while( fread( &dp, sizeof( struct despesa ), 1, arq ) )
    {
        if( dp.id_compra == num_compra && !dp.apagado )
        {
            fseek( arq, reg, SEEK_SET );

            dp.apagado = 1;

            wcscpy( dp.descricao, L" " );

            cad += fwrite( &dp, sizeof( struct despesa ), 1, arq );

            inc++;
        }

        reg = ftell( arq );
    }

    if( !cad ) return 0;

    if( cad != inc ) return -1;

    return 1;
}

LN *edita_manut( BR *b, LN *l )
{
    FILE *arq;
    struct despesa dp;
    struct despesa aux;
    double valor;
    long int reg = 0;
    int linhas = 0;
    int pagina = 1;
    int qtd_lin = 24;    
    int i_desc = 1;
    int *pn;
    int ret;
    wchar_t desc[ 128 ];
    bool ( *fi ) ( wint_t );
    uint8_t lin = LIN_FORM_APP;
    uint8_t col = COL_FORM_APP + 1;
    wchar_t moddesc[ 128 ];

    wcscpy( moddesc, nome_prog );
    wcscat( moddesc, L" - Edição" );
    
    setcurline_bw( b, l );

    fi = filter_input_no;
    
    pn = ( int * ) getlinedatacol_bw( l, 5 );

    init_form( moddesc );
    
    arq = fopen( "despesas.dat", "rb+" );

    if( !arq )
	{
        ret_wmsg_form( L"Falha na abertura do arquivo.", RETMSG_ERRO );
		return l;
	}        
    
    rewind( arq );

    while( fread( &dp, sizeof( struct despesa ), 1, arq ) )
    {
        if( !( dp.apagado ) && dp.id_compra == *pn  )
        {   
            if( i_desc )
            {
                aux = dp;

                setcurs( lin, col );

                mostra_dados( &dp );

                setcurs( lin + 2, col );

                setcolor_on( COR_MSG, NBLK, 1, NBRIG );
                printf( "Descrição ( ENTER para manter ): " );
                setcolor_off();
                
                wcscpy( desc, dp.descricao );
            
                if( input_wstr( lin + 2, col + 34, desc, 80, ENTMODO_EDIT, fi ) == -1 )
                {
                    fclose( arq );
                    ret_wmsg_form( L"Cancelado pelo operador.", RETMSG_ERRO );
                    return l;
                }

                if( !wcslen( desc ) )
                {
                    wcscpy( desc, dp.descricao );
                    break;
                }
                else
                {
                    wcscpy( aux.descricao, desc );
                    cleanarea( lin, col, lin + 2, COLD_FORM_APP );

                    mostra_dados( &aux );

                    ret = hdialog_button( L"Confirma?", CONFIRM_DEFAULT_Y );

                    if( ret == -1 )
                    {
                        fclose( arq );
                        ret_wmsg_form( L"Cancelado pelo operador.", RETMSG_ERRO );
                        return l;
                    }

                    if( ret == 0 ) break;
                }

                i_desc = 0;
            }

            wcscpy( dp.descricao, desc );

            fseek( arq, reg, SEEK_SET );
            
            ret = fwrite( &dp, sizeof( struct despesa ), 1, arq );

            if( ret != 1 )
            {
                fclose( arq );
                ret_wmsg_form( L"Erro na edicão da descricão.", RETMSG_ERRO );
                return l;
            }
        }
        
        reg = ftell( arq );
    }

    init_form( moddesc );
    
    setcurs( lin, col );
    setcolor_on( NLWHT, NBLK, 1, NBRIG );
    printf( "Página: " );
    setcolor_on( NLYEL, NBLK, 1, NBRIG );
    printf( "%02d", pagina );
    setcolor_off();

    reg = 0;

    lin += 2;
            
    rewind( arq );

    while( fread( &dp, sizeof( struct despesa ), 1, arq ) )
    {
        if( !( dp.apagado ) && dp.id_compra == *pn  )
        {   
            cleanarea( lin, col, lin, COLD_FORM_APP );
            setcolor_on( NLWHT, NBLK, 2, NBRIG, NBLKS );
            printf( "->" );
            setcolor_off();
            setcurs( lin, col + 4 );
            mostra_dados( &dp );

            setcurs( lin + 2, col );
            setcolor_on( COR_MSG, NBLK, 1, NBRIG );
            printf( "Valor ( ENTER para manter ): " );
            setcolor_off();

            valor = dp.valor;
            
            ret = input_wdouble( lin + 2, col + 29, &valor, 12, 2, ENTMODO_EDIT );
            
            if( ret == -1 )
            {
                ret_wmsg_form( L"Cancelado pelo operador.", RETMSG_ERRO );
                fclose( arq );
                return l;
            }
            
            if( ret == 0 )
            {
                ret_wmsg_form( L"Valor inválido.", RETMSG_ERRO );
                fclose( arq );
                return l;
            }

            cleanarea( lin + 2, col + 29, lin + 2, COLD_FORM_APP );
            setcolor_on( NRED, NBLK, 1, NBRIG );
            imp_wdoub( valor, 12, 2, J_DIR );
            
            ret = hdialog_button( L"Confirma?", CONFIRM_DEFAULT_Y );

            if( ret == -1 )
            {
                ret_wmsg_form( L"Cancelado pelo operador.", RETMSG_ERRO );
                fclose( arq );
                return l;
            }
                
            cleanarea( lin + 2, col, lin + 2, COLD_FORM_APP );

            if( ret == 1 )
            {
                
                dp.valor = valor == 0.0 ? dp.valor : valor;

                fseek( arq, reg, SEEK_SET );
                
                ret = fwrite( &dp, sizeof( struct despesa ), 1, arq );
                
                if( ret != 1 )
                {
                    ret_wmsg_form( L"Erro na edição do valor.", RETMSG_ERRO );
                    fclose( arq );
                    return l;
                }
            }
            
            cleanarea( lin, col, lin, COLD_FORM_APP );
            setcolor_on( NLYEL, NBLK, 1, NBRIG );
            printf( "Ok" );
            setcolor_off();
            setcurs( lin, col + 4 );
            mostra_dados( &dp );
            
            lin++;
            linhas++;
        }
        
        if( linhas == qtd_lin )
        {
            lin = LIN_FORM_APP;
            col = COL_FORM_APP;
            
            init_form( moddesc );
            
            ret = hdialog_button( L"Continua?", CONFIRM_DEFAULT_Y );

            if( ret == 0 || ret == -1 )    
            {
                ret_wmsg_form( L"Cancelado pelo operador.", RETMSG_ERRO );
                fclose( arq );
                return l;
            }

            init_form( moddesc );

            pagina++;

            setcurs( lin, col );
            setcolor_on( NLWHT, NBLK, 1, NBRIG );
            printf( "Página: " );
            setcolor_on( NLYEL, NBLK, 1, NBRIG );
            printf( "%02d", pagina );
            setcolor_off();

            linhas = 0;
        }
        
        reg = ftell( arq );
    }

    fclose( arq );

    refresh_data = true;
    
    ret_wmsg_form( L"Sucesso.", RETMSG_NO_ERRO );

    return l;
}

LN *dispcompra_manut( BR *b, LN *l )
{
    int *pn;
    int num;
    uint8_t lin = LIN_FORM_APP;
    uint8_t col = COL_FORM_APP;
    int8_t ret = -1;
    BR *browse = NULL;
    TB *tb = NULL;
    SB *sb = NULL;
    wchar_t *statusbar1[] = { L"[ESC]", L"[HOME]", L"[END]", L"[ENTER]", L"[F4]" };
    wchar_t *statusbar2[] = { L"Sai", L"Inicio", L"Fim", L"Edita", L"Liquida" };
    wchar_t moddesc[ 128 ];
    LN * ( *fedit )( BR *, LN * );
    LN * ( *fpaga )( BR *, LN * );

    wcscpy( moddesc, nome_prog );
    wcscat( moddesc, L" - Compra / Parcelas" );
    
    fedit = edita_parcela;
    fpaga = paga_parcela;

    setcurline_bw( b, l );

    pn = ( int * ) getlinedatacol_bw( l, 5 );

    num = *pn;

    browse = create_bw();

    setbox_bw( browse, BOX_MENU_BW );
    press_keys_bw( browse, ENTER, fedit );
    press_keys_bw( browse, F4, fpaga );

    tb = addtitlebar_bw( browse, L" Compra - Parcelas " );
    setcolortitlebar_bw( tb, NGRY, NBLK, 2, NBRIG, NREVE );

    for( uint8_t inc = 0; inc < 5; inc++ )
    {
        sb = addstatusbar_bw( browse, statusbar1[ inc ] );
        setcolorstatusbar_bw( sb, NGRY, NBLK, 2, NNORM, NREVE );
        
        sb = addstatusbar_bw( browse, statusbar2[ inc ] );
        setcolorstatusbar_bw( sb, NLCYN, NWHT, 3, NNORM, NITAL, NREVE );
    
        if( inc < 4 ) addstatusbar_bw( browse, L" " );
    }

    setdatatypekey_bw( browse, sizeof( int ) );
    setfunckey_bw( browse, cmpintline_bw );

    if( !dispcompra_dados( browse, num ) ) return l;

    do
    {
        init_form( moddesc );
        
        resumo_compra( lin, col, num );
        
        setcoord_bw( browse, lin + 13, col, getsize_bw( browse ) > 16 ? 16 : getsize_bw( browse ) );
    
        init_bw( browse );
        
        ret = exec_bw( browse );
    }
    while( ret != -1 );

    free_bw( browse );

    return l;
}

LN *edita_parcela( BR *b, LN *l )
{
    FILE *arq;
    struct despesa dp;
    long int reg = 0;
    double valor;
    int ret;
    int *pn;
    bool vazio_arq = true;
    bool erro_arq = false;
    uint8_t lin = getcoords_bw( b, COORD_BLIN ) + 2;
    uint8_t col = getcoords_bw( b, COORD_COL ) + 1;

    setcurline_bw( b, l );
    
    arq = fopen( "despesas.dat", "rb+" );

    if( !arq )
	{
        ret_wmsg_form( L"Falha na abertura do arquivo.", RETMSG_ERRO );
		return l;
	}        

    pn = ( int * ) getlinedatacol_bw( l, 1 );

    rewind( arq );

    while( fread( &dp, sizeof( struct despesa ), 1, arq ) )
    {
        if( !( dp.apagado ) && dp.id == *pn  )
        {
            if( dp.pago )
            {
                ret_wmsg_form( L"Parcela já liquidada.", RETMSG_ERRO );
                fclose( arq );
                return l;
            }
            
            vazio_arq = false;

            cleanarea( lin, col, lin, COLD_FORM_APP );
            setcolor_on( COR_MSG, NBLK, 1, NBRIG );
            printf( "Valor ( ENTER para manter ): " );
            setcolor_off();
            
            valor = dp.valor;
            
            ret = input_wdouble( lin, col + 29, &valor, 12, 2, ENTMODO_EDIT );
            
            if( ret == -1 )
            {
                cleanarea( lin, col, lin, COLD_FORM_APP );
                ret_wmsg_form( L"Cancelado pelo operador.", RETMSG_ERRO );
                fclose( arq );
                return l;
            }
            
            if( ret == 0 )
            {
                cleanarea( lin, col, lin, COLD_FORM_APP );
                ret_wmsg_form( L"Valor inválido.", RETMSG_ERRO );
                fclose( arq );
                return l;
            }

            cleanarea( lin, col + 29, lin + 2, COLD_FORM_APP );
            setcolor_on( NRED, NBLK, 1, NBRIG );
            imp_wdoub( valor, 12, 2, J_DIR );
            
            ret = hdialog_button( L"Confirma?", CONFIRM_DEFAULT_Y );

            if( ret == -1 )
            {
                cleanarea( lin, col, lin, COLD_FORM_APP );
                ret_wmsg_form( L"Cancelado pelo operador.", RETMSG_ERRO );
                fclose( arq );
                return l;
            }
                
            cleanarea( lin, col, lin, COLD_FORM_APP );

            if( ret == 1 )
            {
                dp.valor = valor == 0.0 ? dp.valor : valor;

                fseek( arq, reg, SEEK_SET );
                
                ret = fwrite( &dp, sizeof( struct despesa ), 1, arq );
                
                if( ret != 1 )
                {
                    ret_wmsg_form( L"Erro na edição do valor.", RETMSG_ERRO );
                    fclose( arq );
                    return l;
                }

                refresh_data = true;
            }

            break;
        }

        reg = ftell( arq );
    }

    if( ferror( arq ) ) erro_arq = true;

    fclose( arq );

    if( erro_arq )
    {
        ret_wmsg_form( L"Erro de acesso ao arquivo", RETMSG_ERRO );
        return l;
    }

    if( vazio_arq )
    {
        ret_wmsg_form( L"arquivo vazio", RETMSG_ERRO );
        return l;
    }

    if( !dispcompra_dados( b, dp.id_compra ) ) return l;

    refresh_data = true;

    return l;
}

LN *paga_parcela( BR *b, LN *l )
{
    FILE *arq;
    struct despesa dp;
    long int reg = 0;
    int ret;
    int *pn;
    bool vazio_arq = true;
    bool erro_arq = false;
    uint8_t lin = getcoords_bw( b, COORD_BLIN ) + 2;
    uint8_t col = getcoords_bw( b, COORD_COL ) + 1;

    setcurline_bw( b, l );
    
    arq = fopen( "despesas.dat", "rb+" );

    if( !arq )
	{
        ret_wmsg_form( L"Falha na abertura do arquivo.", RETMSG_ERRO );
		return l;
	}        

    pn = ( int * ) getlinedatacol_bw( l, 1 );

    rewind( arq );

    while( fread( &dp, sizeof( struct despesa ), 1, arq ) )
    {
        if( !( dp.apagado ) && dp.id == *pn  )
        {
            if( dp.pago )
            {
                ret_wmsg_form( L"Parcela já liquidada.", RETMSG_ERRO );
                fclose( arq );
                return l;
            }
            
            vazio_arq = false;
            
            ret = hdialog_button( L"Confirma a liquidação?", CONFIRM_DEFAULT_Y );

            if( ret == -1 )
            {
                cleanarea( lin, col, lin, COLD_FORM_APP );
                ret_wmsg_form( L"Cancelado pelo operador.", RETMSG_ERRO );
                fclose( arq );
                return l;
            }
                
            cleanarea( lin, col, lin, COLD_FORM_APP );

            if( ret == 1 )
            {
                dp.pago = 1;

                fseek( arq, reg, SEEK_SET );
                
                ret = fwrite( &dp, sizeof( struct despesa ), 1, arq );
                
                if( ret != 1 )
                {
                    ret_wmsg_form( L"Erro na liquidação.", RETMSG_ERRO );
                    fclose( arq );
                    return l;
                }
                
                refresh_data = true;
            }

            break;
        }

        reg = ftell( arq );
    }

    if( ferror( arq ) ) erro_arq = true;

    fclose( arq );

    if( erro_arq )
    {
        ret_wmsg_form( L"Erro de acesso ao arquivo", RETMSG_ERRO );
        return l;
    }

    if( vazio_arq )
    {
        ret_wmsg_form( L"arquivo vazio", RETMSG_ERRO );
        return l;
    }

    if( !dispcompra_dados( b, dp.id_compra ) ) return l;

    refresh_data = true;

    return l;
}

bool dispcompra_dados( BR *b, int num )
{
    FILE *arq;
	struct despesa dp;
    int cor_c;
    time_t segs_hoje;
    bool vazio_arq = true;
    bool erro_arq = false;
    wchar_t buffer[ 128 ];
    LN *li = NULL;
    CO *co = NULL;

    freelines_bw( b );
    
    segs_hoje = now_segs();
    
    arq = fopen( "despesas.dat", "rb" );

    if( !arq )
	{
        ret_wmsg_form( L"Falha na abertura do arquivo.", RETMSG_ERRO );
        return false;
	}

    rewind( arq );

    while( fread( &dp, sizeof( struct despesa ), 1, arq ) )
    {
        if( dp.id_compra == num && !( dp.apagado ) )
        {
            li = addline_bw( b, &dp.parcela, ( uint32_t ) dp.id, NULL ); 

            if( dp.pago )
            {
                wcscpy( buffer, L"Liquidada" );
                cor_c = NLBLU;
            }
            else
            {
                if( dp.data >= segs_hoje )
                {
                    wcscpy( buffer, L"A Vencer" );
                    cor_c = NLGRN;
                }
                else
                {
                    wcscpy( buffer, L"Vencida" );
                    cor_c = NRED;
                }
            }

            co = addnewcol_bw( b, li, buffer, DWCHAR ); // 0
            setattrcol_bw( co, wcslen( buffer ), 0, J_ESQ, MAIUSC );
            setcolorcol_bw( co, cor_c, NBLK, 1, NNORM );

            co = addnewcol_bw( b, li, &dp.id, DINT ); // 1
            setattrcol_bw( co, 6, 0, J_DIR, MINUSC );
            setcolorcol_bw( co, NGRY, NBLK, 1, NNORM );

            co = addnewcol_bw( b, li, &dp.parcela, DINT ); // 2
            setattrcol_bw( co, 6, 0, J_DIR, MINUSC );
            setcolorcol_bw( co, NWHT, NBLK, 1, NNORM );

            co = addnewcol_bw( b, li, &dp.data, DDATE ); // 3
            setattrcol_bw( co, 15, 0, J_CEN, MINUSC );
            setcolorcol_bw( co, NGRY, NBLK, 1, NNORM );

            co = addnewcol_bw( b, li, &dp.valor, DFLOAT ); // 9
            setattrcol_bw( co, 12, 2, J_DIR, MINUSC );
            setcolorcol_bw( co, dp.valor < 0 ? NBLU : NLRED, NBLK, 1, NNORM );

            vazio_arq = false;
        }
    }

    if( ferror( arq ) ) erro_arq = true;

    fclose( arq );

    if( erro_arq )
    {
        ret_wmsg_form( L"Erro de acesso ao arquivo", RETMSG_ERRO );
        return false;
    }

    if( vazio_arq )
    {
        ret_wmsg_form( L"arquivo vazio", RETMSG_ERRO );
        return false;
    }

    return true;
}

void resumo_compra( uint8_t lin, uint8_t col, int num_compra )
{
    FILE *arq;
	struct despesa dp;
    struct despesa aux;
    bool arq_vazio = true;
    double tot_ser = 0.0;
    wchar_t buffer[ 128 ];
    int cor_c;
    int inc = 0;
    char s[] = "\xe2\x97\x86";
    BR *browse = NULL;
    LN *l = NULL;
    CO *c = NULL;

    browse = create_bw();
    
    setbox_bw( browse, BOX_MENU_BW );
    setctrlget_bw( browse, false );
    setcoord_bw( browse, lin, col, 8 );
    setdatatypekey_bw( browse, sizeof( int ) );
    setfunckey_bw( browse, cmpintline_bw );

    arq = fopen( "despesas.dat", "rb" );

    if( !arq )
	{
        ret_wmsg_form( L"Falha na abertura do arquivo.", RETMSG_ERRO );
        return;
	}

    while( fread( &dp, sizeof( struct despesa ), 1, arq ) )
    {
        if( dp.id_compra == num_compra && !( dp.apagado ) )
        {
            aux = dp;
            tot_ser += dp.valor;
            arq_vazio = false;
        }
    }

    if( ferror( arq ) )
    {
        ret_wmsg_form( L"Erro de acesso ao arquivo", RETMSG_ERRO );
        fclose( arq );
        return;
    }

    fclose( arq );

    if( arq_vazio )
    {
        ret_wmsg_form( L"arquivo vazio", RETMSG_ERRO );
        return;
    }

    l = addline_bw( browse, &inc, ( uint32_t ) inc, NULL ); 

    wcscpy( buffer, L"Descrição" );
    
    c = addnewcol_bw( browse, l, s, DCHAR );
    setattrcol_bw( c, 1, 0, J_NAT, false );
    setcolorcol_bw( c, NYEL, NBLK, 1, NNORM );

    c = addnewcol_bw( browse, l, buffer, DWCHAR );
    setattrcol_bw( c, wcslen( buffer ), 0, J_ESQ, false );
    setcolorcol_bw( c, COR_MSG, NBLK, 1, NNORM );

    c = addnewcol_bw( browse, l, aux.descricao, DWCHAR );
    setattrcol_bw( c, wcslen( aux.descricao ), 0, J_ESQ, true );
    setcolorcol_bw( c, NYEL, NBLK, 1, NNORM );

    inc++;

    l = addline_bw( browse, &inc, ( uint32_t ) inc, NULL ); 

    wcscpy( buffer, L"Tipo" );
    
    c = addnewcol_bw( browse, l, s, DCHAR );
    setattrcol_bw( c, 1, 0, J_NAT, false );
    setcolorcol_bw( c, NCYN, NBLK, 1, NNORM );

    c = addnewcol_bw( browse, l, buffer, DWCHAR );
    setattrcol_bw( c, wcslen( buffer ), 0, J_ESQ, false );
    setcolorcol_bw( c, COR_MSG, NBLK, 1, NNORM );

    c = addnewcol_bw( browse, l, tp[ aux.tipo - 1 ].desc, DWCHAR );
    setattrcol_bw( c, wcslen( tp[ aux.tipo - 1 ].desc ), 0, J_ESQ, true );
    setcolorcol_bw( c, NCYN, NBLK, 1, NNORM );

    inc++;

    l = addline_bw( browse, &inc, ( uint32_t ) inc, NULL ); 

    if( aux.fixa )
    {
        cor_c = NMAG;
        wcscpy( buffer, L"Fixa" );
    }
    else
    {
        cor_c = NCYN;
        wcscpy( buffer, L"Variável" );
    }

    wcscpy( buffer, L"Estado" );
    c = addnewcol_bw( browse, l, s, DCHAR );
    setattrcol_bw( c, 1, 0, J_NAT, false );
    setcolorcol_bw( c, cor_c, NBLK, 1, NNORM );

    c = addnewcol_bw( browse, l, buffer, DWCHAR );
    setattrcol_bw( c, wcslen( buffer ), 0, J_ESQ, false );
    setcolorcol_bw( c, COR_MSG, NBLK, 1, NNORM );

    c = addnewcol_bw( browse, l, buffer, DWCHAR );
    setattrcol_bw( c, wcslen( buffer ), 0, J_ESQ, true );
    setcolorcol_bw( c, cor_c, NBLK, 1, NNORM );

    inc++;

    l = addline_bw( browse, &inc, ( uint32_t ) inc, NULL ); 

    wcscpy( buffer, L"Conta" );
    
    c = addnewcol_bw( browse, l, s, DCHAR );
    setattrcol_bw( c, 1, 0, J_NAT, false );
    setcolorcol_bw( c, ct[ aux.conta - 1 ].cor, NBLK, 1, NNORM );

    c = addnewcol_bw( browse, l, buffer, DWCHAR );
    setattrcol_bw( c, wcslen( buffer ), 0, J_ESQ, false );
    setcolorcol_bw( c, COR_MSG, NBLK, 1, NNORM );

    c = addnewcol_bw( browse, l, ct[ aux.conta - 1 ].descc, DWCHAR );
    setattrcol_bw( c, wcslen( ct[ aux.conta - 1 ].descc ), 0, J_ESQ, true );
    setcolorcol_bw( c, ct[ aux.conta - 1 ].cor, NBLK, 1, NNORM );

    inc++;

    l = addline_bw( browse, &inc, ( uint32_t ) inc, NULL ); 

    wcscpy( buffer, L"Compra" );
    
    c = addnewcol_bw( browse, l, s, DCHAR );
    setattrcol_bw( c, 1, 0, J_NAT, false );
    setcolorcol_bw( c, NGRN, NBLK, 1, NNORM );

    c = addnewcol_bw( browse, l, buffer, DWCHAR );
    setattrcol_bw( c, wcslen( buffer ), 0, J_ESQ, false );
    setcolorcol_bw( c, COR_MSG, NBLK, 1, NNORM );

    c = addnewcol_bw( browse, l, &num_compra, DINT );
    setattrcol_bw( c, 6, 0, J_ESQ, false );
    setcolorcol_bw( c, NGRN, NBLK, 1, NNORM );

    inc++;

    l = addline_bw( browse, &inc, ( uint32_t ) inc, NULL ); 

    wcscpy( buffer, L"Data Compra" );
    
    c = addnewcol_bw( browse, l, s, DCHAR );
    setattrcol_bw( c, 1, 0, J_NAT, false );
    setcolorcol_bw( c, NGRY, NBLK, 1, NNORM );

    c = addnewcol_bw( browse, l, buffer, DWCHAR );
    setattrcol_bw( c, wcslen( buffer ), 0, J_ESQ, false );
    setcolorcol_bw( c, COR_MSG, NBLK, 1, NNORM );

    c = addnewcol_bw( browse, l, &aux.data_reg, DDATE );
    setattrcol_bw( c, 15, 0, J_ESQ, false );
    setcolorcol_bw( c, NGRY, NBLK, 1, NNORM );

    inc++;

    l = addline_bw( browse, &inc, ( uint32_t ) inc, NULL ); 

    wcscpy( buffer, L"Parcelas" );
    
    c = addnewcol_bw( browse, l, s, DCHAR );
    setattrcol_bw( c, 1, 0, J_NAT, false );
    setcolorcol_bw( c, NWHT, NBLK, 1, NNORM );

    c = addnewcol_bw( browse, l, buffer, DWCHAR );
    setattrcol_bw( c, wcslen( buffer ), 0, J_ESQ, false );
    setcolorcol_bw( c, COR_MSG, NBLK, 1, NNORM );

    c = addnewcol_bw( browse, l, &aux.parcelas, DINT );
    setattrcol_bw( c, 5, 0, J_ESQ, false );
    setcolorcol_bw( c, NWHT, NBLK, 1, NNORM );

    inc++;

    l = addline_bw( browse, &inc, ( uint32_t ) inc, NULL ); 

    wcscpy( buffer, L"Valor" );
    
    c = addnewcol_bw( browse, l, s, DCHAR );
    setattrcol_bw( c, 1, 0, J_NAT, false );
    setcolorcol_bw( c, tot_ser < 0 ? NBLU : NLRED, NBLK, 1, NNORM );

    c = addnewcol_bw( browse, l, buffer, DWCHAR );
    setattrcol_bw( c, wcslen( buffer ), 0, J_ESQ, false );
    setcolorcol_bw( c, COR_MSG, NBLK, 1, NNORM );

    c = addnewcol_bw( browse, l, &tot_ser, DFLOAT );
    setattrcol_bw( c, 10, 2, J_ESQ, false );
    setcolorcol_bw( c, tot_ser < 0 ? NBLU : NLRED, NBLK, 1, NNORM );

    setcolor_off();

    init_bw( browse );
    exec_bw( browse );

    free_bw( browse );

    return;
}

LN *con_descricao( BR *b, LN *l )
{
    if( !b ) return NULL;
    if( !l ) return NULL;
    
    wchar_t text[] = L"Descrição: ";
    int ( *fc ) ( const void *, const void * );
    int lin;
    int col = getcoords_bw( b, COORD_COL ) + wcslen( text ) + 2;
    int ret;

    wchar_t buffer[ 128 ] = L"";
    bool ( *fi ) ( wint_t );

    fi = filter_input_no;
    fc = cmpwstrparcsearch_bw;
    /* fc = cmpwstrexactsearch_bw; */
    
    lin = setsearchbar_bw( b, text );

    ret = input_wstr( lin, col, buffer, 42, ENTMODO_CAD, fi );
    
    if( ret == -1 || ret == 0 ) return NULL;

    return searchcol_bw( b, l, 0, buffer, fc );
}

LN *consulta_tipo( BR *b, LN *l )
{
    if( !b ) return NULL;
    if( !l ) return NULL;
    
    wchar_t text[] = L"Tipo: ";
    int ( *fc ) ( const void *, const void * );
    int lin;
    int col = getcoords_bw( b, COORD_COL ) + wcslen( text ) + 2;
    int ret;

    wchar_t buffer[ 128 ] = L"";
    bool ( *fi ) ( wint_t );

    fi = filter_input_no;
    fc = cmpwstrparcsearch_bw;
    /* fc = cmpwstrexactsearch_bw; */
    
    lin = setsearchbar_bw( b, text );

    ret = input_wstr( lin, col, buffer, 30, ENTMODO_CAD, fi );
    
    if( ret == -1 || ret == 0 ) return NULL;

    return searchcol_bw( b, l, 0, buffer, fc );
}

LN *filtro_manut( BR *b, LN *l )
{
    int retnum;
    int mes, ano, mes_prox, ano_prox;
    size_t t;
    struct pesquisa ps;
    bool ( *fi ) ( wint_t );
    uint8_t lin = LIN_FORM_APP;
    uint8_t col = COL_FORM_APP;
    wchar_t moddesc[ 128 ];

    /* fi = filter_input_num; */
    fi = filter_input_no;

    wcscpy( moddesc, nome_prog );
    wcscat( moddesc, L" - Filtros" );
    
    init_form( moddesc );
    
    setcurs( lin + 1, col + 1 );
    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
    printf( "Descrição: " );
    setcolor_off();

    wcscpy( ps.descricao, L"" );
    
    if( input_wstr( lin + 1, col + 12, ps.descricao, 42, ENTMODO_CAD, fi ) == -1 )
    {
        ret_wmsg_form( L"Cancelado pelo operador.", RETMSG_ERRO );
        return l;
    }

    setcurs( lin + 1, col + 12 );
    setcolor_on( NBLU, NBLK, 1, NBRIG );

    t = wcslen( ps.descricao );

    if( t )
    {
        cleanarea( lin + 1, col + 12, lin + 1, col + 12 + t + 1 ); 
        imp_wstr( ps.descricao, t, J_ESQ, MAIUSC );
    }
        
    setcurs( lin + 2, col + 1 );
    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
    printf( "Número da compra: " );
    setcolor_off();

    ps.num_compra = 0;

    retnum = input_wint( lin + 2, col + 20, &ps.num_compra, 6, ENTMODO_EDIT );

    if( retnum == -1 )
    {
        ret_wmsg_form( L"Cancelado pelo operador.", RETMSG_ERRO );
        return l;
    }
    
    if( retnum == 0 )
    {
        ret_wmsg_form( L"Número inválido.", RETMSG_ERRO );
        return l;
    }

    setcurs( lin + 2, col + 20 );
    
    setcolor_on( NBLU, NBLK, 1, NBRIG );

    cleanarea( lin + 2, col + 20, lin + 2, col + 26 );

    if( ps.num_compra > 0 )
        imp_wint( ps.num_compra, 6, 0, J_DIR );
    else
        ps.num_compra = 0;

    setcurs( lin + 3, col + 1 );
    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
    printf( "Periodo: " );
    setcolor_off();

    if( ( ps.periodo = input_daterange( lin + 3, col + 11, &ps.segs_ini, &ps.segs_fin ) ) == -1 ||
        !filtra_conta( lin + 4, col + 1 ) ||
        ( ps.estado = filtra_estado( lin + 5, col + 1 ) ) == -1 ||
        ( ps.situacao = filtra_situacao( lin + 6, col + 1 ) ) == -1 ||
        ( ps.tipo = filtra_tipo( lin + 7, col + 1 ) ) == -1 ||
        ( ps.parcela = filtra_parcelas( lin + 8, col + 1, &ps.parc_min, &ps.parc_max ) ) == -1 ||
        ( ps.ordem = filtra_ordenacao( lin + 9, col + 1 ) ) == -1 )
    {
        ret_wmsg_form( L"Cancelado pelo operador.", RETMSG_ERRO );
        return l;
    }

    if( ps.periodo == 2 )
    {
        mes = decomp_date( ps.segs_ini, 2 );
        ano = decomp_date( ps.segs_ini, 3 );

        ps.segs_ini = date_segs( 20, mes, ano );

        mes_prox = mes + 1;
        ano_prox = ano;

        if( mes_prox > 12 )
        {
            mes_prox = 1;
            ano_prox = ano + 1;
        }

        ps.segs_fin = date_segs( 19, mes_prox, ano_prox );
    }

    pesq.periodo = ps.periodo;
    pesq.estado = ps.estado;
    pesq.situacao = ps.situacao;
    pesq.tipo = ps.tipo;
    pesq.parcela = ps.parcela;
    pesq.ordem = ps.ordem;
    pesq.num_compra = ps.num_compra;
    pesq.parc_min = ps.parc_min;
    pesq.parc_max = ps.parc_max;
    pesq.segs_ini = ps.segs_ini;
    pesq.segs_fin = ps.segs_fin;
    wcscpy( pesq.descricao, ps.descricao );

    carrega_dados( b );

    return l;
}

int filtra_conta( int lin, int col )
{
    BR *cta = NULL;
    BR *ctatodas = NULL;
    LN *li = NULL;
    CO *co = NULL;
    TB *tb = NULL;
    SB *sb = NULL;
    wchar_t *statusbar1[] = { L"[ESC]", L"[ESPAÇO]" };
    wchar_t *statusbar2[] = { L"Sai", L"Marca" };
    wchar_t *statusbar3[] = { L"[ESC]", L"[HOME]", L"[END]", L"[ESPAÇO]" };
    wchar_t *statusbar4[] = { L"Sai", L"Inicio", L"Fim", L"Marca" };

    int s = 0;
    int todas;
    int contas;
    wchar_t titulo[ 80 ] = L"Conta: ";

    cleanarea( lin, col, lin, col + wcslen( titulo ) + 1 );

    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
    imp_wstr( titulo, wcslen( titulo ), J_ESQ, false );

    ctatodas = create_bw();

    setbox_bw( ctatodas, BOX_MENU_BW );
    setcolor_bw( ctatodas, COLOR_FOCUS_BW, NWHT, 238, 1, NNORM );
    setcoord_bw( ctatodas, lin, col + wcslen( titulo ), 1 );
    setctrlpercent_bw( ctatodas, false );

    for( uint8_t inc = 0; inc < 2; inc++ )
    {
        sb = addstatusbar_bw( ctatodas, statusbar1[ inc ] );
        setcolorstatusbar_bw( sb, NGRY, NBLK, 2, NNORM, NREVE );
        
        sb = addstatusbar_bw( ctatodas, statusbar2[ inc ] );
        setcolorstatusbar_bw( sb, NLCYN, NWHT, 3, NNORM, NITAL, NREVE );
    
        if( inc < 1 ) addstatusbar_bw( ctatodas, L" " );
    }

    tb = addtitlebar_bw( ctatodas, L" Contas " );
    setcolortitlebar_bw( tb, NGRY, NBLK, 2, NBRIG, NREVE );

    li = addline_bw( ctatodas, NULL, 1, f_void );
    setcheckline_bw( li, true );
    
    co = addnewcol_bw( ctatodas, li, NULL, CHKBOX );
    setattrcol_bw( co, 3, 2, J_CEN, MINUSC );
    setcolorcol_bw( co, NLYEL, NBLK, 1, NNORM );
    setshowdbarcol_bw( co, false );

    co = addnewcol_bw( ctatodas, li, L"Todas", DWCHAR );
    setattrcol_bw( co, 10, 0, J_ESQ, false );
    setcolorcol_bw( co, NGRY, NBLK, 1, NNORM );

    init_bw( ctatodas );

    todas = exec_bw( ctatodas );

    if( todas == -1 )
    {
        free_bw( ctatodas );
        return 0;
    }

    if( getcheckline_bw( li ) )
    {
        for( int8_t inc = 0; inc < T_CONTA; inc++ ) ct[ inc ].ctrl = 1;
        setcolor_on( NBLU, NBLK, 1, NBRIG );
        printf( "Todas" );
    }
    else
    {
        cta = create_bw();

        setbox_bw( cta, BOX_MENU_BW );
        setcolor_bw( cta, COLOR_FOCUS_BW, NWHT, 238, 1, NNORM );
        setcoord_bw( cta, lin, col + wcslen( titulo ), T_CONTA );
        setctrlpercent_bw( cta, false );

        tb = addtitlebar_bw( cta, L" Contas " );
        setcolortitlebar_bw( tb, NGRY, NBLK, 2, NBRIG, NREVE );

        for( uint8_t inc = 0; inc < 4; inc++ )
        {
            sb = addstatusbar_bw( cta, statusbar3[ inc ] );
            setcolorstatusbar_bw( sb, NGRY, NBLK, 2, NNORM, NREVE );
        
            sb = addstatusbar_bw( cta, statusbar4[ inc ] );
            setcolorstatusbar_bw( sb, NLCYN, NWHT, 3, NNORM, NITAL, NREVE );
    
            if( inc < 3 ) addstatusbar_bw( cta, L" " );
        }

        for( uint8_t inc = 0; inc < T_CONTA; inc++ )
        {
            li = addline_bw( cta, NULL, inc, f_void );
    
            co = addnewcol_bw( cta, li, NULL, CHKBOX );
            setattrcol_bw( co, 3, 2, J_CEN, MINUSC );
            setcolorcol_bw( co, NLYEL, NBLK, 1, NNORM );
            setshowdbarcol_bw( co, false );

            co = addnewcol_bw( cta, li, ct[ inc ].descc, DWCHAR );
            setattrcol_bw( co, 30, 0, J_ESQ, false );
            setcolorcol_bw( co, NGRY, NBLK, 1, NNORM );
        }

        for( uint8_t inc = 0; inc < T_CONTA; inc++ ) ct[ inc ].ctrl = 0;

        init_bw( cta );

        contas = exec_bw( cta );

        if( contas == -1 )
        {
            free_bw( ctatodas );
            free_bw( cta );
            return 0;
        }

        li = getini_bw( cta );

        uint8_t inc = 0;

        while( li )
        {
            if( getcheckline_bw( li ) )
            {
                ct[ inc ].ctrl = 1;
                setcolor_on( ct[ inc ].cor, NBLK, 1, NBRIG );
                imp_wstr( ct[ inc ].desc, wcslen( ct[ inc ].desc ) + 1, J_ESQ, false );
                s = 1;
            }

            li = moveline_bw( li );
            inc++;
        }

        if( !s ) for( uint8_t inc = 0; inc < T_CONTA; inc++ ) ct[ inc ].ctrl = 1;

        free_bw( cta );
    }
    
    free_bw( ctatodas );

    return 1;
}

int filtra_estado( int lin, int col )
{
    BR *menu = NULL;
    LN *li = NULL;
    CO *co = NULL;
    TB *tb = NULL;
    SB *sb = NULL;
    int ret;
    wchar_t titulo[ 80 ] = L"Estado: ";
    wchar_t *opcs[] = { L"Todos", L"Fixas", L"Variáveis" };
    wchar_t *statusbar1[] = { L"[ESC]", L"[HOME]", L"[END]" };
    wchar_t *statusbar2[] = { L"Sai", L"Inicio", L"Fim" };

    cleanarea( lin, col, lin, col + wcslen( titulo ) + 1 );
    
    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
    imp_wstr( titulo, wcslen( titulo ), J_ESQ, false );
   
    menu = create_bw();

    setbox_bw( menu, BOX_MENU_BW );
    setcolor_bw( menu, COLOR_FOCUS_BW, NWHT, 238, 1, NNORM );
    setcoord_bw( menu, lin, col + wcslen( titulo ), 3 );
    setctrlpercent_bw( menu, false );

    for( uint8_t inc = 0; inc < 3; inc++ )
    {
        sb = addstatusbar_bw( menu, statusbar1[ inc ] );
        setcolorstatusbar_bw( sb, NGRY, NBLK, 2, NNORM, NREVE );
        
        sb = addstatusbar_bw( menu, statusbar2[ inc ] );
        setcolorstatusbar_bw( sb, NLCYN, NWHT, 3, NNORM, NITAL, NREVE );
    
        if( inc < 2 ) addstatusbar_bw( menu, L" " );
    }

    tb = addtitlebar_bw( menu, L" Estado " );
    setcolortitlebar_bw( tb, NGRY, NBLK, 2, NBRIG, NREVE );

    for( uint8_t inc = 0; inc < 3; inc++ )
    {
        li = addline_bw( menu, NULL, inc, f_void );

        co = addnewcol_bw( menu, li, opcs[ inc ], DWCHAR );
        setattrcol_bw( co, 15, 0, J_ESQ, false );
        setcolorcol_bw( co, NGRY, NBLK, 1, NNORM );
    }

    init_bw( menu );

    ret = exec_bw( menu );

    if( ret == -1 )
    {
        free_bw( menu );
        return -1;
    }

    setcurs( lin, col + wcslen( titulo ) );
    setcolor_on( NBLU, NBLK, 1, NBRIG );
    imp_wstr( ( wchar_t * ) getdatacol_bw( menu, ret, 0 ), 10, J_ESQ, false );

    free_bw( menu );

    return ret;
}

int filtra_situacao( int lin, int col )
{
    BR *menu = NULL;
    LN *li = NULL;
    CO *co = NULL;
    TB *tb = NULL;
    SB *sb = NULL;
    wchar_t titulo[ 80 ] = L"Situação: ";
    wchar_t *opcs[] = { L"Todas", L"Em ser", L"Pagas" };
    wchar_t *statusbar1[] = { L"[ESC]", L"[HOME]", L"[END]" };
    wchar_t *statusbar2[] = { L"Sai", L"Inicio", L"Fim" };
    int ret;

    cleanarea( lin, col, lin, col + wcslen( titulo ) + 1 );
    
    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
    imp_wstr( titulo, wcslen( titulo ), J_ESQ, false );
   
    menu = create_bw();

    setbox_bw( menu, BOX_MENU_BW );
    setcolor_bw( menu, COLOR_FOCUS_BW, NWHT, 238, 1, NNORM );
    setcoord_bw( menu, lin, col + wcslen( titulo ), 3 );
    setctrlpercent_bw( menu, false );

    for( uint8_t inc = 0; inc < 3; inc++ )
    {
        sb = addstatusbar_bw( menu, statusbar1[ inc ] );
        setcolorstatusbar_bw( sb, NGRY, NBLK, 2, NNORM, NREVE );
        
        sb = addstatusbar_bw( menu, statusbar2[ inc ] );
        setcolorstatusbar_bw( sb, NLCYN, NWHT, 3, NNORM, NITAL, NREVE );
    
        if( inc < 2 ) addstatusbar_bw( menu, L" " );
    }

    tb = addtitlebar_bw( menu, L" Situação " );
    setcolortitlebar_bw( tb, NGRY, NBLK, 2, NBRIG, NREVE );

    for( uint8_t inc = 0; inc < 3; inc++ )
    {
        li = addline_bw( menu, NULL, inc, f_void );

        co = addnewcol_bw( menu, li, opcs[ inc ], DWCHAR );
        setattrcol_bw( co, 15, 0, J_ESQ, false );
        setcolorcol_bw( co, NGRY, NBLK, 1, NNORM );
    }

    init_bw( menu );

    ret = exec_bw( menu );

    if( ret == -1 )
    {
        free_bw( menu );
        return -1;
    }

    setcurs( lin, col + wcslen( titulo ) );
    setcolor_on( NBLU, NBLK, 1, NBRIG );
    imp_wstr( ( wchar_t * ) getdatacol_bw( menu, ret, 0 ), 10, J_ESQ, false );

    free_bw( menu );

    return ret;
}

int filtra_parcelas( int lin, int col, int *min_parc, int *max_parc )
{
    BR *menu = NULL;
    LN *li = NULL;
    CO *co = NULL;
    TB *tb = NULL;
    SB *sb = NULL;
    uint8_t ttit = 0;
    uint8_t tbuf = 0;
    int ret;
    int parcs = 0;
    int parcela;
    wchar_t buffer[ 128 ];  
    wchar_t titulo[ 80 ] = L"Parcelamento: ";
    wchar_t *opcs[] = { L"Todos", L"A vista", L"Parceladas", L"Parceladas com última parcela", 
                        L"Parceladas com parcelas iguais a" };
    wchar_t *statusbar1[] = { L"[ESC]", L"[HOME]", L"[END]" };
    wchar_t *statusbar2[] = { L"Sai", L"Inicio", L"Fim" };


    cleanarea( lin, col, lin, col + wcslen( titulo ) + 1 );
    
    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
    imp_wstr( titulo, wcslen( titulo ), J_ESQ, false );
   
    menu = create_bw();

    setbox_bw( menu, BOX_MENU_BW );
    setcolor_bw( menu, COLOR_FOCUS_BW, NWHT, 238, 1, NNORM );
    setcoord_bw( menu, lin, col + wcslen( titulo ), 5 );
    setctrlpercent_bw( menu, false );

    for( uint8_t inc = 0; inc < 3; inc++ )
    {
        sb = addstatusbar_bw( menu, statusbar1[ inc ] );
        setcolorstatusbar_bw( sb, NGRY, NBLK, 2, NNORM, NREVE );
        
        sb = addstatusbar_bw( menu, statusbar2[ inc ] );
        setcolorstatusbar_bw( sb, NLCYN, NWHT, 3, NNORM, NITAL, NREVE );
    
        if( inc < 2 ) addstatusbar_bw( menu, L" " );
    }

    tb = addtitlebar_bw( menu, L" Parcelamento " );
    setcolortitlebar_bw( tb, NGRY, NBLK, 2, NBRIG, NREVE );

    for( uint8_t inc = 0; inc < 5; inc++ )
    {
        li = addline_bw( menu, NULL, inc, f_void );

        co = addnewcol_bw( menu, li, opcs[ inc ], DWCHAR );
        setattrcol_bw( co, 35, 0, J_ESQ, false );
        setcolorcol_bw( co, NGRY, NBLK, 1, NNORM );
    }

    init_bw( menu );

    parcela = exec_bw( menu );

    ttit = wcslen( titulo );
        
    *min_parc = 1;
    *max_parc = 99;

    curs_show();
    
    switch( parcela )
	{
        case 0:
            break;    
        case 1:
            *max_parc = 1;
			break;
		case 2:
            setcurs( lin, col + ttit ); 
            setcolor_on( NWHT, NBLK, 1, NBRIG );
            wcscpy( buffer, L"No máximo quantas parcelas ( ENTER para todas )? : " );
            
            tbuf = wcslen( buffer );
            
            imp_wstr( buffer, tbuf, J_ESQ, false );
            setcolor_off();
            
            ret = input_wint( lin, col + ttit + tbuf, &parcs, 10, ENTMODO_CAD );

            if( ret == -1 ) return - 1;

            if( ret == 0 ) parcs = 0;

            cleanarea( lin, col + ttit, lin, col + ttit + tbuf );

            *min_parc = 2;
            *max_parc = parcs < *min_parc ? *max_parc : parcs; 
			break;
        case 3:
            break;
        case 4:
            setcurs( lin, col + ttit ); 
            setcolor_on( NWHT, NBLK, 1, NBRIG );
            wcscpy( buffer, L"Quantas parcelas ( ENTER para duas )? : " );
            
            tbuf = wcslen( buffer );
            
            imp_wstr( buffer, tbuf, J_ESQ, false );
            setcolor_off();
            
            ret = input_wint( lin, col + ttit + tbuf, &parcs, 10, ENTMODO_CAD );
            
            if( ret == -1  ) return - 1;

            if( ret == 0 ) parcs = 0;
            
            cleanarea( lin, col + ttit, lin, col + ttit + tbuf );
            
            *max_parc = parcs < 2 ? 2 : parcs;
            break;    
		case -1:
            free_bw( menu );
            return -1;
	}

    setcurs( lin, col + wcslen( titulo ) );
    setcolor_on( NBLU, NBLK, 1, NBRIG );

    if( parcela == 0 ) printf( "Todas" );
    if( parcela == 1 ) printf( "A vista" );
    
    if( parcela == 2 )
    {
        if( *max_parc != 99 ) printf( "Parceladas em no máximo %2d parcelas", *max_parc );
        else printf( "Todas as parceladas" );
    }
    
    if( parcela == 3 ) printf( "Parceladas com última parcela" );
    
    if( parcela == 4 ) printf( "Parceladas com %2d parcelas", *max_parc );

    free_bw( menu );

    return parcela;
}

int filtra_ordenacao( int lin, int col )
{
    BR *menu = NULL;
    LN *li = NULL;
    CO *co = NULL;
    TB *tb = NULL;
    SB *sb = NULL;
    int ret;
    wchar_t titulo[ 80 ] = L"Ordem: ";
    wchar_t *opcs[] = { L"Por data", L"Por ID", L"Por descrição", L"Por valor", 
                        L"Por data e valor", L"Por data da compra" };
    wchar_t *statusbar1[] = { L"[ESC]", L"[HOME]", L"[END]" };
    wchar_t *statusbar2[] = { L"Sai", L"Inicio", L"Fim" };

    cleanarea( lin, col, lin, col + wcslen( titulo ) + 1 );
    
    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
    imp_wstr( titulo, wcslen( titulo ), J_ESQ, false );
   
    menu = create_bw();

    setbox_bw( menu, BOX_MENU_BW );
    setcolor_bw( menu, COLOR_FOCUS_BW, NWHT, 238, 1, NNORM );
    setcoord_bw( menu, lin, col + wcslen( titulo ), 6 );
    setctrlpercent_bw( menu, false );

    for( uint8_t inc = 0; inc < 3; inc++ )
    {
        sb = addstatusbar_bw( menu, statusbar1[ inc ] );
        setcolorstatusbar_bw( sb, NGRY, NBLK, 2, NNORM, NREVE );
        
        sb = addstatusbar_bw( menu, statusbar2[ inc ] );
        setcolorstatusbar_bw( sb, NLCYN, NWHT, 3, NNORM, NITAL, NREVE );
    
        if( inc < 2 ) addstatusbar_bw( menu, L" " );
    }

    tb = addtitlebar_bw( menu, L" Ordenação ( crescente ) " );
    setcolortitlebar_bw( tb, NGRY, NBLK, 2, NBRIG, NREVE );

    for( uint8_t inc = 0; inc < 6; inc++ )
    {
        li = addline_bw( menu, NULL, inc, f_void );

        co = addnewcol_bw( menu, li, opcs[ inc ], DWCHAR );
        setattrcol_bw( co, 25, 0, J_ESQ, false );
        setcolorcol_bw( co, NGRY, NBLK, 1, NNORM );
    }

    init_bw( menu );

    ret = exec_bw( menu );

    if( ret == -1 )
    {
        free_bw( menu );
        return -1;
    }

    setcurs( lin, col + wcslen( titulo ) );
    setcolor_on( NBLU, NBLK, 1, NBRIG );

    imp_wstr( opcs[ ret ], 25, J_ESQ, false );

    free_bw( menu );

    return ret;
}

int filtra_tipo( int lin, int col )
{
    BR *todos = NULL;
    BR *menu = NULL;
    LN *li = NULL;
    CO *co = NULL;
    TB *tb = NULL;
    SB *sb = NULL;
    LN * ( *fcon )( BR *, LN * );
    int ret;
    wchar_t dwchar[ 128 ];
    wchar_t titulo[ 80 ] = L"Tipo: ";
    wchar_t *t[ T_TIPO ];
    wchar_t *statusbar1[] = { L"[ESC]", L"[ESPAÇO]" };
    wchar_t *statusbar2[] = { L"Sai", L"Marca" };
    wchar_t *statusbar3[] = { L"[ESC]", L"[HOME]", L"[END]", L"[F2]" };
    wchar_t *statusbar4[] = { L"Sai", L"Inicio", L"Fim", L"Pesquisa" };

    for( uint8_t inc = 0; inc < T_TIPO; inc++ ) t[ inc ] = tp[ inc ].desc;

    cleanarea( lin, col, lin, col + wcslen( titulo ) + 1 );

    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
    imp_wstr( titulo, wcslen( titulo ), J_ESQ, false );

    todos = create_bw();

    setbox_bw( todos, BOX_MENU_BW );
    setcolor_bw( todos, COLOR_FOCUS_BW, NWHT, 238, 1, NNORM );
    setcoord_bw( todos, lin, col + wcslen( titulo ), 1 );
    setctrlpercent_bw( todos, false );

    for( uint8_t inc = 0; inc < 2; inc++ )
    {
        sb = addstatusbar_bw( todos, statusbar1[ inc ] );
        setcolorstatusbar_bw( sb, NGRY, NBLK, 2, NNORM, NREVE );
        
        sb = addstatusbar_bw( todos, statusbar2[ inc ] );
        setcolorstatusbar_bw( sb, NLCYN, NWHT, 3, NNORM, NITAL, NREVE );
    
        if( inc < 1 ) addstatusbar_bw( todos, L" " );
    }

    tb = addtitlebar_bw( todos, L" Tipo " );
    setcolortitlebar_bw( tb, NGRY, NBLK, 2, NBRIG, NREVE );

    li = addline_bw( todos, NULL, 0, f_void );
    setcheckline_bw( li, true );
    
    co = addnewcol_bw( todos, li, NULL, CHKBOX );
    setattrcol_bw( co, 3, 2, J_CEN, MINUSC );
    setcolorcol_bw( co, NLYEL, NBLK, 1, NNORM );
    setshowdbarcol_bw( co, false );

    co = addnewcol_bw( todos, li, L"Todos", DWCHAR );
    setattrcol_bw( co, 10, 0, J_ESQ, false );
    setcolorcol_bw( co, NGRY, NBLK, 1, NNORM );

    init_bw( todos );

    ret = exec_bw( todos );

    if( ret == -1 )
    {
        free_bw( todos );
        return 0;
    }

    if( getcheckline_bw( li ) )
    {
        setcurs( lin, col + wcslen( titulo ) );
        setcolor_on( NBLU, NBLK, 1, NBRIG );
        printf( "Todos" );
    }
    else
    {
        fcon = consulta_tipo;

        menu = create_bw();
    
        press_keys_bw( menu, F2, fcon );
        setdatatypekey_bw( menu, sizeof( dwchar ) );
        setfunckey_bw( menu, cmpwcharline_bw );

        setbox_bw( menu, BOX_BROWSE_BW );
        setcolor_bw( menu, COLOR_FOCUS_BW, NWHT, 238, 1, NNORM );
        setcoord_bw( menu, lin, col + wcslen( titulo ), 15 );
        setctrlpercent_bw( menu, true );

        tb = addtitlebar_bw( menu, L" Tipo " );
        setcolortitlebar_bw( tb, NGRY, NBLK, 2, NBRIG, NREVE );

        for( uint8_t inc = 0; inc < 4; inc++ )
        {
            sb = addstatusbar_bw( menu, statusbar3[ inc ] );
            setcolorstatusbar_bw( sb, NGRY, NBLK, 2, NNORM, NREVE );
        
            sb = addstatusbar_bw( menu, statusbar4[ inc ] );
            setcolorstatusbar_bw( sb, NLCYN, NWHT, 3, NNORM, NITAL, NREVE );
    
            if( inc < 3 ) addstatusbar_bw( menu, L" " );
        }

        for( uint8_t inc = 0; inc < T_TIPO; inc++ )
        {
            li = addline_bw( menu, t[ inc ], ( uint32_t ) inc + 1, f_void ); 

            co = addnewcol_bw( menu, li, t[ inc ], DWCHAR ); // 0
            setattrcol_bw( co, 50, 0, J_ESQ, MINUSC );
            setcolorcol_bw( co, NGRY, NBLK, 1, NBRIG );
        }

        init_bw( menu );

        ret = exec_bw( menu );

        if( ret == -1 )
        {
            free_bw( todos );
            free_bw( menu );
            return 0;
        }

        setcurs( lin, col + wcslen( titulo ) );
        setcolor_on( NBLU, NBLK, 1, NBRIG );
        imp_wstr( ( wchar_t * ) getdatacol_bw( menu, ret, 0 ), 50, J_ESQ, false );

        free_bw( menu );
    }

    free_bw( todos );

    return ret;
}

void mostra_filtros( int lin, int col )
{
    int lin_d = lin + 9 + 1;
    int col_d = col + 22;
    int mes;
    int ano;
    int i;
    wchar_t *t[ T_TIPO + 1 ];
    wchar_t buffer[ 80 ] = L"Todos";
    wchar_t *periodo[] = { L"Todas",
                           L"Uma data",
                           L"Um mês",
                           L"Um período",
                           L"Vencidas" };
    wchar_t *estado[] = { L"Todos",
                          L"Fixas",
                          L"Variáveis" };
    wchar_t *situacao[] = { L"Todas",
                            L"Em ser",
                            L"Pagas" };
    wchar_t *ordem[] = { L"Por registro",
                         L"Por ID",
                         L"Por descrição",
                         L"Por valor",
                         L"Por data e valor",
                         L"Por data da compra" };

    setcolor_on( NGRY, NBLK, 1, NNORM );
    box( BOX, lin, col, lin_d, col_d );
    box( BOX, lin, col_d + 1, lin_d, COLD_FORM_APP );

    setcurs( lin + 1, col + 2 );
    setcolor_on( NWHT, NBLK, 1, NBRIG );
    printf( "\xe2\x97\x86" );
    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
    printf( " Descrição" );

    setcurs( lin + 1, col_d + 3 );
    setcolor_on( NBLU, NBLK, 1, NBRIG );
    if( wcslen( pesq.descricao ) ) imp_wstr( pesq.descricao, wcslen( pesq.descricao ), J_ESQ, true );

    setcurs( lin + 2, col + 2 );
    setcolor_on( NWHT, NBLK, 1, NBRIG );
    printf( "\xe2\x97\x86" );
    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
    printf( " Número da compra" );

    setcurs( lin + 2, col_d + 3 );
    setcolor_on( NBLU, NBLK, 1, NBRIG );
    if( pesq.num_compra > 0 ) imp_wint( pesq.num_compra, 10, 2, J_DIR );

    setcurs( lin + 3, col + 2 );
    setcolor_on( NWHT, NBLK, 1, NBRIG );
    printf( "\xe2\x97\x86" );
    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
    printf( " Período" );

    setcurs( lin + 3, col_d + 3 );
    setcolor_on( NBLU, NBLK, 1, NBRIG );
    imp_wstr( periodo[ pesq.periodo ], wcslen( periodo[ pesq.periodo ] ), J_ESQ, false );
    setcolor_off();

    if( pesq.periodo == 1 )
    {
        printf( " -> ( " );
        imp_colordate( pesq.segs_ini, 15, NWHT, NBLK, 1, NNORM );
        printf( " )" );
    }

    if( pesq.periodo == 2 )
    {
        mes = decomp_date( pesq.segs_ini, 2 );
        ano = decomp_date( pesq.segs_ini, 3 );
        printf( " -> ( %02d de %04d", mes, ano );
        printf( " )" );
    }

    if( pesq.periodo == 3 )
    {
        printf( " -> ( " );
        imp_colordate( pesq.segs_ini, 15, NWHT, NBLK, 1, NNORM );
        printf( " a " );
        imp_colordate( pesq.segs_fin, 15, NWHT, NBLK, 1, NNORM );
        printf( " )" );
    }

    setcurs( lin + 4, col + 2 );
    setcolor_on( NWHT, NBLK, 1, NBRIG );
    printf( "\xe2\x97\x86" );
    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
    printf( "%s", " Conta" );

    setcurs( lin + 4, col_d + 3 );

    for( i = 0; i < T_CONTA; i++ ) if( !ct[ i ].ctrl ) break;
    
    if( T_CONTA == i )
    {
        setcolor_on( NBLU, NBLK, 1, NBRIG );
        printf( "%s", "Todas" );
    }
    else
    {
        for( i = 0; i < T_CONTA; i++ )
        {
            if( ct[ i ].ctrl )
            {
                setcolor_on( ct[ i ].cor, NBLK, 1, NBRIG );
                imp_wstr( ct[ i ].desc, 4, J_ESQ, false );
                printf( " " );
            }
        }
    }

    setcurs( lin + 5, col + 2 );
    setcolor_on( NWHT, NBLK, 1, NBRIG );
    printf( "\xe2\x97\x86" );
    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
    printf( "%s", " Estado" );

    setcurs( lin + 5, col_d + 3 );
    setcolor_on( NBLU, NBLK, 1, NBRIG );
    imp_wstr( estado[ pesq.estado ], 10, J_ESQ, false );

    setcurs( lin + 6, col + 2 );
    setcolor_on( NWHT, NBLK, 1, NBRIG );
    printf( "\xe2\x97\x86" );
    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
    printf( "%s", " Situação" );

    setcurs( lin + 6, col_d + 3 );
    setcolor_on( NBLU, NBLK, 1, NBRIG );
    imp_wstr( situacao[ pesq.situacao ], 6, J_ESQ, false );

    t[ 0 ] = buffer;

    for( i = 1; i < T_TIPO + 1; i++ ) t[ i ] = tp[ i - 1 ].desc;

    setcurs( lin + 7, col + 2 );
    setcolor_on( NWHT, NBLK, 1, NBRIG );
    printf( "\xe2\x97\x86" );
    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
    printf( "%s", " Tipo" );

    setcurs( lin + 7, col_d + 3 );
    setcolor_on( NBLU, NBLK, 1, NBRIG );
    imp_wstr( t[ pesq.tipo ], 42, J_ESQ, false );

    setcurs( lin + 8, col + 2 );
    setcolor_on( NWHT, NBLK, 1, NBRIG );
    printf( "\xe2\x97\x86" );
    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
    printf( "%s", " Parcelamento" );

    setcurs( lin + 8, col_d + 3 );
    setcolor_on( NBLU, NBLK, 1, NBRIG );

    if( pesq.parcela == 0 ) printf( "Todos" );
    if( pesq.parcela == 1 ) printf( "A vista" );
    
    if( pesq.parcela == 2 )
    {
        if( pesq.parc_max != 99 ) printf( "Parceladas em no máximo %2d parcelas", pesq.parc_max );
        else printf( "Todas as parceladas" );
    }
    
    if( pesq.parcela == 3 ) printf( "Parceladas com última parcela" );
    
    if( pesq.parcela == 4 ) printf( "Parceladas com %2d parcelas", pesq.parc_max );

    setcurs( lin + 9, col + 2 );
    setcolor_on( NWHT, NBLK, 1, NBRIG );
    printf( "\xe2\x97\x86" );
    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
    printf( "%s", " Ordem" );

    setcurs( lin + 9, col_d + 3 );
    setcolor_on( NBLU, NBLK, 1, NBRIG );
    
    imp_wstr( ordem[ pesq.ordem ], 20, J_ESQ, false );

    return;
}

int menu_tipo( uint8_t lin, uint8_t col, uint8_t t )
{
    BR *menu = NULL;
    LN *li = NULL;
    CO *co = NULL;
    TB *tb = NULL;
    SB *sb = NULL;
    LN * ( *fcon )( BR *, LN * );

    int ret;
    wchar_t dwchar[ 128 ];
    wchar_t *statusbar1[] = { L"[ESC]", L"[HOME]", L"[END]", L"[F2]" };
    wchar_t *statusbar2[] = { L"Sai", L"Inicio", L"Fim", L"Pesquisa" };

    fcon = consulta_tipo;

    menu = create_bw();
    
    press_keys_bw( menu, F2, fcon );
    setdatatypekey_bw( menu, sizeof( dwchar ) );
    setfunckey_bw( menu, cmpwcharline_bw );

    setbox_bw( menu, BOX_BROWSE_BW );
    setcolor_bw( menu, COLOR_FOCUS_BW, NWHT, 238, 1, NNORM );
    setcoord_bw( menu, lin, col, t );
    setctrlpercent_bw( menu, true );

    tb = addtitlebar_bw( menu, L" Tipo " );
    setcolortitlebar_bw( tb, NGRY, NBLK, 2, NBRIG, NREVE );

    for( uint8_t inc = 0; inc < 4; inc++ )
    {
        sb = addstatusbar_bw( menu, statusbar1[ inc ] );
        setcolorstatusbar_bw( sb, NGRY, NBLK, 2, NNORM, NREVE );
        
        sb = addstatusbar_bw( menu, statusbar2[ inc ] );
        setcolorstatusbar_bw( sb, NLCYN, NWHT, 3, NNORM, NITAL, NREVE );
    
        if( inc < 3 ) addstatusbar_bw( menu, L" " );
    }

    for( uint8_t inc = 0; inc < T_TIPO; inc++ )
    {
        li = addline_bw( menu, tp[ inc ].desc, ( uint32_t ) tp[ inc ].idt, f_void ); 

        co = addnewcol_bw( menu, li, tp[ inc ].desc, DWCHAR ); // 0
        setattrcol_bw( co, 50, 0, J_ESQ, MINUSC );
        setcolorcol_bw( co, NGRY, NBLK, 1, NBRIG );
    }

    init_bw( menu );

    ret = exec_bw( menu );

    if( ret == -1 )
    {
        free_bw( menu );
        return 0;
    }

    setcurs( lin, col );

    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
    printf( "Tipo:                  | " );
    setcolor_off();

    setcolor_on( NLRED, NBLK, 1, NBRIG );
    imp_wstr( ( wchar_t * ) getdatacol_bw( menu, ret, 0 ), 50, J_ESQ, false );
    setcolor_off();

    free_bw( menu );

    return ret;
}

int16_t menuh_est( uint8_t lin, uint8_t col )
{
    MNH *menu = NULL;
    COL *co = NULL;
    TIH *tit = NULL;
    int16_t ret;
    wchar_t *opcs[] = { L"Variável", L"Fixa" };

    menu = create_mh();

    setcoord_mh( menu, lin, col, 1 );
    setcolor_mh( menu, COLOR_NOFOCUS_MH, NGRY, 238, 1, NNORM );
    setcolor_mh( menu, COLOR_BACKGND_MH, NBLK, NBLK, 1, NNORM );

    tit = addtitle_mh( menu, L"Despesa:               |" );
    setcolortitle_mh( tit, COR_MSG, NBLK, 1, NBRIG );

    for( uint8_t i = 0; i < 2; i++ )
    {
        co = addcol_mh( menu, i, opcs[ i ], NULL );
        setcolorcol_mh( co, COLOR_FOCUS_MH, NWHT, NGRY, 2, NBRIG, NBLKS );
        setattrcol_mh( co, wcslen( opcs[ i ] ) + 2, J_CEN, MINUSC );
    }

    init_mh( menu );

    ret = exec_mh( menu );

    free_mh( menu );
    
    setcurs( lin, col );

    printf( BRIG BBLK SETFGC "Despesa:               | ", COR_MSG );
    
    if( ret == -1 ) return ret;
    
    printf( BRIG BBLK LRED "%s", ret ? "FIXA" : "VARIAVEL" );

    printf( RESETC );

    return ret;
}

int16_t menuh_lco( uint8_t lin, uint8_t col )
{
    MNH *menu = NULL;
    COL *co = NULL;
    TIH *tit = NULL;

    int16_t ret;
    wchar_t *opc_est[] = { L"Débito", L"Crédito" };

    menu = create_mh();

    setcoord_mh( menu, lin, col, 1 );
    setcolor_mh( menu, COLOR_NOFOCUS_MH, NGRY, 238, 1, NNORM );
    setcolor_mh( menu, COLOR_BACKGND_MH, NBLK, NBLK, 1, NNORM );

    tit = addtitle_mh( menu, L"Lançamento a:          |" );
    setcolortitle_mh( tit, COR_MSG, NBLK, 1, NBRIG );

    for( uint8_t i = 0; i < 2; i++ )
    {
        co = addcol_mh( menu, i, opc_est[ i ], NULL );
        setcolorcol_mh( co, COLOR_FOCUS_MH, NWHT, NGRY, 2, NBRIG, NBLKS );
        setattrcol_mh( co, wcslen( opc_est[ i ] ) + 2, J_CEN, MINUSC );
    }

    init_mh( menu );

    ret = exec_mh( menu );

    free_mh( menu );
    
    setcurs( lin, col );

    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
    printf( "Lançamento a:          | " );
    
    if( ret == -1 ) return ret;
    
    setcolor_on( NLRED, NBLK, 1, NBRIG );
    printf( "%s", ret ? "Crédito" : "Débito" );

    setcolor_off();

    return ret;
}

int16_t menuh_cta( uint8_t lin, uint8_t col )
{
    MNH *menu = NULL;
    COL *co = NULL;
    TIH *tit = NULL;

    int16_t ret;

    menu = create_mh();

    setcoord_mh( menu, lin, col, 1 );
    setcolor_mh( menu, COLOR_NOFOCUS_MH, NGRY, 238, 1, NNORM );
    setcolor_mh( menu, COLOR_BACKGND_MH, NBLK, NBLK, 1, NNORM );

    tit = addtitle_mh( menu, L"Conta:                 |" );
    setcolortitle_mh( tit, COR_MSG, NBLK, 1, NBLK );

    for( uint8_t i = 0; i < T_CONTA; i++ )
    {
        co = addcol_mh( menu, i + 1, ct[ i ].desc, NULL );
        setcolorcol_mh( co, COLOR_FOCUS_MH, NWHT, NGRY, 2, NBRIG, NBLKS );
        setattrcol_mh( co, wcslen( ct[ i ].desc ) + 2, J_CEN, MINUSC );
    
        addhintcol_mh( co, LIN_FORM_ROD, COL_FORM_ROD + 1, ct[ i ].descc );
        setcolorhintcol_mh( co, NLYEL, COR_FUNDO_ROD, 2, NITAL, NNORM );
    }
    
    setfocus_mh( menu, 4 );

    init_mh( menu );

    ret = exec_mh( menu );

    free_mh( menu );
    
    setcurs( lin, col );

    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
    printf( "Conta:                 | " );
    
    if( ret == -1 ) return ret;
    
    setcolor_on( NLRED, NBLK, 1, NBRIG );
    imp_wstr(  ct[ ret - 1 ].descc, 33, 1, 1 );

    setcolor_off();

    return ret;
}
