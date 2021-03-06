/*
 dev: Mickeymouse, Moutarde and Nepta
 manager: Word
 Copyright © 2011
 
 You should have received a copy of the
 GNU General Public License along with this program.
 If not, see  <http://www.gnu.org/licenses/>.
 */


#include "event.h"


int eventFilter(SDL_Event* event) {
	switch(event->type) {
		case SDL_QUIT:
		case SDL_KEYDOWN:
		case SDL_MOUSEBUTTONDOWN:
			return 1;
		default:
			return 0;
	}
}

/**
 * \fn bool manageEvent(SDL_Event event, Viewport* viewport)
 * \brief Manage an event.
 *
 * \param event The event to manage.
 * \param viewport The viewport that can be moved.
 * \return False if the the game ends, true if not.
 */
bool manageEvent(SDL_Event event, Viewport* viewport, Events *flags) {
	bool isInPlay = true;
	
	switch(event.type) {
		// Quit game
		case SDL_QUIT:
			isInPlay = false;
			break;
			
		// Key pressed
		case SDL_KEYDOWN:
			switch(event.key.keysym.sym) {
				// Quit game
				case SDLK_ESCAPE:
					isInPlay = false;
					break;
					
				// Move view
				case SDLK_UP:
					moveViewport(viewport, UP);
					break;
					
				case SDLK_DOWN:
					moveViewport(viewport, DOWN);
					break;
					
				case SDLK_LEFT:
					moveViewport(viewport, LEFT);
					break;
					
				case SDLK_RIGHT:
					moveViewport(viewport, RIGHT);
					break;
					
				default:
					break;
			}
		   break;

      // Mouse left click
      case SDL_MOUSEBUTTONDOWN:
         if(event.button.button == SDL_BUTTON_LEFT) {
            Case* caseClicked = whichCase(event.button.x, event.button.y);
            TypeTo* simpleTowerType = flags->selectedTower;
            Case *viewportOffset = whichCase(viewport->mapsurface.x,viewport->mapsurface.y);
            int mapPositionX = caseClicked->xx + viewportOffset->xx;
            int mapPositionY = caseClicked->yy + viewportOffset->yy;
            Tower *tower = createTower(mapPositionX, mapPositionY, simpleTowerType);
            if(tower){
               flags->enemy_Path_Calculation = true;
               pushList((void*)flags->towerList,tower);
               drawTower(tower);
            }
         }
         break;
			
		default:
			break;
	}
	
	return isInPlay;
}

bool manageEvents(Viewport* viewport, Events *flags) {
	SDL_Event event;
	// Process events until game exit, or there's none left
	while(SDL_PollEvent(&event)) {
		if(!manageEvent(event, viewport, flags)) return false;
	}
	return true;
}

/**
 * \fn createEventFlags()
 * \brief Deficient by design?
 *
 * \return hiden global variable :- °
 */

Events* createEventFlags(){
   Events *flags = (Events*)malloc(sizeof *flags);
   flags->enemy_Path_Calculation = false;
  return flags;
}

/**
 * \fn void pathReCalculation(List *enemyList)
 * \brief set the enemy in "waiting for path" state
 * \param enemyList a list of enemy who have to change there path
 */
 
void pathReCalculation(List *enemyList){
   while(enemyList->nextList){
      ((Enemy*)enemyList->item)->list = NULL;
      enemyList = enemyList->nextList;
   }
   ((Enemy*)enemyList->item)->list = NULL;
}
