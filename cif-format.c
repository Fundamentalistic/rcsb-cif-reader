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

int is_new_chain_record(Atom* atom_record)
{
    static char recent_chain_id = 0;
    if( recent_chain_id != atom_record->chain_id[0] ){
        recent_chain_id = atom_record->chain_id[0];
        return 1; // new chain
    }
    return 0; // old chain
}

int is_new_res_record(Atom* atom_record)
{
    static unsigned int recent_res_id = 0;
    if(recent_res_id != atom_record->res_id){
        recent_res_id = atom_record->res_id;
        return 1; // new res
    }
    return 0; // old res
}

int count_res(Atom* atom_record, unsigned int* res_counter)
{
    static unsigned int last_res_id = 0;

    if(last_res_id != atom_record->res_id){
        *res_counter = *res_counter + 1;
        last_res_id = atom_record->res_id;
        return 1; // new res
    }
    return 0; // old res
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

    int* res_sizes = (int*)malloc(sizeof(int) * MAX_CHAINS_SIZE);
    unsigned int res_sizes_cursor = -1;

    bzero(chains_sizes, sizeof(int) * MAX_CHAINS_SIZE);
    bzero(res_sizes, sizeof(int) * MAX_CHAINS_SIZE);

    while(read_line_status != 1){
        read_line_status = read_line(file, line);
        
        record_reading_status = parse_if_atom_record(line, atom_record);

        if(!record_reading_status){
            continue;
        }

        int is_new_chain = count_chains(atom_record, &params->chains_count);

        if(!is_new_chain){
            chains_sizes[chains_sizes_cursor]++;
        }else{
            chains_sizes_cursor++;

        }

        int is_new_res = count_res(atom_record, &params->residues_count);

        if(is_new_res){
            res_sizes[chains_sizes_cursor]++;
        }

        params->atoms_count++;

    }

    params->chainsParameters = (ChainParams*)malloc(sizeof(ChainParams) * params->chains_count);
    bzero(params->chainsParameters, sizeof(ChainParams) * params->chains_count);

    for(int i = 0; i < params->chains_count; i++){
        params->chainsParameters[i].atoms_len = chains_sizes[i];
        params->chainsParameters[i].residues_len = res_sizes[i];
    }

    free(line);
    fclose(file);

    // params->chains_count++;

    return params;
}


CifFile* make_cif_object(char* path, CifParams* params)
{
    FILE * file = fopen(path, "r");

    if(!file){
        perror("Unable to open file: ");
        exit(-1);
    }

    CifFile* cif_file = (CifFile*)malloc(sizeof(CifFile));
    bzero(cif_file, sizeof(CifFile));

    cif_file->chains = (Chain*)malloc(sizeof(Chain) * params->chains_count);
    bzero(cif_file->chains, sizeof(Chain) * params->chains_count);
    
    for(int i = 0; i < params->chains_count; i++){
        cif_file->chains[i].atoms_size = params->chainsParameters[i].atoms_len;
        cif_file->chains[i].residue_size = params->chainsParameters[i].residues_len;
        cif_file->chains[i].atoms = (Atom*)malloc(sizeof(Atom) * cif_file->chains[i].atoms_size + 1);
        cif_file->chains[i].residues = (Residue*)malloc(sizeof(Residue) * cif_file->chains[i].residue_size + 1);
        bzero(cif_file->chains[i].atoms, sizeof(Atom) * cif_file->chains[i].atoms_size + 1);
        bzero(cif_file->chains[i].residues, sizeof(Residue) * cif_file->chains[i].residue_size + 1);
    }

    int read_line_status = 0;
    char* line = (char*)malloc(MAX_LINE_SIZE * sizeof(char));
    int record_reading_status;
    Atom * atom_record = (Atom*)malloc(sizeof(Atom));

    unsigned int chains_cursor = -1;
    unsigned int res_cursor = 0;
    unsigned int atom_cursor = 0;

    while(read_line_status != 1){
        
        read_line_status = read_line(file, line);
        
        record_reading_status = parse_if_atom_record(line, atom_record);

        if(!record_reading_status){
            continue;
        }
        
        if(is_new_chain_record(atom_record)){
            chains_cursor++;
            atom_cursor = 0;
            res_cursor = 0;
        }

        int is_new_res = count_res(atom_record, &params->residues_count);

        if(is_new_res_record(atom_record)){
            cif_file->chains[chains_cursor].residues[res_cursor].res_name[0] = atom_record->res_name[0];
            cif_file->chains[chains_cursor].residues[res_cursor].res_name[1] = atom_record->res_name[1];
            cif_file->chains[chains_cursor].residues[res_cursor].res_name[2] = atom_record->res_name[2];

            cif_file->chains[chains_cursor].residues[res_cursor].res_name[3] = cif_file->chains[chains_cursor].residues[res_cursor].res_name[4] = 0;
            cif_file->chains[chains_cursor].residues[res_cursor].res_id = atom_record->res_id;
            cif_file->chains[chains_cursor].residues[res_cursor].chain_id = *atom_record->chain_id;

            if(chains_cursor > 6 && cif_file->chains[chains_cursor].residues[res_cursor].res_name[0] != 'A'){
                printf("MEMORY ERROR!!! chains_cursor: %i res_cursor: %i\n", chains_cursor, res_cursor);
            }

            if((chains_cursor >= 6) && res_cursor == 0){
                printf("ERROR RES: %s\n", cif_file->chains[chains_cursor].residues[res_cursor].res_name);
            }

            res_cursor++;
        }

        cif_file->chains[chains_cursor].atoms[atom_cursor].atom_id = atom_record->atom_id;
        memcpy(cif_file->chains[chains_cursor].atoms[atom_cursor].atom_pdb_type, atom_record->atom_pdb_type, 4);
        cif_file->chains[chains_cursor].atoms[atom_cursor].atom_type = atom_record->atom_type;
        memcpy(cif_file->chains[chains_cursor].atoms[atom_cursor].chain_id, atom_record->chain_id, 2);
        cif_file->chains[chains_cursor].atoms[atom_cursor].coordinates.x = atom_record->coordinates.x;
        cif_file->chains[chains_cursor].atoms[atom_cursor].coordinates.y = atom_record->coordinates.y;
        cif_file->chains[chains_cursor].atoms[atom_cursor].coordinates.z = atom_record->coordinates.z;
        cif_file->chains[chains_cursor].atoms[atom_cursor].res_id = atom_record->res_id;
        memcpy(cif_file->chains[chains_cursor].atoms[atom_cursor].res_name, atom_record->res_name, 5);

        atom_cursor++;

    }


    fclose(file);

    return cif_file;
}