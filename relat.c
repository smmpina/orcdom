#include "colors.h"
#include "newlib.h"
#include "orcdom.h"
#include "brws.h"
#include <stdint.h>
#include <wchar.h>

void relat_grupo();
void relat_tipo();
void relat_conta();

void menu_relat()
{
    int opc = 0;
    uint8_t lin = LIN_FORM_APP;
    uint8_t col = COL_FORM_APP;
    uint8_t qtd_opcs = 3;
    BR *menu = NULL;
    LN *li = NULL;
    CO *co = NULL;
    TB *tb = NULL;
    SB *sb = NULL;
    void (*f[])() = { relat_grupo, relat_tipo, relat_conta };
    wchar_t *opcs[] = { L"Relatório - grupo", L"Relatório - tipo", L"Relatório - conta" };
    wchar_t *statusbar1[] = { L"[ESC]", L"[HOME]", L"[END]" };
    wchar_t *statusbar2[] = { L"Sai", L"Inicio", L"Fim" };
    wchar_t moddesc[ 128 ];

    wcscpy( moddesc, nome_prog );
    wcscat( moddesc, L" - Relatórios" );
    
    menu = create_bw();

    setbox_bw( menu, BOX_MENU_BW );
    setcolor_bw( menu, COLOR_FOCUS_BW, NWHT, 238, 1, NNORM );
    setcoord_bw( menu, lin, col, qtd_opcs );
    setctrlpercent_bw( menu, false );

    for( uint8_t inc = 0; inc < 3; inc++ )
    {
        sb = addstatusbar_bw( menu, statusbar1[ inc ] );
        setcolorstatusbar_bw( sb, NGRY, NBLK, 2, NNORM, NREVE );
        
        sb = addstatusbar_bw( menu, statusbar2[ inc ] );
        setcolorstatusbar_bw( sb, NLCYN, NWHT, 3, NNORM, NITAL, NREVE );
    
        if( inc < 2 ) addstatusbar_bw( menu, L" " );
    }

    tb = addtitlebar_bw( menu, L" Relatórios " );
    setcolortitlebar_bw( tb, NGRY, NBLK, 2, NBRIG, NREVE );

    for( uint8_t inc = 0; inc < qtd_opcs; inc++ )
    {
        li = addline_bw( menu, NULL, inc + 1, f[ inc ] );

        co = addnewcol_bw( menu, li, opcs[ inc ], DWCHAR );
        setattrcol_bw( co, wcslen( opcs[ inc  ] ), 0, J_ESQ, false );
        setcolorcol_bw( co, NGRY, NBLK, 1, NNORM );
    }

	while( opc != -1 )
	{
		init_form( moddesc );
        
        init_bw( menu );

        opc = exec_bw( menu );
	}

    free_bw( menu );

	return;
}

void relat_grupo()
{
    FILE *arq;
    bool arq_vazio = true;
    int periodo;
    time_t segs_ini;
    time_t segs_fin;
    double total = 0.0;
	struct despesa dp;
    BR *br = NULL;
    TB *tb = NULL;
    SB *sb = NULL;
    LN *l = NULL;
    CO *c = NULL;
    uint8_t lin = LIN_FORM_APP;
    uint8_t col = COL_FORM_APP;
    wchar_t *statusbar1[] = { L"[ESC]", L"[HOME]", L"[END]" };
    wchar_t *statusbar2[] = { L"Sai", L"Inicio", L"Fim" };
    wchar_t dwchar[ 128 ];
    wchar_t buf[ 128 ];
    wchar_t buf_val[ 128 ];
    wchar_t moddesc[ 128 ];

    wcscpy( moddesc, nome_prog );
    wcscat( moddesc, L" - Relatório / Grupo" );
    
    for( uint8_t inc = 0; inc < T_GRUPO; inc++ ) gp[ inc ].valor = 0.0;

    init_form( moddesc );
    
    arq = fopen( "despesas.dat", "rb+" );

    if( !arq )
	{
        ret_wmsg_form( L"Falha na abertura do arquivo.", RETMSG_ERRO );
		return;
	}

    periodo = input_daterange( lin, col + 1, &segs_ini, &segs_fin );

    if( periodo == -1 )
    {
        fclose( arq );
        ret_wmsg_form( L"Cancelado pelo operador.", RETMSG_ERRO );
        return;
    }
    
    if( periodo == 2 )
    {
        uint16_t mes, ano, mes_prox, ano_prox;
        
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
    }

    rewind( arq );

    while( fread( &dp, sizeof( struct despesa ), 1, arq ) )
	{
        if( !dp.apagado && ( periodo ? ( dp.data >= segs_ini && dp.data <= segs_fin ) : 1 ) )
        {
            gp[ tp[ dp.tipo - 1 ].idg - 1 ].valor = gp[ tp[ dp.tipo - 1 ].idg - 1 ].valor + dp.valor;
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

    br = create_bw();

    setbox_bw( br, BOX_MENU_BW );
    setcolor_bw( br, COLOR_FOCUS_BW, NWHT, 238, 1, NNORM );
    setdatatypekey_bw( br, sizeof( dwchar ) );
    setfunckey_bw( br, cmpwcharline_bw );

    for( uint8_t inc = 0; inc < 3; inc++ )
    {
        sb = addstatusbar_bw( br, statusbar1[ inc ] );
        setcolorstatusbar_bw( sb, NGRY, NBLK, 2, NNORM, NREVE );
        
        sb = addstatusbar_bw( br, statusbar2[ inc ] );
        setcolorstatusbar_bw( sb, NLCYN, NWHT, 3, NNORM, NITAL, NREVE );
    
        if( inc < 2 ) addstatusbar_bw( br, L" " );
    }

    for( uint8_t inc = 0; inc < T_GRUPO; inc++ )
    {
        l = addline_bw( br, gp[ inc ].desc, ( uint32_t ) gp[ inc ].idg, NULL ); 

        c = addnewcol_bw( br, l, gp[ inc ].desc, DWCHAR ); // 0
        setattrcol_bw( c, 45, 0, J_ESQ, MINUSC );
        setcolorcol_bw( c, COR_GRUPO, NBLK, 1, NNORM );

        c = addnewcol_bw( br, l, &gp[ inc ].valor, DFLOAT ); // 1
        setattrcol_bw( c, 12, 2, J_DIR, MINUSC );
        setcolorcol_bw( c, gp[ inc ].valor < 0 ? NBLU : NLRED, NBLK, 1, NBRIG );

        total += gp[ inc ].valor;
    }
    
    wcscpy( buf, L" Relatório / Grupo - " );
    wcscat( buf, doub_to_wstr( fabs( total ), 2, buf_val ) );
    wcscat( buf, L" " );

    tb = addtitlebar_bw( br, buf );
    setcolortitlebar_bw( tb, NGRY, NBLK, 2, NBRIG, NREVE );

    setcoord_bw( br, lin + 2, col, T_GRUPO );

    setcolor_off();

    init_bw( br );
    exec_bw( br );

    free_bw( br );
}

void relat_tipo()
{
    FILE *arq;
	bool arq_vazio = true;
    int periodo;
    time_t segs_ini;
    time_t segs_fin;
    double total = 0.0;
	struct despesa dp;
    uint8_t lin = LIN_FORM_APP;
    uint8_t col = COL_FORM_APP;
    BR *br = NULL;
    TB *tb = NULL;
    SB *sb = NULL;
    LN *l = NULL;
    CO *c = NULL;
    wchar_t dwchar[ 128 ];
    wchar_t buf[ 128 ];
    wchar_t buf_val[ 128 ];
    wchar_t *statusbar1[] = { L"[ESC]", L"[HOME]", L"[END]" };
    wchar_t *statusbar2[] = { L"Sai", L"Inicio", L"Fim" };
    wchar_t moddesc[ 128 ];

    wcscpy( moddesc, nome_prog );
    wcscat( moddesc, L" - Relatório / Tipo" );
    
    for( uint8_t inc = 0; inc < T_TIPO; inc++ ) tp[ inc ].valor = 0.0;
    
    init_form( moddesc );

    arq = fopen( "despesas.dat", "rb+" );

    if( !arq )
	{
        ret_wmsg_form( L"Falha na abertura do arquivo.", RETMSG_ERRO );
		return;
	}

    periodo = input_daterange( lin, col + 1, &segs_ini, &segs_fin );

    if( periodo == -1 )
    {
        fclose( arq );
        ret_wmsg_form( L"Cancelado pelo operador.", RETMSG_ERRO );
        return;
    }
    
    if( periodo == 2 )
    {
        int mes, ano, mes_prox, ano_prox;
        
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
    }

    rewind( arq );

    while( fread( &dp, sizeof( struct despesa ), 1, arq ) )
	{
        if( !dp.apagado && ( periodo ? ( dp.data >= segs_ini && dp.data <= segs_fin ) : 1 ) )
        {
            tp[ dp.tipo - 1 ].valor = tp[ dp.tipo - 1 ].valor + dp.valor;
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

    br = create_bw();

    setbox_bw( br, BOX_MENU_BW );
    setcolor_bw( br, COLOR_FOCUS_BW, NWHT, 238, 1, NNORM );
    setdatatypekey_bw( br, sizeof( dwchar ) );
    setfunckey_bw( br, cmpwcharline_bw );

    for( uint8_t inc = 0; inc < 3; inc++ )
    {
        sb = addstatusbar_bw( br, statusbar1[ inc ] );
        setcolorstatusbar_bw( sb, NGRY, NBLK, 2, NNORM, NREVE );
        
        sb = addstatusbar_bw( br, statusbar2[ inc ] );
        setcolorstatusbar_bw( sb, NLCYN, NWHT, 3, NNORM, NITAL, NREVE );
    
        if( inc < 2 ) addstatusbar_bw( br, L" " );
    }

    for( uint8_t inc = 0; inc < T_TIPO; inc++ )
    {
        l = addline_bw( br, tp[ inc ].desc, ( uint32_t ) tp[ inc ].idt, NULL ); 

        c = addnewcol_bw( br, l, tp[ inc ].desc, DWCHAR ); // 0
        setattrcol_bw( c, 45, 0, J_ESQ, MINUSC );
        setcolorcol_bw( c, COR_TIPO, NBLK, 1, NNORM );

        c = addnewcol_bw( br, l, gp[ tp[ inc ].idg - 1 ].desc, DWCHAR ); // 1
        setattrcol_bw( c, 35, 0, J_ESQ, MINUSC );
        setcolorcol_bw( c, COR_GRUPO, NBLK, 1, NNORM );

        c = addnewcol_bw( br, l, &tp[ inc ].valor, DFLOAT ); // 1
        setattrcol_bw( c, 12, 2, J_DIR, MINUSC );
        setcolorcol_bw( c, tp[ inc ].valor < 0 ? NBLU : NLRED, NBLK, 1, NBRIG );

        total += tp[ inc ].valor;
    }
    
    wcscpy( buf, L" Relatório / Tipo - " );
    wcscat( buf, doub_to_wstr( fabs( total ), 2, buf_val ) );
    wcscat( buf, L" " );

    tb = addtitlebar_bw( br, buf );
    setcolortitlebar_bw( tb, NGRY, NBLK, 2, NBRIG, NREVE );

    setcoord_bw( br, lin + 2, col, 25 );

    setcolor_off();

    init_bw( br );
    exec_bw( br );

    free_bw( br );
}

void relat_conta()
{
    FILE *arq;
	int arq_vazio = true;
    int periodo;
    time_t segs_ini;
    time_t segs_fin;
    double total = 0.0;
	struct despesa dp;
    BR *br = NULL;
    TB *tb = NULL;
    SB *sb = NULL;
    LN *l = NULL;
    CO *c = NULL;
    uint8_t lin = LIN_FORM_APP;
    uint8_t col = COL_FORM_APP;
    wchar_t *statusbar1[] = { L"[ESC]", L"[HOME]", L"[END]" };
    wchar_t *statusbar2[] = { L"Sai", L"Inicio", L"Fim" };
    wchar_t dwchar[ 128 ];
    wchar_t buf[ 128 ];
    wchar_t buf_val[ 128 ];
    wchar_t moddesc[ 128 ];

    wcscpy( moddesc, nome_prog );
    wcscat( moddesc, L" - Relatório / Conta" );
    
    for( uint8_t inc = 0; inc < T_CONTA; inc++ ) ct[ inc ].valor = 0.0;
    
    init_form( moddesc );

    arq = fopen( "despesas.dat", "rb+" );

    if( !arq )
	{
        ret_wmsg_form( L"Falha na abertura do arquivo.", RETMSG_ERRO );
		return;
	}

    periodo = input_daterange( lin, col + 1, &segs_ini, &segs_fin );

    if( periodo == -1 )
    {
        fclose( arq );
        ret_wmsg_form( L"Cancelado pelo operador.", RETMSG_ERRO );
        return;
    }
    
    if( periodo == 2 )
    {
        int mes, ano, mes_prox, ano_prox;
        
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
    }

    rewind( arq );

    while( fread( &dp, sizeof( struct despesa ), 1, arq ) )
	{
        if( !dp.apagado && ( periodo ? ( dp.data >= segs_ini && dp.data <= segs_fin ) : 1 ) )
        {
            ct[ dp.conta - 1 ].valor = ct[ dp.conta - 1 ].valor + dp.valor;
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

    br = create_bw();

    setbox_bw( br, BOX_MENU_BW );
    setcolor_bw( br, COLOR_FOCUS_BW, NWHT, 238, 1, NNORM );
    setdatatypekey_bw( br, sizeof( dwchar ) );
    setfunckey_bw( br, cmpwcharline_bw );

    for( uint8_t inc = 0; inc < 3; inc++ )
    {
        sb = addstatusbar_bw( br, statusbar1[ inc ] );
        setcolorstatusbar_bw( sb, NGRY, NBLK, 2, NNORM, NREVE );
        
        sb = addstatusbar_bw( br, statusbar2[ inc ] );
        setcolorstatusbar_bw( sb, NLCYN, NWHT, 3, NNORM, NITAL, NREVE );
    
        if( inc < 2 ) addstatusbar_bw( br, L" " );
    }

    for( uint8_t inc = 0; inc < T_CONTA; inc++ )
    {
        l = addline_bw( br, ct[ inc ].descc, ( uint32_t ) ct[ inc ].idc, NULL ); 

        c = addnewcol_bw( br, l, ct[ inc ].descc, DWCHAR ); // 0
        setattrcol_bw( c, 45, 0, J_ESQ, MINUSC );
        setcolorcol_bw( c, COR_CONTA, NBLK, 1, NNORM );

        c = addnewcol_bw( br, l, &ct[ inc ].valor, DFLOAT ); // 1
        setattrcol_bw( c, 12, 2, J_DIR, MINUSC );
        setcolorcol_bw( c, ct[ inc ].valor < 0 ? NBLU : NLRED, NBLK, 1, NBRIG );

        total += ct[ inc ].valor;
    }
    
    wcscpy( buf, L" Relatório / Conta - " );
    wcscat( buf, doub_to_wstr( fabs( total ), 2, buf_val ) );
    wcscat( buf, L" " );

    tb = addtitlebar_bw( br, buf );
    setcolortitlebar_bw( tb, NGRY, NBLK, 2, NBRIG, NREVE );

    setcoord_bw( br, lin + 2, col, T_CONTA );

    setcolor_off();

    init_bw( br );
    exec_bw( br );

    free_bw( br );
}
