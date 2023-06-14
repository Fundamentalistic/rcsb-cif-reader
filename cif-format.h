#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_SIZE 256


typedef struct {
    double x;
    double y;
    double z;
} vector_3d;

typedef struct
{
    unsigned int atom_id;
    unsigned int res_id;
    char atom_type;
    char atom_pdb_type;
    char res_name;
    char chain_id;
    vector_3d coordinates;
} Atom;

typedef struct 
{
    unsigned int res_id;
    unsigned int res_name;
    int atom_sizes;
    Atom * atoms;
    char chain_id;
} Residue;

typedef struct
{
    Atom * atoms;
    Residue * residues;
    unsigned int atoms_size;
    unsigned int residue_size;
} Chain;

typedef struct {
    Chain * chains;
} CifFile;

typedef struct {
    unsigned int residues_len;
    unsigned int atoms_len;
} ChainParams;

typedef struct {
    unsigned int chains_count;
    ChainParams * chainsParameters;
} CifParams;


CifParams* read_cif_params(char* path);
CifFile* read_cif_file(char* path);