#ifndef ESPOD_MENU_H
#define ESPOD_MENU_H

// #include <Arduino.h>
// #include <FS.h>
#include "piPod.h"

// #include "Menu.h"

void menu_init();
void menu_pushSong(AudioTrackData *song);
bool menu_hasSong(const char *filename);
void menu_makeMenu();
bool menu_createMusic(MenuList *list, MenuList *album, MenuList *ref);

void menu_utilDisplayList(MenuList *list);

int16_t menu_sortByName(MenuItem *a, MenuItem *b);
int16_t menu_sortByArtist(MenuItem *a, MenuItem *b);
int16_t menu_sortByAlbum(MenuItem *a, MenuItem *b);
int16_t menu_sortByYear(MenuItem *a, MenuItem *b);
int16_t menu_sortByTrack(MenuItem *a, MenuItem *b);
int16_t menu_sortBySet(MenuItem *a, MenuItem *b);
int16_t menu_sortRank(MenuItem *a, MenuItem *b);

void menu_write(MenuList *list);
void menu_update();
void menu_enter();
void menu_exit();
void menu_next();
void menu_prev();

void menu_test();
void menu_testExit();
void menu_testValuePlus();
void menu_testValueMinus();
void menu_testValueMiddle();
void menu_cbTest(void *empty);

void menu_cbList(void *list);
void menu_cbSong(void *data);
void menu_cbPlayer(void *data);

#endif