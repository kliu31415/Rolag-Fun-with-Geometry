#include "temporary_mod.h"
TemporaryMod::TemporaryMod(Name name, Mod mod, Type type, double duration, double var1)
{
    this->name = name;
    this->mod = mod;
    this->type = type;
    this->timeLeft = this->maxTimeLeft = duration;
    vars.push_back(var1);
}
TemporaryMod::TemporaryMod(Name name, Mod mod, Type type, double duration, double var1, double var2)
{
    this->name = name;
    this->mod = mod;
    this->type = type;
    this->timeLeft = this->maxTimeLeft = duration;
    vars.push_back(var1);
    vars.push_back(var2);
}
double TemporaryMod::getVarValue(const std::vector<TemporaryMod> &tempMods, Mod mod, double initV, double *V, double *XV)
{
    double xV = 1;
    for(auto &i: tempMods)
    {
        if(i.mod == mod)
        {
            switch(i.type)
            {
            case TemporaryMod::Type::Multiplicative:
                initV *= i.vars[0];
                break;
            case TemporaryMod::Type::MultiplicativeDecaying:
                initV *= (1 + (i.vars[0] - 1) * i.timeLeft / i.maxTimeLeft);
                break;
            case TemporaryMod::Type::AdditiveDecaying:
                xV += i.vars[0] * i.timeLeft / i.maxTimeLeft;
                break;
            case TemporaryMod::Type::Additive:
                xV += i.vars[0];
                break;
            default:
                break;
            }
        }
    }
    if(V != nullptr)
        *V = initV;
    if(XV != nullptr)
        *XV = xV;
    return initV * xV;
}
