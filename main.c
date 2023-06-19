#include "cif-format.h"

int main(int argc, char * argv[])
{
    int is_aa_sequence_builder_required = 0;

    printf("CIF stat reading...");
    
    CifParams * params = read_cif_params("1H8E.cif");
    
    printf("completed\n");

    for(int i = 0; i < params->chains_count; i++){
        printf("chain: %5i\tatoms_size: %5u\tres_size: %5u\n", i + 1, params->chainsParameters[i].atoms_len, params->chainsParameters[i].residues_len);
    }
    printf("chains_count: %u\n", params->chains_count);
    printf("residues_count: %u\n", params->residues_count);
    printf("atoms_count: %u\n", params->atoms_count);

    CifFile* cif_file = make_cif_object("1H8E.cif", params);

    printf("cif object is generated\n");

    if(is_aa_sequence_builder_required){
        for(int i = 0; i < params->chains_count; i++){
            printf("chain_id: %i\n", i);
            for(int j = 0; j < cif_file->chains[i].residue_size; j++){
                printf("%s->", cif_file->chains[i].residues[j].res_name);
            }
            printf("\b\b  \n\n");
        }
    }

    free(cif_file);

    return 0;
}