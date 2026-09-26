#include "creature.h"
#include "game.h"

const char* message_names[LastMessage + 1] = {
	"Cancel", "Continue", "Title", "Yes", "No", "OK",
	"Start a new game", "Load existing game", "Exit game",
	"Information", "CharacterSkills", "PartyStatusFormat",
	"Select Race", "Select Gender", "Select Class", "Select Alignment",
	"Select the box of the character you wish to create or view.",
	"Press Enter to play game.",
	"Do you really want to delete this character?",
	"Class", "Race", "Level", "LevelShort", "Experience", "ExperienceShort"
};
const char* direction_names[Down + 1] = {
	"Center", "Left", "Up", "Right", "Down"
};
const char* ability_names[Hits + 1] = {
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
	"Hits"
};
const char* ability_short[Hits + 1] = {
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
	"HP"
};
const char* alignment_names[ChaoticEvil + 1] = {
	"True Neutral",
	"Lawful Good", "Neutral Good", "Chaotic Good",
	"Lawful Neutral", "Chaotic Neutral",
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
const char* monster_names[LastMonster + 1] = {
	"Nobody",
	"Kobold", "Leech", "Dwarf", "Spider",
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