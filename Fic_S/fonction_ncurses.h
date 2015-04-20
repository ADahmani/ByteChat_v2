/**
*\file fonction_ncurses.h
*\brief Prototypes des fonction liées à ncurses
*
*Fichier de définition des prototypes des fonctions intervenant sur le fichier client et ncurses
*
*/

#ifndef FONCTIONS_NCURSES_H
#define FONCTION_NCURSES_H

#include <ncurses.h>


/**
*\fn void initfenetres();
*\brief initialisation des 3 fenetres ncurces
*/
void initfenetres();

/**
*\fn WINDOW *create_newwin(int height, int width, int starty, int startx);
*\brief initialisation des 3 fenetres ncurces
*\param height : hauteur , width : largeur, starty : position vertical , startx : position horizontal 
*\return un type WINDOW d'ncurses 
*/
WINDOW *create_newwin(int height, int width, int starty, int startx);

/**
*\fn void destroy_win(WINDOW *local_win);
*\brief suppresion d'une fenetre
*\param *local_win : une fenetre
*/
void destroy_win(WINDOW *local_win);


#endif
