#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/stat.h>
#include "sgeditor.h"

int main(int argc, char **argv)
{
    // variables required to read file
    FILE *file;
    char *buffer;
    char argbuffer[256];
    unsigned long fileLen;
    SAVEGAME save;

    //
    // VERIFY THAT FILE EXISTS
    //
    // note! this code allows for buffer overflow!
    if (argc > 1)
    {
        strcpy(argbuffer, argv[1]);
        if (stat(argbuffer) != 0)
        {
            fprintf(stderr, "ERROR! File not found!\n");
            return 1;
        }
    }
    else
    {
        strcpy(argbuffer, argv[0]);
        usage(argbuffer);
        return 1;
    }
    //
    // READ FILE
    //
    file = fopen(argbuffer, "rb");
    if (!file) // verify that we can open the file
    {
        fprintf(stderr, "ERROR! Unable to open file!");
        fclose(file);
        return 1;
    }
    // get file length
    fseek(file, 0, SEEK_END);
    fileLen=ftell(file);
    fseek(file, 0, SEEK_SET);
    buffer=(char *)malloc(fileLen+1);
    if (!buffer)
    {
        fprintf(stderr, "ERROR! Out of memory!\n");
        return 1;
    }
    fread(buffer, fileLen, 1, file);
    parse_data(save, buffer);
    print_data(save);
    fclose(file);

    // garbage collection
    free(buffer);
    free(save.ss_name);
    return 0;
}
void parse_data(SAVEGAME save, char *buffer)
{
    save.r1_1 = read_4_le_bytes_as_int(buffer, 0+4*0);
    save.r1_2 = read_4_le_bytes_as_int(buffer, 0+4*1);
    save.r1_3 = read_4_le_bytes_as_int(buffer, 0+4*2);
    save.r1_4 = read_4_le_bytes_as_int(buffer, 0+4*3);
    save.r2_1 = read_4_le_bytes_as_int(buffer, 0+4*4);
    save.max_crew = read_4_le_bytes_as_int(buffer, 0+4*5);
    save.ss_name_len = read_4_le_bytes_as_int(buffer, 0+4*6);
    // allocate memory for ship name and parse name
    save.ss_name=(char *)malloc(save.ss_name_len+1);
    for (int i=0; i<save.ss_name_len+1;i++)
    {
        save.ss_name[i] = (char)buffer[0+4*7+i];
    }
    save.ss_name[save.ss_name_len] = '\0';
    printf("Max crew: %d\n", save.max_crew);
    printf("Ship name: %s\n", save.ss_name);
}
int read_4_le_bytes_as_int(char *buffer, int offset)
{
    return buffer[offset+0] | (buffer[offset+1]<<8) | (buffer[offset+2]<<16) | (buffer[offset+3]<<24);
}
void print_data(SAVEGAME save)
{

}
void usage(char *argbuffer)
{
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "%s <savegame file>\n", argbuffer);
    fprintf(stderr, "Example: %s continue.sav\n", argbuffer);
}
