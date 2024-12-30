#ifndef _CONFIG_H
#define _CONFIG_H

#include <stdint.h>

struct config
{
    double previ;
    double inss;
    double inpc;
    double pdesc_previ;
    double pdesc_cassi_pes;
    double pdesc_cassi_dep;
    int nr_deps;
    int mes1_previ13;
    int mes2_previ13;
    int mes1_inss13;
    int mes2_inss13;
    double valor_es;
    double valor_capec;
};

int init_config();
void config();

#endif
