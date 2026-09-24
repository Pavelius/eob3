#include "creature.h"
#include "game.h"

const char* message_names[LastMessage + 1] = {
	"Cancel", "Continue", "Title", "Yes", "No", "OK",
	"Start a new game", "Load existing game", "Exit game",
	"Information", "CharacterSkills", "PartyStatusFormat",
	"Select Race", "Select Gender", "Select Class", "Select Alignment",
	"Select the box of the character you wish to create or view.",
	"Class", "Race", "Level", "LevelShort", "Experience", "ExperienceShort"
};

const char* direction_names[Down + 1] = {
	"Center", "Left", "Up", "Right", "Down"
};

const char* ability_names[Hits + 1];

const char* ability_short[Hits + 1];

const char* alignment_names[ChaoticEvil + 1] = {
	"Lawful Good", "Neutral Good", "Chaotic Good",
	"Lawful Neutral", "True Neutral", "Chaotic Neutral",
	"Lawful Evil", "Neutral Evil", "Chaotic Evil"
};

const char* class_names[FighterMageTheif + 1] = {
	"Fighter", "Ranger", "Paladin", "Mage", "Cleric", "Theif",
	"Fighter/Cleric", "Fighter/Mage", "Fighter/Theif",
	"Mage/Theif",
	"Fighter/Mage/Theif",
};

const char* gender_names[Female + 1] = {
	"Male", "Female"
};

const char* race_names[Halfling + 1] = {
	"Human", "Dwarf", "Elf", "Half-Elf", "Halfling",
};

const char* name_names[20] = {
};