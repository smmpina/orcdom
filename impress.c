#include "colors.h"
#include "newlib.h"
#include "orcdom.h"

//extern struct conta ct[];
//extern struct tipo tp[];

void impdados( struct despesa *dp )
{
    double total = dp->parcelas * dp->valor;
    time_t segs_hoje;
    double saldo;

    segs_hoje = now_segs();
    
    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
    printf( "Descrição..........: " );
    
    setcolor_on( NMAG, NBLK, 2, NBRIG, NUNDE );
    printf( "%03d", dp->id );
    setcolor_off();
    printf( " | " );
    
    setcolor_on( NWHT, NBLK, 2, NBRIG, NUNDE );
    imp_wstr( dp->descricao, 42, J_ESQ, MAIUSC );

    printf( "\n" );
    
    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
    printf( "Compra.............: " );
    
    setcolor_on( NGRN, NBLK, 1, NBRIG );
    printf( "%03d", dp->id_compra );
    setcolor_off();
    printf( " | " );

    setcolor_on( dp->fixa ? NMAG : NYEL, NBLK, 2, NBRIG, NUNDE );
    printf( "%s", dp->fixa ? "FIX" : "VAR" );
    setcolor_off();
    printf( " | " );
   
    imp_colordate( dp->data_reg, 15, NWHT, NBLK, 1, NBRIG );

    printf( " | " );

    setcolor_on( total < 0 ? NBLU : NRED, NBLK, 1, NBRIG );
    imp_wdoub( fabs( total ), 10, 2, J_DIR );
    setcolor_off();

    printf( "\n" );

    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
    printf( "Tipo...............: " );
    setcolor_on( NCYN, NBLK, 1, NBRIG );
    printf( "%3d", dp->tipo );
    setcolor_off();
    printf( " | " );

    setcolor_on( NCYN, NBLK, 1, NBRIG );
    imp_wstr( tp[ dp->tipo - 1 ].desc, 40, J_ESQ, MAIUSC );
    printf( "\n" );

    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
    printf( "Conta..............: " );
    setcolor_on( NCYN, NBLK, 1, NBRIG );
    printf( "%3d", dp->conta );
    setcolor_off();
    printf( " | " );

    setcolor_on( NCYN, NBLK, 1, NBRIG );
    imp_wstr( ct[ dp->conta - 1 ].descc, 20, J_ESQ, MINUSC );
    printf( "\n" );
    
    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
    printf( "Parcela............: " );
    setcolor_on( 16, 252, 1, NBRIG );
    printf( "%3d", dp->parcela );
    setcolor_off();
    printf( " | " );

    setcolor_on( 16, 252, 1, NBRIG );
    printf( "%3d", dp->parcelas );
    setcolor_off();
    printf( " | " );

    imp_colordate( dp->data, NLWHT, 16, 252, 1, NBRIG );
    printf( " | " );
    
    setcolor_on( dp->valor < 0 ? NBLU : NRED, 252, 1, NBRIG );
    imp_wdoub( fabs( dp->valor ), 10, 2, J_DIR );
    setcolor_off();
    printf( " -> " );

    saldo = dp->valor * ( dp->parcelas - dp->parcela );
    
    setcolor_on( saldo < 0 ? NBLU: NRED, NBLK, 1, NBRIG );
    imp_wdoub( fabs( saldo ), 10, 2, J_DIR );
    setcolor_off();
    printf( "(" );
    setcolor_on( NLYEL, NBLK, 1, NBRIG );
    printf( "%d", dp->parcelas - dp->parcela );
    setcolor_off();
    printf( ")" );
    printf( "\n" );

    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
    printf( "Situação...........: " );

    if( dp->pago )
    {
        setcolor_on( NBLU, NBLK, 1, NBLK );
        printf( "Liquidada" );
    }
    else
    {
        if( dp->data >= segs_hoje )
        {
            setcolor_on( NGRN, NBLK, 1, NBRIG );
            printf( "A Vencer" );
        }
        else
        {
            setcolor_on( NRED, NBLK, 1, NBRIG );
            printf( "Vencida" );
        }
    }

    printf( "\n" );
}

void mostra_dados( struct despesa *dp )
{
    time_t segs_hoje;
    
    segs_hoje = now_segs();
    
    if( dp->pago )
    {
        setcolor_on( NBLK, NLBLU, 1, NBRIG );
        printf( "#" );
        setcolor_off();
    }        
    else
    {
        if( dp->data >= segs_hoje )
        {
            setcolor_on( NBLK, NLGRN, 1, NBRIG );
            printf( "#" );
            setcolor_off();
        }
        else
        {
            setcolor_on( NBLK, NLRED, 1, NBRIG );
            printf( "#" );
            setcolor_off();
        }
    }    
    
    printf( " | " );
    
    setcolor_on( NMAG, NBLK, 2, NBRIG, NUNDE );
    printf( "%05d", dp->id );
    setcolor_off();
    printf( " | " );
    
    setcolor_on( NGRN, NBLK, 1, NBRIG );
    printf( "%05d", dp->id_compra );
    setcolor_off();
    printf( " | " );

    setcolor_on( dp->fixa ? NMAG : NCYN, NBLK, 1, NBRIG );
    printf( "%s", dp->fixa ? "FIX" : "VAR" );
    setcolor_off();
    printf( " | " );   
    
    setcolor_on( NWHT, NBLK, 1, NBRIG );
    printf( "%3d", dp->parcela );
    setcolor_off();
    printf( " | " );

    setcolor_on( NWHT, NBLK, 1, NBRIG );
    printf( "%3d", dp->parcelas );
    setcolor_off();
    printf( " | " );

    imp_colordate( dp->data, 15, NGRY, NBLK, 1, NBRIG );
    printf( " | " );
    
    setcolor_on( dp->valor < 0 ? NBLU : NRED, NBLK, 1, NBRIG );
    imp_wdoub( fabs( dp->valor ), 10, 2, J_DIR );
    setcolor_off();
    printf( " | " );
    
    setcolor_on( ct[ dp->conta - 1 ].cor, NBLK, 1, NBRIG );
    imp_wstr( ct[ dp->conta - 1 ].desc, 6, J_ESQ, MINUSC );
    setcolor_off();
    printf( " | " );
    
    setcolor_on( NYEL, NBLK, 1, NBRIG );
    imp_wstr( dp->descricao, 42, J_ESQ, MAIUSC );
    setcolor_off();
    printf( " | " );
}

void impdesp_det( uint8_t lin, uint8_t col, struct despesa dp )
{
    double total = dp.parcelas * dp.valor;
    time_t segs_hoje;
    double saldo;

    segs_hoje = now_segs();
    
    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
    setcurs( lin, col );
    printf( "Descrição..........: " );

    setcolor_on( NGRY, NBLK, 1, NBRIG );
    printf( "%4d", dp.id );
    setcolor_off();
    printf( " | " );
    
    setcolor_on( NYEL, NBLK, 1, NBRIG );
    imp_wstr( dp.descricao, 42, J_ESQ, MAIUSC );

    lin++;
    
    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
    setcurs( lin, col );
    printf( "Compra.............: " );
    setcolor_on( NGRN, NBLK, 1, NBRIG );
    printf( "%4d", dp.id_compra );
    setcolor_off();
    printf( " | " );

    setcolor_on( dp.fixa ? NMAG : NCYN, NBLK, 1, NBRIG );
    printf( "%s", dp.fixa ? "FIX" : "VAR" );

    setcolor_off();
    printf( " | " );

    imp_colordate( dp.data_reg, 15, NGRN, NBLK, 1, NBRIG );

    printf( " | " );

    setcolor_on( total < 0 ? NBLU : NRED, NBLK, 1, NBRIG );
    imp_wdoub( fabs( total ), 10, 2, J_DIR );
    setcolor_off();

    lin++;

    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
    setcurs( lin, col );
    printf( "Tipo...............: " );
    setcolor_on( COR_TIPO, NBLK, 1, NBRIG );
    printf( "%4d", dp.tipo );
    setcolor_off();
    printf( " | " );

    setcolor_on( COR_TIPO, NBLK, 1, NBRIG );
    imp_wstr( tp[ dp.tipo - 1 ].desc, 42, J_ESQ, MAIUSC );
    
    lin++;

    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
    setcurs( lin, col );
    printf( "Conta..............: " );
    setcolor_on( COR_CONTA, NBLK, 1, NBRIG );
    printf( "%4d", dp.conta );
    setcolor_off();
    printf( " | " );

    setcolor_on( COR_CONTA, NBLK, 1, NBRIG );
    imp_wstr( ct[ dp.conta - 1 ].descc, 30, J_ESQ, MAIUSC );
    
    lin++;
    
    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
    setcurs( lin, col );
    printf( "Parcela............: " );
    setcolor_on( NWHT, NBLK, 1, NBRIG );
    printf( "%4d", dp.parcela );
    setcolor_off();
    printf( " | " );

    setcolor_on( NWHT, NBLK, 1, NBRIG );
    printf( "%3d", dp.parcelas );
    setcolor_off();
    printf( " | " );

    imp_colordate( dp.data, 15, NWHT, NBLK, 1, NBRIG );

    printf( " | " );
  
    setcolor_on( dp.valor < 0 ? NBLU : NRED, NBLK, 1, NBRIG ); 

    imp_wdoub( fabs( dp.valor ), 10, 2, J_DIR );

    setcolor_off();
    printf( "  -> " );

    saldo = dp.valor * ( dp.parcelas - dp.parcela );
    
    setcolor_on( saldo < 0 ? NBLU : NRED, NBLK, 1, NBRIG );
    imp_wdoub( fabs( saldo ), 10, 2, J_DIR );
    setcolor_off();
    printf( " (" );
    setcolor_on( NLYEL, NBLK, 1, NBRIG );
    printf( "%d", dp.parcelas - dp.parcela );
    setcolor_off();
    printf( ")" );
    
    lin++;

    setcolor_on( COR_MSG, NBLK, 1, NBRIG );
    setcurs( lin, col );
    printf( "Situação...........: " );

    if( dp.pago )
    {
        setcolor_on( NBLU, NBLK, 1, NBRIG );
        printf( "liquidada" );
    }
    else
    {
        if( dp.data >= segs_hoje )
        {
            setcolor_on( NLGRN, NBLK, 1, NBRIG );
            printf( "a vencer" );
        }
        else
        {
            setcolor_on( NRED, NBLK, 1, NBRIG );
            printf( "vencida" );
        }
    }
}
