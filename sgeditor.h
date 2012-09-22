struct savegame
{
    int r1_1;
    int r1_2;
    int r1_3;
    int r1_4;
    int r2_1;
    int max_crew;
    int ss_name_len;
    char *ss_name;
};
typedef struct savegame SAVEGAME;
void usage(char *argv);
void parse_data(SAVEGAME save, char *buffer);
void print_data(SAVEGAME save);
int read_4_le_bytes_as_int(char *buffer, int offset);
