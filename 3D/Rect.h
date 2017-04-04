#include "Include.h"

template<class T>
class rect
{
public:
	rect() {}
	rect(T pLeft, T pBot, T pWidth, T pHeight) : left(pLeft), bot(pBot), width(pWidth), height(pHeight) {}

	union
	{
		struct { T left, bot, width, height; };
		//struct { glm::vec2<T> botLeft, size; };
		//glm::vec2<T> asd;
	};

	T top() { return bot + height; }
	T right() { return left + width; }
	//glm::vec2<T> mid() { return glm::vec2<T>(left + width*0.5, bot + height*0.5); }

	bool operator==(const rect<T>& rhs)
	{
		return ((bot == rhs.bot) && (left == rhs.left) && (width == rhs.width) && (height == rhs.height));
	}

	bool operator!=(const rect<T>& rhs)
	{
		return !(this == rhs);
	}

	rect<T> operator+(const rect<T>& rhs)
	{
		T minX1 = min(left, left + width);
		T maxX1 = max(left, left + width);
		T minY1 = min(bot, bot + height);
		T maxY1 = max(bot, bot + height);

		T minX2 = min(rhs.left, rhs.left + rhs.width);
		T maxX2 = max(rhs.left, rhs.left + rhs.width);
		T minY2 = min(rhs.bot, rhs.bot + rhs.height);
		T maxY2 = max(rhs.bot, rhs.bot + rhs.height);

		T minBot = min(minY1, minY2);
		T minLeft = min(minX1, minX2);
		T maxRight = max(maxX1, maxX2);
		T maxTop = max(maxY1, maxY2);

		return frect(minLeft, minBot, maxRight - minLeft, maxTop - minBot);
	}

	void zero()
	{
		bot = 0; left = 0; width = 0; height = 0;
	}

	glm::fvec2 s;

	template<class R>
	//bool contains(const glm::vec2<R>& pPoint)
	//{
	//return pPoint.x > left && pPoint.x < left + width && pPoint.y > bot && pPoint.y < bot + height;
	//}

	bool intersects(const rect<T>& pTarget, rect<T>& pIntersection)
	{
		T minX1 = std::min(left, left + width);
		T maxX1 = std::max(left, left + width);
		T minY1 = std::min(bot, bot + height);
		T maxY1 = std::max(bot, bot + height);

		T minX2 = std::min(pTarget.left, pTarget.left + pTarget.width);
		T maxX2 = std::max(pTarget.left, pTarget.left + pTarget.width);
		T minY2 = std::min(pTarget.bot, pTarget.bot + pTarget.height);
		T maxY2 = std::max(pTarget.bot, pTarget.bot + pTarget.height);

		T botIntersect = std::max(minY1, minY2);
		T leftIntersect = std::max(minX1, minX2);
		T rightIntersect = std::min(maxX1, maxX2);
		T topIntersect = std::min(maxY1, maxY2);

		if ((leftIntersect < rightIntersect) && (botIntersect < topIntersect))
		{
			pIntersection = rect<T>(leftIntersect, botIntersect, rightIntersect - leftIntersect, botIntersect - botIntersect);
			return true;
		}
		else
		{
			pIntersection = rect<T>(0, 0, 0, 0);
			return false;
		}
	}

	bool intersects(const rect<T>& pTarget)
	{
		rect<T> intersection;
		return this->intersects(pTarget, intersection);
	}

};

typedef rect<float> frect;
typedef rect<double> drect;
typedef rect<int> irect;