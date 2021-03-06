/*
 dev: Mickeymouse, Moutarde and Nepta
 manager: Word
 Copyright © 2011
 
 You should have received a copy of the
 GNU General Public License along with this program.
 If not, see  <http://www.gnu.org/licenses/>.
 */


#include <stdbool.h>

#include "map/map.h"
#include "enemy/enemy.h"
#include "tower/tower.h"
#include "utils/viewport.h"
#include "event/event.h"
#include "list/list.h"
#include "utils/menu.h"
#include "utils/button.h"

// Global variables

Viewport* _viewport;
Map* _map;
char* _path;
Case _cell; // for debug (candy_cane)
int framecounter = 0;

/**
 * \fn void initPath(char* argv0)
 * \brief Finds the executable path from the current working directory
 *
 * \param argv0 Invocation command of the executable
 * \return 
 */
void initPath(char* argv0) {
	int trimLength = strrchr(argv0, '/') + 1 - argv0;
	_path = calloc(trimLength, 1);
	strncpy(_path, argv0, trimLength);
}

// FIXME This function is used for file loading in other files too..
/**
 * \fn char* getPath(char* resource)
 * \brief Converts a resource path from executable-relative to a path relative to the current working directory
 *
 * \param resource The resource path relative to the executable
 * \return The resource path relative to the working directory
 */
char* getPath(char* resource) {
	// Basically we just append the resource to the root _path
	char* fullPath = calloc(strlen(_path) + strlen(resource) + 1, 1);
	return strcat(strcpy(fullPath, _path), resource);
}

int main(int argc, char* argv[]) {
	// Init
	initPath(argv[0]);
	SDL_Surface* screen = NULL;
	SDL_Event event;
	bool isInPlay = true;
	void* seed;
	srand((int)seed);
	int previousTime = 0, currentTime = 0;
   Events *flags = createEventFlags();

	SDL_Init(SDL_INIT_VIDEO);
	SDL_SetEventFilter(eventFilter);
	
	screen = SDL_SetVideoMode(800, 600, 32, SDL_HWSURFACE | SDL_ASYNCBLIT | SDL_DOUBLEBUF | SDL_NOFRAME);
	SDL_WM_SetCaption("Tower Defense", NULL);
	
	Map* map = createMap(getPath("resources/Forest.png"));
	_map = map;
	
	SDL_Rect surface = {0, 0, 720, 600};
	Viewport* viewport = createViewport(screen, surface, map);
	_viewport = viewport;
	
	// FIXME uh? what's this thing?
	surface.x = 800 - 80; surface.y = 0; surface.h = 80; surface.w = 600;
	
	// Creation of the enemies
	TypeEn *whiteCat = createTypeEn(100, 5, false, true, true, false, 1,getPath("resources/white_transparent_cat.png"));
	TypeEn *blackCat = createTypeEn(100, 5, false, true, true, false, 1,getPath("resources/black_transparent_cat.png"));
	Enemy *cat1 = createEnemy(1,1,whiteCat);
	Enemy *cat2 = createEnemy(1,10,whiteCat);
	Enemy  *cat3 = createEnemy(5,5,blackCat);
	Enemy *cat4 = createEnemy(21,4,blackCat);
	
   TypeEn *zombie = createTypeEn(100,5,false,true,true,false,1,getPath("resources/zombie.png"));
   Enemy *zombie1 = createEnemy(4,4,zombie);
   Enemy *zombie2 = createEnemy(9,4,zombie);
   Enemy *zombie3 = createEnemy(9,9,zombie);
   Enemy *zombie4 = createEnemy(7,14,zombie);

   //Add enemy in the List
   List *catList = newList(cat4);
   pushList((void*)catList,cat2);
   pushList((void*)catList,cat3);
//   pushList((void*)catList,cat1);
   
   List *zombieList = newList(zombie1);
/*   pushList((void*)zombieList,zombie2);*/
/*   pushList((void*)zombieList,zombie3);*/
/*   pushList((void*)zombieList,zombie4);*/
   
//   removeEnemyFromList(cat4,catList);

   //TOWER
   TypeTo *tower = createTypeTo(0,0,0,0,false,false,false,false,NULL,NULL,getPath("resources/tower.png"));
   upgradeTypeTo(tower,0.5,getPath("resources/towerUP.png"));
   flags->selectedTower = tower->nextType;
   Tower *tower1 = createTower(7,7,tower);

   List *towerList = newList(tower1);
   flags->towerList = towerList;
   
	// Create and Renders the right panel game menu
	SDL_Rect surfaceMenu = {720, 0, 800, 600};
	Menu* menu = menu_create(screen, surfaceMenu);
	menu_loadBackground(menu, "resources/enemyFont.gif");
		// For testing only, we add a few random buttons
		menu_addButton(menu, button_createBuildButton(tower));
		menu_addButton(menu, button_createBuildButton(tower));
		menu_addButton(menu, button_createBuildButton(tower));
	menu_render(menu);


	_cell = *getCase(20,11);
	// Main loop
	while(isInPlay) {
		// Managing the events
		isInPlay = manageEvents(viewport, flags);

		// Redraws the map (viewport contents) before blitting stuff on it
		updateViewport(viewport);
		
		
///////////////////////////// DEBUG WALL /////////////////////////////
   SDL_Rect position;
	for(int i=0;i < _map->nbCaseW;i++){
		for(int j=0;j < _map->nbCaseH;j++){
		   Case cell = *getCase(i,j);
		   position.x = cell.x;
		   position.y = cell.y;
			if(map->matrice[i][j].hasTower == 2){
            SDL_Surface *wall = IMG_Load(getPath("resources/brick.png"));
			   blitToViewport(viewport, wall, NULL, &position);
			}
		}
	}
	position.x = _cell.x;
	position.y = _cell.y;
    blitToViewport(viewport, IMG_Load(getPath("resources/candy_cane.png")), NULL, &position);
/////////////////////////////////////////////////////////////////////
		
      // Move enemies
      if(flags->enemy_Path_Calculation){
         pathReCalculation(catList);
         pathReCalculation(zombieList);
         flags->enemy_Path_Calculation = false;
      }
      moveEnemyList(zombieList);
      moveEnemyList(catList);

		// Blit enemies
      drawEnemyList(zombieList);
      drawEnemyList(catList);
      //Blit TOWER
/*      if(event.key.keysym.sym == SDLK_u){*/
/*         upgrade(tower1);*/
/*      }*/
      drawTowerList(towerList);
	
	/* This should be handled by event.c
      switch(event.key.keysym.sym){
         case SDLK_a:
            flags->selectedTower = tower;
          break;
         case SDLK_b:
            flags->selectedTower = tower->nextType;
          break;
         default:
          break;
	
      }*/
/*      */

      
		// Ask SDL to swap framebuffers to update the displayed screen
		SDL_Flip(screen);
	
		// Managing frames
		currentTime = SDL_GetTicks();
		if (currentTime - previousTime <= 20) {
			SDL_Delay(20 - (currentTime - previousTime));
		}
		
		// DEBUG
		printf("Frame %i : %ims\n", framecounter++, currentTime - previousTime);		

		previousTime = SDL_GetTicks();
	}
	SDL_Quit();
	
	return EXIT_SUCCESS;
}
