#include "base/map.h"
#include "util/util.h"

using std::vector;
using std::string;
using std::list;

Map::Map(const SizeType rowCnt_, const SizeType colCnt_)
    : content(rowCnt_, vector<Point>(colCnt_)) {
    size = (rowCnt_ - 2) * (colCnt_ - 2);
    // Add boundary walls
    SizeType row = getRowCount(), col = getColCount();
    for (SizeType i = 0; i < row; ++i) {
        if (i == 0 || i == row - 1) {  // The first and last row
            for (SizeType j = 0; j < col; ++j) {
                content[i][j].setType(Point::Type::WALL);
            }
        } else {  // Rows in the middle
            content[i][0].setType(Point::Type::WALL);
            content[i][col - 1].setType(Point::Type::WALL);
        }
    }
}

Map::~Map() {}

Point& Map::getPoint(const Pos &p) {
    return content[p.getX()][p.getY()];
}

const Point& Map::getPoint(const Pos &p) const {
    return content[p.getX()][p.getY()];
}

Map::SizeType Map::getRowCount() const {
    return content.size();
}

Map::SizeType Map::getColCount() const {
    return content[0].size();
}

Map::SizeType Map::getSize() const {
    return size;
}

void Map::setTestEnabled(const bool e) {
    testEnabled = e;
}

bool Map::isTestEnabled() const {
    return testEnabled;
}

bool Map::isInside(const Pos &p) const {
    return p.getX() > 0 && p.getY() > 0
        && p.getX() < getRowCount() - 1
        && p.getY() < getColCount() - 1;
}

bool Map::isEmpty(const Pos &p) const {
    return isInside(p) && getPoint(p).getType() == Point::Type::EMPTY;
}

bool Map::isEmptyNotVisit(const Pos &p) const {
    return isEmpty(p) && !getPoint(p).isVisit();
}

bool Map::isSafe(const Pos &p) const {
    const Point &point = getPoint(p);
    return isInside(p) && (point.getType() == Point::Type::EMPTY 
                           || point.getType() == Point::Type::FOOD);
}

bool Map::isAllBody() const {
    SizeType row = getRowCount(), col = getColCount();
    for (SizeType i = 1; i < row - 1; ++i) {
        for (SizeType j = 1; j < col - 1; ++j) {
            Point::Type type = content[i][j].getType();
            if (!(type == Point::Type::SNAKE_HEAD
                || type == Point::Type::SNAKE_BODY
                || type == Point::Type::SNAKE_TAIL)) {
                return false;
            }
        }
    }
    return true;
}

void Map::createRandFood() {
    auto random = util::Random<>::getInstance();
    vector<Pos> emptyPoints = getEmptyPoints();
    if (!emptyPoints.empty()) {
        SizeType i = random->nextInt((SizeType)0, emptyPoints.size() - 1);
        createFood(emptyPoints[i]);
    }
}

void Map::createFood(const Pos &pos) {
    food = pos;
    content[food.getX()][food.getY()].setType(Point::Type::FOOD);
}

void Map::removeFood() {
    if (hasFood()) {
        content[food.getX()][food.getY()].setType(Point::Type::EMPTY);
    }
}

bool Map::hasFood() const {
    return isInside(food) && getPoint(food).getType() == Point::Type::FOOD;
}

const Pos& Map::getFood() const {
    return food;
}

Map::SizeType Map::distance(const Pos &from, const Pos &to) {
    SizeType fromX = from.getX(), toX = to.getX();
    SizeType fromY = from.getY(), toY = to.getY();
    SizeType dx = fromX > toX ? fromX - toX : toX - fromX;
    SizeType dy = fromY > toY ? fromY - toY : toY - fromY;
    return dx + dy;
}

void Map::setPointTypeWithDelay(const Pos &p, const Point::Type type) {
    getPoint(p).setType(type);
    util::sleep(20);
}

void Map::showTestPos(const Pos &p) {
    if (testEnabled) {
        setPointTypeWithDelay(p, Point::Type::TEST_VISIT);
    }
}

void Map::showTestPath(const Pos &start, const list<Direction> &path) {
    if (testEnabled) {
        Pos tmp = start;
        for (const Direction &d : path) {
            setPointTypeWithDelay(tmp, Point::Type::TEST_PATH);
            tmp = tmp.getAdj(d);
        }
        setPointTypeWithDelay(tmp, Point::Type::TEST_PATH);
    }
}

vector<Pos> Map::getEmptyPoints() const {
    vector<Pos> points;
    SizeType row = getRowCount(), col = getColCount();
    for (SizeType i = 1; i < row - 1; ++i) {
        for (SizeType j = 1; j < col - 1; ++j) {
            if (content[i][j].getType() == Point::Type::EMPTY) {
                points.push_back(Pos(i, j));
            }
        }
    }
    return points;
}
