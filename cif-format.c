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
        "atom_id: %u res_id: %u atom_type: %c atom_pdb_type: %s res_name: %s chain_id: %i x: %.3lf y: %.3lf x: %.3lf\n",
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
        "%s %d %c %s . %s %c %d %d ? %lf %lf %lf", 
        record, &atom->atom_id, &atom->atom_type, atom->atom_pdb_type, atom->res_name, &atom->chain_id, &delimiter1, &atom->res_id,
        &atom->coordinates.x, &atom->coordinates.y, &atom->coordinates.z
    );
    return 1;
}

CifParams * read_cif_params(char * path)
{
    CifParams * params = (CifParams*)malloc(sizeof(CifParams));

    FILE * file = fopen(path, "r");

    if(!file){
        perror("Unable to open file: ");
        exit(-1);
    }

    int read_line_status = 0;
    char* line = (char*)malloc(MAX_LINE_SIZE * sizeof(char));
    int record_reading_status;
    Atom * atom_record = (Atom*)malloc(sizeof(Atom));
    while(read_line_status != 1){
        read_line_status = read_line(file, line);
        
        record_reading_status = parse_if_atom_record(line, atom_record);

        if(!record_reading_status){
            continue;
        }

        print_atom_record(atom_record);
    }
    free(line);
}
