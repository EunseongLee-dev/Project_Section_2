#pragma once

#include "Util/Util.h"
#include <iostream>
#include <vector>

namespace Wanted
{
	// DLL 내부에서만 사용하도록.
	class Renderer
	{
	public:
		// 그리기 함수.
		static void Draw(const char image)
		{
			std::cout << image;
		}

		// 위치 설정 및 그리기 한번에 처리하는 함수.
		static void Draw(const Vector2& position, const char image)
		{
			Util::SetConsolePosition(position);
			Draw(image);
		}

		// 위치,색상 설정 및 그리기.
		static void Draw(
			const Vector2& position, 
			Color color,
			const char image)
		{
			// 커서 위치 설정.
			Util::SetConsolePosition(position);
			
			// 텍스트 색상 설정.
			Util::SetConsoleTextColor(color);
			
			// 글자 출력.
			Draw(image);

			// 원래 색상으로 다시 설정 (흰색으로).
			Util::SetConsoleTextColor(Color::White);
		}
	};
}