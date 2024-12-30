#include "orcdom.h"
#include "colors.h"
#include "newlib.h"
#include "brws.h"
#include "ordenacao.h"
#include <wchar.h>

char kb[ BUFSIZ ];


struct conta ct[] = { { 1, L"BOLE", L"boleto bancário", NRED },
                      { 2, L"CELO", L"cartão elo", NBLU },
                      { 3, L"CAME", L"cartão ame mastercard", NMAG },
                      { 4, L"VISA", L"cartão visa", NWHT },
                      { 5, L"DCTA", L"débito em conta", NGRY },
                      { 6, L"DFOL", L"débito em folha", NCYN },
                      { 7, L"DINH", L"dinheiro", NGRN },
                      { 8, L"TBAN", L"transferência bancária", NYEL } };

struct grupo gp[] = { { 1, L"alimentação" },
                      { 2, L"saúde" },
                      { 3, L"educação" },
                      { 4, L"cultura" },
                      { 5, L"transporte" },
                      { 6, L"lazer/Entretenimento" },
                      { 7, L"moradia" },
                      { 8, L"pessoal/família" },
                      { 9, L"animais de estimação" },
                      { 10, L"comunicação" },
                      { 11, L"seguros" },
                      { 12, L"impostos/Tarifas" },
                      { 13, L"dívidas" } };
                    
struct tipo tp[] = { { 1, L"açougue", 1 },
                     { 2, L"conta de Água", 7 },
                     { 3, L"aluguel de imóvel", 7 },
                     { 4, L"condomínio", 7 },
                     { 5, L"cartão anuidade", 12 },
                     { 6, L"bares e restaurantes", 6 },
                     { 7, L"clube recreativo", 6 },
                     { 8, L"combustível", 5 },
                     { 9, L"dentista", 2 },
                     { 10, L"oftamologista", 2 },
                     { 11, L"empréstimo pessoal", 13 },
                     { 12, L"empréstimo imobiliário", 13 },
                     { 13, L"empréstimo outros", 13 },
                     { 14, L"associações e Entidades de classe", 12 },
                     { 15, L"escola material", 3 },
                     { 16, L"escola livros", 3 },
                     { 17, L"escola mensalidade", 3 },
                     { 18, L"escola uniformes", 3 },
                     { 19, L"exames médicos", 2 },
                     { 20, L"farmácia", 2 },
                     { 21, L"feira livre", 1 },
                     { 22, L"gás", 7 },
                     { 23, L"hoteis e pousadas", 6 },
                     { 24, L"informática", 8 },
                     { 25, L"eletrônicos", 8 },
                     { 26, L"internet acesso", 10 },
                     { 27, L"aluguel de carro", 5 },
                     { 28, L"prestação do carro", 13 },
                     { 29, L"internet conteúdo", 6 },
                     { 30, L"iptu", 12 },
                     { 31, L"irpf", 12 },
                     { 32, L"licenciamento, DPVAT e IPVA", 12 },
                     { 33, L"limpeza faxineira", 7 },
                     { 34, L"livros", 4 },
                     { 35, L"conta de Luz", 7 },
                     { 36, L"material de construção", 7 },
                     { 37, L"mecânico", 5 },
                     { 38, L"médico", 2 },
                     { 39, L"mesada", 8 },
                     { 40, L"móveis e eletrodomésticos", 7 },
                     { 41, L"multas de trânsito", 5 },
                     { 42, L"padaria", 1 },
                     { 43, L"passagens", 5 },
                     { 44, L"peças e acessórios automotivos", 5 },
                     { 45, L"pedreiro, pintor, eletricista e encanador", 7 },
                     { 46, L"pet alimentação", 9 },
                     { 47, L"pet saúde", 9 },
                     { 48, L"pet higiene", 9 },
                     { 49, L"pet outros", 9 },
                     { 50, L"óculos", 8 },
                     { 51, L"plano de celular", 10 },
                     { 52, L"cinema", 4 },
                     { 53, L"teatro", 4 },
                     { 54, L"estádios", 10 },
                     { 55, L"plano de saúde", 2 },
                     { 56, L"plano dentário", 2 },
                     { 57, L"previdência privada", 11 },
                     { 58, L"seguro de carro", 11 },
                     { 59, L"seguro de vida", 11 },
                     { 60, L"seguro residencial", 11 },
                     { 61, L"supermercado", 1 },
                     { 62, L"suplementos naturais", 1 },
                     { 63, L"telefonia", 10 },
                     { 64, L"tv por assinatura", 6 },
                     { 65, L"utilidades domésticas", 7 },
                     { 66, L"vestuário", 8 },
                     { 67, L"calçados", 8 },
                     { 68, L"cama, mesa e banho", 8 },
                     { 69, L"presentes", 8 },
                     { 70, L"perfumaria", 8 },
                     { 71, L"cabeleireiro, manicure e pedicure", 8 },
                     { 72, L"estacionamento", 5 },
                     { 73, L"cursos", 3 } };

wchar_t nome_prog[ 128 ] = L"Orçamento Doméstico - 3.5";

int main( int argc, char *argv[] )
{
	FILE *arq;
	int16_t opc;
    uint8_t lin = LIN_FORM_APP;
    uint8_t col = COL_FORM_APP;
    uint8_t qtd_opcs = 8;
    char c[] = "\xe2\x96\xb6";
    BR *menu = NULL;
    LN *li = NULL;
    CO *co = NULL;
    TB *tb = NULL;
    SB *sb = NULL;
    wchar_t *statusbar1[] = { L"[ESC]", L"[HOME]", L"[END]" };
    wchar_t *statusbar2[] = { L"Sai", L"Inicio", L"Fim" };
    void (*f[])() = { cadastra, manut, pagamento, menu_relat, imp_fopag, ordenacao, config, NULL };
    wchar_t *opcs[] = { L"Cadastro", L"Manutenção", L"Pagamento", L"Relatórios", L"Resultado",
                        L"Ordena arquivo", L"Configuração", L"Sair" };

    setlocale( LC_ALL, "" );
    setlocale( LC_COLLATE, "pt_BR.UTF-8" );
    //setlocale( LC_CTYPE, "pt_BR.UTF-8" );
    
    cls_scr();
    
    arq = fopen( "despesas.dat", "rb+" );

    if( !arq )
	{
		arq = fopen( "despesas.dat", "wb+" );

		if( !arq )
		{
            ret_wmsg_form( L"Falha na abertura do arquivo de despesas.", RETMSG_ERRO );
			return 1;
		}
	}
	
	fclose( arq );

    if( !init_config() )
    {
        ret_wmsg_form( L"Falha na criacão/abertura do arquivo de configuracão.", RETMSG_ERRO );
		return 1;
    }

    menu = create_bw();

    setbox_bw( menu, BOX_MENU_BW );
    setcolor_bw( menu, COLOR_FOCUS_BW, NWHT, 238, 1, NNORM );
    setcoord_bw( menu, lin, col, qtd_opcs );
    setctrlpercent_bw( menu, false );
    /* setdatatypekey_bw( menu, sizeof( int ) ); */
    /* setfunckey_bw( menu, cmpintline_bw ); */

    for( uint8_t inc = 0; inc < 3; inc++ )
    {
        sb = addstatusbar_bw( menu, statusbar1[ inc ] );
        setcolorstatusbar_bw( sb, NGRY, NBLK, 2, NNORM, NREVE );
        
        sb = addstatusbar_bw( menu, statusbar2[ inc ] );
        setcolorstatusbar_bw( sb, NLCYN, NWHT, 3, NNORM, NITAL, NREVE );
    
        if( inc < 2 ) addstatusbar_bw( menu, L" " );
    }

    tb = addtitlebar_bw( menu, L" Menu Principal " );
    setcolortitlebar_bw( tb, NGRY, NBLK, 2, NBRIG, NREVE );

    for( uint8_t inc = 0; inc < qtd_opcs; inc++ )
    {
        /* li = addline_bw( menu, &inc, inc + 1, f[ inc ] ); */
        li = addline_bw( menu, NULL, inc + 1, f[ inc ] );

        co = addnewcol_bw( menu, li, opcs[ inc ], DWCHAR );
        setattrcol_bw( co, wcslen( opcs[ inc  ] ), 0, J_ESQ, false );
        setcolorcol_bw( co, NGRY, NBLK, 1, NNORM );
        
        if( inc == 3 )
        {
            co = addnewcol_bw( menu, li, c, DCHAR );
            setattrcol_bw( co, 1, 0, J_NAT, false );
            setcolorcol_bw( co, NLYEL, NBLK, 1, NNORM );
            /* setshowdbarcol_bw( co, false ); */
        }
    }
        
	while( true )
	{
        init_form( nome_prog );
        
        setcolor_off();

        init_bw( menu );

        opc = exec_bw( menu );

        if( opc == -1 || opc == qtd_opcs )
        {
            if( hdialog_button( L"Confirma a saida?", CONFIRM_DEFAULT_N ) == 1 ) break;
        }
	}

    cls_scr();

    free_bw( menu );
	
	return 0;
}

bool ordena()
{
    if( copia_arqbin( "despesas.dat", "entrada.dat", sizeof( struct despesa ) ) &&
        copia_arqbin( "despesas.dat", "despesas.dat.bkp", sizeof( struct despesa ) ) )
    {
        if( ord_arq_ext( "entrada.dat", "saida.dat", 100, sizeof( struct despesa ), cmp_sort ) )
        {
            if( rename( "saida.dat", "despesas.dat" ) == 0 )
            {
                remove( "entrada.dat" );
                remove( "saida.dat" );
                return true;
            }
        }
    }

    return false;
}

void ordenacao()
{
    if( ordena() )
    {
        ret_wmsg_form( L"Ordenação realizada com sucesso. ", RETMSG_NO_ERRO );
    }
    else
    {
        ret_wmsg_form( L"Falha na Ordenação. ", RETMSG_ERRO );
    }
}

int cmp_sort( const void *a, const void *b )
{
    if( ( *( const struct despesa * )a ).data > ( *( const struct despesa * )b ).data ) return 1;
    else if( ( *( const struct despesa * )a ).data < ( *( const struct despesa * )b ).data ) return -1;
    else return 0;
    
    //return wcscoll( ( *( const struct despesa * )a ).descricao, ( *( const struct despesa * )b ).descricao );

}
