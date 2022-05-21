
#pragma warning( disable : 4996 ) 


#include <cstdlib>
#include <vector>
#include <iostream>
#include <string>
#include "G2D.h"
#include <ctime>


using namespace std;

struct Bumper
{
	V2 Position = V2(0, 0);
	float rayon;
	float T0;
	float RayonRouge;
	bool colision = false;


	Bumper(V2 pos, float ray)
	{
		Position = pos;
		rayon = ray;
	}

	void disparait()
	{
		float dt = G2D::ElapsedTimeFromStartSeconds() - T0;
		if (0 <= dt * 1000 <= 2 && RayonRouge > 40)
		{
			RayonRouge = 60 - (60 - 40) * dt * 1000 / 2;
		}
		else {
			colision = false;
			RayonRouge = 40;
		}
	}

};

//fonction qui détermine si deux points M et P sont du même coté d’une droite AB
bool SameSide(V2 A, V2 B, V2 M, V2 P)
{
	V2 AB = B - A;
	V2 AM = M - A;
	V2 AP = P - A;
	float r1 = AB ^ AM;
	float r2 = AB ^ AP;
	if (r1 >= 0 && r2 >= 0)
	{
		return true;   // retourne vrai si les signes de r1 et r2 sont +
	}
	if (r1 <= 0 && r2 <= 0)
	{
		return true;   // retourne vrai si les signes de r1 et r2 sont -
	}
	return false;
}

//retourne l'inverse de SameSide
bool OppositeSide(V2 A, V2 B, V2 M, V2 P)
{
	return !SameSide(A, B, M, P);
}

//fonction qui deterùine si deux segement sont en intersection
bool InterSegmentSegment(V2 E, V2 F, V2 A, V2 B)
{
	return OppositeSide(E, F, A, B) && OppositeSide(A, B, E, F);
}



struct Cible
{
	V2 sommetA = V2(0, 0);
	V2 sommetB = V2(0, 0);
	bool active;
	Color couleur = Color::Red;

	Cible(V2 sa, V2 sb, bool a)
	{
		sommetA = sa;
		sommetB = sb;
		active = a;
	}

	//allonge le sommet A d'un segment
	V2 AllongeSommetA(float r)
	{
		V2 T = sommetB - sommetA;
		T.normalize();
		V2 As = sommetA - r * T;
		return As;
	}

	//Allonge le sommet B d'un segment
	V2 AllongeSommetB(float r)
	{
		V2 T = sommetB - sommetA;
		T.normalize();
		V2 Bs = sommetB + r * T;
		return Bs;
	}


};


// information de partie
struct GameData
{
	int idFrame = 0;
	int HeighPix = 800;
	int WidthPix = 600;
	V2 BallPos = V2(550, 30);
	V2 BallSpeed = V2(200, 400);
	int score = 0;

	V2 LP[11] = { V2(600, 550), V2(585, 596), V2(542, 638), V2(476, 671), V2(392, 692), V2(300, 700), V2(207, 692),
		V2(123, 671), V2(57, 638), V2(14, 596), V2(0, 550) };

	Bumper B1 = Bumper(V2(200, 400), 40);
	Bumper B2 = Bumper(V2(400, 400), 40);
	Bumper B3 = Bumper(V2(300, 550), 40);

	Cible C1 = Cible(V2(50, 50), V2(150, 150), true);
	Cible C2 = Cible(V2(50, 250), V2(150, 350), true);
	Cible C3 = Cible(V2(50, 450), V2(150, 550), true);

	Cible C4 = Cible(V2(500, 150), V2(600, 50), true);
	Cible C5 = Cible(V2(500, 350), V2(600, 250), true);
	Cible C6 = Cible(V2(500, 550), V2(600, 450), true);
};

GameData G;


V2 Rebond(V2 v, V2 n)
{
	n.normalize();
	V2 t = V2(n.y, -n.x);
	return (v * t) * t - (v * n) * n;
}

//renvoie un booléen qui dit si la bille est en collision avec le côté droit de la cible
bool CollisionDroiteCible(V2 BallPos, V2 CurrentPos, Cible C)
{
	bool a = C.active;
	bool b = InterSegmentSegment(BallPos, CurrentPos, C.AllongeSommetA(15) + V2(15, 0), C.AllongeSommetB(15) + V2(15, 0));

	return (a && b);
}

//renvoie un booléen qui dit si la bille est en collision avec le côté gauche de la cible
bool CollisionGaucheCible(V2 BallPos, V2 CurrentPos, Cible C)
{
	bool a = C.active;
	bool c = InterSegmentSegment(BallPos, CurrentPos, C.AllongeSommetA(15) - V2(44, 0), C.AllongeSommetB(15) - V2(44, 0));

	return (a && c);
}

//renvoie un booléen qui dit si la bille est en collision avec le côté supérieur de la cible
bool CollisionSupCible(V2 BallPos, V2 CurrentPos, Cible C)
{
	bool a = C.active;
	bool d = InterSegmentSegment(BallPos, CurrentPos, C.AllongeSommetA(15) + V2(15, 0), C.AllongeSommetA(15) - V2(44, 0));

	return (a && d);
}

//renvoie un booléen qui dit si la bille est en collision avec le côté inférieur de la cible
bool CollisionInfCible(V2 BallPos, V2 CurrentPos, Cible C)
{
	bool a = C.active;
	bool e = InterSegmentSegment(BallPos, CurrentPos, C.AllongeSommetB(15) + V2(15, 0), C.AllongeSommetB(15) - V2(44, 0));

	return (a && e);
}

//détecte les différentes collision qu'il peut y avoir entre la bille et la cible
void CollisionCible(V2 & BallSpeed, V2 &BallPos, V2 &CurrentPos, Cible &C)
{
	if (CollisionDroiteCible(BallPos, CurrentPos, C))
	{
		C.active = false;
		C.couleur = Color::Green;
		BallPos = CurrentPos;
		BallSpeed = Rebond(BallSpeed, V2((C.sommetA - C.sommetB).x, -(C.sommetA - C.sommetB).y));
		G.score += 500;
	}

	else if (CollisionGaucheCible(BallPos, CurrentPos, C))
	{
		C.active = false;
		C.couleur = Color::Green;
		BallPos = CurrentPos;
		BallSpeed = Rebond(BallSpeed, V2(-(C.sommetA - C.sommetB).x, (C.sommetA - C.sommetB).y));
		G.score += 500;
	}

	else if (CollisionSupCible(BallPos, CurrentPos, C))
	{
		C.active = false;
		C.couleur = Color::Green;
		BallPos = CurrentPos;
		BallSpeed = Rebond(BallSpeed, V2(0, 1));
		G.score += 500;
	}

	else if (CollisionInfCible(BallPos, CurrentPos, C))
	{
		C.active = false;
		C.couleur = Color::Green;
		BallPos = CurrentPos;
		BallSpeed = Rebond(BallSpeed, V2(0, -1));
		G.score += 500;
	}
}

void render()
{


	G2D::ClearScreen(Color::Black);

	G2D::DrawStringFontMono(V2(80, G.HeighPix - 50), string("Super Flipper"), 50, 5, Color::Blue);

	G2D::DrawRectangle(V2(0, 0), V2(G.WidthPix, G.HeighPix), Color::Green);

	for (int i = 0; i < 30; i++)
	{
		G2D::DrawLine(V2(G.C1.sommetA.x - i, G.C1.sommetA.y), V2(G.C1.sommetB.x - i, G.C1.sommetB.y), G.C1.couleur);
		G2D::DrawLine(V2(G.C2.sommetA.x - i, G.C2.sommetA.y), V2(G.C2.sommetB.x - i, G.C2.sommetB.y), G.C2.couleur);
		G2D::DrawLine(V2(G.C3.sommetA.x - i, G.C3.sommetA.y), V2(G.C3.sommetB.x - i, G.C3.sommetB.y), G.C3.couleur);
		G2D::DrawLine(V2(G.C4.sommetA.x - i, G.C4.sommetA.y), V2(G.C4.sommetB.x - i, G.C4.sommetB.y), G.C4.couleur);
		G2D::DrawLine(V2(G.C5.sommetA.x - i, G.C5.sommetA.y), V2(G.C5.sommetB.x - i, G.C5.sommetB.y), G.C5.couleur);
		G2D::DrawLine(V2(G.C6.sommetA.x - i, G.C6.sommetA.y), V2(G.C6.sommetB.x - i, G.C6.sommetB.y), G.C6.couleur);
	}

	G2D::DrawCircle(G.BallPos, 15, Color::Red, true);

	G.B1.disparait();
	G.B2.disparait();
	G.B3.disparait();

	G2D::DrawCircle(V2(200, 400), G.B1.RayonRouge, Color::Red, G.B1.colision);
	G2D::DrawCircle(V2(400, 400), G.B2.RayonRouge, Color::Red, G.B2.colision);
	G2D::DrawCircle(V2(300, 550), G.B3.RayonRouge, Color::Red, G.B3.colision);

	G2D::DrawCircle(V2(200, 400), 40, Color::Blue, true);
	G2D::DrawCircle(V2(400, 400), 40, Color::Blue, true);
	G2D::DrawCircle(V2(300, 550), 40, Color::Blue, true);



	for (int i = 0; i < 10; i++)
		G2D::DrawLine(G.LP[i], G.LP[i + 1], Color::Green);

	G2D::DrawStringFontMono(V2(10, 710), string("score: " + to_string(G.score) + " pts"), 30, 2, Color::White);

	G2D::Show();
}




void Logic()
{

	G.idFrame += 1;

	float x = G.BallPos.x;
	float y = G.BallPos.y;

	V2 CurrentPos = G.BallPos;

	double dt = G2D::ElapsedTimeFromLastCallbackSeconds();

	G.BallPos.y += G.BallSpeed.y * dt;
	G.BallPos.x += G.BallSpeed.x * dt;

	if (G.BallPos.y > G.HeighPix - 15)
	{
		G.BallPos.x = x;
		G.BallPos.y = y;
		G.BallSpeed = Rebond(G.BallSpeed, V2(0, -1));
	}
	else if (G.BallPos.y < 15)
	{
		G.BallPos.x = x;
		G.BallPos.y = y;
		G.BallSpeed = Rebond(G.BallSpeed, V2(0, 1));
	}
	else if (G.BallPos.x < 15)
	{
		G.BallPos.x = x;
		G.BallPos.y = y;
		G.BallSpeed = Rebond(G.BallSpeed, V2(1, 0));
	}
	else if (G.BallPos.x > G.WidthPix - 15)
	{
		G.BallPos.x = x;
		G.BallPos.y = y;
		G.BallSpeed = Rebond(G.BallSpeed, V2(-1, 0));
	}
	else if ((G.BallPos - G.B1.Position).norm() < 15 + G.B1.rayon)
	{
		G.BallPos.x = x;
		G.BallPos.y = y;
		G.BallSpeed = Rebond(G.BallSpeed, G.B1.Position - G.BallPos);
		G.B1.RayonRouge = 60;
		G.B1.colision = true;
		G.B1.T0 = G2D::ElapsedTimeFromStartSeconds();
		G.score += 100;

	}
	else if ((G.BallPos - G.B2.Position).norm() < 15 + G.B2.rayon)
	{
		G.BallPos.x = x;
		G.BallPos.y = y;
		G.BallSpeed = Rebond(G.BallSpeed, G.B2.Position - G.BallPos);
		G.B2.RayonRouge = 60;
		G.B2.colision = true;
		G.B2.T0 = G2D::ElapsedTimeFromStartSeconds();
		G.score += 100;

	}
	else if ((G.BallPos - G.B3.Position).norm() < 15 + G.B3.rayon)
	{
		G.BallPos.x = x;
		G.BallPos.y = y;
		G.BallSpeed = Rebond(G.BallSpeed, G.B3.Position - G.BallPos);
		G.B3.RayonRouge = 60;
		G.B3.colision = true;
		G.B3.T0 = G2D::ElapsedTimeFromStartSeconds();
		G.score += 100;
	}

	CollisionCible(G.BallSpeed, G.BallPos, CurrentPos, G.C1);
	CollisionCible(G.BallSpeed, G.BallPos, CurrentPos, G.C2);
	CollisionCible(G.BallSpeed, G.BallPos, CurrentPos, G.C3);
	CollisionCible(G.BallSpeed, G.BallPos, CurrentPos, G.C4);
	CollisionCible(G.BallSpeed, G.BallPos, CurrentPos, G.C5);
	CollisionCible(G.BallSpeed, G.BallPos, CurrentPos, G.C6);

}


int main(int argc, char* argv[])
{
	//V2 A(5, 10);
	//V2 B = V2(6, 15);
	//V2 AB = B - A;
	//cout << AB << endl;

	//V2 u(5, 7);
	//V2 v(2, 3);
	//cout << "Produit scalaire  : " << (u * v) << endl;
	//cout << "Produit vectoriel : " << (u ^ v) << endl;

	//cout << Rebond(V2(1, 0), V2(-1, 1)) << endl;
	//cout << Rebond(V2(1, -1), V2(-1, 1)) << endl;
	//cout << Rebond(V2(-1, 1), V2(-1, 1)) << endl;
	//cout << Rebond(V2(0, 1), V2(-1, 1)) << endl;

	//cout << Rebond(V2(1, -1), V2(0, 1)) << endl;
	//cout << Rebond(V2(0, -1), V2(0, 1)) << endl;
	//cout << Rebond(V2(-1, 1), V2(0, 1)) << endl;
	//cout << Rebond(V2(-1, -1), V2(0, 1)) << endl;

	G2D::InitWindow(argc, argv, V2(G.WidthPix, G.HeighPix), V2(200, 200), string("Super Flipper 600 !!"));

	G2D::Run(Logic, render);
}


