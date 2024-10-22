#include "chess.h"

static bool setNewFigure(_Vec *_Field, Vector2 _NewPos, Vector2 _OldPos, Team _Team, FigureType _Type, PosKing *_PosKing, Chess *_Chess) {
	std::vector<Figure> backup{ (*_Field)[(int)_OldPos.x][(int)_OldPos.y], (*_Field)[(int)_NewPos.x][(int)_NewPos.y] };
	float distanceY{ _NewPos.y - _OldPos.y };

	if (_Type != KING) {
		(*_Field)[(int)_NewPos.x][(int)_NewPos.y].setTeam(_Team);
		(*_Field)[(int)_NewPos.x][(int)_NewPos.y].setType(_Type);
		(*_Field)[(int)_OldPos.x][(int)_OldPos.y].setTeam(_NOTEAM);
		(*_Field)[(int)_OldPos.x][(int)_OldPos.y].setType(EMPTY);
	}
	else if (distanceY != 2 && distanceY != -2) {
		_PosKing->setPosKing(_Team, _NewPos);
		(*_Field)[(int)_NewPos.x][(int)_NewPos.y].setTeam(_Team);
		(*_Field)[(int)_NewPos.x][(int)_NewPos.y].setType(_Type);
		(*_Field)[(int)_OldPos.x][(int)_OldPos.y].setTeam(_NOTEAM);
		(*_Field)[(int)_OldPos.x][(int)_OldPos.y].setType(EMPTY);
	}

	_Chess->checkForCheck();
	if (_Chess->getCheck()->getVec(_Team)->size() != 0) {
		_Chess->getCheck()->getVec(_Team)->clear();
		(*_Field)[(int)_OldPos.x][(int)_OldPos.y] = backup[0];
		(*_Field)[(int)_NewPos.x][(int)_NewPos.y] = backup[1];
		return false; 
	}

	if (backup[1].getType() == KING) {
		_Chess->setGameOver(true);
	}

	(*_Field)[(int)_NewPos.x][(int)_NewPos.y].setTexture((*_Field)[(int)_OldPos.x][(int)_OldPos.y].getTexture());
	(*_Field)[(int)_OldPos.x][(int)_OldPos.y].setTexture(Texture2D());

	if (_Type == KING) {
		float dist = (_NewPos.y - _OldPos.y);
		auto setRock = [=](float xO, float yO, float xN, float yN) {
			(*_Field)[(int)_NewPos.x][(int)_NewPos.y].setTeam(_Team);
			(*_Field)[(int)_NewPos.x][(int)_NewPos.y].setType(_Type);
			(*_Field)[(int)_OldPos.x][(int)_OldPos.y].setTeam(_NOTEAM);
			(*_Field)[(int)_OldPos.x][(int)_OldPos.y].setType(EMPTY);

			(*_Field)[(int)xN][(int)yN].setTeam(_Team);
			(*_Field)[(int)xN][(int)yN].setType(ROCK);
			switch (_Team) {
			case _WHITE:
				(*_Field)[(int)xN][(int)yN].setTexture(_Chess->getSaveTexture()->_White["Rock"]);
				break;
			case _BLACK:
				(*_Field)[(int)xN][(int)yN].setTexture(_Chess->getSaveTexture()->_Black["Rock"]);
				break;
			default:
				break;
			}


			(*_Field)[(int)xO][(int)yO].setTeam(_NOTEAM);
			(*_Field)[(int)xO][(int)yO].setType(EMPTY);
			(*_Field)[(int)xO][(int)yO].setTexture(Texture2D());
		};

		switch ((int)dist) {
		case 2:
			switch (_Team) {
			case _WHITE:
				setRock(7, 7, 7, 5);
				break;
			case _BLACK:
				setRock(0, 7, 0, 5);
				break;
			default:
				break;
			}
			break;
		case -2:
			switch (_Team) {
			case _WHITE:
				setRock(7, 0, 7, 3);
				break;
			case _BLACK:
				setRock(0, 0, 0, 3);
				break;
			default:
				break;
			}
			break;
		}

		_PosKing->setPosKing(_Team, _NewPos);
		_Chess->getKingMovedVar()->setKingMoved(_Team, true);
	}
	else if (_Type == ROCK) {
		_Chess->getKingMovedVar()->setKingMoved(_Team, true);
	}

	_Chess->getCheck()->getVec(_Team)->clear();
	return true;
}

static void checkIfVerticallMovePossible(_Vec *_Field, Vector2 _NewPos, Vector2 _OldPos, Team *_Team, LastMove *_LastMove, FigureType _Type, PosKing *_PosKing, Chess *_Chess) {
	Vector2 distance = { _NewPos.x - _OldPos.x, _NewPos.y - _OldPos.y };
	if (abs(distance.x) == abs(distance.y)) {
		for (unsigned int i{}; i < abs(distance.x); ++i) {
			Vector2 newPos = Vector2{ _OldPos.x + ((distance.x / abs(distance.x)) * (i + 1)), _OldPos.y + ((distance.y / abs(distance.y)) * (i + 1)) };
			if ((*_Field)[(int)newPos.x][(int)newPos.y].getType() != EMPTY && i != (abs(distance.x) - 1)) break;
			else if (i == (abs(distance.x) - 1)) {

				switch (*_Team) {
				case _BLACK:
					if ((*_Field)[(int)newPos.x][(int)newPos.y].getTeam() != _BLACK) {
						if (setNewFigure(_Field, Vector2{ newPos.x, newPos.y },
							Vector2{ _OldPos.x, _OldPos.y }, *_Team, _Type, _PosKing, _Chess)) {
							_LastMove->_Figure = _Type;
							*_Team = _WHITE;
						}
					}
					break;
				case _WHITE:
					if ((*_Field)[(int)newPos.x][(int)newPos.y].getTeam() != _WHITE) {
						if (setNewFigure(_Field, Vector2{ newPos.x, newPos.y },
							Vector2{ _OldPos.x, _OldPos.y }, *_Team, _Type, _PosKing, _Chess)) {
							_LastMove->_Figure = _Type;
							*_Team = _BLACK;
						}
					}
					break;
				default: break;
				}
			}
		}
	}
}

static Vector2 divideVector2(Vector2 _Vec1, Vector2 _Vec2) {
	return Vector2{ std::floor(_Vec1.x / _Vec2.x), std::floor(_Vec1.y / _Vec2.y) };
}

static _FigVec createFigureRow(FigureType _Type, unsigned int _NuberOfFigure, int _RowNumber, Team _Team, Texture2D texture) {
	_FigVec _Ret{};
	for (unsigned int i{}; i < _NuberOfFigure; ++i) {
		Figure whiteFigure(Vector2{ (float)(_RowNumber * CELL_SIZE), (float)i }, _Team, _Type, texture);
		_Ret.push_back(whiteFigure);
	}
	return _Ret;
}

static void doPawnMoveIfPossible(_Vec *_Field, Vector2 _PosMouse, Vector2 _OldPos, Team *_Team, bool *_PromotePawn, LastMove *_LastMove, PosKing *_PosKing, Chess *_Chess) {
	Vector2 distance{ _PosMouse.x - _OldPos.x, _PosMouse.y - _OldPos.y };
	auto changePosition = [=]() {
		if (setNewFigure(_Field, Vector2{ _PosMouse.x, _PosMouse.y },
			Vector2{ _OldPos.x, _OldPos.y }, *_Team, PAWN, _PosKing, _Chess)) {
			_LastMove->_Figure = PAWN;
			_LastMove->_Distance = (int)distance.x;
			_LastMove->_Pos = _PosMouse;
		
			switch (*_Team) {
			case _BLACK:
				if (_PosMouse.x == 7) *_PromotePawn = true;
				break;
			case _WHITE:
				if (_PosMouse.x == 0) *_PromotePawn = true;
				break;
			default: break;
			}

			if (*_Team == _WHITE) *_Team = _BLACK;
			else *_Team = _WHITE;
		}
	};

	auto enPassant = [](Figure *_Figure) {
		_Figure->setTexture(Texture2D());
		_Figure->setTeam(_NOTEAM);
		_Figure->setType(EMPTY);
	};

	int positionOfPossiblePawn{};
	Figure *figureNext;

	switch (*_Team) {
	case _WHITE:
		switch ((int)distance.x) {
		case -1:
			switch ((int)distance.y) {
			case 0:
				if ((*_Field)[(int)_PosMouse.x][(int)_PosMouse.y].getType() == EMPTY) {
					changePosition();
				}
				break;
			case 1:
				positionOfPossiblePawn = (int)_OldPos.y + 1;
				figureNext = &(*_Field)[(int)_OldPos.x][positionOfPossiblePawn];
				if ((*_Field)[(int)_PosMouse.x][(int)_PosMouse.y].getTeam() == _BLACK) {
					changePosition();
				}
				else if ((figureNext->getType() == PAWN && figureNext->getTeam() == _BLACK &&
					_LastMove->_Figure == PAWN && _LastMove->_Distance == 2 &&
					_LastMove->_Pos.y == (int)_PosMouse.y)) {
					changePosition();
					enPassant(figureNext);
				}
				break;
			case -1:
				positionOfPossiblePawn = (int)_OldPos.y - 1;
				figureNext = &(*_Field)[(int)_OldPos.x][positionOfPossiblePawn];
				if ((*_Field)[(int)_PosMouse.x][(int)_PosMouse.y].getTeam() == _BLACK) {
					changePosition();
				}
				else if ((figureNext->getType() == PAWN && figureNext->getTeam() == _BLACK &&
					_LastMove->_Figure == PAWN && _LastMove->_Distance == 2 &&
					_LastMove->_Pos.y == (int)_PosMouse.y)) {
					changePosition();
					enPassant(figureNext);
				}
				break;
			}
			break;
		case -2:
			if (distance.y == 0 && _OldPos.x == 6) {
				int positionBefore{ (int)_PosMouse.x + 1 };
				if ((*_Field)[positionBefore][(int)_PosMouse.y].getType() == EMPTY) { 
					changePosition(); 
				}
			}
			break;
		}
		break;
	case _BLACK:
		switch ((int)distance.x) {
		case 1:
			switch ((int)distance.y) {
			case 0:
				if ((*_Field)[(int)_PosMouse.x][(int)_PosMouse.y].getType() == EMPTY) {
					changePosition();
				}
				break;
			case 1:
				positionOfPossiblePawn = (int)_OldPos.y + 1;
				figureNext = &(*_Field)[(int)_OldPos.x][positionOfPossiblePawn];
				if ((*_Field)[(int)_PosMouse.x][(int)_PosMouse.y].getTeam() == _WHITE) {
					changePosition();
				}
				else if ((figureNext->getType() == PAWN && figureNext->getTeam() == _WHITE &&
					_LastMove->_Figure == PAWN && _LastMove->_Distance == -2 && 
					_LastMove->_Pos.y == (int)_PosMouse.y)) {
					changePosition();
					enPassant(figureNext);
				}
				break;
			case -1:
				positionOfPossiblePawn = (int)_OldPos.y - 1;
				figureNext = &(*_Field)[(int)_OldPos.x][positionOfPossiblePawn];
				if ((*_Field)[(int)_PosMouse.x][(int)_PosMouse.y].getTeam() == _WHITE) {
					changePosition();
				}
				else if ((figureNext->getType() == PAWN && figureNext->getTeam() == _WHITE &&
					_LastMove->_Figure == PAWN && _LastMove->_Distance == -2 &&
					_LastMove->_Pos.y == (int)_PosMouse.y)) {
					changePosition();
					enPassant(figureNext);
				}
				break;
			}
			break;
		case 2:
			if (distance.y == 0 && _OldPos.x == 1) {
				int positionBefore{ (int)_PosMouse.x - 1 };
				if ((*_Field)[positionBefore][(int)_PosMouse.y].getType() == EMPTY) changePosition();
			}
			break;
		}
		break;
	default: break;
	}
}

static void doHorseMoveIfPossible(_Vec *_Field, Vector2 _NewPos, Vector2 _OldPos, Team *_Team, PosKing *_PosKing, Chess *_Chess) {
	Vector2 distance = Vector2{ _NewPos.x - _OldPos.x, _NewPos.y - _OldPos.y };
	
	switch ((int)distance.x) {
	case 1:
		if (distance.y == 2 || distance.y == -2) {
			if ((*_Field)[(int)_NewPos.x][(int)_NewPos.y].getTeam() != *_Team) {
				if (setNewFigure(_Field, _NewPos, _OldPos, *_Team, HORSE, _PosKing, _Chess)) {
					if (*_Team == _WHITE) *_Team = _BLACK;
					else *_Team = _WHITE;
				}
			}
		}
		break;
	case 2:
		if (distance.y == 1 || distance.y == -1) {
			if ((*_Field)[(int)_NewPos.x][(int)_NewPos.y].getTeam() != *_Team) {
				if (setNewFigure(_Field, _NewPos, _OldPos, *_Team, HORSE, _PosKing, _Chess)) {
					if (*_Team == _WHITE) *_Team = _BLACK;
					else *_Team = _WHITE;
				}
			}
		}
		break;
	case -1:
		if (distance.y == 2 || distance.y == -2) {
			if ((*_Field)[(int)_NewPos.x][(int)_NewPos.y].getTeam() != *_Team) {
				if (setNewFigure(_Field, _NewPos, _OldPos, *_Team, HORSE, _PosKing, _Chess)) {
					if (*_Team == _WHITE) *_Team = _BLACK;
					else *_Team = _WHITE;
				}
			}
		}
		break;
	case -2:
		if (distance.y == 1 || distance.y == -1) {
			if ((*_Field)[(int)_NewPos.x][(int)_NewPos.y].getTeam() != *_Team) {
				if (setNewFigure(_Field, _NewPos, _OldPos, *_Team, HORSE, _PosKing, _Chess)) {
					if (*_Team == _WHITE) *_Team = _BLACK;
					else *_Team = _WHITE;
				}
			}
		}
		break;
	}
}

static void checkIfStraightMoveIsPossible(_Vec *_Field, Vector2 _NewPos, Vector2 _OldPos, Team *_Team, LastMove *_LastMove, FigureType _Type, PosKing *_PosKing, Chess *_Chess) {
	Vector2 distance = Vector2{ _NewPos.x - _OldPos.x, _NewPos.y - _OldPos.y };
	auto checkMove = [=](Vector2 _TmpPos, unsigned int i, unsigned int compareWithI) ->bool {
		if ((*_Field)[(int)_TmpPos.x][(int)_TmpPos.y].getType() != EMPTY && i != compareWithI) return false;
		else if (i == compareWithI) {

			switch (*_Team) {
			case _BLACK:
				if ((*_Field)[(int)_TmpPos.x][(int)_TmpPos.y].getTeam() != _BLACK) {
					if (setNewFigure(_Field, Vector2{ _TmpPos.x, _TmpPos.y },
						Vector2{ _OldPos.x, _OldPos.y }, *_Team, _Type, _PosKing, _Chess)) {
						_LastMove->_Figure = ROCK;
						*_Team = _WHITE;
					}
				}
				break;
			case _WHITE:
				if ((*_Field)[(int)_TmpPos.x][(int)_TmpPos.y].getTeam() != _WHITE) {
					if (setNewFigure(_Field, Vector2{ _TmpPos.x, _TmpPos.y },
						Vector2{ _OldPos.x, _OldPos.y }, *_Team, _Type, _PosKing, _Chess)) {
						_LastMove->_Figure = ROCK;
						*_Team = _BLACK;
					}
				}
				break;
			default: break;
			}
		}
		return true;
	};
	
	switch ((int)distance.x) {
	case 0:
		if (distance.y != 0) {
			for (unsigned int i{}; i < abs(distance.y); ++i) {
				Vector2 tmpPos = Vector2{ _OldPos.x, _OldPos.y + ((distance.y / abs(distance.y)) * (i + 1)) };
				if (!checkMove(tmpPos, i, (int)(abs(distance.y) - 1))) break;
			}
		}
		break;
	default:
		if (distance.y == 0) {
			for (unsigned int i{}; i < abs(distance.x); ++i) {
				Vector2 tmpPos = Vector2{ _OldPos.x + ((distance.x / abs(distance.x)) * (i + 1)), _OldPos.y };
				if (!checkMove(tmpPos, i, (int)(abs(distance.x) - 1))) break;
			}
		}
		break;
	}
}

static void checkForStraight(_Vec *_Field, Team _Team, PosKing _PosKing, HoldPosOfCheck *_Check) {
	Vector2 copyVec = _PosKing.getPosKing(_Team);
	auto checkStraight = [=, &copyVec](Team t_team) ->void {
		--copyVec.x;
		while (copyVec.x > -1) {
			Figure figure = (*_Field)[(int)copyVec.x][(int)copyVec.y];
			if (figure.getTeam() != _NOTEAM) {
				if (figure.getTeam() == t_team && (figure.getType() == QUEEN || figure.getType() == ROCK)) {
					_Check->pushValue(_Team, VecType{ copyVec, figure.getType() });
					break;
				}
				break;
			}
			--copyVec.x;
		}
		copyVec.x = _PosKing.getPosKing(_Team).x + 1;
		while (copyVec.x < 8) {
			Figure figure = (*_Field)[(int)copyVec.x][(int)copyVec.y];
			if (figure.getTeam() != _NOTEAM) {
				if (figure.getTeam() == t_team && (figure.getType() == QUEEN || figure.getType() == ROCK)) {
					_Check->pushValue(_Team, VecType{ copyVec, figure.getType() });
					break;
				}
				break;
			}
			++copyVec.x;
		}
		copyVec.x = _PosKing.getPosKing(_Team).x;
		--copyVec.y;
		while (copyVec.y > -1) {
			Figure figure = (*_Field)[(int)copyVec.x][(int)copyVec.y];
			if (figure.getTeam() != _NOTEAM) {
				if (figure.getTeam() == t_team && (figure.getType() == QUEEN || figure.getType() == ROCK)) {
					_Check->pushValue(_Team, VecType{ copyVec, figure.getType() });
					break;
				}
				break;
			}
			--copyVec.y;
		}
		copyVec.y = _PosKing.getPosKing(_Team).y + 1;
		while (copyVec.y < 8) {
			Figure figure = (*_Field)[(int)copyVec.x][(int)copyVec.y];
			if (figure.getTeam() != _NOTEAM) {
				if (figure.getTeam() == t_team && (figure.getType() == QUEEN || figure.getType() == ROCK)) {
					_Check->pushValue(_Team, VecType{ copyVec, figure.getType() });
					break;
				}
				break;
			}
			++copyVec.y;
		}
	};

	switch (_Team) {
	case _WHITE:
		checkStraight(_BLACK);
		break;
	case _BLACK:
		checkStraight(_WHITE);
		break;
	default: break;
	}
}

static void checkForVertical(_Vec *_Field, Team _Team, PosKing _PosKing, HoldPosOfCheck *_Check) {
	Vector2 copyVec = _PosKing.getPosKing(_Team);
	auto checkVertical = [=, &copyVec](Vector2 _Direction, Team t_team) ->void {
		copyVec = Vector2{ copyVec.x + _Direction.x, copyVec.y + _Direction.y };
		while (-1 < copyVec.x && copyVec.x < 8 && -1 < copyVec.y && copyVec.y < 8) {
			Figure figure = (*_Field)[(int)copyVec.x][(int)copyVec.y];
			if (figure.getType() != EMPTY) {
				if (figure.getTeam() == t_team && (figure.getType() == BISHOP || figure.getType() == QUEEN)) {
					_Check->pushValue(_Team, VecType{ copyVec, figure.getType() });
					break;
				}
				break;
			}
			copyVec = Vector2{ copyVec.x + _Direction.x, copyVec.y + _Direction.y };
		}
		copyVec = _PosKing.getPosKing(_Team);
	};

	switch (_Team) {
	case _WHITE:
		checkVertical(Vector2{ 1, 1 }, _BLACK);
		checkVertical(Vector2{ 1, -1 }, _BLACK);
		checkVertical(Vector2{ -1, 1 }, _BLACK);
		checkVertical(Vector2{ -1, -1 }, _BLACK);
		break;
	case _BLACK:
		checkVertical(Vector2{ 1, 1 }, _WHITE);
		checkVertical(Vector2{ 1, -1 }, _WHITE);
		checkVertical(Vector2{ -1, 1 }, _WHITE);
		checkVertical(Vector2{ -1, -1 }, _WHITE);
		break;
	default: break;
	}
}

static void checkForHorse(_Vec *_Field, Team _Team, PosKing _PosKing, HoldPosOfCheck *_Check) {
	Vector2 posOfHorse = _PosKing.getPosKing(_Team);
	auto checkHorse = [=, &posOfHorse](float x, float y, Team t_team) ->void {
		posOfHorse = Vector2{ posOfHorse.x + x, posOfHorse.y + y };
		if (posOfHorse.x < 8 && posOfHorse.x > -1 && posOfHorse.y < 8 && posOfHorse.y > -1) {
			Figure figure = (*_Field)[(int)posOfHorse.x][(int)posOfHorse.y];
			if (figure.getTeam() == t_team && figure.getType() == HORSE) {
				_Check->pushValue(t_team, VecType{ posOfHorse, HORSE });
			}
		}

		posOfHorse = _PosKing.getPosKing(_Team);
	};

	auto callLambda = [&checkHorse](Team t_team) ->void {
		checkHorse(2, 1, t_team);
		checkHorse(2, -1, t_team);
		checkHorse(-2, 1, t_team);
		checkHorse(-2, -1, t_team);
		checkHorse(1, 2, t_team);
		checkHorse(-1, 2, t_team);
		checkHorse(1, -2, t_team);
		checkHorse(-1, -2, t_team);
	};

	switch (_Team) {
	case _WHITE:
		callLambda(_BLACK);
		break;
	case _BLACK:
		callLambda(_WHITE);
		break;
	default: break;
	}
}

static void rochade(_Vec *_Field, Team *_Team, Vector2 _NewPos, Vector2 _OldPos, KingMoved *_KingMoved, PosKing *_PosKing, Chess *_Chess) {
	if (!_KingMoved->getKingMoved(*_Team)) {
		Vector2 distance = Vector2{ _NewPos.x - _OldPos.x, _NewPos.y - _OldPos.y };
		if (distance.x == 0) {
			Vector2 _PosToTest = _OldPos;

			switch ((int)distance.y) {
			case 2:
				++_PosToTest.y;
				while (_PosToTest.y < 8) {
					Figure figure = (*_Field)[(int)_PosToTest.x][(int)_PosToTest.y];
					if (figure.getTeam() != _NOTEAM) {
						if (figure.getType() == ROCK && figure.getTeam() == *_Team) {
							if (setNewFigure(_Field, _NewPos, _OldPos, *_Team, KING, _PosKing, _Chess)) {
								if (*_Team == _WHITE) *_Team = _BLACK;
								else *_Team = _WHITE;
							}
						}
						break;
					}
					++_PosToTest.y;
				}
				break;
			case -2:
				--_PosToTest.y;
				while (_PosToTest.y > -1) {
					Figure figure = (*_Field)[(int)_PosToTest.x][(int)_PosToTest.y];
					if (figure.getTeam() != _NOTEAM) {
						if (figure.getType() == ROCK && figure.getTeam() == *_Team) {
							if (setNewFigure(_Field, _NewPos, _OldPos, *_Team, KING, _PosKing, _Chess)) {
								if (*_Team == _WHITE) *_Team = _BLACK;
								else *_Team = _WHITE;
							}
						}
						break;
					}
					--_PosToTest.y;
				}
				break;
			}
		}
	}
}

void Chess::checkForCheck() {
		checkForStraight(&m_field, m_team, m_posKing, &m_check);
		checkForVertical(&m_field, m_team, m_posKing, &m_check);
		checkForHorse(&m_field, m_team, m_posKing, &m_check);
}

void Chess::doMoveIfPossible(Vector2 _PosMouse) {
	switch (m_field[(int)m_selectedVec.y][(int)m_selectedVec.x].getType()) {
	case EMPTY:
		break;
	case PAWN:
		doPawnMoveIfPossible(&m_field, _PosMouse, Vector2{ m_selectedVec.y, m_selectedVec.x }, &m_team, &m_promotePawn, &m_lastMove, &m_posKing, this);
		break;
	case HORSE:
		doHorseMoveIfPossible(&m_field, _PosMouse, Vector2{ m_selectedVec.y, m_selectedVec.x }, &m_team, &m_posKing, this);
		break;
	case BISHOP:
		checkIfVerticallMovePossible(&m_field, _PosMouse, Vector2{ m_selectedVec.y, m_selectedVec.x }, &m_team, &m_lastMove, BISHOP, &m_posKing, this);
		break;
	case ROCK:
		checkIfStraightMoveIsPossible(&m_field, _PosMouse, Vector2{ m_selectedVec.y, m_selectedVec.x }, &m_team, &m_lastMove, ROCK, &m_posKing, this);
		break;
	case QUEEN:
		if (_PosMouse.x == m_selectedVec.y || _PosMouse.y == m_selectedVec.x) {
			checkIfStraightMoveIsPossible(&m_field, _PosMouse, Vector2{ m_selectedVec.y, m_selectedVec.x }, &m_team, &m_lastMove, QUEEN, &m_posKing, this);
		}
		else {
			checkIfVerticallMovePossible(&m_field, _PosMouse, Vector2{ m_selectedVec.y, m_selectedVec.x }, &m_team, &m_lastMove, QUEEN, &m_posKing, this);
		}
		break;
	case KING:
		Vector2 distance = Vector2{ _PosMouse.x - m_selectedVec.y, _PosMouse.y - m_selectedVec.x };
		if (!(abs(distance.x) > 1 || abs(distance.y) > 1)) {
			if (distance.x == 0 || distance.y == 0) {
				checkIfStraightMoveIsPossible(&m_field, _PosMouse, Vector2{ m_selectedVec.y, m_selectedVec.x }, &m_team, &m_lastMove, KING, &m_posKing, this);
			}
			else {
				checkIfVerticallMovePossible(&m_field, _PosMouse, Vector2{ m_selectedVec.y, m_selectedVec.x }, &m_team, &m_lastMove, KING, &m_posKing, this);
			}
		}
		else if (distance.x == 0 && (distance.y == 2 || distance.y == -2)) {
			rochade(&m_field, &m_team, _PosMouse, Vector2{ m_selectedVec.y, m_selectedVec.x }, &m_kingMoved, &m_posKing, this);
		}
		break;
	}
}

void Chess::createFigureForBoard() {
	Figure blackRock1(Vector2{ 0, 0 }, _BLACK, ROCK, m_loadedTexture._Black["Rock"]);
	Figure blackHorse1(Vector2{ 1, 0 }, _BLACK, HORSE, m_loadedTexture._Black["Horse"]);
	Figure blackBishop1(Vector2{ 2, 0 }, _BLACK, BISHOP, m_loadedTexture._Black["Bishop"]);
	Figure blackQueen(Vector2{ 3, 0 }, _BLACK, QUEEN, m_loadedTexture._Black["Queen"]);
	Figure blackKing(Vector2{ 4, 0 }, _BLACK, KING, m_loadedTexture._Black["King"]);
	Figure blackBishop2(Vector2{ 5, 0 }, _BLACK, BISHOP, m_loadedTexture._Black["Bishop"]);
	Figure blackHorse2(Vector2{ 6, 0 }, _BLACK, HORSE, m_loadedTexture._Black["Horse"]);
	Figure blackRock2(Vector2{ 7, 0 }, _BLACK, ROCK, m_loadedTexture._Black["Rock"]);

	Figure whiteRock1(Vector2{ 0, 7 }, _WHITE, ROCK, m_loadedTexture._White["Rock"]);
	Figure whiteHorse1(Vector2{ 1, 7 }, _WHITE, HORSE, m_loadedTexture._White["Horse"]);
	Figure whiteBishop1(Vector2{ 2, 7 }, _WHITE, BISHOP, m_loadedTexture._White["Bishop"]);
	Figure whiteQueen(Vector2{ 3, 7 }, _WHITE, QUEEN, m_loadedTexture._White["Queen"]);
	Figure whiteKing(Vector2{ 4, 7 }, _WHITE, KING, m_loadedTexture._White["King"]);
	Figure whiteBishop2(Vector2{ 5, 7 }, _WHITE, BISHOP, m_loadedTexture._White["Bishop"]);
	Figure whiteHorse2(Vector2{ 6, 7 }, _WHITE, HORSE, m_loadedTexture._White["Horse"]);
	Figure whiteRock2(Vector2{ 7, 7 }, _WHITE, ROCK, m_loadedTexture._White["Rock"]);

	_FigVec blackLine{ blackRock1, blackHorse1, blackBishop1, blackQueen, blackKing, blackBishop2, blackHorse2, blackRock2 };
	_FigVec whiteLine{ whiteRock1, whiteHorse1, whiteBishop1, whiteQueen, whiteKing, whiteBishop2, whiteHorse2, whiteRock2 };

	this->m_field.push_back(blackLine);
	this->m_field.push_back(createFigureRow(PAWN, 8, 6, _BLACK, m_loadedTexture._Black["Pawn"]));
	this->m_field.push_back(createFigureRow(EMPTY, 8, 5, _NOTEAM, Texture2D()));
	this->m_field.push_back(createFigureRow(EMPTY, 8, 4, _NOTEAM, Texture2D()));
	this->m_field.push_back(createFigureRow(EMPTY, 8, 3, _NOTEAM, Texture2D()));
	this->m_field.push_back(createFigureRow(EMPTY, 8, 2, _NOTEAM, Texture2D()));
	this->m_field.push_back(createFigureRow(PAWN, 8, 1, _WHITE, m_loadedTexture._White["Pawn"]));
	this->m_field.push_back(whiteLine);
}

void Chess::update() {
	if (!gameOver) {
		int drawWhite{ 0 };
		for (unsigned int i{}; i < CELL_COUNT; ++i) {
			for (unsigned int j{}; j < CELL_COUNT; ++j) {
				switch (drawWhite) {
				case 0:
					DrawRectangle(i * CELL_SIZE, j * CELL_SIZE, CELL_SIZE, CELL_SIZE, BOARD_WHITE);
					drawWhite = 1;
					break;
				case 1:
					DrawRectangle(i * CELL_SIZE, j * CELL_SIZE, CELL_SIZE, CELL_SIZE, BOARD_GREEN);
					drawWhite = 0;
					break;
				}
			}
			switch (drawWhite) {
			case 0:
				drawWhite = 1;
				break;
			case 1:
				drawWhite = 0;
				break;
			}
		}
	
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			Vector2 posMouse = divideVector2(GetMousePosition(), Vector2{ CELL_SIZE, CELL_SIZE });
		
			if (!this->m_promotePawn) {
				if (this->m_selected) {
					if (!(m_selectedVec.x == posMouse.x && m_selectedVec.y == posMouse.y)) {
						if (m_field[(int)m_selectedVec.y][(int)m_selectedVec.x].getTeam() == m_team) {
							doMoveIfPossible(Vector2{ posMouse.y, posMouse.x });
						}
					}
					this->m_selected = false;
					this->m_selectedVec = { -1, -1 };
				}
				else {
					this->m_selected = true;
					this->m_selectedVec = posMouse;
				}
			}
			else {
				auto loadNewPiece = [=](Vector2 _Pos) ->void {
					auto getPositionOfVector = [=](int _Row) ->Vector2 {
						float index{};
						for (Figure &figure : m_field[_Row]) {
							if (figure.getType() == PAWN) {
								return Vector2{ index, (float)_Row };
							}
							++index;
						}
						return Vector2{ -1, -1 };
					};
				
					Vector2 positionOfPawn{};
					switch (m_team) {
					case _WHITE:
						positionOfPawn = getPositionOfVector(7);
						break;
					case _BLACK:
						positionOfPawn = getPositionOfVector(0);
						break;
					default: break;
					}
					auto loadCorrectTexture = [=](std::string figureName, FigureType _Type) ->void {
						switch (m_team) {
						case _WHITE:
							m_field[(int)positionOfPawn.y][(int)positionOfPawn.x].setTexture(m_loadedTexture._Black[figureName]);
							m_field[(int)positionOfPawn.y][(int)positionOfPawn.x].setTeam(_BLACK);
							m_field[(int)positionOfPawn.y][(int)positionOfPawn.x].setType(_Type);
							break;
						case _BLACK:
							m_field[(int)positionOfPawn.y][(int)positionOfPawn.x].setTexture(m_loadedTexture._White[figureName]);
							m_field[(int)positionOfPawn.y][(int)positionOfPawn.x].setTeam(_WHITE);
							m_field[(int)positionOfPawn.y][(int)positionOfPawn.x].setType(_Type);
							break;
						default: break;
						}
					};

					switch ((int)_Pos.x) {
					case 3:
						switch ((int)_Pos.y) {
						case 3:
							loadCorrectTexture("Queen", QUEEN);
							break;
						case 4:
							loadCorrectTexture("Bishop", BISHOP);
							break;
						}
						break;
					case 4:
						switch ((int)_Pos.y) {
						case 3:
							loadCorrectTexture("Rock", ROCK);
							break;
						case 4:
							loadCorrectTexture("Horse", HORSE);
							break;
						}
						break;
					}

					m_field[3][3].setTexture(Texture2D());
					m_field[3][4].setTexture(Texture2D());
					m_field[4][3].setTexture(Texture2D());
					m_field[4][4].setTexture(Texture2D());

					m_field[3][3].setTeam(_NOTEAM);
					m_field[3][4].setTeam(_NOTEAM);
					m_field[4][3].setTeam(_NOTEAM);
					m_field[4][4].setTeam(_NOTEAM);

					m_field[3][3].setType(EMPTY);
					m_field[3][4].setType(EMPTY);
					m_field[4][3].setType(EMPTY);
					m_field[4][4].setType(EMPTY);

					for (TypeAndTeam &figure : m_saveTexturePawn) {
						if (figure._Type == EMPTY) continue;
						[=]() ->void {
							switch (figure._Type) {
							case PAWN:
								switch (m_team) {
								case _WHITE:
									m_field[(int)figure._Pos.y][(int)figure._Pos.x].setTexture(m_loadedTexture._White["Pawn"]);
									break;
								case _BLACK:
									m_field[(int)figure._Pos.y][(int)figure._Pos.x].setTexture(m_loadedTexture._Black["Pawn"]);
									break;
								default: break;
								}
								break;
							case HORSE:
								switch (m_team) {
								case _WHITE:
									m_field[(int)figure._Pos.y][(int)figure._Pos.x].setTexture(m_loadedTexture._White["Horse"]);
									break;
								case _BLACK:
									m_field[(int)figure._Pos.y][(int)figure._Pos.x].setTexture(m_loadedTexture._Black["Horse"]);
									break;
								default: break;
								}
								break;
							case BISHOP:
								switch (m_team) {
								case _WHITE:
									m_field[(int)figure._Pos.y][(int)figure._Pos.x].setTexture(m_loadedTexture._White["Bishop"]);
									break;
								case _BLACK:
									m_field[(int)figure._Pos.y][(int)figure._Pos.x].setTexture(m_loadedTexture._Black["Bishop"]);
									break;
								default: break;
								}
								break;
							case ROCK:
								switch (m_team) {
								case _WHITE:
									m_field[(int)figure._Pos.y][(int)figure._Pos.x].setTexture(m_loadedTexture._White["Rock"]);
									break;
								case _BLACK:
									m_field[(int)figure._Pos.y][(int)figure._Pos.x].setTexture(m_loadedTexture._Black["Rock"]);
									break;
								default: break;
								}
								break;
							case QUEEN:
								switch (m_team) {
								case _WHITE:
									m_field[(int)figure._Pos.y][(int)figure._Pos.x].setTexture(m_loadedTexture._White["Queen"]);
									break;
								case _BLACK:
									m_field[(int)figure._Pos.y][(int)figure._Pos.x].setTexture(m_loadedTexture._Black["Queen"]);
									break;
								default: break;
								}
								break;
							case KING:
								switch (m_team) {
								case _WHITE:
									m_field[(int)figure._Pos.y][(int)figure._Pos.x].setTexture(m_loadedTexture._White["King"]);
									break;
								case _BLACK:
									m_field[(int)figure._Pos.y][(int)figure._Pos.x].setTexture(m_loadedTexture._Black["King"]);
									break;
								default: break;
								}
								break;
							default: break;
							}
						}();
					}

					m_saveTexturePawn.clear();
					this->m_promotePawn = false;
				};

				if (posMouse.x == 3 && posMouse.y == 3) {
					loadNewPiece(Vector2{ 3, 3 });
				}
				else if (posMouse.x == 3 && posMouse.y == 4) {
					loadNewPiece(Vector2{ 3, 4 });
				}
				else if (posMouse.x == 4 && posMouse.y == 3) {
					loadNewPiece(Vector2{ 4, 3 });
				}
				else if (posMouse.x == 4 && posMouse.y == 4) {
					loadNewPiece(Vector2{ 4, 4 });
				}
			}
		}

		if (this->m_selected) {
			DrawRectangle((int)m_selectedVec.x * CELL_SIZE, (int)m_selectedVec.y * CELL_SIZE, CELL_SIZE, CELL_SIZE, FIGURE_SELECTED);
		}

		if (m_promotePawn) {
			float extraSize{ CELL_SIZE / 8 };
			DrawRectangleRounded(Rectangle{ 3 * CELL_SIZE - extraSize, 3 * CELL_SIZE - extraSize, 2 * CELL_SIZE + 2 * extraSize, 2 * CELL_SIZE + 2 * extraSize }, 0.2f, 0, Color{ 255, 255, 255, 100 });
			if (m_saveTexturePawn.size() == 0) {
				m_saveTexturePawn.push_back(TypeAndTeam{ m_field[3][3].getType(), m_field[3][3].getTeam(), Vector2{ 3, 3 } });
				m_saveTexturePawn.push_back(TypeAndTeam{ m_field[3][4].getType(), m_field[3][4].getTeam(), Vector2{ 3, 4 } });
				m_saveTexturePawn.push_back(TypeAndTeam{ m_field[4][3].getType(), m_field[3][3].getTeam(), Vector2{ 4, 3 } });
				m_saveTexturePawn.push_back(TypeAndTeam{ m_field[4][4].getType(), m_field[4][4].getTeam(), Vector2{ 4, 4 } });

				switch (m_team) {
				case _BLACK:
					m_field[3][3].setTexture(m_loadedTexture._White["Queen"]);
					m_field[3][4].setTexture(m_loadedTexture._White["Rock"]);
					m_field[4][3].setTexture(m_loadedTexture._White["Bishop"]);
					m_field[4][4].setTexture(m_loadedTexture._White["Horse"]);
					break;
				case _WHITE:
					m_field[3][3].setTexture(m_loadedTexture._Black["Queen"]);
					m_field[3][4].setTexture(m_loadedTexture._Black["Rock"]);
					m_field[4][3].setTexture(m_loadedTexture._Black["Bishop"]);
					m_field[4][4].setTexture(m_loadedTexture._Black["Horse"]);
					break;
				default: break;
				}

				m_field[3][3].setType(QUEEN);
				m_field[3][4].setType(ROCK);
				m_field[4][3].setType(BISHOP);
				m_field[4][4].setType(HORSE);
			}
		}

		//checkForCheck();

		int i{ 0 };
		for (_FigVec &figureVec : m_field) {
			int j{ 0 };
			for (Figure &figure : figureVec) {
				if (figure.getType() != EMPTY) {
					DrawTexture(figure.getTexture(), j * CELL_SIZE, i * CELL_SIZE, WHITE);
				}
				++j;
			}
			++i;
		}
	}
	else {
		switch (m_team) {
		case _WHITE:
			DrawText("Black Wins", (CELL_SIZE * CELL_COUNT) / 2, (CELL_SIZE * CELL_COUNT) / 2, 20, WHITE);
			break;
		case _BLACK:
			DrawText("White Wins", (CELL_SIZE * CELL_COUNT) / 2, (CELL_SIZE * CELL_COUNT) / 2, 20, WHITE);
			break;
		default: break;
		}
	}
}

Chess::Chess() {
	gameOver = false;
	m_selected = false;
	m_selectedVec = { -1, -1 };
	m_team = _WHITE;
	m_promotePawn = false;
	m_posKing.setPosKing(_WHITE, Vector2{ 7, 4 });
	m_posKing.setPosKing(_BLACK, Vector2{ 0, 4 });
	for (unsigned int i{}; i < CELL_COUNT; ++i) {
		int drawWhite{ 0 };
		for (unsigned int j{}; j < CELL_COUNT; ++j) {
			switch (drawWhite) {
			case 0:
				DrawRectangle(i * CELL_SIZE, j * CELL_SIZE, CELL_SIZE, CELL_SIZE, BOARD_WHITE);
				drawWhite = 1;
				break;
			case 1:
				DrawRectangle(i * CELL_SIZE, j * CELL_SIZE, CELL_SIZE, CELL_SIZE, BOARD_GREEN);
				drawWhite = 0;
				break;
			}
		}
	}

	auto loadTexture = [](const char *path) ->Texture2D {
		Image image = LoadImage(path);
		ImageResize(&image, CELL_SIZE, CELL_SIZE);
		Texture2D texture = LoadTextureFromImage(image);
		UnloadImage(image);
		return texture;
	};

	// White Figure Texture
	m_loadedTexture._White.insert({ "Pawn", loadTexture("Assets/White/Pawn.png" )});
	m_loadedTexture._White.insert({ "Horse", loadTexture("Assets/White/Horse.png") });
	m_loadedTexture._White.insert({ "Bishop", loadTexture("Assets/White/Bishop.png") });
	m_loadedTexture._White.insert({ "Rock", loadTexture("Assets/White/Rock.png") });
	m_loadedTexture._White.insert({ "Queen", loadTexture("Assets/White/Queen.png") });
	m_loadedTexture._White.insert({ "King", loadTexture("Assets/White/King.png") });

	// Black Figure Texture
	m_loadedTexture._Black.insert({ "Pawn", loadTexture("Assets/Black/Pawn.png") });
	m_loadedTexture._Black.insert({ "Horse", loadTexture("Assets/Black/Horse.png") });
	m_loadedTexture._Black.insert({ "Bishop", loadTexture("Assets/Black/Bishop.png") });
	m_loadedTexture._Black.insert({ "Rock", loadTexture("Assets/Black/Rock.png") });
	m_loadedTexture._Black.insert({ "Queen", loadTexture("Assets/Black/Queen.png") });
	m_loadedTexture._Black.insert({ "King", loadTexture("Assets/Black/King.png") });

	createFigureForBoard();

	int i{ 0 };
	for (_FigVec &figureVec : m_field) {
		int j{ 0 };
		for (Figure &figure : figureVec) {
			if (figure.getType() != EMPTY) {
				DrawTexture(figure.getTexture(), i * CELL_SIZE, j * CELL_SIZE, WHITE);
			}
			++j;
		}
		++i;
	}
}

Chess::~Chess() {
	UnloadTexture(m_loadedTexture._White["Pawn"]);
	UnloadTexture(m_loadedTexture._White["Horse"]);
	UnloadTexture(m_loadedTexture._White["Bishop"]);
	UnloadTexture(m_loadedTexture._White["Rock"]);
	UnloadTexture(m_loadedTexture._White["Queen"]);
	UnloadTexture(m_loadedTexture._White["King"]);

	UnloadTexture(m_loadedTexture._Black["Pawn"]);
	UnloadTexture(m_loadedTexture._Black["Horse"]);
	UnloadTexture(m_loadedTexture._Black["Bishop"]);
	UnloadTexture(m_loadedTexture._Black["Rock"]);
	UnloadTexture(m_loadedTexture._Black["Queen"]);
	UnloadTexture(m_loadedTexture._Black["King"]);
}