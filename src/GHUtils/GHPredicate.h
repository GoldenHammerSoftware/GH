#pragma once

// interface for something that says yes or no.
class GHPredicate
{
public:
	virtual ~GHPredicate(void) {}
	virtual bool eval(void) const = 0;
};

