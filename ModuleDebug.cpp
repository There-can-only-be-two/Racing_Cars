#include "Application.h"
#include "ModuleDebug.h"
#include "ModuleWindow.h"
#include "ModuleRenderer2D.h"
#include "ModuleTextures.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleFonts.h"
#include "ModuleSceneIntro.h"
//#include "ModuleFadeToBlack.h"

#include <string>
//#include <chrono>

using namespace std::chrono;


ModuleDebug::ModuleDebug(Application* app, bool start_enabled) : Module(app, start_enabled) { drawDebug = false; }
ModuleDebug::~ModuleDebug() {}

bool ModuleDebug::Start()
{
	drawDebug = true;
	drawPhysics = true;
	variables = true;
	hideUI = false;

	return true;
}

update_status ModuleDebug::Update(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_TAB) == KEY_DOWN)
		drawDebug = !drawDebug;


	//if (drawDebug)
	//{
	//	// F1: View colliders / logic
	//	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	//		drawPhysics = !drawPhysics;

	//	//F2: Lights ON/OFF
	//	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	//		fpsCap = !fpsCap;

	//	//FPS
	//	if (App->input->GetKey(SDL_SCANCODE_U) == KEY_DOWN && desiredFPS < 120)
	//		desiredFPS += 5;
	//	else if (App->input->GetKey(SDL_SCANCODE_I) == KEY_DOWN && desiredFPS > 10)
	//		desiredFPS -= 5;

	//	//Gravity
	//	if (App->input->GetKey(SDL_SCANCODE_J) == KEY_REPEAT && App->physics->GetGravity() > -100)
	//		App->physics->SetGravity(App->physics->GetGravity() - 1);
	//	else if (App->input->GetKey(SDL_SCANCODE_K) == KEY_REPEAT && App->physics->GetGravity() < 100)
	//		App->physics->SetGravity(App->physics->GetGravity() + 1);

	//	//Wind
	//	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_REPEAT && App->scene_intro->atm->windx > -20)
	//		App->scene_intro->atm->windx -= 1;
	//	else if (App->input->GetKey(SDL_SCANCODE_T) == KEY_REPEAT && App->scene_intro->atm->windx < 20)
	//		App->scene_intro->atm->windx += 1;

	//	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_REPEAT && App->scene_intro->atm->windy > -20)
	//		App->scene_intro->atm->windy -= 1;
	//	else if (App->input->GetKey(SDL_SCANCODE_4) == KEY_REPEAT && App->scene_intro->atm->windy < 20)
	//		App->scene_intro->atm->windy += 1;

	//	//Switch between grenade or player
	//	if (App->input->GetKey(SDL_SCANCODE_B) == KEY_DOWN)
	//		isGrenade = !isGrenade;

	//	if (!isGrenade)
	//	{
	//		//Bounce Coef
	//		if (App->input->GetKey(SDL_SCANCODE_N) == KEY_REPEAT && App->player->coef_rest_player > 0)
	//			App->player->coef_rest_player -= 0.1;
	//		else if (App->input->GetKey(SDL_SCANCODE_M) == KEY_REPEAT && App->player->coef_rest_player < 1)
	//			App->player->coef_rest_player += 0.1;

	//		//Aerodynamic Drag coefficient
	//		if (App->input->GetKey(SDL_SCANCODE_Z) == KEY_REPEAT && App->player->cd_player > 0)
	//			App->player->cd_player -= 0.1;
	//		else if (App->input->GetKey(SDL_SCANCODE_X) == KEY_REPEAT && App->player->cd_player < 1)
	//			App->player->cd_player += 0.1;

	//		//Hydrodynamic Drag coefficient
	//		if (App->input->GetKey(SDL_SCANCODE_G) == KEY_REPEAT && App->player->b_player > -10)
	//			App->player->b_player -= 0.1;
	//		else if (App->input->GetKey(SDL_SCANCODE_H) == KEY_REPEAT && App->player->b_player < 20)
	//			App->player->b_player += 0.1;
	//	}
	//	else if (isGrenade)
	//	{
	//		//Bounce Coef
	//		if (App->input->GetKey(SDL_SCANCODE_N) == KEY_REPEAT && App->player->coef_rest_grenade > 0)
	//			App->player->coef_rest_grenade -= 0.1;
	//		else if (App->input->GetKey(SDL_SCANCODE_M) == KEY_REPEAT && App->player->coef_rest_grenade < 1)
	//			App->player->coef_rest_grenade += 0.1;

	//		//Aerodynamic Drag coefficient
	//		if (App->input->GetKey(SDL_SCANCODE_Z) == KEY_REPEAT && App->player->cd_grenade > 0)
	//			App->player->cd_grenade -= 0.1;
	//		else if (App->input->GetKey(SDL_SCANCODE_X) == KEY_REPEAT && App->player->cd_grenade < 1)
	//			App->player->cd_grenade += 0.1;

	//		//Hydrodynamic Drag coefficient
	//		if (App->input->GetKey(SDL_SCANCODE_G) == KEY_REPEAT && App->player->b_grenade > -10)
	//			App->player->b_grenade -= 0.1;
	//		else if (App->input->GetKey(SDL_SCANCODE_H) == KEY_REPEAT && App->player->b_grenade < 20)
	//			App->player->b_grenade += 0.1;
	//	}

	//	//ACTIVATE / DISACTIVATE FORCES

	//		//Gravity
	//	if (App->input->GetKey(SDL_SCANCODE_5) == KEY_DOWN)
	//	{
	//		noGravity = !noGravity;
	//		if (noGravity)
	//		{
	//			saveGravity = App->physics->GetGravity();
	//			App->physics->SetGravity(0);
	//		}
	//		else
	//		{
	//			App->physics->SetGravity(saveGravity);
	//		}
	//	}



	//	//P: Change Player
	//	if (App->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN)
	//	{
	//		if (App->scene_intro->turn == PLAYER_1)
	//			App->scene_intro->turn = PLAYER_2;
	//		else
	//			App->scene_intro->turn = PLAYER_1;
	//	}


	//	//U: HIDE UI
	//	if (App->input->GetKey(SDL_SCANCODE_U) == KEY_DOWN)
	//		hideUI = !hideUI;


	//	//F4: Sfx ON/OFF
	//	if (App->input->GetKey(SDL_SCANCODE_O) == KEY_DOWN)
	//		sfxON = !sfxON;

	//	if (sfxON)
	//		Mix_Volume(-1, 32);
	//	else
	//		Mix_Volume(-1, 0);

	//	//Variables
	//	if (App->input->GetKey(SDL_SCANCODE_V) == KEY_DOWN)
	//		variables = !variables;
	//}


	return UPDATE_CONTINUE;
}

update_status ModuleDebug::PostUpdate()
{
	if (drawDebug)
		DrawDebug();

	return UPDATE_CONTINUE;
}

void ModuleDebug::DrawDebug()
{
	//int fontId = 0;
	//int debugX = 50;
	//int debugY = 50;

	//App->fonts->BlitText(debugX, debugY + 0, fontId, "#DEBUG MODE (TAB)  ON/OFF");

	////Colliders
	//if (drawPhysics)
	//	App->fonts->BlitText(debugX, debugY + 20, fontId, "#COLLIDERS  (F1)   ON");
	//else
	//	App->fonts->BlitText(debugX, debugY + 20, fontId, "#COLLIDERS  (F1)   OFF");

	////FPS Cap
	//if (fpsCap)
	//	App->fonts->BlitText(debugX, debugY + 40, fontId, "#FRAME CAP  (F2)   ON");
	//else
	//	App->fonts->BlitText(debugX, debugY + 40, fontId, "#FRAME CAP  (F2)   OFF");

	//std::string string = std::string("MAX FPS   (U-/I+)  ") + std::to_string(desiredFPS);
	//App->fonts->BlitText(debugX + 16, debugY + 60, fontId, string.c_str());


	////FORCES
	//string = std::string("#FORCES");
	//App->fonts->BlitText(debugX, debugY + 100, fontId, string.c_str());

	////Gravity
	//string = std::string("GRAVITY.Y  (J-/K+)  ") + std::to_string(App->physics->GetGravity());
	//App->fonts->BlitText(debugX + 16, debugY + 120, fontId, string.c_str());

	////Wind
	//string = std::string("WIND X  (R-/T+)  ") + std::to_string(App->scene_intro->atm->windx);
	//App->fonts->BlitText(debugX + 16, debugY + 140, fontId, string.c_str());

	//string = std::string("WIND Y  (3-/4+)  ") + std::to_string(App->scene_intro->atm->windy);
	//App->fonts->BlitText(debugX + 16, debugY + 160, fontId, string.c_str());


	//////Bounce coef
	//float bouncePrint, cdPrint, bPrint;

	//if (isGrenade)
	//{
	//	bouncePrint = App->player->coef_rest_grenade;
	//	cdPrint = App->player->cd_grenade;
	//	bPrint = App->player->b_grenade;
	//	App->fonts->BlitText(debugX, debugY + 200, fontId, "#SWITCH ENTITY (B) = GRENADE");
	//}
	//else if (!isGrenade)
	//{
	//	bouncePrint = App->player->coef_rest_player;
	//	cdPrint = App->player->cd_player;
	//	bPrint = App->player->b_player;
	//	App->fonts->BlitText(debugX, debugY + 200, fontId, "#SWITCH ENTITY (B) = PLAYER");
	//}

	//string = std::string("BOUNCE COEF     (N-/M+)  ") + std::to_string(bouncePrint);
	//App->fonts->BlitText(debugX + 16, debugY + 220, fontId, string.c_str());

	//string = std::string("AERO DRAG COEF  (Z-/X+)  ") + std::to_string(cdPrint);
	//App->fonts->BlitText(debugX + 16, debugY + 240, fontId, string.c_str());

	//string = std::string("HYDRO DRAG COEF (G-/H+)  ") + std::to_string(bPrint);
	//App->fonts->BlitText(debugX + 16, debugY + 260, fontId, string.c_str());



	////Change player
	//std::string turn;
	//if (App->scene_intro->turn == PLAYER_1)
	//	turn = "PLAYER 1";
	//else
	//	turn = "PLAYER 2";

	//string = std::string("#TURN       (P)    ") + turn;
	//App->fonts->BlitText(debugX, debugY + 300, fontId, string.c_str());



	////Hide UI
	//if (hideUI)
	//	App->fonts->BlitText(debugX, debugY + 320, fontId, "#HIDE UI    (U)    ON");
	//else
	//	App->fonts->BlitText(debugX, debugY + 320, fontId, "#HIDE UI    (U)    OFF");

	////Mute SFX
	//if (sfxON)
	//	App->fonts->BlitText(debugX, debugY + 340, fontId, "#SFX        (M)    ON");
	//else
	//	App->fonts->BlitText(debugX, debugY + 340, fontId, "#SFX        (M)    OFF");


	////DEBUG ANGLE
	//string = std::string("ANGLE:  ") + std::to_string(App->physics->angle);
	//App->fonts->BlitText(debugX, debugY + 380, fontId, string.c_str());




	////Variables
	//if (variables)
	//	App->fonts->BlitText(debugX, debugY + 400, fontId, "#VARIABLES  (V)    ON");
	//else
	//	App->fonts->BlitText(debugX, debugY + 400, fontId, "#VARIABLES  (V)    OFF");


	//if (variables)
	//{
	//	//Last pBody on List
	//	p2List_item<Circle*>* item;
	//	Circle* pBody = NULL;

	//	item = App->physics->listBodies.getLast();

	//	if (item != NULL)
	//	{
	//		pBody = item->data;

	//		//Proyectiles
	//		std::string string = std::string("PBODY.AX = ") + std::to_string(pBody->ax);
	//		App->fonts->BlitText(debugX + 16, debugY + 420, fontId, string.c_str());

	//		string = std::string("PBODY.AY = ") + std::to_string(pBody->ay);
	//		App->fonts->BlitText(debugX + 16, debugY + 440, fontId, string.c_str());

	//		string = std::string("PBODY.VX = ") + std::to_string(pBody->vx);
	//		App->fonts->BlitText(debugX + 16, debugY + 460, fontId, string.c_str());

	//		string = std::string("PBODY.VY = ") + std::to_string(pBody->vy);
	//		App->fonts->BlitText(debugX + 16, debugY + 480, fontId, string.c_str());

	//		string = std::string("PBODY.PX = ") + std::to_string(pBody->px);
	//		App->fonts->BlitText(debugX + 16, debugY + 500, fontId, string.c_str());

	//		string = std::string("PBODY.PY = ") + std::to_string(pBody->py);
	//		App->fonts->BlitText(debugX + 16, debugY + 520, fontId, string.c_str());


	//		//Players
	//		p2List_item<Circle*>* item;
	//		Circle* pBody = NULL;

	//		for (item = App->player->listPlayers.getFirst(); item != NULL; item = item->next)
	//		{
	//			pBody = item->data;

	//			if (pBody->label == PLAYER_1)
	//			{
	//				string = std::string("PLAYER 1.PX = ") + std::to_string(pBody->px);
	//				App->fonts->BlitText(debugX + 16, debugY + 560, fontId, string.c_str());

	//				string = std::string("PLAYER 1.PY = ") + std::to_string(pBody->py);
	//				App->fonts->BlitText(debugX + 16, debugY + 580, fontId, string.c_str());
	//			}
	//			else if (pBody->label == PLAYER_2)
	//			{
	//				string = std::string("PLAYER 2.PX = ") + std::to_string(pBody->px);
	//				App->fonts->BlitText(debugX + 16, debugY + 600, fontId, string.c_str());

	//				string = std::string("PLAYER 2.PY = ") + std::to_string(pBody->py);
	//				App->fonts->BlitText(debugX + 16, debugY + 620, fontId, string.c_str());

	//			}

	//		}


	//		//Grenade cooldown
	//		string = std::string("GRENADECOOLDOWN = ") + std::to_string(App->player->explosionTimer);
	//		App->fonts->BlitText(debugX + 16, debugY + 660, fontId, string.c_str());

	//		if (App->player->isShootingGrenade == false && App->player->isShootingMissile == false) {
	//			string = std::string("SHOOTING = AVAILABLE");
	//			App->fonts->BlitText(debugX + 16, debugY + 680, fontId, string.c_str());
	//		}
	//		else {
	//			string = std::string("SHOOTING = UNAVAILABLE");
	//			App->fonts->BlitText(debugX + 16, debugY + 680, fontId, string.c_str());
	//		}

	//		if (App->player->weaponType == 0) {
	//			string = std::string("WEAPON: GRENADE");
	//			App->fonts->BlitText(debugX + 16, debugY + 720, fontId, string.c_str());
	//		}
	//		else {
	//			string = std::string("WEAPON: MISSILE");
	//			App->fonts->BlitText(debugX + 16, debugY + 720, fontId, string.c_str());
	//		}

	//	}


		//Spring force
		/*if (App->scene_intro->springForce == 420)
			string = std::string("SPRING.F = ") + std::to_string(App->scene_intro->springForce) + "  X)";
		else
			string = std::string("SPRING.F = ") + std::to_string(App->scene_intro->springForce);
		App->fonts->BlitText(debugX + 16, debugY + 260, fontId, string.c_str());*/
	//}
}