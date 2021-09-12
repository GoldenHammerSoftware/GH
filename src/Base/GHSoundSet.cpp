// Copyright Golden Hammer Software
#include "GHSoundSet.h"
#include "GHSoundEmitter.h"
#include "GHMath/GHRandom.h"
#include <math.h>

GHSoundSet::GHSoundSet(const GHIdentifier& category, GHSoundVolumeMgr& volumeMgr)
: GHSoundEmitter(category, volumeMgr)
{
}

GHSoundSet::~GHSoundSet(void)
{
	std::vector<GHSoundEmitter*>::iterator iter;
	for (iter = mEmitters.begin(); iter != mEmitters.end(); ++iter) {
		delete *iter;
	}
}

void GHSoundSet::play(void)
{
	if (!mEmitters.size()) return; 
    unsigned int randIndex = GHRandom::getRandInt((int)mEmitters.size());
	mEmitters[randIndex]->play();
}

void GHSoundSet::stop(void)
{
	std::vector<GHSoundEmitter*>::iterator iter;
	for (iter = mEmitters.begin(); iter != mEmitters.end(); ++iter) {
		(*iter)->stop();
	}
}

void GHSoundSet::pause(void)
{
	std::vector<GHSoundEmitter*>::iterator iter;
	for (iter = mEmitters.begin(); iter != mEmitters.end(); ++iter) {
		GHSoundEmitter* emitter = *iter;
		emitter->pause();
	}
}

void GHSoundSet::applyGain(float gain)
{
	std::vector<GHSoundEmitter*>::iterator iter;
	for (iter = mEmitters.begin(); iter != mEmitters.end(); ++iter) {
		(*iter)->setGain(gain);
	}
}

void GHSoundSet::addSoundEmitter(GHSoundEmitter* emitter) 
{ 
	if (!emitter) return;
	mEmitters.push_back(emitter);
}

void GHSoundSet::setPosition(const GHPoint3& pos)
{
	std::vector<GHSoundEmitter*>::iterator iter;
	for (iter = mEmitters.begin(); iter != mEmitters.end(); ++iter) {
		(*iter)->setPosition(pos);
	}
}

void GHSoundSet::setVelocity(const GHPoint3& pos)
{
	std::vector<GHSoundEmitter*>::iterator iter;
	for (iter = mEmitters.begin(); iter != mEmitters.end(); ++iter) {
		(*iter)->setVelocity(pos);
	}
}

void GHSoundSet::setLooping(bool looping)
{
	std::vector<GHSoundEmitter*>::iterator iter;
	for (iter = mEmitters.begin(); iter != mEmitters.end(); ++iter) {
		(*iter)->setLooping(looping);
	}
}

void GHSoundSet::setPitch(float pitch)
{
	std::vector<GHSoundEmitter*>::iterator iter;
	for (iter = mEmitters.begin(); iter != mEmitters.end(); ++iter) {
		(*iter)->setPitch(pitch);
	}
}

bool GHSoundSet::isPlaying(void) const
{
	// return true if any are playing
	std::vector<GHSoundEmitter*>::const_iterator iter;
	for (iter = mEmitters.begin(); iter != mEmitters.end(); ++iter) {
		if ((*iter)->isPlaying()) {
			return true;
		}
	}
	return false;
}

bool GHSoundSet::isLooping(void) const
{
	// assume all our sounds are either looping or not looping,
	// so look at only the first one.
	if (!mEmitters.size()) return false;
	return mEmitters[0]->isLooping();
}

void GHSoundSet::seek(float t)
{
	std::vector<GHSoundEmitter*>::iterator iter;
	for (iter = mEmitters.begin(); iter != mEmitters.end(); ++iter) {
		(*iter)->seek(t);
	}
}

void GHSoundSet::setIsPositional(bool val)
{
	std::vector<GHSoundEmitter*>::iterator iter;
	for (iter = mEmitters.begin(); iter != mEmitters.end(); ++iter) {
		(*iter)->setIsPositional(val);
	}
}

bool GHSoundSet::isPositional(void) const
{
	// assume all emitters are the same.
	std::vector<GHSoundEmitter*>::const_iterator iter;
	for (iter = mEmitters.begin(); iter != mEmitters.end(); ++iter) {
		return (*iter)->isPositional();
	}
	return false;
}

