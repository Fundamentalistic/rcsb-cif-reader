#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_SIZE 256
#define MAX_CHAINS_SIZE 512


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
    char atom_pdb_type[4];
    char res_name[5];
    char chain_id[2];
    vector_3d coordinates;
} Atom;

typedef struct 
{
    unsigned int res_id;
    char res_name[5];
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
    unsigned int atoms_count;
    unsigned int residues_count;
    ChainParams * chainsParameters;
} CifParams;


CifParams* read_cif_params(char* path);
CifFile* make_cif_object(char* path, CifParams* params);