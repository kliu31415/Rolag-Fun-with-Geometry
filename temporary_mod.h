#pragma once
#include <vector>
class TemporaryMod
{
public:
    enum class Name{DirtySerratedShirt, RedBoots, GreenBoots, WaterGunSpecial_1, WaterGunSpecial_2, WaterGunSpecial_3,
    BigStick_1, BigStick_2, BigStick_3, RedHeartUnit, PoofyPistolSpecial_1, PoofyPistolSpecial_2,
    CadmiumCrossbowSpecial_1, CadmiumCrossbowSpecial_2,
    HuntingRifle_1, HuntingRifle_2, HuntingRifle_3, HuntingRifle_4, HuntingRifle_5,
    UnbrandedSniperRifle_1, UnbrandedSniperRifle_2, UnbrandedSniperRifle_3, UnbrandedSniperRifle_4, UnbrandedSniperRifle_5,
    BiggerStick, Pi_1, Pi_2, Railgun_1, Railgun_2, Railgun_3, Railgun_4, SSS_1, SSS_2, SSS_3} name;
    //DamageDealt affects units (and all their weapons). ProjDamage affects one weapon only.
    enum class Mod{AlwaysFire, MovementSpeed, DamageDealt, FireRate, ProjLife, ProjSpeed, ProjSpread, ProjDamage, PierceCount,
    HurtTileDamage, ProjPerShot} mod;
    enum class Type{Additive, Multiplicative, MultiplicativeDecaying, AdditiveDecaying, None} type;
    std::vector<double> vars;
    double timeLeft, maxTimeLeft;
    TemporaryMod(Name name, Mod mod, Type type, double duration, double var1);
    TemporaryMod(Name name, Mod mod, Type type, double duration, double var1, double var2);
    static double getVarValue(const std::vector<TemporaryMod> &tempMods, Mod mod, double initV, double *V = nullptr, double *XV = nullptr);
};
