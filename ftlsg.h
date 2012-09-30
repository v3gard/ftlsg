struct event
{
    int name_len;
    char name[50];
    int value;
}; typedef struct event EVENT;
struct crew
{
    int name_len;
    int race_len;
    char name[50];
    char race[50];
    int unkn1;
    int health;
    int x_coord;
    int y_coord;
    int room;
    int room_tile;
    int unkn7;
    int skill_pilot;
    int skill_engines;
    int skill_shields;
    int skill_weapons;
    int skill_repair;
    int skill_combat;
    int gender;
    int stat_repairs;
    int stat_combat_kills;
    int stat_piloted_evasions;
    int stat_skill_masteries;
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
    unsigned long remaining_len;
    char* remaining;
};
typedef struct savegame SAVEGAME;
void usage(char *argv);
void parse_data(SAVEGAME *save, char *buffer, unsigned long fileLen);
void print_data(SAVEGAME *save);
void save_data(SAVEGAME *save);
int read_4_le_bytes_as_int(char *buffer, int offset);
void user_input(SAVEGAME *save);
void user_input_crew(SAVEGAME *save, int crewid);
