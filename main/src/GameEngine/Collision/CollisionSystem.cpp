#include "CollisionSystem.h"
#include <utility>
#include "RectCC.h"
#include "CircleCC.h"
#include "PolygonCC.h"
#include <map>
#include <glm.hpp>
#include <ext.hpp>
#include <gtx/matrix_transform_2d.hpp>
#include <gtx/vector_angle.hpp>

#undef abs

CollisionSystem::CollisionSystem(const Game* game) :
		GameSystem(game), Walls({
										.top =  { nullptr, std::make_unique<RectCC>(glm::vec2{ 128, 100 }) },
										.bot =  { nullptr, std::make_unique<RectCC>(glm::vec2{ 128, 100 }) },
										.left =  { nullptr, std::make_unique<RectCC>(glm::vec2{ 100, 128 }) },
										.right =  { nullptr, std::make_unique<RectCC>(glm::vec2{ 100, 128 }) }
								}){
	Walls.top.setPos({ 0, -100 });
	Walls.bot.setPos({ 0, 128 });
	Walls.left.setPos({ -100, 0 });
	Walls.right.setPos({ 128, 0 });
}

void CollisionSystem::update(uint32_t deltaMicros){
	for(auto& pair : pairs){

		if(std::find(removedPairs.begin(), removedPairs.end(), pair) != removedPairs.end()){
			continue;
		}

		auto type1 = pair.first->getCollisionComponent()->getType();
		auto type2 = pair.second->getCollisionComponent()->getType();

		std::pair<CollisionType, CollisionType> types = { type1, type2 };

		std::map<std::pair<CollisionType, CollisionType>, std::function<bool(const GameObject&, const GameObject&)>> map = {
				{ { CollisionType::Circle,  CollisionType::Circle },  &CollisionSystem::circleCircle },
				{ { CollisionType::Rect,    CollisionType::Rect },    &CollisionSystem::rectRect },
				{ { CollisionType::Rect,    CollisionType::Circle },  &CollisionSystem::rectCircle },
				{ { CollisionType::Circle,  CollisionType::Rect },    [](const GameObject& circle, const GameObject& rect){
					return rectCircle(rect, circle);
				} },
				{ { CollisionType::Polygon, CollisionType::Polygon }, &CollisionSystem::polyPoly },
				{ { CollisionType::Polygon, CollisionType::Rect },    &CollisionSystem::polyRect },
				{ { CollisionType::Rect,    CollisionType::Polygon }, [](const GameObject& rect, const GameObject& poly){ return polyRect(poly, rect); } },
				{ { CollisionType::Polygon, CollisionType::Circle },  &CollisionSystem::polyCircle },
				{ { CollisionType::Circle,  CollisionType::Polygon }, [](const GameObject& circle, const GameObject& poly){ return polyCircle(poly, circle); } }
		};

		bool overlap = map[types](*pair.first, *pair.second);

		if(overlap && !pair.colliding){
			pair.handler();
		}

		pair.colliding = overlap;
	}

	for(auto& pair : removedPairs){
		if(pairs.empty()) break;
		pairs.erase(std::remove(pairs.begin(), pairs.end(), pair), pairs.end());
	}

	pairs.insert(pairs.end(), addedPairs.begin(), addedPairs.end());

	addedPairs.clear();
	removedPairs.clear();
}

CollisionSystem::Polygon CollisionSystem::getRotatedTranslatedPoly(const GameObject& poly){
	float rot = poly.getRot();
	auto polyPoints = poly.getCollisionComponent()->getPolygon()->getPoints();
	auto pos = poly.getPos();

	if(rot == 0){
		std::transform(polyPoints.begin(), polyPoints.end(), polyPoints.begin(), [&pos](glm::vec2& point){
			return point + pos;
		});
		return polyPoints;
	}

	auto center = poly.getPos() + poly.getCollisionComponent()->getPolygon()->getCenter();

	auto rotMat = glm::identity<glm::mat3>();
	rotMat = glm::translate(rotMat, center);
	rotMat = glm::rotate(rotMat, glm::radians(rot));
	rotMat = glm::translate(rotMat, -center);

	auto polyCopy = polyPoints;

	std::transform(polyCopy.begin(), polyCopy.end(), polyCopy.begin(), [&rotMat, &pos](glm::vec2& point){
		return rotMat * glm::vec3(point + pos, 1.0f);
	});

	return polyCopy;
}

CollisionSystem::Polygon CollisionSystem::getRotatedTranslatedRect(const GameObject& rect){
	float rot = rect.getRot();
	auto dim = rect.getCollisionComponent()->getRect()->getDim();
	auto offset = rect.getCollisionComponent()->getRect()->getOffset();
	auto pos = rect.getPos() + offset;
	glm::vec2 point1 = pos;
	glm::vec2 point2 = { pos.x + dim.x, pos.y };
	glm::vec2 point3 = pos + dim;
	glm::vec2 point4 = { pos.x, pos.y + dim.y };
	std::vector<glm::vec2> rectPoints = { point1, point2, point3, point4 };

	if(rot == 0){
		std::transform(rectPoints.begin(), rectPoints.end(), rectPoints.begin(), [](glm::vec2& point){
			return point;
		});
		return rectPoints;
	}

	auto center = rect.getPos() + offset + (dim / 2.0f);

	auto rotMat = glm::identity<glm::mat3>();
	rotMat = glm::translate(rotMat, center);
	rotMat = glm::rotate(rotMat, glm::radians(rot));
	rotMat = glm::translate(rotMat, -center);

	auto polyCopy = rectPoints;

	std::transform(polyCopy.begin(), polyCopy.end(), polyCopy.begin(), [&rotMat](glm::vec2& point){
		return rotMat * glm::vec3(point, 1.0f);
	});

	return polyCopy;
}

void CollisionSystem::addPair(const GameObject& first, const GameObject& second, std::function<void()> handler){
	if(&first == &second) return;
	if(!first.getCollisionComponent() || !second.getCollisionComponent()) return;

	addedPairs.push_back({ &first, &second, std::move(handler) });

	if(removedPairs.empty()) return;

	auto it = remove_if(removedPairs.begin(), removedPairs.end(), [&first, &second](const Pair& pair) -> bool{
		return (pair.first == &first && pair.second == &second) || (pair.first == &second && pair.second == &first);
	});
	removedPairs.erase(it, removedPairs.end());
}

void CollisionSystem::removePair(const GameObject& first, const GameObject& second){
	std::for_each(pairs.begin(), pairs.end(), [&first, &second, this](const Pair& pair){
		if((pair.first == &first && pair.second == &second) || (pair.first == &second && pair.second == &first)){
			removedPairs.push_back(pair);
		}
	});

	if(addedPairs.empty()) return;

	auto it = remove_if(addedPairs.begin(), addedPairs.end(), [&first, &second](const Pair& pair) -> bool{
		return (pair.first == &first && pair.second == &second) || (pair.first == &second && pair.second == &first);
	});
	addedPairs.erase(it, addedPairs.end());
}

void CollisionSystem::removeObject(const GameObject& GO){
	std::for_each(pairs.begin(), pairs.end(), [&GO, this](const Pair& pair){
		if(pair.first == &GO || pair.second == &GO){
			removedPairs.push_back(pair);
		}
	});

	if(addedPairs.empty()) return;

	auto it = remove_if(addedPairs.begin(), addedPairs.end(), [&GO](const Pair& pair) -> bool{
		return pair.first == &GO || pair.second == &GO;
	});
	addedPairs.erase(it, addedPairs.end());
}

void CollisionSystem::wallTop(const GameObject& obj, std::function<void()> handler){
	if(handler){
		addPair(obj, Walls.top, std::move(handler));
	}else{
		removePair(obj, Walls.top);
	}
}

void CollisionSystem::wallBot(const GameObject& obj, std::function<void()> handler){
	if(handler){
		addPair(obj, Walls.bot, std::move(handler));
	}else{
		removePair(obj, Walls.bot);
	}
}

void CollisionSystem::wallLeft(const GameObject& obj, std::function<void()> handler){
	if(handler){
		addPair(obj, Walls.left, std::move(handler));
	}else{
		removePair(obj, Walls.left);
	}
}

void CollisionSystem::wallRight(const GameObject& obj, std::function<void()> handler){
	if(handler){
		addPair(obj, Walls.right, std::move(handler));
	}else{
		removePair(obj, Walls.right);
	}
}

void CollisionSystem::wallsVertical(const GameObject& obj, std::function<void()> handler){
	wallLeft(obj, handler);
	wallRight(obj, handler);
}

void CollisionSystem::wallsHorizontal(const GameObject& obj, std::function<void()> handler){
	wallTop(obj, handler);
	wallBot(obj, handler);
}

void CollisionSystem::wallsAll(const GameObject& obj, std::function<void()> handler){
	wallLeft(obj, handler);
	wallRight(obj, handler);
	wallTop(obj, handler);
	wallBot(obj, handler);
}

bool CollisionSystem::rectRect(const GameObject& square1, const GameObject& square2){
	if(square1.getRot() != 0.0f || square2.getRot() != 0.0f){
		auto points1 = getRotatedTranslatedRect(square1);
		auto points2 = getRotatedTranslatedRect(square2);

		for(auto i : points2){
			if(polyContainsPoint(points1, i)){
				return true;
			}
		}

		for(auto i : points1){
			if(polyContainsPoint(points2, i)){
				return true;
			}
		}

		return false;
	}


	auto offset1 = square1.getCollisionComponent()->getRect()->getOffset();
	auto offset2 = square2.getCollisionComponent()->getRect()->getOffset();
	auto pos1 = square1.getPos() + offset1 - 0.5f;
	auto pos2 = square2.getPos() + offset2 - 0.5f;
	auto dim1 = square1.getCollisionComponent()->getRect()->getDim() + 1.0f;
	auto dim2 = square2.getCollisionComponent()->getRect()->getDim() + 1.0f;

	return (pos1.x <= pos2.x + dim2.x && pos1.x + dim1.x >= pos2.x) &&
		   (pos1.y <= pos2.y + dim2.y && pos1.y + dim1.y >= pos2.y);
}

bool CollisionSystem::circleCircle(const GameObject& circle1, const GameObject& circle2){
	auto r1 = circle1.getCollisionComponent()->getCircle()->getRadius();
	auto r2 = circle2.getCollisionComponent()->getCircle()->getRadius();

	auto pos1 = circle1.getPos() + circle1.getCollisionComponent()->getCircle()->getOffset();
	auto pos2 = circle2.getPos() + circle2.getCollisionComponent()->getCircle()->getOffset();

	return glm::distance(pos1, pos2) <= r1 + r2;
}

bool CollisionSystem::rectCircle(const GameObject& rect, const GameObject& circle){
	if(rect.getRot() != 0){
		auto points = getRotatedTranslatedRect(rect);

		glm::vec2 center = circle.getPos() + circle.getCollisionComponent()->getCircle()->getOffset();

		for(int i = 0; i < points.size(); i++){
			glm::vec2 start = points[i];
			glm::vec2 end = points[(i + 1) % points.size()];
			if(intersectSegmentCircle(start, end, center, circle.getCollisionComponent()->getCircle()->getRadius())) return true;
		}

		if(polyContainsPoint(points, center)) return true;

		return false;
	}


	auto cPos = circle.getPos() + circle.getCollisionComponent()->getCircle()->getOffset();
	auto r = circle.getCollisionComponent()->getCircle()->getRadius();
	auto rDim = rect.getCollisionComponent()->getRect()->getDim();
	auto rOffset = rect.getCollisionComponent()->getRect()->getOffset();
	auto rPos = (rect.getPos() + rOffset) + 0.5f * rDim;

	// Thank you https://www.gamedevelopment.blog/collision-detection-circles-rectangles-and-polygons/
	auto distance = glm::abs(cPos - rPos);
	if(distance.x > (rDim.x / 2 + r)){ return false; }
	if(distance.y > (rDim.y / 2 + r)){ return false; }
	if(distance.x <= (rDim.x / 2)){ return true; }
	if(distance.y <= (rDim.y / 2)){ return true; }

	return glm::distance(distance, rDim * 0.5f) <= r;
}

bool CollisionSystem::polyPoly(const GameObject& obj1, const GameObject& obj2){
	auto poly1 = obj1.getCollisionComponent()->getPolygon();
	auto poly2 = obj2.getCollisionComponent()->getPolygon();

	if(!poly1->isConvex() || !poly2->isConvex()) return false;

	auto points1 = getRotatedTranslatedPoly(obj1);
	auto points2 = getRotatedTranslatedPoly(obj2);

	for(const auto& i : points2){
		if(polyContainsPoint(points1, i)){
			return true;
		}
	}

	for(const auto& i : points1){
		if(polyContainsPoint(points2, i)){
			return true;
		}
	}

	return false;
}

bool CollisionSystem::polyRect(const GameObject& poly, const GameObject& rect){
	if(!poly.getCollisionComponent()->getPolygon()->isConvex()) return false;

	auto rectPoints = getRotatedTranslatedRect(rect);

	auto polyPoints = getRotatedTranslatedPoly(poly);

	for(auto i : polyPoints){
		if(polyContainsPoint(rectPoints, i)){
			return true;
		}
	}

	for(auto i : rectPoints){
		if(polyContainsPoint(polyPoints, i)){
			return true;
		}
	}

	return false;
}

bool CollisionSystem::polyCircle(const GameObject& poly, const GameObject& circle){
	if(!poly.getCollisionComponent()->getPolygon()->isConvex()) return false;

	auto points = getRotatedTranslatedPoly(poly);
	glm::vec2 center = circle.getPos() + circle.getCollisionComponent()->getCircle()->getOffset();

	for(int i = 0; i < points.size(); i++){
		glm::vec2 start = points[i];
		glm::vec2 end = points[(i + 1) % points.size()];
		if(intersectSegmentCircle(start, end, center, circle.getCollisionComponent()->getCircle()->getRadius())) return true;
	}

	if(polyContainsPoint(points, center)) return true;

	return false;
}

bool CollisionSystem::polyContainsPoint(const CollisionSystem::Polygon& polygon, glm::vec2 point){
	int intersects = 0;                                 // counter for times line crosses

	for(size_t i = 0; i < polygon.size(); i++){
		glm::vec2 point1 = polygon[i];
		glm::vec2 point2 = polygon[(i + 1) % polygon.size()];

		//  (point is between y1 and y2)                 && (point) < (( gradient) * (distance) + vertex.x)
		if(((point.y <= point1.y && point.y > point2.y) || (point.y <= point2.y && point.y > point1.y))
		   && point.x < ((point2.x - point1.x) / (point2.y - point1.y) * (point.y - point1.y) + point1.x)){
			intersects++;
		}
	}
	return (intersects & 1) == 1;
}

void CollisionSystem::drawDebug(Sprite& canvas){
	std::set<const GameObject*> drawn;

	for(const auto& pair : pairs){
		Color c = pair.colliding ? TFT_GREEN : TFT_RED;

		const auto draw = [&canvas, &drawn](Color c, const GameObject& obj){
			if(drawn.count(&obj) && c != TFT_GREEN) return;
			drawn.insert(&obj);

			auto col = obj.getCollisionComponent();

			if(!col) return;

			if(col->getType() == CollisionType::Rect){
				drawRect(obj, canvas, c);
			}else if(col->getType() == CollisionType::Circle){
				canvas.drawCircle(obj.getPos().x + obj.getCollisionComponent()->getCircle()->getOffset().x,
								  obj.getPos().y + obj.getCollisionComponent()->getCircle()->getOffset().y,
								  col->getCircle()->getRadius(), c);
			}else if(col->getType() == CollisionType::Polygon){
				CollisionSystem::drawPolygon(obj, canvas, c);
			}
		};

		draw(c, *pair.first);
		draw(c, *pair.second);
	}
}

void CollisionSystem::drawPolygon(const GameObject& poly, Sprite& canvas, Color color){
	auto points = poly.getCollisionComponent()->getPolygon()->getPoints();

	if(points.empty()) return;
	if(points.size() == 1){
		canvas.drawPixel(points.front().x, points.front().y);
		return;
	}

	points = getRotatedTranslatedPoly(poly);

	for(size_t i = 0; i < points.size(); i++){
		glm::vec2 p1 = points[i];
		glm::vec2 p2 = points[(i + 1) % points.size()];

		canvas.drawLine(p1.x, p1.y, p2.x, p2.y, color);
	}
}

bool CollisionSystem::intersectSegmentCircle(glm::vec2 start, glm::vec2 end, glm::vec2 center, float radius){
	glm::vec2 tmp = { end.x - start.x, end.y - start.y };        // holds polygon line vector
	glm::vec2 tmp1 = { center.x - start.x, center.y - start.y };    // holds start to circle center vector
	float l = glm::length(tmp);                    // the Euclidean length of line :(w * w + h * h) squared
	float u = glm::dot(tmp1, tmp);                // the dot product for these 2 vectors

	glm::vec2 tmp2;

	if(u <= 0){                                           // circle center is closest to start
		tmp2 = { start.x, start.y };                  // set point to start point
	}else if(u >= l){                                    // circle is closest to end
		tmp2 = { end.x, end.y };                      // set point to end point
	}else{                                                // circle is in between
		glm::vec2 tmp3 = tmp * u;                           // scale the normal by the dot product to get position on border
		tmp2 = tmp3 + start;                               // set point to position on line closest to circle center
	}
	// check if circle radius is longer than the line from our position on line to circle center (true for collisions)

	glm::vec2 line = center - tmp2;
	return glm::length(line) <= radius;
}

void CollisionSystem::drawRect(const GameObject& rect, Sprite& canvas, Color color){
	auto col = rect.getCollisionComponent();
	if(rect.getRot() == 0){
		auto rOffset = col->getRect()->getOffset();
		auto pos = rect.getPos() + rOffset;
		canvas.drawRect(pos.x, pos.y, col->getRect()->getDim().x, col->getRect()->getDim().y, color);
	}else{
		auto points = getRotatedTranslatedRect(rect);

		for(size_t i = 0; i < points.size(); i++){
			glm::vec2 p1 = points[i];
			glm::vec2 p2 = points[(i + 1) % points.size()];

			canvas.drawLine(p1.x, p1.y, p2.x, p2.y, color);
		}
	}
}
