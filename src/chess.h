#ifndef CHESS_H
#define CHESS_H

#define BOARD_GREEN Color{ 118,150,86, 255 }
#define BOARD_WHITE Color{ 238,238,210, 255 }
#define FIGURE_SELECTED Color{ 185, 202, 66, 255 }

#define CELL_SIZE 64 
#define CELL_COUNT 8

#include <iostream>
#include <raylib.h>
#include <vector>
#include <map>

class Figure;

typedef std::vector<Figure> _FigVec;
typedef std::vector<_FigVec> _Vec;

typedef enum {
	_NOTEAM,
	_WHITE, 
	_BLACK,
} Team;

typedef enum {
	EMPTY,
	PAWN,
	HORSE,
	BISHOP,
	ROCK,
	QUEEN,
	KING,
} FigureType;

typedef struct {
	FigureType _Type;
	Team _Team;
	Vector2 _Pos;
} TypeAndTeam;

typedef struct {
	std::map<std::string, Texture2D> _White;
	std::map<std::string, Texture2D> _Black;
} SaveTexture;

typedef struct {
	FigureType _Figure;
	int _Distance;
	Vector2 _Pos;
} LastMove;

typedef struct PosKing {
private:
	Vector2 _WhiteKing;
	Vector2 _BlackKing;

public:
	inline void setPosKing(Team T, Vector2 _Pos) {
		switch (T) {
		case _WHITE:
			_WhiteKing = _Pos;
			break;
		case _BLACK:
			_BlackKing = _Pos;
			break;
		case _NOTEAM:
			break;
		}
	}

	inline Vector2 getPosKing(Team T) const {
		switch (T) {
		case _WHITE:
			return _WhiteKing;
			break;
		case _BLACK:
			return _BlackKing;
			break;
		case _NOTEAM:
			break;
		}
		return Vector2{ -1, -1 };
	}
} PosKing;

typedef struct {
	Vector2 _Pos;
	FigureType _Type;
} VecType;

typedef struct HoldPosOfCheck {
private:
	std::vector<VecType> _White;
	std::vector<VecType> _Black;

public:
	inline HoldPosOfCheck() {
		_White = {};
		_Black = {};
	}
	inline void pushValue(Team _Team, VecType _Val) {
		switch (_Team) {
		case _WHITE:
			_White.push_back(_Val);
			break;
		case _BLACK:
			_Black.push_back(_Val);
			break;
		default:
			break;
		}
	}
	inline std::vector<VecType> *getVec(Team _Team) {
		switch (_Team) {
		case _WHITE:
			return &this->_White;
			break;
		case _BLACK:
			return &this->_Black;
			break;
		default:
			break;
		}
		return NULL;
	}
} HoldPosOfCheck;

typedef struct KingMoved {
private:
	bool _WhiteKing;
	bool _BlackKing;

public:
	inline void setKingMoved(Team _Team, bool _Changed) {
		switch (_Team) {
		case _WHITE:
			_WhiteKing = _Changed;
			break;
		case _BLACK:
			_BlackKing = _Changed;
			break;
		default:
			break;	
		}
	}
	inline bool getKingMoved(Team _Team) const {
		switch(_Team) {
		case _WHITE:
			return _WhiteKing;
			break;
		case _BLACK:
			return _BlackKing;
			break;
		default:
			break;
		}
		return false;
	}
} KingMoved;

typedef class Figure {
private:
	Vector2 m_position;
	Team m_team;
	FigureType m_type;
	Rectangle m_rect;
	Texture2D m_texture;

public:
	inline Figure(Vector2 _Position, Team _Team, FigureType _Type, Texture2D _Texture) : m_position(_Position),
		m_team(_Team), m_type(_Type), m_texture(_Texture) {
		m_rect = Rectangle{ m_position.x, m_position.y, CELL_SIZE, CELL_SIZE };
	};
	inline Figure() : m_position(), m_team(), m_type(), m_rect(), m_texture() {}
	inline Team getTeam() const { return this->m_team; }
	inline FigureType getType() const { return this->m_type; }
	inline Vector2 getPosition() const { return this->m_position; }
	inline Rectangle getRect() const { return m_rect; }
	inline Texture2D getTexture() const { return m_texture; }
	inline void setTeam(Team _Team) { m_team = _Team; }
	inline void setType(FigureType _Type) { m_type = _Type; }
	inline void setPosition(Vector2 _Position) { m_position = _Position; }
	inline void setTexture(Texture2D _Texture) { m_texture = _Texture; }
} Figure;

typedef class Chess {
private:
	_Vec m_field;
	bool m_selected;
	bool gameOver;
	Vector2 m_selectedVec;
	Team m_team;
	bool m_promotePawn;
	std::vector< TypeAndTeam> m_saveTexturePawn;
	SaveTexture m_loadedTexture;
	LastMove m_lastMove;
	HoldPosOfCheck m_check;
	PosKing m_posKing;
	KingMoved m_kingMoved;

public:
	Chess();
	~Chess();
	inline HoldPosOfCheck *getCheck() { return &m_check; }
	inline void setGameOver(bool _Over) { gameOver = _Over; }
	inline KingMoved *getKingMovedVar() { return &this->m_kingMoved; }
	inline SaveTexture *getSaveTexture() { return &m_loadedTexture; }
	void doMoveIfPossible(Vector2 _PosMouse);
	void createFigureForBoard();
	void update();
	void checkForCheck();
} Chess;

#endif