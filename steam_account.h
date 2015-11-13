#ifndef STEAM_ACCOUNT_H_
#define STEAM_ACCOUNT_H_

#include <stdlib.h>
#include <stdio.h>
#include "list.h"

#define EHYR_MEMORYOUT -1
#define EHYR_NOTFOUND -2

struct company {
        struct list_head company_node;  //node in the company list

        char *name;
        char *intro;
};

struct achievement {
        struct list_head achvmnt_node;  //node in the achievement list

        char *intro;
};

struct game {
        struct list_head game_node;  //node in the game list

        char *name;
        char *intro;
        double hrs_played;
        struct company *company;
        struct list_head achievements;  //achievement list
};

struct steam_account {
        struct list_head companies;  //company list
        struct list_head games;   //game list
};

enum sort_option {
        BY_NAME,
        BY_INTRO,
        BY_HRS,
        BY_COMPANY,
};

//function manipulating accounts
void init_account(struct steam_account *acct);
void destroy_account(struct steam_account *acct);

//functions manipulating games
struct game *add_game(struct steam_account *acct, char *name,
             char *intro, double hrs_played, char *company_name);
struct game *query_game(struct steam_account *acct, char *name);
int delete_game(struct steam_account *acct, char *name);
int add_achievement(struct steam_account *acct,
                    char *game_name, char *achvmnt_intro);
void sort_game(struct steam_account *acct, enum sort_option option);



//functions manipulating companies
struct company *add_company(struct steam_account *acct, char *name, char *intro);
struct company *query_company(struct steam_account *acct, char *name);

//printing functions
static inline void print_achievement(struct list_head *achvmnt_node)
{
        struct achievement *achvmnt_ptr;

        achvmnt_ptr = list_container_of(achvmnt_node, struct achievement,
                                        achvmnt_node);

        printf("\t%s\n", achvmnt_ptr->intro);

}

static inline void print_company(struct list_head *company_node)
{
        struct company *company_ptr;

        company_ptr = list_container_of(company_node, struct company
                                        , company_node);

        printf("Company Name : %s\n", company_ptr->name);
        printf("Company Introduction : \n"
               "\t%s\n", company_ptr->intro);
}

static inline void print_all_achievement(struct game *game)
{
        list_iter(&game->achievements, print_achievement);
}

static inline void print_all_company(struct steam_account *acct)
{
        list_iter(&acct->companies, print_company);
}

static inline void print_game(struct list_head *game_node)
{
        struct game *game_ptr;

        game_ptr = list_container_of(game_node, struct game, game_node);
        printf("Game : %s\n", game_ptr->name);
        if(NULL != game_ptr->company)
                print_company(&game_ptr->company->company_node);
        printf("Introduction :\n"
               "\t%s\n", game_ptr->intro);
        printf("Hours played : %f\n", game_ptr->hrs_played);
        printf("Achievements : \n");
        print_all_achievement(game_ptr);
        printf("\n");
}

static inline void print_all_game(struct steam_account *acct)
{
        list_iter(&acct->games, print_game);
}

void hyr_perror(int errno);

#endif
