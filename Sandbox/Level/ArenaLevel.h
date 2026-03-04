#pragma once

#include "Level/Level.h"

#include <string>

using namespace Wanted;

class ArenaLevel: public Level
{
	RTTI_DECLARATIONS(ArenaLevel, Level)

public:
	ArenaLevel();
	virtual ~ArenaLevel();
	
	// 맵 로드 함수
	bool LoadMap(const wchar_t* filename);

	// Level-Draw override 
	virtual void Draw() override;

public:
	// 좌표가 이동 가능한 곳인지 확인하는 함수
	bool IsWalkable(int x, int y);


private:
	// map 자료용 변수
	// 맵 데이터만 저장(플레이어 및 몬스터에 정보는 별도관리)
	std::vector<std::wstring> mapData;

};

