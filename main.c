#include "steam_account.h"

int main(void)
{
        struct steam_account hyr;

        init_account(&hyr);

        add_company(&hyr, "Valve", "a good company");
        add_company(&hyr, "Galve", "a gay company");
        add_company(&hyr, "Balve", "a gnu based company");
        add_game(&hyr, "CS:GO", "a very fantastic FPS.", 50, "Valve");
        add_game(&hyr, "CS:GAY", "a very fantastic GPF.", 102, "Galve");
        add_game(&hyr, "CS:GBA", "based on GNU, a coding game", 22, "Balve");
        add_game(&hyr, "CS:BOY", "it's not a gal game", 1024.33, "Balve");

        add_achievement(&hyr, "CS:GO", "shoot 1000 people");
        add_achievement(&hyr, "CS:GAY", "meet 1000 gays");
        add_achievement(&hyr, "CS:GAY", "meet 10000 gays");
        add_achievement(&hyr, "CS:BOY", "gang 1024 boys");

        printf("==========sort by name==========\n");
        sort_game(&hyr, BY_NAME);
        print_all_game(&hyr);

        printf("==========sort by company==========\n");
        sort_game(&hyr, BY_COMPANY);
        print_all_game(&hyr);

        printf("==========sort by hrs_played==========\n");
        sort_game(&hyr, BY_HRS);
        print_all_game(&hyr);

        printf("==========sort by intro==========\n");
        sort_game(&hyr, BY_INTRO);
        print_all_game(&hyr);

        destroy_account(&hyr);

        return 0;
}
