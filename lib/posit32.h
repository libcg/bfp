#ifndef __POSIT32_H
#define __POSIT32_H

#include "posit.h"

class Posit32 : public Posit
{
public:
    Posit32() :
        Posit()
    { }

    Posit32(float v) :
        Posit()
    {
        set(v);
    }

    Posit32(double v);
}

#endif
