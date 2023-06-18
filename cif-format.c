#include "cif-format.h"


int read_line(FILE * file, char* line)
{
    bzero(line, MAX_LINE_SIZE * sizeof(char));

    char ch = -1;
    char line_cursor = 0;
    while(ch != '\n'){
        ch = (char)fgetc(file);
        line[line_cursor++] = ch;

        if(ch == EOF){
            return 1;
        }

        if(line_cursor >= MAX_LINE_SIZE){
            printf("MAX_LINE_SIZE error\n");
            exit(-1);
        }
    }

    return 0;
}

CifFile* read_cif_file(char* path)
{

}

void print_atom_record(Atom* atom)
{
    printf(
        "atom_id: %u\tres_id: %u\tatom_type: %c\tatom_pdb_type: %s\tres_name: %s\tchain_id: %s\tx: %.3lf\ty: %.3lf\tx: %.3lf\n",
        atom->atom_id, atom->res_id, atom->atom_type, atom->atom_pdb_type, atom->res_name, atom->chain_id, 
        atom->coordinates.x, atom->coordinates.y, atom->coordinates.z
        );
}

int is_atom_record(char* line)
{
    int checking = line[0] == 'A' && line[1] == 'T' && line[2] == 'O' && line[3] == 'M';
    return checking;
}

int parse_if_atom_record(char* line, Atom* atom)
{

    if(!is_atom_record(line)){
        return 0;
    }
    char record[6] = {0};
    int delimiter1 = 0;
    sscanf(
        line, 
        "%s %d %c %s . %s %s %d %d ? %lf %lf %lf", 
        record, &atom->atom_id, &atom->atom_type, atom->atom_pdb_type, atom->res_name, atom->chain_id, &delimiter1, &atom->res_id,
        &atom->coordinates.x, &atom->coordinates.y, &atom->coordinates.z
    );
    return 1;
}

int count_chains(Atom* atom_record, unsigned int* chains_counter)
{
    static char last_chain_id = 0;
    if( last_chain_id != atom_record->chain_id[0] ){
        last_chain_id = atom_record->chain_id[0];
        *chains_counter = *chains_counter + 1;
        return 1; // new chain
    }
    return 0; // old chain
}

void count_res(Atom* atom_record, unsigned int* res_counter)
{
    static unsigned int recent_res_id = 0;

    if(recent_res_id != atom_record->res_id){
        *res_counter = *res_counter + 1;
        recent_res_id = atom_record->res_id;
    }
}

CifParams * read_cif_params(char * path)
{
    CifParams * params = (CifParams*)malloc(sizeof(CifParams));
    bzero(params, sizeof(CifParams));

    FILE * file = fopen(path, "r");

    if(!file){
        perror("Unable to open file: ");
        exit(-1);
    }

    int read_line_status = 0;
    char* line = (char*)malloc(MAX_LINE_SIZE * sizeof(char));
    int record_reading_status;
    Atom * atom_record = (Atom*)malloc(sizeof(Atom));

    int* chains_sizes = (int*)malloc(sizeof(int) * MAX_CHAINS_SIZE);
    unsigned int chains_sizes_cursor = -1;
    bzero(chains_sizes, sizeof(int) * MAX_CHAINS_SIZE);

    while(read_line_status != 1){
        read_line_status = read_line(file, line);
        
        record_reading_status = parse_if_atom_record(line, atom_record);

        if(!record_reading_status){
            continue;
        }

        // print_atom_record(atom_record);

        int is_new_chain = count_chains(atom_record, &params->chains_count);

        if(!is_new_chain){
            chains_sizes[chains_sizes_cursor]++;
        }else{
            chains_sizes_cursor++;
        }

        count_res(atom_record, &params->residues_count);
        params->atoms_count++;

    }

    params->chainsParameters = (ChainParams*)malloc(sizeof(ChainParams) * params->chains_count);
    bzero(params->chainsParameters, sizeof(ChainParams) * params->chains_count);

    for(int i = 0; i < params->chains_count; i++){
        params->chainsParameters[i].atoms_len = chains_sizes[i];
        printf("chain: %i, size: %u\n", i + 1, params->chainsParameters[i].atoms_len);
    }

    printf("chains_count: %u\n", params->chains_count);
    printf("residues_count: %u\n", params->residues_count);
    printf("atoms_count: %u\n", params->atoms_count);
    free(line);
}
