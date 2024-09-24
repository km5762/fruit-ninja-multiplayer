//
// util.h
//

#ifndef UTIL_H
#define UTIL_H

// Function prototypes.
void loadResources(void);
void populateWorld(void);
void splash(void);
void create_trail(df::Vector p1, df::Vector p2, df::Color color = df::CYAN);
void play_sound(std::string sound);

#endif // UTIL_H
