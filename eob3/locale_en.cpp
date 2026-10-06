#include "action.h"
#include "creature.h"
#include "dungeon.h"
#include "game.h"
#include "quest.h"

const char* message_names[LastMessage + 1] = {
	"Cancel", "Continue", "Title", "Yes", "No", "OK", "Agree", "Decline", "On", "Off",
	"Music", "Sound",
	"Information", "Skills", "You have %Gold coins.",
	"Select Race", "Select Gender", "Select Class", "Select Alignment",
	"Select the box of the character you wish to create or view.",
	"Press Enter to play game.",
	"Do you really want to delete this character?",
	"Healing is not for free. Priestess want some compensation for herbs and pouches and for wasted time. Do you agree to restore all %Name's hit points and get rid of poison and disease for %1i coins?",
	"This one will be cost %1i coins. Really want to buy it?",
	"Do you really want pay %1i coins for food and drink for all party?",
	"Rent room in this inn for all party will be cost %1i coins. And no fleas or rats. Do you accept this offer?",
	"Do you really want buy a drinks to everyone and go with all party to fully carousing? This will be cost for you totally %1i coins. Do you want this?",
	"Do you really want to rest party all night?",
	"Do you really want return to the city?",
	"Do you really want to make a camp and consume food?",
	"Do you really want to exit game and lost any unsaved progress?",
	"What %1 do?", "Which way to go?", "Which item you buy?", "Cast on who?",
	"No spells available", "Avaliable %1i of %2i spells",
	"Welcome, friends", "Who is you?", "How you get here?", "You are liers!",
	"I don't know waht is this",
	"Looks lika a old drainage gate",
	"Strange mechanic device",
	"This sign is mark something",
	"%Opponent stay still and carefully watching you.",
	"%1 is disabled", "%Player pick up %1",
	"Party going up ...", "Party going down ...", "Party fall into the pit.", "%1 became a %2i level %1",
	"This door is opened by key", "%Player sneack attack enemy", "%1 feel poison",
	"Magic device", "Portal", "Trap launcher",
	"Button", "Cellar",
	"Current goals", "Visit %1",
	"Class", "Race"
};
const char* action_names[LastAction + 1] = {
	"City Iriaebor",
	"Tavern", "Blacksmith", "Temple", "Inn", "Wizard Tower", "Palace", "Go Adventure",
	"Pick pockets",
	"Eat and Drink",
	"All party members eat tasted food and drink wine or beer. All of you is satisfied now and no more hunger or thirty.",
	"Gambling",
	"You sit to play a friendly card game with drunken tavern customers. Maybe today you will win. Who know? How much coins you bet?",
	"Today you are invisible luck and get this game. You gain %1i coins.",
	"Today you not your day. You are lose %1i coins.",
	"Carousing", "Donate",
	"Buy Weapons",
	"\"I'm all out of weapons. Adventurers bought up everything I had. Come back next week - I might have a fresh shipment by then.\"",
	"Repair Weapons",
	"Rest Party", "Scrible scrolls", "Memorize Spells", "Pray for Spells",
	"Game options", "Start a new game", "Load game", "Save game", "Exit game", "Perferences",
	"Ambush", "Attack", "CalmDown", "Hunt", "Talk", "Lie", "Bribe",
	"You are so tired to do this again. You must stop and rest. Do something else that doesn’t require as much effort.",
	"Leave outside",
};
const char* fatigue_status[4] = {
	"Fresh", "Fatigued", "Tired", "Exhausted",
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
const char* variable_names[LastVariable + 1] = {
	"Reputation", "Gold", "Blessing", "Time",
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
const char* language_names[LastRace + 1] = {
	"Common", "Dwarvish", "Elvish", "Elvish", "Halflings",
	"Goblin's", "Animal's"
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
	"I can't read %-1",
	"This is a %1",
	"This is %1",
	"I don't see any item here",
	"Can't put %1 here",
	"Some kind of a %1",
	"%1 don't fit in cellar",
	"Nothing to grab",
	"I must take it in hand",
	"I must wear this to use",
	"Drop this one to quiver",
	"Where is keyhole?",
	"This key doesn't match",
	"Aha! Doors to treasure!",
	"I am scarry!",
	"Not in the city!",
};
const char* speech_names2[LastSpeech + 1] = {
	"I don't use %1.",
	"I don't understand %-1 language",
	0,
	0,
	"And where is item?",
	"Can't wearn %1 that way",
	"It look like a %1",
	"Can't put item here",
	"There’s nothing there",
	"Only in hands work",
	"It usable when wear",
	"Ammunition usable in quiver",
	"I don't see any keyhole here",
	"The key don't fit in hole",
	"I know, it's a secret door",
	"Leave me alone!",
	"This one usable in dungeon",
};
const char* speech_names3[LastSpeech + 1] = {
	"I don't need %1.",
	"I can't read %-1 shit!",
	0,
	0,
	"Nothing examine",
	"Wrong item place",
	"It's probably a %1",
	"It does not fit",
	"It's empty",
	"Place in hand to use",
	"I need dress this",
	"Only in quiver can use it",
	"Key need put into keyhole.",
	"This is a wrong key",
	"Secret door, lead to treasure!",
	"Help me! Help!",
	"Wrong place to use",
};
const char* wallmsg_names[LastWellMessage + 1] = {
	"Find %1i magic weapons on this level",
	"Find %1i magic ring in this halls",
	"Find %1i hidden rooms in this place",
	"Beware %1i deadly traps",
	"Find %1i keys to open treasure doors",
	"Mighty artifact lie somewhere in this halls",
	"Beware cursed items",
	"Find %DungeonSpecial somewere in this place",
	"Deadly %DungeonBoss is hunt for you",
	"%Habbitant1 and %Habbitant2 dwelve this place",
	"You not find any magic weapons here",
	"There is no magic ring around",
	"This place is what it seems to be",
	"This level is safe",
	"This place is open for all visitors",
	"Magic power leave this place long ago",
	"There is a holy site",
	"There is no something special on this level",
	"Hey, lucky, no tought monsters live here",
};
extern const char* spell_names[LastSpell + 1] = {
	"Bless", "Cure Light Wound", "Detect Evil", "Detect Magic", "Protection From Evil", "Purify Food",
	"Armor", "Burning Hands", "Chill Touch", "Comprehend languages", "Friends", "Identify", "Magic Missile", "Mending", "Shield", "Shocking Grasp",
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