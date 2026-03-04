#pragma once

#include "Actor/Actor.h"

using namespace Wanted;

class Player : public Actor
{
	RTTI_DECLARATIONS(Player, Actor)

public:
	Player();
	
	virtual void BeginPlay() override;

	virtual void Tick(float deltaTime) override;

private:

};

