#include "action.h"
#include "creature.h"
#include "game.h"
#include "quest.h"

const char* message_names[LastMessage + 1] = {
	"Cancel", "Continue", "Title", "Yes", "No", "OK",
	"Start a new game", "Load existing game", "Exit game",
	"Information", "Skills", "You have %Gold gold.",
	"Select Race", "Select Gender", "Select Class", "Select Alignment",
	"Select the box of the character you wish to create or view.",
	"Press Enter to play game.",
	"Do you really want to delete this character?",
	"What %1 do?", "Which way to go?",
	"%1 is disabled",
	"City Iriaebor", "Current goals",
	"Rest Party", "Scrible scrolls",
	"Class", "Race"
};
extern const char* action_names[LastAction + 1] = {
	"Tavern", "Blacksmith", "Temple", "Inn", "Wizard Tower", "Go Adventure",
	"Pick pockets", "Eat and Drink", "Gambling", "Carousing",
};
const char* direction_names[Down + 1] = {
	"Center", "Left", "Up", "Right", "Down"
};
const char* ability_names[Experience + 1] = {
	"Strenght", "Dexterity", "Constitution", "Intellegence", "Wisdow", "Charisma",
	"Save vs Paralization", "Save vs Poison", "Save vs Traps", "Save vs Magic",
	"Climb Walls", "Hear Noise", "Move Silently", "Open Locks", "Pick Pockets", "Remove Traps", "Read Languages",
	"Learn Spell",
	"Resist Magic",
	"Critical Deflect", "Detect Secrets",
	"AC", "Melee", "Range", "Damage", "Damage",
	"Speed", "Turn Undead", "Backstab", "Attacks",
	"Spell1", "Spell2", "Spell3", "Spell4", "Spell5", "Spell6", "Spell7", "Spell8", "Spell9", "Spells",
	"Bonus Experience", "Reaction Bonus",
	"Exeptional Strenght",
	"AcidD1", "AcidD2", "Poison", "Disease", "Clones",
	"DrainedStrenght", "DrainedConstitution", "DrainedLevels",
	"Hits",
	"Level", "Experience"
};
const char* ability_short[Experience + 1] = {
	"Str", "Dex", "Con", "Int", "Wis", "Cha",
	"SvPr", "SvPo", "SvT", "SvM",
	"CW", "HN", "MS", "OL", "PP", "RT", "RL",
	"LS",
	"MR",
	"CD", "DS",
	"AC", "Att", "Rng", "Dam", "Dam",
	"Speed", "Turn Undead", "Backstab", "Attacks",
	"S1", "S2", "S3", "S4", "S5", "S6", "S7", "S8", "S9", "Spells",
	"BE", "RA",
	"ExS",
	"AcidD1", "AcidD2", "Poi", "Dis", "Clones",
	"DStr", "DCon", "DrLev",
	"HP",
	"Lev", "Exp"
};
extern const char* variable_names[LastVariable + 1] = {
	"Reputation", "Gold", "Blessing"
};
const char* alignment_names[ChaoticEvil + 1] = {
	"True Neutral",
	"Lawful Good", "Neutral Good", "Chaotic Good",
	"Lawful Neutral", "Chaotic Neutral",
	"Lawful Evil", "Neutral Evil", "Chaotic Evil"
};
const char* class_names[FighterMageTheif + 1] = {
	"Monster",
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
const char* item_names[LastItem + 1] = {
	"NoItem",
	"Axe", "Axe", "Club", "Dagger", "Flail", "Halberd", "Warhammer", "Mace", "Spear", "Staff",
	"Longsword", "Shortsword", "Big Sword",
	"Bow", "Sling",
	"Robe", "Cloack", "Cloack",
	"Leather Armor", "Scale Mail", "Chain Mail", "Banded Mail", "Plate Mail",
	"Helm", "Helm",
	"Shield", "Shield", "Boots", "Bracers",
	"Ring", "Ring", "Ring", "Amulet", "Medalion",
	"Potion", "Potion", "Potion",
	"Iron Ration", "Ration",
	"Scroll", "Scroll", "Map", "Wand",
	"Theif Tools", "Grappling Hook", "Holy Symbol", "Holy Symbol", "Tome", "Horn",
	"Bones", "Mantist Head", "Monster Teeth", "Skull", "Bone",
	"Sphere", "Sphere",
	"Gem", "Gem", "Gem", "Gem",
	"Iron Key", "Bronze Key", "Cooper Key", "Bone Key", "Manist Key", "Steel Key", "Skull Key", "Moon Key", "Jewel Key",
	"Stone Dagger", "Stone Gem", "Stone Amulet", "Stone Sphere", "Stone Holy Symbol", "Stone Crest",
	"Circle",
	"Chill Touch", "Flame Blade",
	"Bite", "Claws", "Claws", "Hag", "Mandibules", "Slam", "Slam", "Sting",
	"Arrow", "Stone", "Dart"
};
const char* monster_names[LastMonster + 1] = {
	"Nobody",
	"Kobold", "Leech", "Dwarf", "Spider",
};
const char* speech_names1[LastSpeech + 1] = {
	"How can I use %1?",
	"This is a %1",
	"This is %1",
	"I don't see any item here",
	"Can't put %1 here",
	"Not in the city!",
};
const char* speech_names2[LastSpeech + 1] = {
	"I don't use %1.",
	0,
	0,
	"And where is item?",
	"Can't wearn %1 that way",
	"This one usable in dungeon",
};
const char* speech_names3[LastSpeech + 1] = {
	"I don't need %1.",
	0,
	0,
	"Nothing examine",
	"Wrong item place",
	"Wrong place to use",
};
const char* name_names[50 * 4] = {
	"Aldren", "Elira", "Garrick", "Mirena", "Taren", // Human names
	"Lianna", "Corwin", "Selena", "Branor", "Alicia",
	"Edric", "Rowena", "Darren", "Talia", "Roderick",
	"Iliana", "Cedric", "Marissa", "Reynar", "Velena",
	"Torvin", "Cassandra", "Faren", "Evelina", "Gavin",
	"Nerissa", "Varrick", "Melissa", "Oldric", "Fiona",
	"Devran", "Alessia", "Tristan", "Mirelle", "Baelor",
	"Shayla", "Corren", "Isolde", "Damian", "Calista",
	"Rendal", "Sylvia", "Eldren", "Ariana", "Talren",
	"Brianna", "Kylen", "Rosalyn", "Beric", "Lavinia",
	"Caelith", "Aelwen", "Thaelar", "Sylira", "Elarion", // Elvish names 
	"Vaelissa", "Faenor", "Lethiel", "Aerandir", "Naerissa",
	"Caladrel", "Ilyrana", "Therion", "Saelith", "Vaeril",
	"Elanwe", "Lorandir", "Maelyra", "Aethrin", "Thalira",
	"Fenrion", "Caelynn", "Erevar", "Sylwen", "Laerith",
	"Vaelora", "Ithilwen", "Faelith", "Nimriel", "Aerendyl",
	"Selanna", "Thalion", "Lirael", "Corathir", "Aelara",
	"Maerion", "Velanna", "Ithron", "Saelira", "Elaris",
	"Naevys", "Galadren", "Ariella", "Vaelorin", "Myrielle",
	"Thaelis", "Elowyn", "Raelith", "Letharia", "Arianel",
	"Thordek", "Brenna", "Darrak", "Vistra", "Baern", // Dwarf names
	"Helja", "Kildrak", "Gunnloda", "Rurik", "Kathra",
	"Barendd", "Diesa", "Torgar", "Eldeth", "Morgran",
	"Riswynn", "Harbek", "Gurdis", "Adrik", "Sannl",
	"Brottor", "Finellen", "Eberk", "Ilde", "Veit",
	"Nyx", "Alston", "Bimpnottin", "Wrenn", "Caramip",
	"Zook", "Ellywick", "Dimble", "Lilli", "Fonkin",
	"Orla", "Gerbo", "Tana", "Namfoodle", "Breena",
	"Jebeddo", "Nissa", "Seebo", "Mardnab", "Boddynock",
	"Shamil", "Warryn", "Tervaround", "Kellen", "Duvamil",
	"Alton", "Andry", "Beau", "Bree", "Cade", // Halfling names
	"Callie", "Corrin", "Cora", "Eldon", "Euphemia",
	"Finnan", "Jillian", "Garret", "Kithri", "Lyle",
	"Lavinia", "Merric", "Lidda", "Milo", "Merla",
	"Osborn", "Nedda", "Perrin", "Paela", "Reed",
	"Portia", "Roscoe", "Seraphina", "Wellby", "Shaena",
	"Wilby", "Trym", "Bramble", "Vani", "Tobin",
	"Verna", "Fendrel", "Wella", "Corby", "Myria",
	"Jasper", "Pella", "Nobbin", "Rilla", "Bungo",
	"Tessa", "Hobson", "Daisy", "Merrin", "Posy"
};