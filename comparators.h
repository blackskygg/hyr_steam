#ifndef COMPARATORS_H_
#define COMPARATORS_H_

#include <string.h>
#include "steam_account.h"

typedef int game_comparator_t(struct game *, struct game *);

static inline int compare_by_name(struct game *game1, struct game *game2)
{
        return strcmp(game1->name, game2->name);
}

static inline int compare_by_intro(struct game *game1, struct game *game2)
{
        return strcmp(game1->intro, game2->intro);
}

static inline int compare_by_hrs(struct game *game1, struct game *game2)
{
        return game1->hrs_played - game2->hrs_played > 0 ? 1 : -1;
}

static inline int compare_by_company(struct game *game1, struct game *game2)
{
        return strcmp(game1->company->name, game2->company->name);
}

#endif
