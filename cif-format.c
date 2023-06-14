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
    while(read_line_status != 1){
        read_line_status = read_line(file, line);

        // Выполняем действие по парсингу параметров для каждой строки cif файла
        // Данные пишем в params

        // Прежде всего определяем количество цепей. Для этого значения инициализируем память для параметров цепей
        // Заполняем количество атомов, количество остатков а так же параметры для каждой цепи в порядке очереди

        printf("%s ", line);
    }
    free(line);
}
