#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <inttypes.h>
#include "steam_account.h"
#include "comparators.h"

#define THRESHOLD 64

struct merge_arg {
        struct list_head *head;
        size_t n;
        game_comparator_t *cmp;
};

//map errno to err info
static char* hyr_errmap[] = {
        "success",  //0
        "out of memory", //-1, EHYR_MEMORYOUT
        "item not exist", //-2 EHYR_NOTFOUND
};

//inline local functions
static inline void free_company(struct list_head *node);
static inline void free_achievement(struct list_head *node);
static inline void free_game(struct list_head *node);
static inline void *merge_sort_packed(void *arg);

//local functions
static void merge_sort(struct list_head *head, size_t n, game_comparator_t *cmp);
static void merge(struct list_head *head, struct list_head *mid,
                  game_comparator_t *cmp);


static inline void free_company(struct list_head *node)
{
        struct company *company_ptr = list_container_of(node,
                                                        struct company,
                                                        company_node);
        free(company_ptr->intro);
        free(company_ptr->name);

        list_delete(node, struct company, company_node);
}

static inline void free_achievement(struct list_head *node)
{
        struct achievement *achvmnt_ptr = list_container_of(node,
                                                            struct achievement,
                                                            achvmnt_node);

        free(achvmnt_ptr->intro);

        list_delete(node, struct achievement, achvmnt_node);
}

static inline void free_game(struct list_head *node)
{
        struct game *game_ptr = list_container_of(node, struct game, game_node);

        free(game_ptr->name);
        free(game_ptr->intro);
        list_iter(&game_ptr->achievements, free_achievement);

        list_delete(node, struct game, game_node);
}

void init_account(struct steam_account *acct)
{
        list_init(&acct->companies);
        list_init(&acct->games);
}

void destroy_account(struct steam_account *acct)
{
        list_iter(&acct->companies, free_company);
        list_iter(&acct->games, free_game);
}

//helper function to make pthread_crate happy
static inline void *merge_sort_packed(void *arg)
{
        struct merge_arg *mrg_arg = arg;

        merge_sort(mrg_arg->head, mrg_arg->n, mrg_arg->cmp);
        return NULL;
}

//merge TWO parts within ONE list
static void merge(struct list_head *head, struct list_head *mid,
                  game_comparator_t *cmp)
{
        struct list_head *head_ptr1, *head_ptr2;  //next node to be sorted
        struct list_head *curr_ptr;  //next position to insert after
        struct game *game_ptr1, *game_ptr2;

        //the loop invariant here is:
        //curr_ptr : next position to insert after
        //(curr_ptr, head_ptr1) : sorted
        //[head_ptr1, head_ptr2) : part1
        //[head_ptr2, head) : part2
        head_ptr1 = head->next;
        head_ptr2 = mid->next;
        curr_ptr = head;
        while (head_ptr1 != head_ptr2 && head_ptr2 != head) {
                game_ptr1 = list_container_of(head_ptr1, struct game, game_node);
                game_ptr2 = list_container_of(head_ptr2, struct game, game_node);

                //we HAVE TO obtain the next pointer before we move the current
                if ((*cmp)(game_ptr1, game_ptr2) < 0) {
                        head_ptr1 = head_ptr1->next;
                        list_move_front(curr_ptr, head_ptr1->prev);
                } else {
                        head_ptr2 = head_ptr2->next;
                        list_move_front(curr_ptr, head_ptr2->prev);
                }

                curr_ptr = curr_ptr->next;
        }
#ifdef DEBUG
        printf("================================================================\n");
        for(struct list_head *curr = head->next; curr != head; curr = curr->next)
        {
                print_game(curr);
        }
#endif
}

//sort a list
static void merge_sort(struct list_head *head, size_t n, game_comparator_t *cmp)
{
        struct list_head *midl, *midr;
        struct list_head listl, listr;
        size_t count;

        if (n <= 1)
                return;

        list_init(&listl);
        list_init(&listr);

        //find the middle node
        midl = head;
        for (count = n/2; count != 0; --count)
                midl = midl->next;
        midr = midl->next;

        //construct two lists for their own merge sort
        //so that we WON'T have critical region here
        listl.next = head->next;
        head->next->prev = &listl;
        listl.prev = midl;
        midl->next = &listl;

        listr.next = midr;
        midr->prev = &listr;
        listr.prev = head->prev;
        head->prev->next = &listr;


        if (n <= THRESHOLD) {
                //simply do the typical merge sort
                merge_sort(&listl, n/2, cmp);
                merge_sort(&listr, n - n/2, cmp);
        } else {
                //do it multi-threadedly
                pthread_t merge_thread[2];
                struct merge_arg merge_arg1, merge_arg2;

                merge_arg1.head = &listl;
                merge_arg1.n = n/2;
                merge_arg1.cmp = cmp;

                merge_arg2.head = &listr;
                merge_arg2.n = n - merge_arg1.n;
                merge_arg2.cmp = cmp;

                pthread_create(&merge_thread[0], NULL,
                               merge_sort_packed, &merge_arg1);
                pthread_create(&merge_thread[1], NULL,
                               merge_sort_packed, &merge_arg2);

                pthread_join(merge_thread[0], NULL);
                pthread_join(merge_thread[1], NULL);

        }

        //recalculate the mid pointer
        midl = listl.prev;
        midr = listr.next;

        //recontruct the original list;
        midl->next = midr;
        midr->prev = midl;
        head->next = listl.next;
        listl.next->prev = head;
        head->prev = listr.prev;
        listr.prev->next = head;

        merge(head, midl, cmp);


}

void sort_game(struct steam_account *acct, enum sort_option option)
{
        game_comparator_t *cmp;
        struct list_head *curr;
        size_t n;

        n = 0;
        curr = &acct->games;
        while (curr->next != &acct->games) {
                n++;
                curr = curr->next;
        }

        if (n <= 1)
                return;

        switch (option) {
        case BY_NAME:
                cmp = compare_by_name;
                break;
        case BY_INTRO:
                cmp = compare_by_intro;
                break;
        case BY_HRS:
                cmp = compare_by_hrs;
                break;
        case BY_COMPANY:
                cmp = compare_by_company;
                break;
        }

        merge_sort(&acct->games, n, cmp);
}

struct game *add_game(struct steam_account *acct, char *name,
             char *intro, double hrs_played, char *company_name)
{
        struct game *new_game = malloc(sizeof(struct game));
        struct company *company;

        if(NULL == new_game) {
                return NULL;
        }

        //copy the attributes to the new struct, init the achievement list
        if (NULL == (new_game->name = malloc(strlen(name) + 1))) {
                free(new_game);
                return NULL;
        }
        strcpy(new_game->name, name);

        if (NULL == (new_game->intro = malloc(strlen(intro) + 1))) {
                free(new_game->name);
                free(new_game);
                return NULL;
        }
        strcpy(new_game->intro, intro);

        new_game->hrs_played = hrs_played;
        list_init(&new_game->achievements);

        //fill the company entry
        if (NULL == (company = query_company(acct, company_name))) {
                free(new_game->name);
                free(new_game->intro);
                free(new_game);
                return NULL;
        }
        new_game->company = company;

        list_insert_back(&acct->games, &new_game->game_node);

        return new_game;
}


int delete_game(struct steam_account *acct, char *name)
{
        struct game *game_ptr;

        if (NULL ==  (game_ptr = query_game(acct, name)))
                return EHYR_NOTFOUND;

        free_game(&game_ptr->game_node);

        return 0;
}

struct game *query_game(struct steam_account *acct, char *name)
{
        struct game *game_ptr;
        struct list_head *curr;

        curr = &acct->games;
        while (curr->next != &acct->games) {
                curr = curr->next;
                game_ptr = list_container_of(curr, struct game, game_node);

                if (!strcmp(name, game_ptr->name)) {
                        return game_ptr;
                }
        }

        //not found
        if (curr->next == &acct->games) {
                return NULL;
        }
}

int add_achievement(struct steam_account *acct,
                    char *game_name, char *achvmnt_intro)
{
        struct achievement *achvmnt_ptr;
        struct game *game_ptr;

        if (NULL == (achvmnt_ptr = malloc(sizeof(struct achievement)))) {
                return EHYR_MEMORYOUT;
        }

        if (NULL == (game_ptr = query_game(acct, game_name))) {
                free(achvmnt_ptr);
                return EHYR_NOTFOUND;
        }

        //copy the intro to our new struct
        if (NULL == (achvmnt_ptr->intro = malloc(strlen(achvmnt_intro + 1)))) {
                free(achvmnt_ptr);
                return EHYR_MEMORYOUT;
        }
        strcpy(achvmnt_ptr->intro, achvmnt_intro);

        list_insert_back(&game_ptr->achievements, &achvmnt_ptr->achvmnt_node);

        return 0;
}

struct company *add_company(struct steam_account *acct, char *name, char *intro)
{
        struct company *company_ptr;

        if (NULL == (company_ptr = malloc(sizeof(struct company)))) {
                return NULL;
        }

        if (NULL == (company_ptr->name = malloc(strlen(name) + 1))) {
                free(company_ptr);
                return NULL;
        }

        if (NULL == (company_ptr->intro = malloc(strlen(intro) + 1))) {
                free(company_ptr->name);
                free(company_ptr);
                return NULL;
        }

        strcpy(company_ptr->name, name);
        strcpy(company_ptr->intro, intro);

        list_insert_back(&acct->companies, &company_ptr->company_node);

        return company_ptr;

}

struct company *query_company(struct steam_account *acct, char *name)
{
        struct company *company_ptr;
        struct list_head *curr;

        curr = &acct->companies;
        while (curr->next != &acct->companies) {
                curr = curr->next;
                company_ptr = list_container_of(curr, struct company,
                                                company_node);

                if (!strcmp(company_ptr->name, name)) {
                        return company_ptr;
                }
        }

        return NULL;
}

void hyr_perror(int errno)
{
        printf("hry's steam lib manager error : %s\n",
               hyr_errmap[-errno]);
}

