#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <sys/stat.h>
#include "ftlsg.h"

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
    struct stat sb;
    if (argc > 1)
    {
        strcpy(argbuffer, argv[1]);
        if (stat(argbuffer, &sb) != 0)
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
    if (read_4_le_bytes_as_int(buffer, 0) != 2) // simple file format verification
    {
        fprintf(stderr, "ERROR! Not an FTL savegame!\n");
        return 1;
    }
    parse_data(&save, buffer, fileLen);
    //print_data(&save);
    user_input(&save);
    save_data(&save);
    fclose(file);

    // garbage collection
    free(buffer);
    free(save.ss_name);
    free(save.ss_type);
    free(save.ss_type2);
    free(save.ss_name2);
    free(save.ss_class);
    free(save.remaining);
    free(save.events);
    return 0;
}
void parse_data(SAVEGAME *save, char *buffer, unsigned long fileLen)
{
    int adrp = 0;
    (*save).unkn1 = read_4_le_bytes_as_int(buffer, (adrp));
    (*save).unkn2 = read_4_le_bytes_as_int(buffer, (adrp+=4));
    (*save).unkn3 = read_4_le_bytes_as_int(buffer, (adrp+=4));
    (*save).unkn4 = read_4_le_bytes_as_int(buffer, (adrp+=4));
    (*save).unkn5 = read_4_le_bytes_as_int(buffer, (adrp+=4));
    (*save).max_crew = read_4_le_bytes_as_int(buffer, (adrp+=4));
    (*save).ss_name_len = read_4_le_bytes_as_int(buffer, (adrp+=4));
    // allocate memory for ship name and parse
    (*save).ss_name=(char *)malloc((*save).ss_name_len+1);
    adrp+=4;
    for (int i=0; i<(*save).ss_name_len+1;i++)
    {
        (*save).ss_name[i] = (char)buffer[adrp+i];
    }
    (*save).ss_name[(*save).ss_name_len] = '\0';
    (*save).ss_type_len = read_4_le_bytes_as_int(buffer, (adrp+=(*save).ss_name_len));
    // allocate memory for ship type and parse
    (*save).ss_type=(char *)malloc((*save).ss_type_len+1);
    adrp+=4;
    for (int i=0; i<(*save).ss_type_len+1;i++)
    {
        (*save).ss_type[i] = (char)buffer[adrp+i];
    }
    (*save).ss_type[(*save).ss_type_len] = '\0';
    (*save).unkn6 = read_4_le_bytes_as_int(buffer, (adrp+=(*save).ss_type_len));
    (*save).unkn7 = read_4_le_bytes_as_int(buffer, (adrp+=4));
    (*save).max_events = read_4_le_bytes_as_int(buffer, (adrp+=4));
    // parse all events
    (*save).events = (EVENT *)malloc(sizeof(EVENT)*(*save).max_events); 
    for (int i=0; i<(*save).max_events; i++)
    {
        (*save).events[i].name_len = read_4_le_bytes_as_int(buffer, (adrp+=4));
        adrp+=4;
        for (int j=0; j<((*save).events[i].name_len);j++)
        {
            (*save).events[i].name[j] = (char)buffer[adrp+j];
        }
        (*save).events[i].name[(*save).events[i].name_len] = '\0';
        adrp+=(*save).events[i].name_len;
        (*save).events[i].value = read_4_le_bytes_as_int(buffer, (adrp));
    }
    (*save).ss_type2_len = read_4_le_bytes_as_int(buffer, (adrp+=4));
    // allocate memory for ship type (again) and parse
    (*save).ss_type2=(char *)malloc((*save).ss_type2_len+1);
    adrp+=4;
    for (int i=0; i<(*save).ss_type_len+1;i++)
    {
        (*save).ss_type2[i] = (char)buffer[adrp+i];
    }
    (*save).ss_type2[(*save).ss_type_len] = '\0';
    (*save).ss_name2_len = read_4_le_bytes_as_int(buffer, (adrp+=(*save).ss_type2_len));
    // allocate memory for ship type (again) and parse
    (*save).ss_name2=(char *)malloc((*save).ss_name2_len+1);
    adrp+=4;
    for (int i=0; i<(*save).ss_name2_len+1;i++)
    {
        (*save).ss_name2[i] = (char)buffer[adrp+i];
    }
    (*save).ss_name2[(*save).ss_name2_len] = '\0';
    (*save).ss_class_len = read_4_le_bytes_as_int(buffer, (adrp+=(*save).ss_name2_len));
    (*save).ss_class=(char *)malloc((*save).ss_class_len+1);
    adrp+=4;
    for (int i=0; i<(*save).ss_class_len+1;i++)
    {
        (*save).ss_class[i] = (char)buffer[adrp+i];
    }
    (*save).ss_class[(*save).ss_class_len] = '\0';
    (*save).start_crew_len = read_4_le_bytes_as_int(buffer, (adrp+=(*save).ss_class_len));
    adrp+=4;
    // parse all starting crew
    (*save).start_crew = (CREW *)malloc(sizeof(CREW)*(*save).start_crew_len); 
    for (int i=0; i<(*save).start_crew_len; i++)
    {
        (*save).start_crew[i].race_len = read_4_le_bytes_as_int(buffer, (adrp));
        adrp+=4;
        for (int j=0; j<((*save).start_crew[i].race_len);j++)
        {
            (*save).start_crew[i].race[j] = (char)buffer[adrp+j];
        }
        (*save).start_crew[i].race[(*save).start_crew[i].race_len] = '\0';
        adrp+=(*save).start_crew[i].race_len;
        (*save).start_crew[i].name_len = read_4_le_bytes_as_int(buffer, (adrp));
        adrp+=4;
        for (int j=0; j<((*save).start_crew[i].name_len);j++)
        {
            (*save).start_crew[i].name[j] = (char)buffer[adrp+j];
        }
        (*save).start_crew[i].name[(*save).start_crew[i].name_len] = '\0';
        adrp+=(*save).start_crew[i].name_len;
    }
    (*save).ss_integrity = read_4_le_bytes_as_int(buffer, (adrp));
    (*save).ss_fuel = read_4_le_bytes_as_int(buffer, (adrp+=4));
    (*save).ss_missiles = read_4_le_bytes_as_int(buffer, (adrp+=4));
    (*save).ss_droids = read_4_le_bytes_as_int(buffer, (adrp+=4));
    (*save).scrap = read_4_le_bytes_as_int(buffer, (adrp+=4));
    (*save).current_crew_len = read_4_le_bytes_as_int(buffer, (adrp+=4));
    // parse all current crew
    (*save).current_crew = (CREW *)malloc(sizeof(CREW)*(*save).current_crew_len); 
    for (int i=0; i<(*save).current_crew_len; i++)
    {
        (*save).current_crew[i].name_len = read_4_le_bytes_as_int(buffer, (adrp+=4));
        adrp+=4;
        for (int j=0;j<(*save).current_crew[i].name_len;j++)
        {
            (*save).current_crew[i].name[j] = (char)buffer[adrp+j];
        }
        (*save).current_crew[i].name[(*save).current_crew[i].name_len] = '\0';
        adrp+=(*save).current_crew[i].name_len;
        (*save).current_crew[i].race_len = read_4_le_bytes_as_int(buffer, (adrp));
        adrp+=4;
        for (int j=0; j<((*save).current_crew[i].race_len);j++)
        {
            (*save).current_crew[i].race[j] = (char)buffer[adrp+j];
        }
        (*save).current_crew[i].race[(*save).current_crew[i].race_len] = '\0';
        adrp+=(*save).current_crew[i].race_len;
        (*save).current_crew[i].unkn1 = read_4_le_bytes_as_int(buffer, (adrp));
        (*save).current_crew[i].health = read_4_le_bytes_as_int(buffer, (adrp+=4));
        (*save).current_crew[i].unkn3 = read_4_le_bytes_as_int(buffer, (adrp+=4));
        (*save).current_crew[i].unkn4 = read_4_le_bytes_as_int(buffer, (adrp+=4));
        (*save).current_crew[i].unkn5 = read_4_le_bytes_as_int(buffer, (adrp+=4));
        (*save).current_crew[i].unkn6 = read_4_le_bytes_as_int(buffer, (adrp+=4));
        (*save).current_crew[i].unkn7 = read_4_le_bytes_as_int(buffer, (adrp+=4));
        (*save).current_crew[i].unkn8 = read_4_le_bytes_as_int(buffer, (adrp+=4));
        (*save).current_crew[i].unkn9 = read_4_le_bytes_as_int(buffer, (adrp+=4));
        (*save).current_crew[i].unkn10 = read_4_le_bytes_as_int(buffer, (adrp+=4));
        (*save).current_crew[i].unkn11 = read_4_le_bytes_as_int(buffer, (adrp+=4));
        (*save).current_crew[i].unkn12 = read_4_le_bytes_as_int(buffer, (adrp+=4));
        (*save).current_crew[i].unkn13 = read_4_le_bytes_as_int(buffer, (adrp+=4));
        (*save).current_crew[i].unkn14 = read_4_le_bytes_as_int(buffer, (adrp+=4));
        (*save).current_crew[i].unkn15 = read_4_le_bytes_as_int(buffer, (adrp+=4));
        (*save).current_crew[i].unkn16 = read_4_le_bytes_as_int(buffer, (adrp+=4));
        (*save).current_crew[i].unkn17 = read_4_le_bytes_as_int(buffer, (adrp+=4));
        (*save).current_crew[i].unkn18 = read_4_le_bytes_as_int(buffer, (adrp+=4));
        (*save).current_crew[i].unkn19 = read_4_le_bytes_as_int(buffer, (adrp+=4));
    }
    // parse remaining data as a byte stream (not modifyable)
    adrp+=4;
    unsigned long remaining = fileLen-adrp;
    (*save).remaining_len = remaining;
    (*save).remaining = (char *)malloc(sizeof(char)*remaining+1); 
    for (int i=0; i<remaining;i++)
    {
        (*save).remaining[i] = buffer[adrp+i];
    }
}
int read_4_le_bytes_as_int(char *buffer, int offset)
{
    return (unsigned char) buffer[offset+0] | (buffer[offset+1]<<8) | (buffer[offset+2]<<16) | (buffer[offset+3]<<24);
}
void clearscreen()
{
    if (system("clear")) system( "cls" );
}
void user_input(SAVEGAME *save)
{
    initscr();
    int quit = 0;
    int ch;
    while (1)
    {
        printw("FTL SAVEGAME EDITOR\n");
        printw("===================\n\n");
        printw(" Spaceship: %s\n\n", (*save).ss_name);
        printw(" - Integrity (i): %d\n", (*save).ss_integrity);
        printw(" - Fuel (f): %d\n", (*save).ss_fuel);
        printw(" - Missiles (m): %d\n", (*save).ss_missiles);
        printw(" - Droids (d): %d\n", (*save).ss_droids);
        printw(" - Scrap (s): %d\n\n", (*save).scrap);
        attron(A_BOLD);
        printw("What do you want to change? [I,F,M,D,S] [e=exit] ");
        attroff(A_BOLD);
        refresh();
        ch =  getch();
        refresh();
        switch(ch)
        {
            case 101: // e 
                quit=1;
                break;
            case 105: // i 
                printw("\nSet current integrity value: ");
                scanw("%d", &(*save).ss_integrity);
                break;
            case 102: // f 
                printw("\nSet current fuel value: ");
                scanw("%d", &(*save).ss_fuel);
                break;
            case 100: // d 
                printw("\nSet current droid value: ");
                scanw("%d", &(*save).ss_droids);
                break;
            case 109: // m 
                printw("\nSet current missile value: ");
                scanw("%d", &(*save).ss_missiles);
                break;
            case 115: // s 
                printw("\nSet current scrap value: ");
                scanw("%d", &(*save).scrap);
                break;
        }
        clear();
        if (quit == 1) break;
    }
    endwin();
    //int input = 1;
    //while(1==1)
    //{
    //    if (input == 1)
    //    {
    //        printf("What do you want to change? [I,F,M,D,S] [esc=exit] ");
    //    }
    //    char input;
    //    scanf("%c", &input);
    //    input = 0;
    //    if (input != 10)
    //    {
    //        input = 1;
    //        printf("\nYou pressed %d\n", input);
    //    }
    //}
}
void print_data(SAVEGAME *save)
{
    printf("Unknown #1: %d\n", (*save).unkn1);
    printf("Unknown #2: %d\n", (*save).unkn2);
    printf("Unknown #3: %d\n", (*save).unkn3);
    printf("Unknown #4: %d\n", (*save).unkn4);
    printf("Unknown #5: %d\n", (*save).unkn5);
    printf("Max crew: %d\n", (*save).max_crew);
    printf("Ship name: %s\n", (*save).ss_name);
    printf("Ship type: %s\n", (*save).ss_type);
    printf("Unknown #6: %d\n", (*save).unkn6);
    printf("Unknown #7: %d\n", (*save).unkn7);
    printf("Max events: %d\n", (*save).max_events);
    printf("Event #1: %s", (*save).events[0].name);
    printf(" (%d)\n", (*save).events[0].value);
    for (int i=0;i<(*save).max_events;i++)
    {
        printf("Event #%d: %s", i+1, (*save).events[i].name);
        printf(" (%d)\n", (*save).events[i].value);
    }
    printf("Ship name (again): %s\n", (*save).ss_name);
    printf("Ship type (again): %s\n", (*save).ss_type2);
    printf("Ship class: %s\n", (*save).ss_class);
    printf("Starting crew size: %d\n", (*save).start_crew_len);
    for (int i=0;i<(*save).start_crew_len;i++)
    {
        printf("Start Crew #%d: %s (%s)\n", i+1, (*save).start_crew[i].name, (*save).start_crew[i].race);
    }
    printf("Spaceship integrity: %d\n", (*save).ss_integrity);
    printf("Spaceship fuel: %d\n", (*save).ss_fuel);
    printf("Spaceship missiles: %d\n", (*save).ss_missiles);
    printf("Spaceship droids: %d\n", (*save).ss_droids);
    printf("Scrap: %d\n", (*save).scrap);
    printf("Current Crew Size: %d\n", (*save).current_crew_len);
    for (int i=0;i<(*save).current_crew_len;i++)
    {
        printf("Current Crew #%d: %s (%s)\n", i+1, (*save).current_crew[i].name, (*save).current_crew[i].race);
        printf(" - Unknown  1: (%d)\n", (*save).current_crew[i].unkn1);
        printf(" - Health: %d\n", (*save).current_crew[i].health);
        printf(" - Unknown  3: (%d)\n", (*save).current_crew[i].unkn3);
        printf(" - Unknown  4: (%d)\n", (*save).current_crew[i].unkn4);
        printf(" - Unknown  5: (%d)\n", (*save).current_crew[i].unkn5);
        printf(" - Unknown  6: (%d)\n", (*save).current_crew[i].unkn6);
        printf(" - Unknown  7: (%d)\n", (*save).current_crew[i].unkn7);
        printf(" - Unknown  8: (%d)\n", (*save).current_crew[i].unkn8);
        printf(" - Unknown  9: (%d)\n", (*save).current_crew[i].unkn9);
        printf(" - Unknown 10: (%d)\n", (*save).current_crew[i].unkn10);
        printf(" - Unknown 11: (%d)\n", (*save).current_crew[i].unkn11);
        printf(" - Unknown 12: (%d)\n", (*save).current_crew[i].unkn12);
        printf(" - Unknown 13: (%d)\n", (*save).current_crew[i].unkn13);
        printf(" - Unknown 14: (%d)\n", (*save).current_crew[i].unkn14);
        printf(" - Unknown 15: (%d)\n", (*save).current_crew[i].unkn15);
        printf(" - Unknown 16: (%d)\n", (*save).current_crew[i].unkn16);
        printf(" - Unknown 17: (%d)\n", (*save).current_crew[i].unkn17);
        printf(" - Unknown 18: (%d)\n", (*save).current_crew[i].unkn18);
        printf(" - Unknown 19: (%d)\n", (*save).current_crew[i].unkn19);
    }

}
void save_data(SAVEGAME *save)
{
   FILE *fp; 
   fp = fopen("output.sav", "wb");
   fwrite(&(*save).unkn1, sizeof(int),1,fp);
   fwrite(&(*save).unkn2, sizeof(int),1,fp);
   fwrite(&(*save).unkn3, sizeof(int),1,fp);
   fwrite(&(*save).unkn4, sizeof(int),1,fp);
   fwrite(&(*save).unkn5, sizeof(int),1,fp);
   fwrite(&(*save).max_crew, sizeof(int),1,fp);
   fwrite(&(*save).ss_name_len, sizeof(int),1,fp);
   fwrite((*save).ss_name, sizeof(char),(*save).ss_name_len,fp);
   fwrite(&(*save).ss_type_len, sizeof(int),1,fp);
   fwrite((*save).ss_type, sizeof(char),(*save).ss_type_len,fp);
   fwrite(&(*save).unkn6, sizeof(int),1,fp);
   fwrite(&(*save).unkn7, sizeof(int),1,fp);
   fwrite(&(*save).max_events, sizeof(int),1,fp);
   for(int i=0;i<(*save).max_events;i++)
   {
       fwrite(&(*save).events[i].name_len, sizeof(int),1,fp);
       fwrite((*save).events[i].name, sizeof(char),(*save).events[i].name_len,fp);
       fwrite(&(*save).events[i].value, sizeof(int),1,fp);
   }
   fwrite(&(*save).ss_type2_len, sizeof(int),1,fp);
   fwrite((*save).ss_type2, sizeof(char),(*save).ss_type2_len,fp);
   fwrite(&(*save).ss_name2_len, sizeof(int),1,fp);
   fwrite((*save).ss_name2, sizeof(char),(*save).ss_name2_len,fp);
   fwrite(&(*save).ss_class_len, sizeof(int),1,fp);
   fwrite((*save).ss_class, sizeof(char),(*save).ss_class_len,fp);
   fwrite(&(*save).start_crew_len, sizeof(int),1,fp);
   for(int i=0;i<(*save).start_crew_len;i++)
   {
       fwrite(&(*save).start_crew[i].race_len, sizeof(int),1,fp);
       fwrite((*save).start_crew[i].race, sizeof(char),(*save).start_crew[i].race_len,fp);
       fwrite(&(*save).start_crew[i].name_len, sizeof(int),1,fp);
       fwrite((*save).start_crew[i].name, sizeof(char),(*save).start_crew[i].name_len,fp);
   }
   fwrite(&(*save).ss_integrity, sizeof(int),1,fp);
   fwrite(&(*save).ss_fuel, sizeof(int),1,fp);
   fwrite(&(*save).ss_missiles, sizeof(int),1,fp);
   fwrite(&(*save).ss_droids, sizeof(int),1,fp);
   fwrite(&(*save).scrap, sizeof(int),1,fp);
   fwrite(&(*save).current_crew_len, sizeof(int),1,fp);
   for(int i=0;i<(*save).current_crew_len;i++)
   {
       fwrite(&(*save).current_crew[i].name_len, sizeof(int),1,fp);
       fwrite((*save).current_crew[i].name, sizeof(char),(*save).current_crew[i].name_len,fp);
       fwrite(&(*save).current_crew[i].race_len, sizeof(int),1,fp);
       fwrite((*save).current_crew[i].race, sizeof(char),(*save).current_crew[i].race_len,fp);
       fwrite(&(*save).current_crew[i].unkn1, sizeof(int),1,fp);
       fwrite(&(*save).current_crew[i].health, sizeof(int),1,fp);
       fwrite(&(*save).current_crew[i].unkn3, sizeof(int),1,fp);
       fwrite(&(*save).current_crew[i].unkn4, sizeof(int),1,fp);
       fwrite(&(*save).current_crew[i].unkn5, sizeof(int),1,fp);
       fwrite(&(*save).current_crew[i].unkn6, sizeof(int),1,fp);
       fwrite(&(*save).current_crew[i].unkn7, sizeof(int),1,fp);
       fwrite(&(*save).current_crew[i].unkn8, sizeof(int),1,fp);
       fwrite(&(*save).current_crew[i].unkn9, sizeof(int),1,fp);
       fwrite(&(*save).current_crew[i].unkn10, sizeof(int),1,fp);
       fwrite(&(*save).current_crew[i].unkn11, sizeof(int),1,fp);
       fwrite(&(*save).current_crew[i].unkn12, sizeof(int),1,fp);
       fwrite(&(*save).current_crew[i].unkn13, sizeof(int),1,fp);
       fwrite(&(*save).current_crew[i].unkn14, sizeof(int),1,fp);
       fwrite(&(*save).current_crew[i].unkn15, sizeof(int),1,fp);
       fwrite(&(*save).current_crew[i].unkn16, sizeof(int),1,fp);
       fwrite(&(*save).current_crew[i].unkn17, sizeof(int),1,fp);
       fwrite(&(*save).current_crew[i].unkn18, sizeof(int),1,fp);
       fwrite(&(*save).current_crew[i].unkn19, sizeof(int),1,fp);
   }
   fwrite((*save).remaining, sizeof(char),(*save).remaining_len,fp);
   fclose(fp);
}
void usage(char *argbuffer)
{
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "%s <savegame file>\n", argbuffer);
    fprintf(stderr, "Example: %s continue.sav\n", argbuffer);
}
