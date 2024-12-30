#include "fopag.h"
#include "brws.h"
#include "colors.h"
#include "config.h"
#include "newlib.h"
#include <stdint.h>
#include <wchar.h>

double tot_benef;
double tot_desc;
double previ13;
double inss13;
double desc_previ;
double desc_previ_ano;
double desc_cassi_pes;
double desc_cassi_dep;
double desc_cassi_previ13;
double desc_cassi_inss13;
double ir_previ;
double ir_inss;
double ir_comp;
double ir_previ13;
double ir_inss13;
double tot_cassi;
double tot_imposto;
double tot_previ;
double total;
double liq_fopag;
double tot_cred;
double tot_benef13;
double val_previ;
double val_inss; 
double resultado;
double capec;
double es;

void imp_fopag()
{
    FILE *arq;
    FILE *arqcfg;
    struct config cfg;
    int mes;
    int ano;
    int mes_prox;
    int ano_prox;
    time_t segs_ini;
    time_t segs_fin;
    struct despesa dp;
    wchar_t buffer[ 128 ];
    int8_t ret;
    uint8_t lin = LIN_FORM_APP;
    uint8_t col = COL_FORM_APP;
    wchar_t moddesc[ 128 ];

    tot_benef = 0.0;
    tot_desc = 0.0;
    previ13 = 0.0;
    inss13 = 0.0;
    desc_previ = 0.0;
    desc_previ_ano = 0.0;
    desc_cassi_pes = 0.0;
    desc_cassi_dep = 0.0;
    desc_cassi_previ13 = 0.0;
    desc_cassi_inss13 = 0.0;
    ir_previ = 0.0;
    ir_inss = 0.0;
    ir_comp = 0.0;
    ir_previ13 = 0.0;
    ir_inss13 = 0.0;
    tot_cassi = 0.0;
    tot_imposto = 0.0;
    tot_previ = 0.0;
    total = 0.0;
    liq_fopag = 0.0;
    tot_cred = 0.0;
    tot_benef13 = 0.0;
    val_previ = 0.0;
    val_inss = 0.0; 
    resultado = 0.0;
    capec = 0.0;
    es = 0.0;

    //fdisp = disp_tip;

    wcscpy( moddesc, nome_prog );
    wcscat( moddesc, L" - Resultado" );
    
    init_form( moddesc );

    arqcfg = fopen( "config.dat", "rb+" );

    if( !arqcfg )
    {
        ret_wmsg_form( L"Falha na abertura do arquivo de configuração.", RETMSG_ERRO );
        return;
    }

    rewind( arqcfg );

    if( fread( &cfg, sizeof( struct config ), 1, arqcfg ) < 1 )
    {
        ret_wmsg_form( L"Erro de acesso ao arquivo de configuração.", RETMSG_ERRO );
        fclose( arqcfg );
        return;
    }

    fclose( arqcfg );

    if( !input_monyear( lin, col + 1, &segs_ini, &segs_fin ) )
    {
        ret_wmsg_form( L"Cancelado pelo operador.", RETMSG_ERRO );
        return;
    }

    mes = decomp_date( segs_ini, 2 );
    ano = decomp_date( segs_ini, 3 );

    segs_ini = date_segs( 20, mes, ano );

    mes_prox = mes + 1;
    ano_prox = ano;

    if( mes_prox > 12 )
    {
        mes_prox = 1;
        ano_prox = ano + 1;
    }

    segs_fin = date_segs( 19, mes_prox, ano_prox );

    val_previ = cfg.previ * cfg.inpc;
    val_inss = cfg.inss * cfg.inpc;

    tot_benef = val_previ + val_inss;
    desc_previ = val_previ * cfg.pdesc_previ;
    desc_cassi_pes = tot_benef * cfg.pdesc_cassi_pes;
    desc_cassi_dep = ceil( ( tot_benef * cfg.pdesc_cassi_dep ) * 100.0f ) / 100.0f;
    ir_previ = calc_IR( val_previ, desc_previ, cfg.nr_deps );
    ir_inss = calc_IR( val_inss, 0, cfg.nr_deps );
    ir_comp = calc_IR( tot_benef, desc_previ, 2 ) - ir_previ - ir_inss;

    if( mes == cfg.mes1_previ13 ) previ13 = floor( ( val_previ / 2 ) * 100.0f ) / 100.0f;

    if( mes == cfg.mes1_inss13 ) inss13 = floor( ( val_inss / 2 ) * 100.0f ) / 100.0f;

    if( mes == cfg.mes2_previ13 )
    {
        previ13 = floor( ( val_previ / 2 ) * 100.0f ) / 100.0f;
        desc_previ_ano = desc_previ;
        desc_cassi_previ13 = val_previ * cfg.pdesc_cassi_pes;
        ir_previ13 = calc_IR( val_previ, desc_previ, cfg.nr_deps );
    }

    if( mes == cfg.mes2_inss13 )
    {
        inss13 = floor( ( val_inss / 2 ) * 100.0f ) / 100.0f;
        desc_cassi_inss13 = val_inss * cfg.pdesc_cassi_pes;
        ir_inss13 = calc_IR( val_inss, 0, cfg.nr_deps );
    }

    capec = cfg.valor_capec;
    es = cfg.valor_es;

    tot_benef13 = previ13 + inss13;
    tot_cred = tot_benef + tot_benef13;
    tot_previ = desc_previ + desc_previ_ano;
    tot_cassi = desc_cassi_pes + desc_cassi_dep + desc_cassi_previ13 + desc_cassi_inss13;
    tot_imposto = ir_previ + ir_inss + ir_comp + ir_previ13 + ir_inss13;
    tot_desc = tot_previ + tot_cassi + tot_imposto + capec + es;
    liq_fopag = tot_cred - tot_desc;

    arq = fopen( "despesas.dat", "rb+" );

    if( !arq )
	{
        ret_wmsg_form( L"Falha na abertura do arquivo.", RETMSG_ERRO );
		return;
	}       

    rewind( arq );
        
    for( uint8_t inc = 0; inc < T_GRUPO; inc++ ) gp[ inc ].valor = 0.0;
    
    while( fread( &dp, sizeof( struct despesa ), 1, arq ) )
	{
        if( !dp.apagado && dp.data >= segs_ini && dp.data <= segs_fin )
        {
            gp[ tp[ dp.tipo - 1 ].idg - 1 ].valor = gp[ tp[ dp.tipo - 1 ].idg - 1 ].valor + dp.valor;
        }
    }

    if( ferror( arq ) )
    {
        ret_wmsg_form( L"Erro de acesso ao arquivo", RETMSG_ERRO );
        fclose( arq );
        return;
    }
    
    fclose( arq );
    
    total = 0.00;

    BR *br = NULL;
    TB *tb = NULL;
    SB *sb = NULL;
    LN *l = NULL;
    CO *c = NULL;
    LN * ( *fdisp )( BR *, LN * );
    wchar_t *statusbar1[] = { L"[ESC]", L"[HOME]", L"[END]", L"[TAB]" };
    wchar_t *statusbar2[] = { L"Sai", L"Inicio", L"Fim", L"Detalha" };

    fdisp = disp_tipo;

    br = create_bw();

    setbox_bw( br, BOX_MENU_BW );
    press_keys_bw( br, TAB, fdisp );
    setcolor_bw( br, COLOR_FOCUS_BW, NWHT, 238, 1, NNORM );
    setcoord_bw( br, lin + 9, col + 52, 15 );
    setdatatypekey_bw( br, sizeof( int ) );
    setfunckey_bw( br, cmpintline_bw );

    tb = addtitlebar_bw( br, L" Despesas do mes " );
    setcolortitlebar_bw( tb, NGRY, NBLK, 2, NBRIG, NREVE );

    for( uint8_t inc = 0; inc < 4; inc++ )
    {
        sb = addstatusbar_bw( br, statusbar1[ inc ] );
        setcolorstatusbar_bw( sb, NGRY, NBLK, 2, NNORM, NREVE );
        
        sb = addstatusbar_bw( br, statusbar2[ inc ] );
        setcolorstatusbar_bw( sb, NLCYN, NWHT, 3, NNORM, NITAL, NREVE );
    
        if( inc < 3 ) addstatusbar_bw( br, L" " );
    }

    for( uint8_t inc = 0; inc < T_GRUPO; inc++ )
    {
        l = addline_bw( br, &gp[ inc ].idg, ( uint32_t ) gp[ inc ].idg, NULL );
                    
        c = addnewcol_bw( br, l, &gp[ inc ].idg, DINT ); // 0
        setshowcol_bw( c, false );

        c = addnewcol_bw( br, l, &segs_ini, DDATE ); // 1
        setshowcol_bw( c, false );

        c = addnewcol_bw( br, l, &segs_fin, DDATE ); // 2
        setshowcol_bw( c, false );

        c = addnewcol_bw( br, l, gp[ inc ].desc, DWCHAR ); // 3
        setattrcol_bw( c, 52, 0, J_ESQ, MAIUSC );
        setcolorcol_bw( c, 95, NBLK, 1, NBRIG );

        c = addnewcol_bw( br, l, &gp[ inc ].valor, DFLOAT ); // 4
        setattrcol_bw( c, 12, 2, J_DIR, MINUSC );
        setcolorcol_bw( c, gp[ inc ].valor < 0 ? NBLU : NLRED, NBLK, 1, NBRIG );

        total += gp[ inc ].valor;
    }

    resultado = liq_fopag - total;

    BR *fg = NULL;
    BR *fgres = NULL;
    BR *res = NULL;
    
    fg = stmenu_fopag( lin, col );
    fgres = stmenu_resfopag( lin + 21, col );
    res = stmenu_resumo( lin, col + 52 );

    swprintf( buffer, 80, L" - %02d/%4d", mes, ano );
    wcscat( moddesc, buffer );
    
    while( true )
    {
        init_form( moddesc );
        
        init_bw( fg );
        exec_bw( fg );
        
        init_bw( fgres );
        exec_bw( fgres );
        
        init_bw( res );
        exec_bw( res );

        init_bw( br );
        ret = exec_bw( br );
        
        if( ret == -1 )
        {
            free_bw( fg );
            free_bw( fgres );
            free_bw( res );
            free_bw( br );
            break;
        }
    }
}

BR *stmenu_fopag( uint8_t lin, uint8_t col )
{
    BR *br = NULL;
    TB *tb = NULL;
    LN *l = NULL;
    CO *c = NULL;
   
    struct fopag fopag[] = { { NBLU, NBLK, NBRIG, val_previ, L"Benefício PREVI" },
                             { NBLU, NBLK, NBRIG, val_inss, L"Benefício INSS" },
                             { NBLU, NBLK, NBRIG, previ13, L"Benefício PREVI 13" },
                             { NBLU, NBLK, NBRIG, inss13, L"Benefício INSS 13" },
                             { NLRED, NBLK, NBRIG, capec, L"Capec" },
                             { NLRED, NBLK, NBRIG, es, L"Empréstimo Simples" },
                             { NLRED, NBLK, NBRIG, desc_previ, L"Desconto PREVI" },
                             { NLRED, NBLK, NBRIG, desc_previ_ano, L"Desconto PREVI Anual" },
                             { NLRED, NBLK, NBRIG, desc_cassi_pes, L"CASSI Contribuição Pessoal" },
                             { NLRED, NBLK, NBRIG, desc_cassi_dep, L"CASSI Dependentes" },
                             { NLRED, NBLK, NBRIG, desc_cassi_previ13, L"CASSI PREVI 13" },
                             { NLRED, NBLK, NBRIG, desc_cassi_inss13, L"CASSI INSS 13" },
                             { NLRED, NBLK, NBRIG, ir_previ, L"Imposto a Pagar PREVI" },
                             { NLRED, NBLK, NBRIG, ir_inss, L"Imposto a Pagar INSS" },
                             { NLRED, NBLK, NBRIG, ir_comp, L"Imposto a Pagar Complementar" },
                             { NLRED, NBLK, NBRIG, ir_previ13, L"Imposto a Pagar PREVI 13" },
                             { NLRED, NBLK, NBRIG, ir_inss13, L"Imposto a Pagar INSS 13" } };

    br = create_bw();

    setbox_bw( br, BOX_MENU_BW );
    setcoord_bw( br, lin, col, 17 );
    setctrlget_bw( br, false );
    
    tb = addtitlebar_bw( br, L" Fopag " );
    setcolortitlebar_bw( tb, NGRY, NBLK, 2, NBRIG, NREVE );

    for( uint8_t inc = 0; inc < 17; inc++ )
    {
        l = addline_bw( br, NULL, inc, NULL );

        c = addnewcol_bw( br, l, fopag[ inc ].titulo, DWCHAR );
        setattrcol_bw( c, 32, 0, J_ESQ, false );
        setcolorcol_bw( c, COR_MSG, NBLK, 1, NBRIG );

        c = addnewcol_bw( br, l, &fopag[ inc ].valor, DFLOAT );
        setattrcol_bw( c, 12, 2, J_DIR, false );
        setcolorcol_bw( c, fopag[ inc ].cor_c, fopag[ inc ].cor_f, 1, fopag[ inc ].cor_a );
    }

    return br;
}

BR *stmenu_resfopag( uint8_t lin, uint8_t col )
{
    BR *br = NULL;
    TB *tb = NULL;
    LN *l = NULL;
    CO *c = NULL;

    struct fopag fopag[] = { { NLRED, NBLK, NBRIG, tot_previ, L"Total de Desconto PREVI" },
                             { NLRED, NBLK, NBRIG, tot_cassi, L"Total de Desconto CASSI" },
                             { NLRED, NBLK, NBRIG, tot_imposto, L"Total de Desconto IR" } };

    br = create_bw();

    setbox_bw( br, BOX_MENU_BW );
    setcoord_bw( br, lin, col, 3 );
    setctrlget_bw( br, false );
    
    tb = addtitlebar_bw( br, L" Descontos Fopag " );
    setcolortitlebar_bw( tb, NGRY, NBLK, 2, NBRIG, NREVE );

    for( uint8_t inc = 0; inc < 3; inc++ )
    {
        l = addline_bw( br, NULL, inc, NULL );

        c = addnewcol_bw( br, l, fopag[ inc ].titulo, DWCHAR );
        setattrcol_bw( c, 32, 0, J_ESQ, false );
        setcolorcol_bw( c, COR_MSG, NBLK, 1, NBRIG );

        c = addnewcol_bw( br, l, &fopag[ inc ].valor, DFLOAT );
        setattrcol_bw( c, 12, 2, J_DIR, false );
        setcolorcol_bw( c, fopag[ inc ].cor_c, fopag[ inc ].cor_f, 1, fopag[ inc ].cor_a );
    }

    return br;
}

BR *stmenu_resumo( uint8_t lin, uint8_t col )
{
    BR *br = NULL;
    TB *tb = NULL;
    LN *l = NULL;
    CO *c = NULL;

    struct fopag fopag[] = { { NBLU, NBLK, NBRIG, tot_cred, L"Rendimentos Fopag" },
                             { NLRED, NBLK, NBRIG, tot_desc, L"Descontos Fopag" },
                             { NBLU, NBLK, NBRIG, liq_fopag, L"Líquido Fopag" },
                             { NLRED, NBLK, NBRIG, total, L"Despesas do mes" },
                             { NGRN, NBLK, NBRIG, resultado, L"Resultado" } };

    br = create_bw();

    setbox_bw( br, BOX_MENU_BW );
    setcoord_bw( br, lin, col, 5 );
    setctrlget_bw( br, false );
    
    tb = addtitlebar_bw( br, L" Resumo " );
    setcolortitlebar_bw( tb, NGRY, NBLK, 2, NBRIG, NREVE );

    for( uint8_t inc = 0; inc < 5; inc++ )
    {
        l = addline_bw( br, NULL, inc, NULL );

        c = addnewcol_bw( br, l, fopag[ inc ].titulo, DWCHAR );
        setattrcol_bw( c, 32, 0, J_ESQ, false );
        setcolorcol_bw( c, COR_MSG, NBLK, 1, NBRIG );

        c = addnewcol_bw( br, l, &fopag[ inc ].valor, DFLOAT );
        setattrcol_bw( c, 12, 2, J_DIR, false );
        setcolorcol_bw( c, fopag[ inc ].cor_c, fopag[ inc ].cor_f, 1, fopag[ inc ].cor_a );
    }

    return br;
}

double calc_IR( double salario, double descprev, int deps )
{
    salario = salario - descprev - deps * 189.59;

    if( salario > 2259.20 )
    {
        if( salario < 2826.66 )
            return fabs( salario * .075 - 169.44 );
        if( salario < 3751.06 )
            return fabs( salario * .150 - 381.44 );
        if( salario < 4664.69 )
            return fabs( salario * .225 - 662.77 );
        if( salario > 4664.68 )
            return fabs( salario * .275 - 896.00 );
    }

    return 0.0;
}

LN *disp_tipo( BR *b, LN *l )
{
    FILE *arq;
	struct despesa dp;
    int mes;
    int ano;
    double total = 0.0;
    int8_t ret = -1;
    time_t *t_ini;
    time_t *t_fin;
    int *pn;
    BR *br = NULL;
    TB *tb = NULL;
    SB *sb = NULL;
    LN *li = NULL;
    CO *co = NULL;
    LN *( *fdisp ) ( BR *, LN * );
    wchar_t dwchar[ 128 ];
    wchar_t buffer[ 128 ];
    wchar_t buf_val[ 128 ];
    wchar_t *statusbar1[] = { L"[ESC]", L"[HOME]", L"[END]", L"[TAB]" };
    wchar_t *statusbar2[] = { L"Sai", L"Inicio", L"Fim", L"Detalha" };
    wchar_t moddesc[ 128 ];

    wcscpy( moddesc, nome_prog );
    wcscat( moddesc, L" - Resultado / Detalhamento" );
    
    pn = ( int * ) getlinedatacol_bw( l, 0 );
    t_ini = ( time_t * ) getlinedatacol_bw( l, 1 );
    t_fin = ( time_t * ) getlinedatacol_bw( l, 2 );

    mes = decomp_date( *t_ini, 2 );
    ano = decomp_date( *t_ini, 3 );

    for( uint8_t inc = 0; inc < T_TIPO; inc++ ) tp[ inc ].valor = 0.0;

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
        if( !dp.apagado && ( dp.data >= *t_ini && dp.data <= *t_fin ) )
        {
            tp[ dp.tipo - 1 ].valor = tp[ dp.tipo - 1 ].valor + dp.valor;
        }
    }

    if( ferror( arq ) )
    {
        ret_wmsg_form( L"Erro de acesso ao arquivo", RETMSG_ERRO );
        fclose( arq );
        return l;
    }

    fclose( arq );

    br = create_bw();

    fdisp = disp_dados;

    setbox_bw( br, BOX_MENU_BW );
    press_keys_bw( br, TAB, fdisp );
    setdatatypekey_bw( br, sizeof( dwchar ) );
    setfunckey_bw( br, cmpwcharline_bw );

    for( uint8_t inc = 0; inc < 4; inc++ )
    {
        sb = addstatusbar_bw( br, statusbar1[ inc ] );
        setcolorstatusbar_bw( sb, NGRY, NBLK, 2, NNORM, NREVE );
        
        sb = addstatusbar_bw( br, statusbar2[ inc ] );
        setcolorstatusbar_bw( sb, NLCYN, NWHT, 3, NNORM, NITAL, NREVE );
    
        if( inc < 3 ) addstatusbar_bw( br, L" " );
    }

    for( uint8_t inc = 0; inc < T_TIPO; inc++ )
    {
        if( tp[ inc ].idg == *pn && tp[ inc ].valor != 0.0 )
        {
            li = addline_bw( br, tp[ inc ].desc, ( uint32_t ) tp[ inc ].idt, NULL );

            co = addnewcol_bw( br, li, &tp[ inc ].idt, DINT ); // 0
            setshowcol_bw( co, false );

            co = addnewcol_bw( br, li, t_ini, DDATE ); // 1
            setshowcol_bw( co, false );

            co = addnewcol_bw( br, li, t_fin, DDATE ); // 2
            setshowcol_bw( co, false );

            co = addnewcol_bw( br, li, tp[ inc ].desc, DWCHAR ); // 3
            setattrcol_bw( co, 45, 0, J_ESQ, MAIUSC );
            setcolorcol_bw( co, COR_TIPO, NBLK, 1, NBRIG );

            co = addnewcol_bw( br, li, &tp[ inc ].valor, DFLOAT ); // 4
            setattrcol_bw( co, 10, 2, J_DIR, MINUSC );
            setcolorcol_bw( co, gp[ inc ].valor < 0 ? NBLU : NLRED, NBLK, 1, NBRIG );

            total += tp[ inc ].valor;
        }
    }

    swprintf( buffer, 128, L" %02d/%4d", mes, ano );

    wcscat( buffer, L" - " );
    wcscat( buffer, gp[ *pn - 1 ].desc );
    wcscat( buffer, L" - " );
    wcscat( buffer, doub_to_wstr( fabs( total ), 2, buf_val ) );
    wcscat( buffer, L" " );

    tb = addtitlebar_bw( br, buffer );
    setcolortitlebar_bw( tb, NGRY, NBLK, 2, NBRIG, NREVE );
            
    setcoord_bw( br, LIN_FORM_APP, COL_FORM_APP, getsize_bw( br ) > 10 ? 10 : getsize_bw( br ) );

    do
    {
        init_form( moddesc );

        init_bw( br );
        ret = exec_bw( br );
    }
    while( ret != -1 );

    free_bw( br );

    return l;
}

LN *disp_dados( BR *b, LN *l )
{
    FILE *arq;
	struct despesa dp;
    int mes;
    int ano;
    double total = 0.0;
    time_t *t_ini;
    time_t *t_fin;
    int *pn;
    wchar_t buffer[ 128 ];
    wchar_t buf_val[ 128 ];
    wchar_t *statusbar1[] = { L"[ESC]", L"[HOME]", L"[END]" };
    wchar_t *statusbar2[] = { L"Sai", L"Inicio", L"Fim" };
    wchar_t moddesc[ 128 ];
    BR *br = NULL;
    TB *tb = NULL;
    SB *sb = NULL;
    LN *li = NULL;
    CO *co = NULL;
    
    wcscpy( moddesc, nome_prog );
    wcscat( moddesc, L" - Resultado / Detalhamento" );

    pn = ( int * ) getlinedatacol_bw( l, 0 );
    t_ini = ( time_t * ) getlinedatacol_bw( l, 1 );
    t_fin = ( time_t * ) getlinedatacol_bw( l, 2 );

    mes = decomp_date( *t_ini, 2 );
    ano = decomp_date( *t_ini, 3 );
    
    init_form( moddesc );
    
    arq = fopen( "despesas.dat", "rb+" );

    if( !arq )
	{
        ret_wmsg_form( L"Falha na abertura do arquivo.", RETMSG_ERRO );
		return l;
	}

    br = create_bw();

    setbox_bw( br, BOX_MENU_BW );
    setdatatypekey_bw( br, sizeof( time_t ) );
    setfunckey_bw( br, cmpdateline_bw );

    for( uint8_t inc = 0; inc < 3; inc++ )
    {
        sb = addstatusbar_bw( br, statusbar1[ inc ] );
        setcolorstatusbar_bw( sb, NGRY, NBLK, 2, NNORM, NREVE );
        
        sb = addstatusbar_bw( br, statusbar2[ inc ] );
        setcolorstatusbar_bw( sb, NLCYN, NWHT, 3, NNORM, NITAL, NREVE );
    
        if( inc < 2 ) addstatusbar_bw( br, L" " );
    }

    rewind( arq );
    
    while( fread( &dp, sizeof( struct despesa ), 1, arq ) )
	{
        if( !dp.apagado && tp[ dp.tipo - 1 ].idt == *pn && ( dp.data >= *t_ini && dp.data <= *t_fin ) )
        {
            li = addline_bw( br, &dp.data_reg, ( uint32_t ) dp.id, NULL ); 

            co = addnewcol_bw( br, li, dp.descricao, DWCHAR ); // 0
            setattrcol_bw( co, 42, 0, J_ESQ, MAIUSC );
            setcolorcol_bw( co, NLYEL, NBLK, 1, NNORM );

            co = addnewcol_bw( br, li, ct[ dp.conta - 1 ].desc, DWCHAR ); // 1
            setattrcol_bw( co, 6, 0, J_CEN, MINUSC );
            setcolorcol_bw( co, ct[ dp.conta - 1 ].cor, NBLK, 1, NNORM );

            co = addnewcol_bw( br, li, &dp.data_reg, DDATE ); // 2
            setattrcol_bw( co, 15, 0, J_CEN, MINUSC );
            setcolorcol_bw( co, NGRY, NBLK, 1, NNORM );

            co = addnewcol_bw( br, li, &dp.parcela, DINT ); // 3
            setattrcol_bw( co, 6, 0, J_DIR, MINUSC );
            setcolorcol_bw( co, NWHT, NBLK, 1, NNORM );

            co = addnewcol_bw( br, li, &dp.parcelas, DINT ); // 4
            setattrcol_bw( co, 6, 0, J_DIR, MINUSC );
            setcolorcol_bw( co, NWHT, NBLK, 1, NNORM );

            co = addnewcol_bw( br, li, &dp.valor, DFLOAT ); // 5
            setattrcol_bw( co, 12, 2, J_DIR, MINUSC );
            setcolorcol_bw( co, dp.valor < 0 ? NBLU : NLRED, NBLK, 1, NNORM );

            total += dp.valor;
        }
    }

    if( ferror( arq ) )
    {
        ret_wmsg_form( L"Erro de acesso ao arquivo", RETMSG_ERRO );
        fclose( arq );
        free_bw( br );
        return l;
    }

    fclose( arq );

    swprintf( buffer, 80, L" %02d/%4d", mes, ano );

    wcscat( buffer, L" - " );
    wcscat( buffer, tp[ *pn - 1 ].desc );
    wcscat( buffer, L" - " );
    wcscat( buffer, doub_to_wstr( fabs( total ), 2, buf_val ) );
    wcscat( buffer, L" " );
    
    tb = addtitlebar_bw( br, buffer );
    setcolortitlebar_bw( tb, NGRY, NBLK, 2, NBRIG, NREVE );

    setcoord_bw( br, LIN_FORM_APP, COL_FORM_APP, getsize_bw( br ) > 25 ? 25 : getsize_bw( br ) );

    init_bw( br );
    exec_bw( br );

    free_bw( br );

    return l;
}
