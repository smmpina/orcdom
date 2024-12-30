#ifndef _FOPAG_H
#define _FOPAG_H

#include "newlib.h"
#include "orcdom.h"
#include "brws.h"

struct fopag
{
    int cor_c;
    int cor_f;
    int cor_a;
    double valor;
    wchar_t titulo[ 64 ];
};

double calc_IR( double salario, double descprev, int deps );
LN *disp_tipo( BR *b, LN *l );
LN *disp_dados( BR *b, LN *l );
BR *stmenu_fopag( uint8_t lin, uint8_t col );
BR *stmenu_resfopag( uint8_t lin, uint8_t col );
BR *stmenu_resumo( uint8_t lin, uint8_t col );

#endif
