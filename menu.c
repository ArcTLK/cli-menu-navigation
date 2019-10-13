#include "menu.h"

#ifdef _WIN32
	#include <conio.h>
	#define UP 72
	#define DOWN 80
	#define LEFT 75
	#define RIGHT 77
	#define CTRLKEY 0xE0
#elif __linux__
	#include <termios.h>
	#include <unistd.h>
	#define UP 'A'
	#define DOWN 'B'
	#define LEFT 'D'
	#define RIGHT 'C'
	#define CTRLKEY '\033'
	int getch() {
		struct termios oldattr, newattr;
		int ch;
		tcgetattr(STDIN_FILENO, &oldattr); //getting old attributes of terminal
		newattr = oldattr;
		/*
		 * The following line modifies the local terminal modes.
		 * It disables ICANON and ECHO
		 * Disabling ICANON disables canonical mode/line editing mode.
		 * When ICANON is disabled, the input is made available to the program immediately.
		 * Disabling ECHO ensures that any input is not re-echoed as output.
		 * */
		newattr.c_lflag &= ~(ICANON | ECHO);
		tcsetattr(STDIN_FILENO, TCSANOW, &newattr); //setting new attributes to terminal
		ch = getchar(); //getting character
		tcsetattr(STDIN_FILENO, TCSANOW, &oldattr); //resetting old attributes of terminal
		return ch;
	}
#endif

void generateMenu(Menu* menu) {
	char menuHeadNames[][MENU_ITEM_SIZE] = {"File", "Edit", "Search", "Help"};
	initMenu(menu, menuHeadNames, 4);
	insertSubMenuItem(menu, 0, "New");
	insertSubMenuItem(menu, 0, "Open");
	insertSubMenuItem(menu, 0, "Save");
	insertSubMenuItem(menu, 0, "Save As");
	insertSubMenuItem(menu, 1, "Cut");
	insertSubMenuItem(menu, 1, "Copy");
	insertSubMenuItem(menu, 1, "Paste");
	insertSubMenuItem(menu, 1, "Delete");
	insertSubMenuItem(menu, 2, "Find");
	insertSubMenuItem(menu, 2, "Replace");
	insertSubMenuItem(menu, 3, "About");
}

void startProcess() {
	Menu menu;
	Node* currentMenu;
	Node* currentSubMenu;
	int input = '\0';
	int inputKey;
	int subMenuWasOpen = 0;
	generateMenu(&menu);
	currentMenu = menu.head;
	currentSubMenu = NULL;
	do {
		//clearing screen
		#ifdef _WIN32
			system("cls");
		#elif __linux__
			system("clear");
		#endif
		
		printf("Use arrow keys to navigate the menu (X to exit)\n"
			"You can also use the first letter of each menu to jump to it\n\n");
		displayMenu(&menu, currentMenu, currentSubMenu);
		//showing previous action
		if (input == 'f' || input == 'F') {
			printf("\n\nYou jumped to the File sub-menu using the 'F' hotkey.");
		}
		else if (input == 'e' || input == 'E') {
			printf("\n\nYou jumped to the Edit sub-menu using the 'E' hotkey.");
		}
		else if (input == 's' || input == 'S') {
			printf("\n\nYou jumped to the Search sub-menu using the 'S' hotkey.");
		}
		else if (input == 'h' || input == 'H') {
			printf("\n\nYou jumped to the Help sub-menu using the 'H' hotkey.");
		}
		else if (input == CTRLKEY && inputKey == UP) {
			if  (currentSubMenu != NULL) printf("\n\nYou pressed the UP arrow key moving from %s to %s.", currentSubMenu->next->data, currentSubMenu->data);
			else printf("\n\nYou pressed the UP arrow key, but nothing happened as no sub-menu was open.");
		}
		else if (input == CTRLKEY && inputKey == DOWN) {
			if (subMenuWasOpen) printf("\n\nYou pressed the DOWN arrow key moving from %s to %s.", currentSubMenu->prev->data, currentSubMenu->data);
			else {
				subMenuWasOpen = 1;
				printf("\n\nYou pressed the DOWN arrow key opening the %s sub-menu.", currentMenu->data);
			}
		}
		else if (input == CTRLKEY && inputKey == LEFT) {
			printf("\n\nYou pressed the LEFT arrow key moving from %s to %s.", currentMenu->next->data, currentMenu->data);
		}
		else if (input == CTRLKEY && inputKey == RIGHT) {
			printf("\n\nYou pressed the RIGHT arrow key moving from %s to %s.", currentMenu->prev->data, currentMenu->data);
		}
		else if (input != '\0') { //first loop run
			printf("\n\nSorry, you seemed to have typed an invalid character (Entered: %c).", input);
		}
		//process new input
		if ((input = getch()) == 'X' || input == 'x') {
			printf("\n\nExiting...\n");
		}
		else if (input == 'f' || input == 'F') {
			currentMenu = menu.head;
			currentSubMenu = *(menu.subMenuHeads + currentMenu->id);
		}
		else if (input == 'e' || input == 'E') {
			currentMenu = menu.head->next;
			currentSubMenu = *(menu.subMenuHeads + currentMenu->id);
		}
		else if (input == 's' || input == 'S') {
			currentMenu = menu.head->next->next;
			currentSubMenu = *(menu.subMenuHeads + currentMenu->id);
		}
		else if (input == 'h' || input == 'H') {
			currentMenu = menu.head->next->next->next;
			currentSubMenu = *(menu.subMenuHeads + currentMenu->id);
		}
		else if (input == CTRLKEY){
			#ifdef __linux__
				/*
				 * Linux assigns 3 characters for each arrow key.
				 * The first one is the control key. Second is '['. 
				 * The third character (A/B/C/D) differentiates the arrow keys.
				 * */
				getch(); //skip '['
			#endif
			switch(inputKey = getch()) { //get arrow key
				case UP:
					if (currentSubMenu != NULL) currentSubMenu = currentSubMenu->prev;
					break;
				case DOWN:
					currentSubMenu = currentSubMenu == NULL ? *(menu.subMenuHeads + currentMenu->id) : currentSubMenu->next;
					break;
				case RIGHT:
					currentMenu = currentMenu->next;
					currentSubMenu = NULL;
					subMenuWasOpen = 0;
					break;
				case LEFT:
					currentMenu = currentMenu->prev;
					currentSubMenu = NULL;
					subMenuWasOpen = 0;
					break;
			}
		}
	} while (input != 'X' && input != 'x');
}

int main() {
	startProcess();
	return 0;
}
