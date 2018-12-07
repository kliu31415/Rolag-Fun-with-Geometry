#pragma once
#include <string>
#include <SDL2/SDL.h>
#include "fundamentals.h"
class Item
{
public:
    static constexpr int NUM_ITEMS = 2000;
    enum class Name{SpikedVest = 0, Bullseye = 1, DirtySerratedShirt = 2, BlueBoots = 3, RedBoots = 4,
    SadistsMachete = 5, GreenBoots = 6, AmuletOfAir = 7, DaltonsAtom = 8, Toothbrush = 9,
    RedGloves = 10, RedPillow = 11, MysteriousVial = 12, MysteriousPhial = 13, TheArtOfSadism = 14,
    PistolEnhancementI = 15,
    BlueTome = 400, PistolEnhancementII = 401, PhilosophersBook = 402, PurpleBoots = 403, TheSadistManifesto = 404,
    Projektyles = 405, IllegallyMadeFurCoat = 406, ProfessionalProtester = 407, WaifuBodyPillow = 408,
    AngryGamersManifesto = 600, PhysicsForBots = 601, FloppyDisk = 602, BookOfWords = 603, HeartPendant = 604,
    S4 = 605, SilverTongue = 606,
    ProfessionalRioter = 800, Attorney = 801, PistolEnhancementIII = 802, Barber = 803, NeighborsGardenHose = 804,
    MahouShoujoTshirt = 805, Aspirin = 806, YellowPill = 807, ShowerPass = 808, ArcheryLesson = 809,
    LaissezFaireManifesto = 810, Fentanyl = 811, IndecisivePersonsManifesto = 812,
    XrayGlasses = 1003, WizardHat = 1004,
    TheCommunistManifesto = 1005, AnIntroductionToRelativity = 1006, LaggyRouter = 1007, PhysicsMechanics = 1008, CSGOSteamCode = 1009,
    Barber_1 = NUM_ITEMS, Barber_2 = NUM_ITEMS + 1, Barber_3 = NUM_ITEMS + 2,
    ArcheryLesson_1 = NUM_ITEMS + 3, ArcheryLesson_2 = NUM_ITEMS + 4, ArcheryLesson_3 = NUM_ITEMS + 5, ArcheryLesson_4 = NUM_ITEMS + 6,
    AngryGamersManifesto_1 = NUM_ITEMS + 7, FloppyDisk_1 = NUM_ITEMS + 8, FloppyDisk_2 = NUM_ITEMS + 9, FloppyDisk_3 = NUM_ITEMS + 10,
    Kworker_Mod = 1700, None = NOT_SET};
    static bool IS_SHOWN[NUM_ITEMS];
    static SDL_Texture *sprites[NUM_ITEMS];
    static std::string BASE_NAME[NUM_ITEMS], BASE_DESC[NUM_ITEMS];
    static int BASE_COST[NUM_ITEMS];
    static void init();
    int type;
    Item() = default;
    Item(int type);
    Item(Name name);
};
