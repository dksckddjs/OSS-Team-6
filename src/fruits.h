#ifndef FRUITS_H
#define FRUITS_H

#include "types.h"
#include <stdlib.h>

void redraw_fruits(FRUITLIST *fruits);
void kill_fruits(FRUITLIST* fruits);
void kill_fruit(FRUITLIST *fruits, int posy, int posx);
void kill_fruit_by_ptr(FRUITLIST *fruits, FRUITNODE *fruit);
void kill_fruit_at_pos(FRUITLIST *fruits, int index);
FRUITNODE *fruit_is_on(FRUITLIST *fruits, int posy, int posx);
void grow_fruit(GAME* game);
void get_fruit(FRUITNODE *fruit, int posy, int posx);

#endif /* FRUITS_H */
