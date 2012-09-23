struct event
{
    int name_len;
    char name[50];
    unsigned int value;
}; typedef struct event EVENT;
struct crew
{
    int name_len;
    int race_len;
    char name[50];
    char race[50];
    int unkn1;
    int health;
    int unkn3;
    int unkn4;
    int unkn5;
    int unkn6;
    int unkn7;
    int unkn8;
    int unkn9;
    int unkn10;
    int unkn11;
    int unkn12;
    int unkn13;
    int unkn14;
    int unkn15;
    int unkn16;
    int unkn17;
    int unkn18;
    int unkn19;
}; typedef struct crew CREW;
struct savegame
{
    int unkn1;
    int unkn2;
    int unkn3;
    int unkn4;
    int unkn5;
    int unkn6;
    int unkn7;
    int start_crew_len;
    int current_crew_len;
    int max_crew;
    int ss_class_len;
    int ss_name_len;
    int ss_name2_len;
    int ss_integrity;
    int ss_missiles;
    int ss_fuel;
    int ss_droids;
    int scrap;
    char *ss_class;
    char *ss_name;
    char *ss_name2;
    int ss_type_len;
    int ss_type2_len;
    char *ss_type;
    char *ss_type2;
    int max_events;
    EVENT *events;
    CREW *start_crew;
    CREW *current_crew;
};
typedef struct savegame SAVEGAME;
void usage(char *argv);
void parse_data(SAVEGAME *save, char *buffer);
void print_data(SAVEGAME *save);
int read_4_le_bytes_as_int(char *buffer, int offset);
