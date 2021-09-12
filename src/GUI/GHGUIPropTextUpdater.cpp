// Copyright Golden Hammer Software
#include "GHGUIPropTextUpdater.h"
#include "GHGUIText.h"
#include "GHUtils/GHPropertyStringGenerator.h"
#include "GHUtils/GHProfiler.h"

GHGUIPropTextUpdater::GHGUIPropTextUpdater(GHGUIText& text, 
                                           GHPropertyStringGenerator* generator)
: mText(text)
, mGenerator(generator)
{
}

GHGUIPropTextUpdater::~GHGUIPropTextUpdater(void)
{
    delete mGenerator;
}

void GHGUIPropTextUpdater::update(void)
{
    mText.setText(mGenerator->generateString());
}
