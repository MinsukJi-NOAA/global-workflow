#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grb2.h"
#include "wgrib2.h"
#include "fnlist.h"
#include "CodeTable4_4.h"

/*
 * Set_ensm_derived_fcst.c
 *
 * converts PDT 0,1 -> 2    8,11 -> 12
 *  changes code table 4.7 and adds "number of ensemble members"
 *
 * 11/2011: Public Domain: Wesley Ebisuzaki
 *
 */

/*
 * HEADER:100:set_ensm_derived_fcst:misc:2:convert PDT 0,1,2 -> 2, 8,11,12 -> 12, X=code table 4.7 Y=num ens members
 */

int f_set_ensm_derived_fcst(ARG2) {

    int i, n, n0, pdt;
    struct local_struct {
        int code_table_4_7;
	int num_ens;
	int n_sec4;
	unsigned char *sec4;
    };
    struct local_struct *save;

    if (mode == -1) {
	*local = save = (struct local_struct *)malloc( sizeof(struct local_struct));
	i = atoi(arg1);
	if (i < 0 || i > 255) fatal_error("set_ensm_derived_fcst: code table 4.7 0..255 found %s", arg1);
	save->code_table_4_7 = i;

	i = atoi(arg2);
	if (i < 0 || i > 255) fatal_error("set_ensm_derived_fcst: num ens emembers 0..255 found %s", arg2);
	save->num_ens = i;

	save->n_sec4 = 0;
	save->sec4 = NULL;
    }
    save = *local;

    if (mode < 0) return 0;

    pdt = code_table_4_0(sec);

    /* all ready pdt=2 or 12 */
    if (pdt == 2) {
	sec[4][35] = save->code_table_4_7;
	sec[4][36] = save->num_ens;
	return 0;
    }
    if (pdt == 12) {
	sec[4][36] = save->code_table_4_7;
	sec[4][37] = save->num_ens;
	return 0;
    }

    n0 = GB2_Sec4_size(sec);

    if (pdt == 0 || pdt == 1) {
	n = 36;
    }
    else if (pdt == 8) {
	n = n0 + 2;
    }
    else if (pdt == 11) {
	n = n0 - 1;
    }
    else {
	fprintf(stderr,"set_ensm_derived_fcst: only works with product defn template 0,1,2,8,11,12\n");
	return 0;
    }

    if (n > save->n_sec4) {
	save->n_sec4 = n;
	if (save->sec4) free(save->sec4);
	save->sec4 = (unsigned char *) malloc(save->n_sec4 * sizeof(unsigned char) );
	if (save->sec4 == NULL) fatal_error("set_ensm_derived_fcst: memory allocation error","");
    }
    
    // now to add ensemble information
    if (pdt == 0 || pdt == 1) {
        for (i = 0; i < 34; i++) save->sec4[i] = sec[4][i];
        save->sec4[34] = save->code_table_4_7;
        save->sec4[35] = save->num_ens;
    }
    else if (pdt == 8) {
        for (i = 0; i < 34; i++) save->sec4[i] = sec[4][i];
        save->sec4[34] = save->code_table_4_7;
        save->sec4[35] = save->num_ens;
        for (i = 36; i < n; i++) save->sec4[i] = sec[4][i-2];
    }
    else if (pdt == 11) {
        for (i = 0; i < 34; i++) save->sec4[i] = sec[4][i];
        save->sec4[34] = save->code_table_4_7;
        save->sec4[35] = save->num_ens;
        for (i = 36; i < n; i++) save->sec4[i] = sec[4][i+1];
    }

    uint_char(n, save->sec4);                   // length of sec[4]
    save->sec4[7] = 0;
    save->sec4[8] = pdt >= 8 ? 12 : 2;
    sec[4] = save->sec4; 
    return 0;
}
