# hyr_steam
As is known to all, hyr is a parvenue. He hire us to write a list implementation to help him out of the overhead of managing his unfathomable long game list on steam.
##interfaces
###function manipulating accounts
`void init_account(struct steam_account *acct);`
>initiate an account `acct`

`void destroy_account(struct steam_account *acct);`
>free all the memory allocated to store information for `acct`  

###functions manipulating games
`struct game *add_game(struct steam_account *acct, char *name,
             char *intro, double hrs_played, char *company_name);`
>given add a game to `acct'  

>`name` : the game's name  

>`intro` : the introcution to the game  

>`hrs_played` : how many hours have been wasted on the game  

>`company_name` : name of the game's company(the company should be exist in the company list, or it'll fail)  

>if succeed, return the newly added game struct, failed NULL  

`struct game *query_game(struct steam_account *acct, char *name);`
>given the game's name, return a game's struct, or NULL if the game is not in the list  

`int delete_game(struct steam_account *acct, char *name);`  
>given the game's name, remove it from the list  

`int add_achievement(struct steam_account *acct, char *game_name, char *achvmnt_intro);`
>add a piece of achievement information to the game specified by `name`  

>return an errno if failed  

`void sort_game(struct steam_account *acct, enum sort_option option);`
>given an account `acct`, sort the game according to the given `option`  

>`possible` options are(one at a time):  

>`BY_NAME` : sort by name  

>`BY_COMPANY` : sort by company  

>`BY_HRS` : sort by hours spent on the game  

>`BY_INTRO` : sort by the game's description  

###functions manipulating companies
`struct company *add_company(struct steam_account *acct, char *name, char *intro);`  
>add a company to `acct`'s company list  

>`name` : the company's name  

>`intro` : the company's description   

>return value : same as `add_game()`  

`struct company *query_company(struct steam_account *acct, char *name);`
>the same thing as `query_game` for the company list  

###printing functions
`static inline void print_game(struct list_head *game_node);`
>given a pointer to the `game` struct's `game_node` member, print the information of a game  

`static inline void print_company(struct list_head *company_node);`
>given a pointer to the `company` struct's `company_node` member, print the information of a company  

`static inline void print_all_company(struct steam_account *acct);`
>print all the companies in to `acct`'s companies list  

`static inline void print_all_game(struct steam_account *acct)`
>print all the games in to `acct`'s game list  

`void hyr_perror(int errno);`
>given errno, print out the error information  


