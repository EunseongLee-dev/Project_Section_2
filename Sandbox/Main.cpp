#include <iostream>
#include "Engine/Engine.h"
#include "Level/ArenaLevel.h"

using namespace Wanted;

int main()
{
	Engine engine;

	ArenaLevel* arena = new ArenaLevel();

	if (arena->LoadMap(L"../Assets/Map.txt"))
	{
		engine.SetNewLevel(arena);

		engine.Run();
	}

}