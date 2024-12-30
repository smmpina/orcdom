#include "colors.h"
#include "newlib.h"
#include "orcdom.h"
#include <stdint.h>

void pagamento()
{
    FILE *arq;
	struct despesa dp;
    time_t segs_ini;
    time_t segs_fin;
    double tot_ser = 0.0;
    long int reg = 0;
    uint16_t num_ser = 0;
    uint16_t linhas = 0;
    uint8_t pagina = 1;
    int16_t periodo;
    int8_t opc;
    uint8_t lin = LIN_FORM_APP;
    uint8_t col = COL_FORM_APP + 1;
    uint8_t max_lin = 24;
    bool arq_vazio = true;
    size_t sucesso;
    wchar_t moddesc[ 128 ];

    wcscpy( moddesc, nome_prog );
    wcscat( moddesc, L" - Pagamento" );
    
    arq = fopen( "despesas.dat", "rb+" );
    
    init_form( moddesc );

    if( !arq )
	{
        ret_wmsg_form( L"Falha na abertura do arquivo.", RETMSG_ERRO );
		return;
	}

    periodo = input_daterange( lin, col, &segs_ini, &segs_fin );

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

    lin += 2;
            
    setcolor_on( NLWHT, NBLK, 1, NBRIG );
    setcurs( lin, col );
    imp_wstr( L"Página: ", 10, J_ESQ, false );
    
    setcolor_on( NLYEL, NBLK, 1, NBRIG );
    printf( "%02d", pagina );
    setcolor_off();

    lin += 2;

    rewind( arq );

    while( fread( &dp, sizeof( struct despesa ), 1, arq ) )
	{
        if( !dp.apagado && !dp.pago && ( periodo ? ( dp.data >= segs_ini && dp.data <= segs_fin ) : 1 ) )  
        {
            arq_vazio = false;

            setcurs( lin, col );
            setcolor_on( NLWHT, NBLK, 2, NBRIG, NBLKS );
            printf( "->" );
            setcolor_off();
            setcurs( lin, col + 3 );
            mostra_dados( &dp );

            opc = hdialog_button( L"Confirma?", CONFIRM_DEFAULT_Y );

            switch( opc )
            {
                case 0:
                    break;
                case 1:
                    dp.pago = 1;

                    fseek( arq, reg, SEEK_SET );
                    sucesso = fwrite( &dp, sizeof( struct despesa ), 1, arq );

                    if( sucesso != 1 )
                    {
                        fclose( arq );
                        ret_wmsg_form( L"Erro no pagamento.", RETMSG_ERRO );
                        return;
                    }

                    break;
                case -1:
                    fclose( arq );
                    ret_wmsg_form( L"Cancelado pelo operador.", RETMSG_ERRO );
                    return;
            }

            setcurs( lin, col );
            setcolor_on( NLYEL, NBLK, 1, NBRIG );
            printf( "Ok" );
            setcolor_off();
            setcurs( lin, col + 3 );
            mostra_dados( &dp );

            lin++;
            linhas++;
            num_ser++;
            tot_ser += dp.valor;
        }

        if( linhas == max_lin )
        {
            opc = hdialog_button( L"Continua na próxima página?", CONFIRM_DEFAULT_Y );

            if( opc == 0 || opc == -1  )    
            {
                ret_wmsg_form( L"Cancelado pelo operador.", RETMSG_ERRO );
                fclose( arq );
                return;
            }

            cleanarea( LIN_FORM_APP + 2, COL_FORM_APP, LIND_FORM_APP, COLD_FORM_APP );

            lin = LIN_FORM_APP + 2;
            pagina++;
            
            setcolor_on( NLWHT, NBLK, 1, NBRIG );
            setcurs( lin, col );
            imp_wstr( L"Página: ", 10, J_ESQ, false );
            
            setcolor_on( NLYEL, NBLK, 1, NBRIG );
            printf( "%02d", pagina );
            
            setcolor_off();

            linhas = 0;
            lin += 2;
        }
            
        reg = ftell( arq );
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

    cleanarea( LIN_FORM_APP, COL_FORM_APP, LIND_FORM_APP, COLD_FORM_APP );

    setcurs( LIN_FORM_APP + 1, COL_FORM_APP + 1 );

    setcolor_on( NLYEL, NBLK, 1, NBRIG );
    printf( "%03d", num_ser );
    setcolor_off();

    printf( " -> " );
            
    setcolor_on( tot_ser < 0 ? NBLU : NRED, NBLK, 1, BRIG );
    imp_wdoub( fabs( tot_ser ), 10, 2, J_DIR );
    setcolor_off();
        
    ret_wmsg_form( L"Fim...", RETMSG_NO_ERRO );
        
    return;
}
