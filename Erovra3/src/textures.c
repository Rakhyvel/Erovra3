#pragma once
#include "./textures.h"
#include "./engine/textureManager.h"
#include "./util/polygon.h"
#include "./util/vector.h"

/*
	Initializes TextureID's used in game, draws polygons onto some of them */
void Textures_Init()
{
    CITY_TEXTURE_ID = Texture_Load("res/city.png");
    CITY_OUTLINE_TEXTURE_ID = Texture_Load("res/city_outline.png");
    CITY_SHADOW_TEXTURE_ID = Texture_Load("res/city_shadow.png");
    CAPITAL_TEXTURE_ID = Texture_Load("res/capital.png");
    CAPITAL_OUTLINE_TEXTURE_ID = Texture_Load("res/capital_outline.png");
    CAPITAL_SHADOW_TEXTURE_ID = Texture_Load("res/capital_shadow.png");
    MINE_TEXTURE_ID = Texture_Load("res/mine.png");
    MINE_OUTLINE_TEXTURE_ID = Texture_Load("res/mine_outline.png");
    MINE_SHADOW_TEXTURE_ID = Texture_Load("res/mine_shadow.png");
    FACTORY_TEXTURE_ID = Texture_Load("res/factory.png");
    FACTORY_OUTLINE_TEXTURE_ID = Texture_Load("res/factory_outline.png");
    FACTORY_SHADOW_TEXTURE_ID = Texture_Load("res/factory_shadow.png");
    PORT_TEXTURE_ID = Texture_Load("res/port.png");
    PORT_OUTLINE_TEXTURE_ID = Texture_Load("res/port_outline.png");
    PORT_SHADOW_TEXTURE_ID = Texture_Load("res/port_shadow.png");
    AIRFIELD_TEXTURE_ID = Texture_Load("res/airfield.png");
    AIRFIELD_OUTLINE_TEXTURE_ID = Texture_Load("res/airfield_outline.png");
    AIRFIELD_SHADOW_TEXTURE_ID = Texture_Load("res/airfield_shadow.png");
    FARM_TEXTURE_ID = Texture_Load("res/farm.png");
    FARM_OUTLINE_TEXTURE_ID = Texture_Load("res/farm_outline.png");
    FARM_SHADOW_TEXTURE_ID = Texture_Load("res/farm_shadow.png");
    ACADEMY_TEXTURE_ID = Texture_Load("res/academy.png");
    ACADEMY_OUTLINE_TEXTURE_ID = Texture_Load("res/academy_outline.png");
    ACADEMY_SHADOW_TEXTURE_ID = Texture_Load("res/academy_shadow.png");
    WALL_TEXTURE_ID = Texture_Load("res/wall.png");
    WALL_OUTLINE_TEXTURE_ID = Texture_Load("res/wall_outline.png");
    WALL_SHADOW_TEXTURE_ID = Texture_Load("res/wall_shadow.png");
    INFANTRY_TEXTURE_ID = Texture_Load("res/infantry.png");
    CAVALRY_TEXTURE_ID = Texture_Load("res/cavalry.png");
    ARTILLERY_TEXTURE_ID = Texture_Load("res/artillery.png");
    ENGINEER_TEXTURE_ID = Texture_Load("res/engineer.png");
    GROUND_OUTLINE_TEXTURE_ID = Texture_Load("res/ground_outline.png");
    GROUND_SHADOW_TEXTURE_ID = Texture_Load("res/ground_shadow.png");
    DESTROYER_TEXTURE_ID = Texture_Load("res/destroyer.png");
    CRUISER_TEXTURE_ID = Texture_Load("res/cruiser.png");
    BATTLESHIP_TEXTURE_ID = Texture_Load("res/battleship.png");
    SHIP_SHADOW_TEXTURE_ID = Texture_Load("res/ship_shadow.png");
    SHIP_OUTLINE_TEXTURE_ID = Texture_Load("res/ship_outline.png");
    FIGHTER_TEXTURE_ID = Texture_Load("res/fighter.png");
    FIGHTER_OUTLINE_TEXTURE_ID = Texture_Load("res/fighter_outline.png");
    FIGHTER_SHADOW_TEXTURE_ID = Texture_Load("res/fighter_shadow.png");
    ATTACKER_TEXTURE_ID = Texture_Load("res/attacker.png");
    ATTACKER_OUTLINE_TEXTURE_ID = Texture_Load("res/attacker_outline.png");
    ATTACKER_SHADOW_TEXTURE_ID = Texture_Load("res/attacker_shadow.png");
    BOMBER_TEXTURE_ID = Texture_Load("res/bomber.png");
    BOMBER_OUTLINE_TEXTURE_ID = Texture_Load("res/bomber_outline.png");
    BOMBER_SHADOW_TEXTURE_ID = Texture_Load("res/bomber_shadow.png");
    BULLET_TEXTURE_ID = Texture_Load("res/bullet.png");
    BULLET_SHADOW_TEXTURE_ID = Texture_Load("res/bullet_shadow.png");
    SHELL_TEXTURE_ID = Texture_Load("res/shell.png");
    SHELL_SHADOW_TEXTURE_ID = Texture_Load("res/shell_shadow.png");
    BOMB_TEXTURE_ID = Texture_Load("res/bomb.png");
    BOMB_SHADOW_TEXTURE_ID = Texture_Load("res/bomb_shadow.png");
    COIN_TEXTURE_ID = Texture_Load("res/coin.png");
    COIN_SHADOW_TEXTURE_ID = Texture_Load("res/coin_shadow.png");
    ORE_TEXTURE_ID = Texture_Load("res/ore.png");
    ORE_SHADOW_TEXTURE_ID = Texture_Load("res/ore_shadow.png");
    COAL_TEXTURE_ID = Texture_Load("res/coal.png");
    COAL_SHADOW_TEXTURE_ID = Texture_Load("res/coal_shadow.png");
    TIMBER_TEXTURE_ID = Texture_Load("res/timber.png");
    TIMBER_SHADOW_TEXTURE_ID = Texture_Load("res/timber_shadow.png");
    POPULATION_TEXTURE_ID = Texture_Load("res/population.png");
    ARROW_TEXTURE_ID = Texture_Load("res/arrow.png");
    ARROW_SHADOW_TEXTURE_ID = Texture_Load("res/arrow_shadow.png");
    TIMBER_INDICATOR_TEXTURE_ID = Texture_Load("res/timber_indicator.png");
    ORE_INDICATOR_TEXTURE_ID = Texture_Load("res/ore_indicator.png");
    COAL_INDICATOR_TEXTURE_ID = Texture_Load("res/coal_indicator.png");

    SDL_SetTextureAlphaMod(TIMBER_INDICATOR_TEXTURE_ID, 100); // So that strokes don't overlap
    SDL_SetTextureAlphaMod(ORE_INDICATOR_TEXTURE_ID, 140);
    SDL_SetTextureAlphaMod(COAL_INDICATOR_TEXTURE_ID, 140);
}

void Textures_Draw()
{
    // 30px padding should be included when creating polygonal sprites
    // Default orientation is that the bottom is the front
    Polygon groundBorderPoly = Polygon_Create("res/ground_border.gon");
    Polygon shipPoly = Polygon_Create("res/ship.gon");

    Polygon fighterWing = Polygon_Create("res/fighter_wings.gon");
    Polygon fighterBody = Polygon_Create("res/fighter_body.gon");
    Polygon fighterTail = Polygon_Create("res/fighter_tail.gon");

    Polygon attackerWing = Polygon_Create("res/attacker_wings.gon");
    Polygon attackerBody = Polygon_Create("res/attacker_body.gon");
    Polygon attackerTail = Polygon_Create("res/attacker_tail.gon");
    Polygon attackerNacelle = Polygon_Create("res/attacker_nacelle.gon");

    Polygon bomberWing = Polygon_Create("res/bomber_wings.gon");
    Polygon bomberBody = Polygon_Create("res/bomber_body.gon");
    Polygon bomberTail = Polygon_Create("res/bomber_tail.gon");
    Polygon bomberNacelle = Polygon_Create("res/bomber_nacelle.gon");
    Polygon tree = Polygon_Create("res/tree.gon");
    Polygon rock0 = Polygon_Create("res/rock0.gon");
    Polygon rock0_highlight = Polygon_Create("res/rock0_highlight.gon");

    BULLET_TEXTURE_ID = Texture_Load("res/bullet.png");
    BULLET_SHADOW_TEXTURE_ID = Texture_Load("res/bullet.png");
    SHELL_TEXTURE_ID = Texture_Create(40, 40);
    SHELL_SHADOW_TEXTURE_ID = Texture_Create(40, 40);
    BOMB_TEXTURE_ID = Texture_Load("res/bomb.png");
    BOMB_SHADOW_TEXTURE_ID = Texture_Load("res/bomb.png");
    COIN_TEXTURE_ID = Texture_Load("res/coin.png");
    COIN_SHADOW_TEXTURE_ID = Texture_Load("res/coin.png");
    ORE_TEXTURE_ID = Texture_Load("res/ore.png");
    ORE_SHADOW_TEXTURE_ID = Texture_Load("res/ore.png");
    TIMBER_TEXTURE_ID = Texture_Load("res/timber.png");
    TIMBER_SHADOW_TEXTURE_ID = Texture_Load("res/timber.png");
    COAL_TEXTURE_ID = Texture_Load("res/coal.png");
    COAL_SHADOW_TEXTURE_ID = Texture_Load("res/coal.png");

    CITY_TEXTURE_ID = Texture_Create(320, 320);
    CITY_OUTLINE_TEXTURE_ID = Texture_Create(320, 320);
    CITY_SHADOW_TEXTURE_ID = Texture_Create(320, 320);
    CAPITAL_TEXTURE_ID = Texture_Create(320, 320);
    CAPITAL_OUTLINE_TEXTURE_ID = Texture_Create(320, 320);
    CAPITAL_SHADOW_TEXTURE_ID = Texture_Create(320, 320);
    MINE_TEXTURE_ID = Texture_Create(640, 640);
    MINE_OUTLINE_TEXTURE_ID = Texture_Create(640, 640);
    MINE_SHADOW_TEXTURE_ID = Texture_Create(640, 640);
    FACTORY_TEXTURE_ID = Texture_Create(320, 320);
    FACTORY_OUTLINE_TEXTURE_ID = Texture_Create(320, 320);
    FACTORY_SHADOW_TEXTURE_ID = Texture_Create(320, 320);
    PORT_TEXTURE_ID = Texture_Create(320, 320);
    PORT_OUTLINE_TEXTURE_ID = Texture_Create(320, 320);
    PORT_SHADOW_TEXTURE_ID = Texture_Create(320, 320);
    AIRFIELD_TEXTURE_ID = Texture_Create(320, 320);
    AIRFIELD_OUTLINE_TEXTURE_ID = Texture_Create(320, 320);
    AIRFIELD_SHADOW_TEXTURE_ID = Texture_Create(320, 320);
    FARM_TEXTURE_ID = Texture_Create(320, 320);
    FARM_OUTLINE_TEXTURE_ID = Texture_Create(320, 320);
    FARM_SHADOW_TEXTURE_ID = Texture_Create(320, 320);
    ACADEMY_TEXTURE_ID = Texture_Create(320, 320);
    ACADEMY_OUTLINE_TEXTURE_ID = Texture_Create(320, 320);
    ACADEMY_SHADOW_TEXTURE_ID = Texture_Create(320, 320);

    INFANTRY_TEXTURE_ID = Texture_Create(360, 200);
    CAVALRY_TEXTURE_ID = Texture_Create(360, 200);
    ARTILLERY_TEXTURE_ID = Texture_Create(360, 200);
    ENGINEER_TEXTURE_ID = Texture_Create(360, 200);
    GROUND_SHADOW_TEXTURE_ID = Texture_Create(360, 200);

    DESTROYER_TEXTURE_ID = Texture_Create(160, 460);
    CRUISER_TEXTURE_ID = Texture_Create(160, 460);
    BATTLESHIP_TEXTURE_ID = Texture_Create(160, 460);
    SHIP_SHADOW_TEXTURE_ID = Texture_Create(160, 460);
    SHIP_OUTLINE_TEXTURE_ID = Texture_Create(160, 460);

    FIGHTER_TEXTURE_ID = Texture_Create(420, 410);
    FIGHTER_OUTLINE_TEXTURE_ID = Texture_Create(420, 410);
    FIGHTER_SHADOW_TEXTURE_ID = Texture_Create(420, 410);

    ATTACKER_TEXTURE_ID = Texture_Create(620, 510);
    ATTACKER_OUTLINE_TEXTURE_ID = Texture_Create(620, 510);
    ATTACKER_SHADOW_TEXTURE_ID = Texture_Create(620, 510);

    BOMBER_TEXTURE_ID = Texture_Create(790, 510);
    BOMBER_OUTLINE_TEXTURE_ID = Texture_Create(790, 510);
    BOMBER_SHADOW_TEXTURE_ID = Texture_Create(790, 510);

    ARROW_TEXTURE_ID = Texture_Create(64, 64);
    ARROW_SHADOW_TEXTURE_ID = Texture_Create(64, 64);

    TIMBER_INDICATOR_TEXTURE_ID = Texture_Create(200, 500);
    ORE_INDICATOR_TEXTURE_ID = Texture_Create(250, 250);
    COAL_INDICATOR_TEXTURE_ID = Texture_Create(250, 250);

    Texture_CreateShadow(BULLET_SHADOW_TEXTURE_ID, BULLET_TEXTURE_ID);
    Texture_CreateShadow(BOMB_SHADOW_TEXTURE_ID, BOMB_TEXTURE_ID);
    Texture_FillCircle(SHELL_TEXTURE_ID, (Vector) { 20, 20 }, 15, (SDL_Color) { 255, 255, 255, 255 });
    Texture_CreateShadow(SHELL_SHADOW_TEXTURE_ID, SHELL_TEXTURE_ID);

    Texture_CreateShadow(COIN_SHADOW_TEXTURE_ID, COIN_TEXTURE_ID);
    Texture_CreateShadow(ORE_SHADOW_TEXTURE_ID, ORE_TEXTURE_ID);
    Texture_CreateShadow(TIMBER_SHADOW_TEXTURE_ID, TIMBER_TEXTURE_ID);
    Texture_CreateShadow(COAL_SHADOW_TEXTURE_ID, COAL_TEXTURE_ID);

    Texture_FillPolygon(ARROW_TEXTURE_ID, Polygon_Create("res/arrow.gon"), (SDL_Color) { 0, 79, 206, 255 });
    Texture_CreateShadow(ARROW_SHADOW_TEXTURE_ID, ARROW_TEXTURE_ID);

    Texture_FillPolygon(CITY_TEXTURE_ID, Polygon_Create("res/city.gon"), (SDL_Color) { 255, 255, 255, 255 });
    Texture_DrawPolygon(CITY_TEXTURE_ID, Polygon_Create("res/city.gon"), (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_DrawPolygon(CITY_OUTLINE_TEXTURE_ID, Polygon_Create("res/city.gon"), (SDL_Color) { 255, 255, 255, 255 }, 30);

    Texture_FillPolygon(CAPITAL_TEXTURE_ID, Polygon_Create("res/capital_pole.gon"), (SDL_Color) { 0, 0, 0, 255 });
    Texture_DrawPolygon(CAPITAL_TEXTURE_ID, Polygon_Create("res/capital_pole.gon"), (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_FillPolygon(CAPITAL_TEXTURE_ID, Polygon_Create("res/capital_flag.gon"), (SDL_Color) { 255, 255, 255, 255 });
    Texture_DrawPolygon(CAPITAL_TEXTURE_ID, Polygon_Create("res/capital_flag.gon"), (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_DrawPolygon(CAPITAL_OUTLINE_TEXTURE_ID, Polygon_Create("res/capital_pole.gon"), (SDL_Color) { 255, 255, 255, 255 }, 30);
    Texture_DrawPolygon(CAPITAL_OUTLINE_TEXTURE_ID, Polygon_Create("res/capital_flag.gon"), (SDL_Color) { 255, 255, 255, 255 }, 30);

    Texture_DrawBezier(MINE_TEXTURE_ID, Polygon_Create("res/mine.gon"), (SDL_Color) { 0, 0, 0, 255 }, 60);
    Texture_DrawBezier(MINE_TEXTURE_ID, Polygon_Create("res/mine.gon"), (SDL_Color) { 255, 255, 255, 255 }, 20);
    Texture_DrawBezier(MINE_OUTLINE_TEXTURE_ID, Polygon_Create("res/mine.gon"), (SDL_Color) { 255, 255, 255, 255 }, 100);

    Texture_FillPolygon(FACTORY_TEXTURE_ID, Polygon_Create("res/factory.gon"), (SDL_Color) { 255, 255, 255, 255 });
    Texture_DrawPolygon(FACTORY_TEXTURE_ID, Polygon_Create("res/factory.gon"), (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_DrawPolygon(FACTORY_OUTLINE_TEXTURE_ID, Polygon_Create("res/factory.gon"), (SDL_Color) { 255, 255, 255, 255 }, 30);

    Texture_DrawPolygon(PORT_TEXTURE_ID, Polygon_Create("res/port_bar.gon"), (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_DrawBezier(PORT_TEXTURE_ID, Polygon_Create("res/port_hook.gon"), (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_DrawBezier(PORT_TEXTURE_ID, Polygon_Create("res/port_hole.gon"), (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_DrawPolygon(PORT_OUTLINE_TEXTURE_ID, Polygon_Create("res/port_bar.gon"), (SDL_Color) { 255, 255, 255, 255 }, 30);
    Texture_DrawBezier(PORT_OUTLINE_TEXTURE_ID, Polygon_Create("res/port_hook.gon"), (SDL_Color) { 255, 255, 255, 255 }, 30);
    Texture_DrawBezier(PORT_OUTLINE_TEXTURE_ID, Polygon_Create("res/port_hole.gon"), (SDL_Color) { 255, 255, 255, 255 }, 30);

    Texture_DrawThickLine(AIRFIELD_TEXTURE_ID, (Vector) { 60, 60 }, (Vector) { 270, 150 }, (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_DrawThickLine(AIRFIELD_TEXTURE_ID, (Vector) { 60, 230 }, (Vector) { 140, 40 }, (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_DrawThickLine(AIRFIELD_TEXTURE_ID, (Vector) { 30, 210 }, (Vector) { 280, 110 }, (SDL_Color) { 0, 0, 0, 255 }, 15);
    Texture_DrawThickLine(AIRFIELD_OUTLINE_TEXTURE_ID, (Vector) { 60, 60 }, (Vector) { 270, 150 }, (SDL_Color) { 255, 255, 255, 255 }, 30);
    Texture_DrawThickLine(AIRFIELD_OUTLINE_TEXTURE_ID, (Vector) { 60, 230 }, (Vector) { 140, 40 }, (SDL_Color) { 255, 255, 255, 255 }, 30);
    Texture_DrawThickLine(AIRFIELD_OUTLINE_TEXTURE_ID, (Vector) { 30, 210 }, (Vector) { 280, 110 }, (SDL_Color) { 255, 255, 255, 255 }, 45);

    Texture_FillPolygon(FARM_TEXTURE_ID, Polygon_Create("res/farm_silo.gon"), (SDL_Color) { 255, 255, 255, 255 });
    Texture_DrawPolygon(FARM_TEXTURE_ID, Polygon_Create("res/farm_silo.gon"), (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_FillPolygon(FARM_TEXTURE_ID, Polygon_Create("res/farm_barn.gon"), (SDL_Color) { 255, 255, 255, 255 });
    Texture_DrawPolygon(FARM_TEXTURE_ID, Polygon_Create("res/farm_barn.gon"), (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_DrawPolygon(FARM_OUTLINE_TEXTURE_ID, Polygon_Create("res/farm_silo.gon"), (SDL_Color) { 255, 255, 255, 255 }, 30);
    Texture_DrawPolygon(FARM_OUTLINE_TEXTURE_ID, Polygon_Create("res/farm_barn.gon"), (SDL_Color) { 255, 255, 255, 255 }, 30);

    Texture_FillPolygon(ACADEMY_TEXTURE_ID, Polygon_Create("res/academy_bldg.gon"), (SDL_Color) { 255, 255, 255, 255 });
    Texture_DrawPolygon(ACADEMY_TEXTURE_ID, Polygon_Create("res/academy_bldg.gon"), (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_FillPolygon(ACADEMY_TEXTURE_ID, Polygon_Create("res/academy_tower.gon"), (SDL_Color) { 255, 255, 255, 255 });
    Texture_DrawPolygon(ACADEMY_TEXTURE_ID, Polygon_Create("res/academy_tower.gon"), (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_DrawPolygon(ACADEMY_OUTLINE_TEXTURE_ID, Polygon_Create("res/academy_bldg.gon"), (SDL_Color) { 255, 255, 255, 255 }, 30);
    Texture_DrawPolygon(ACADEMY_OUTLINE_TEXTURE_ID, Polygon_Create("res/academy_tower.gon"), (SDL_Color) { 255, 255, 255, 255 }, 30);

    Texture_CreateShadow(CAPITAL_SHADOW_TEXTURE_ID, CAPITAL_TEXTURE_ID);
    Texture_CreateShadow(CITY_SHADOW_TEXTURE_ID, CITY_TEXTURE_ID);
    Texture_CreateShadow(FACTORY_SHADOW_TEXTURE_ID, FACTORY_TEXTURE_ID);
    Texture_CreateShadow(PORT_SHADOW_TEXTURE_ID, PORT_TEXTURE_ID);
    Texture_CreateShadow(MINE_SHADOW_TEXTURE_ID, MINE_TEXTURE_ID);
    Texture_CreateShadow(ACADEMY_SHADOW_TEXTURE_ID, ACADEMY_TEXTURE_ID);
    Texture_CreateShadow(AIRFIELD_SHADOW_TEXTURE_ID, AIRFIELD_TEXTURE_ID);
    Texture_CreateShadow(FARM_SHADOW_TEXTURE_ID, FARM_TEXTURE_ID);

    Texture_FillPolygon(INFANTRY_TEXTURE_ID, groundBorderPoly, (SDL_Color) { 255, 255, 255, 255 });
    Texture_DrawPolygon(INFANTRY_TEXTURE_ID, groundBorderPoly, (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_FillPolygon(INFANTRY_TEXTURE_ID, Polygon_Create("res/infantry.gon"), (SDL_Color) { 0, 0, 0, 255 });
    Texture_FillPolygon(CAVALRY_TEXTURE_ID, groundBorderPoly, (SDL_Color) { 255, 255, 255, 255 });
    Texture_DrawPolygon(CAVALRY_TEXTURE_ID, groundBorderPoly, (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_FillPolygon(CAVALRY_TEXTURE_ID, Polygon_Create("res/cavalry.gon"), (SDL_Color) { 0, 0, 0, 255 });
    Texture_FillPolygon(ARTILLERY_TEXTURE_ID, groundBorderPoly, (SDL_Color) { 255, 255, 255, 255 });
    Texture_DrawPolygon(ARTILLERY_TEXTURE_ID, groundBorderPoly, (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_FillPolygon(ARTILLERY_TEXTURE_ID, Polygon_Create("res/artillery.gon"), (SDL_Color) { 0, 0, 0, 255 });
    Texture_FillPolygon(ENGINEER_TEXTURE_ID, groundBorderPoly, (SDL_Color) { 255, 255, 255, 255 });
    Texture_DrawPolygon(ENGINEER_TEXTURE_ID, groundBorderPoly, (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_DrawPolygon(ENGINEER_TEXTURE_ID, Polygon_Create("res/engineer.gon"), (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_CreateShadow(GROUND_SHADOW_TEXTURE_ID, INFANTRY_TEXTURE_ID);

    Texture_FillBezier(BATTLESHIP_TEXTURE_ID, shipPoly, (SDL_Color) { 255, 255, 255, 255 });
    Texture_DrawBezier(BATTLESHIP_TEXTURE_ID, shipPoly, (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_DrawThickLine(BATTLESHIP_TEXTURE_ID, (Vector) { 30, 140 }, (Vector) { 130, 240 }, (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_DrawThickLine(BATTLESHIP_TEXTURE_ID, (Vector) { 30, 220 }, (Vector) { 130, 320 }, (SDL_Color) { 0, 0, 0, 255 }, 10);

    Texture_FillBezier(CRUISER_TEXTURE_ID, shipPoly, (SDL_Color) { 255, 255, 255, 255 });
    Texture_DrawBezier(CRUISER_TEXTURE_ID, shipPoly, (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_DrawThickLine(CRUISER_TEXTURE_ID, (Vector) { 30, 180 }, (Vector) { 130, 280 }, (SDL_Color) { 0, 0, 0, 255 }, 10);

    Texture_FillBezier(DESTROYER_TEXTURE_ID, shipPoly, (SDL_Color) { 255, 255, 255, 255 });
    Texture_DrawBezier(DESTROYER_TEXTURE_ID, shipPoly, (SDL_Color) { 0, 0, 0, 255 }, 10);

    Texture_DrawBezier(SHIP_OUTLINE_TEXTURE_ID, shipPoly, (SDL_Color) { 255, 255, 255, 255 }, 30);
    Texture_CreateShadow(SHIP_SHADOW_TEXTURE_ID, DESTROYER_TEXTURE_ID);

    // Build fighter icon
    Texture_FillBezier(FIGHTER_TEXTURE_ID, fighterWing, (SDL_Color) { 255, 255, 255, 255 });
    Texture_DrawBezier(FIGHTER_TEXTURE_ID, fighterWing, (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_FillBezier(FIGHTER_TEXTURE_ID, fighterTail, (SDL_Color) { 255, 255, 255, 255 });
    Texture_DrawBezier(FIGHTER_TEXTURE_ID, fighterTail, (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_FillBezier(FIGHTER_TEXTURE_ID, fighterBody, (SDL_Color) { 255, 255, 255, 255 });
    Texture_DrawBezier(FIGHTER_TEXTURE_ID, fighterBody, (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_DrawBezier(FIGHTER_OUTLINE_TEXTURE_ID, fighterWing, (SDL_Color) { 255, 255, 255, 255 }, 30);
    Texture_DrawBezier(FIGHTER_OUTLINE_TEXTURE_ID, fighterTail, (SDL_Color) { 255, 255, 255, 255 }, 30);
    Texture_DrawBezier(FIGHTER_OUTLINE_TEXTURE_ID, fighterBody, (SDL_Color) { 255, 255, 255, 255 }, 30);
    Texture_CreateShadow(FIGHTER_SHADOW_TEXTURE_ID, FIGHTER_TEXTURE_ID);

    // Build attacker icon
    attackerNacelle.x = 230;
    attackerNacelle.y = 135;
    Texture_FillBezier(ATTACKER_TEXTURE_ID, attackerNacelle, (SDL_Color) { 255, 255, 255, 255 });
    Texture_DrawBezier(ATTACKER_TEXTURE_ID, attackerNacelle, (SDL_Color) { 0, 0, 0, 255 }, 10);
    attackerNacelle.x = 390;
    Texture_FillBezier(ATTACKER_TEXTURE_ID, attackerNacelle, (SDL_Color) { 255, 255, 255, 255 });
    Texture_DrawBezier(ATTACKER_TEXTURE_ID, attackerNacelle, (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_FillPolygon(ATTACKER_TEXTURE_ID, attackerTail, (SDL_Color) { 255, 255, 255, 255 });
    Texture_DrawPolygon(ATTACKER_TEXTURE_ID, attackerTail, (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_FillBezier(ATTACKER_TEXTURE_ID, attackerWing, (SDL_Color) { 255, 255, 255, 255 });
    Texture_DrawBezier(ATTACKER_TEXTURE_ID, attackerWing, (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_FillBezier(ATTACKER_TEXTURE_ID, attackerBody, (SDL_Color) { 255, 255, 255, 255 });
    Texture_DrawBezier(ATTACKER_TEXTURE_ID, attackerBody, (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_DrawBezier(ATTACKER_OUTLINE_TEXTURE_ID, attackerWing, (SDL_Color) { 255, 255, 255, 255 }, 30);
    Texture_DrawPolygon(ATTACKER_OUTLINE_TEXTURE_ID, attackerTail, (SDL_Color) { 255, 255, 255, 255 }, 30);
    Texture_DrawBezier(ATTACKER_OUTLINE_TEXTURE_ID, attackerBody, (SDL_Color) { 255, 255, 255, 255 }, 30);
    Texture_CreateShadow(ATTACKER_SHADOW_TEXTURE_ID, ATTACKER_TEXTURE_ID);

    // Build bomber icon
    bomberNacelle.x = 323;
    bomberNacelle.y = 137;
    Texture_FillBezier(BOMBER_TEXTURE_ID, bomberNacelle, (SDL_Color) { 255, 255, 255, 255 });
    Texture_DrawBezier(BOMBER_TEXTURE_ID, bomberNacelle, (SDL_Color) { 0, 0, 0, 255 }, 10);
    bomberNacelle.x = 227;
    bomberNacelle.y = 143;
    Texture_FillBezier(BOMBER_TEXTURE_ID, bomberNacelle, (SDL_Color) { 255, 255, 255, 255 });
    Texture_DrawBezier(BOMBER_TEXTURE_ID, bomberNacelle, (SDL_Color) { 0, 0, 0, 255 }, 10);
    bomberNacelle.x = 467;
    bomberNacelle.y = 137;
    Texture_FillBezier(BOMBER_TEXTURE_ID, bomberNacelle, (SDL_Color) { 255, 255, 255, 255 });
    Texture_DrawBezier(BOMBER_TEXTURE_ID, bomberNacelle, (SDL_Color) { 0, 0, 0, 255 }, 10);
    bomberNacelle.x = 563;
    bomberNacelle.y = 143;
    Texture_FillBezier(BOMBER_TEXTURE_ID, bomberNacelle, (SDL_Color) { 255, 255, 255, 255 });
    Texture_DrawBezier(BOMBER_TEXTURE_ID, bomberNacelle, (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_FillPolygon(BOMBER_TEXTURE_ID, bomberTail, (SDL_Color) { 255, 255, 255, 255 });
    Texture_DrawPolygon(BOMBER_TEXTURE_ID, bomberTail, (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_FillBezier(BOMBER_TEXTURE_ID, bomberWing, (SDL_Color) { 255, 255, 255, 255 });
    Texture_DrawBezier(BOMBER_TEXTURE_ID, bomberWing, (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_FillBezier(BOMBER_TEXTURE_ID, bomberBody, (SDL_Color) { 255, 255, 255, 255 });
    Texture_DrawBezier(BOMBER_TEXTURE_ID, bomberBody, (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_DrawBezier(BOMBER_OUTLINE_TEXTURE_ID, bomberWing, (SDL_Color) { 255, 255, 255, 255 }, 30);
    Texture_DrawPolygon(BOMBER_OUTLINE_TEXTURE_ID, bomberTail, (SDL_Color) { 255, 255, 255, 255 }, 30);
    Texture_DrawBezier(BOMBER_OUTLINE_TEXTURE_ID, bomberBody, (SDL_Color) { 255, 255, 255, 255 }, 30);
    Texture_CreateShadow(BOMBER_SHADOW_TEXTURE_ID, BOMBER_TEXTURE_ID);

    tree.x = 100;
    tree.y = 125;
    Texture_DrawPolygon(TIMBER_INDICATOR_TEXTURE_ID, tree, (SDL_Color) { 15, 42, 30, 255 }, 20); // Has to be opaque so that strokes don't overlap
    Texture_FillPolygon(ORE_INDICATOR_TEXTURE_ID, rock0, (SDL_Color) { 90, 90, 90, 255 });
    Texture_FillPolygon(ORE_INDICATOR_TEXTURE_ID, rock0_highlight, (SDL_Color) { 140, 140, 140, 255 });
    Texture_FillPolygon(COAL_INDICATOR_TEXTURE_ID, rock0, (SDL_Color) { 0, 0, 0, 255 });
    Texture_FillPolygon(COAL_INDICATOR_TEXTURE_ID, rock0_highlight, (SDL_Color) { 40, 40, 40, 255 });

    Texture_Save("res/bullet_shadow.png", BULLET_SHADOW_TEXTURE_ID);
    Texture_Save("res/bomb_shadow.png", BOMB_SHADOW_TEXTURE_ID);
    Texture_Save("res/shell.png", SHELL_TEXTURE_ID);
    Texture_Save("res/shell_shadow.png", SHELL_SHADOW_TEXTURE_ID);
    Texture_Save("res/coin_shadow.png", COIN_SHADOW_TEXTURE_ID);
    Texture_Save("res/ore_shadow.png", ORE_SHADOW_TEXTURE_ID);
    Texture_Save("res/coal_shadow.png", COAL_SHADOW_TEXTURE_ID);
    Texture_Save("res/timber_shadow.png", TIMBER_SHADOW_TEXTURE_ID);
    Texture_Save("res/city.png", CITY_TEXTURE_ID);
    Texture_Save("res/city_outline.png", CITY_OUTLINE_TEXTURE_ID);
    Texture_Save("res/city_shadow.png", CITY_SHADOW_TEXTURE_ID);
    Texture_Save("res/capital.png", CAPITAL_TEXTURE_ID);
    Texture_Save("res/capital_outline.png", CAPITAL_OUTLINE_TEXTURE_ID);
    Texture_Save("res/capital_shadow.png", CAPITAL_SHADOW_TEXTURE_ID);
    Texture_Save("res/mine.png", MINE_TEXTURE_ID);
    Texture_Save("res/mine_outline.png", MINE_OUTLINE_TEXTURE_ID);
    Texture_Save("res/mine_shadow.png", MINE_SHADOW_TEXTURE_ID);
    Texture_Save("res/factory.png", FACTORY_TEXTURE_ID);
    Texture_Save("res/factory_outline.png", FACTORY_OUTLINE_TEXTURE_ID);
    Texture_Save("res/factory_shadow.png", FACTORY_SHADOW_TEXTURE_ID);
    Texture_Save("res/port.png", PORT_TEXTURE_ID);
    Texture_Save("res/port_outline.png", PORT_OUTLINE_TEXTURE_ID);
    Texture_Save("res/port_shadow.png", PORT_SHADOW_TEXTURE_ID);
    Texture_Save("res/airfield.png", AIRFIELD_TEXTURE_ID);
    Texture_Save("res/airfield_outline.png", AIRFIELD_OUTLINE_TEXTURE_ID);
    Texture_Save("res/airfield_shadow.png", AIRFIELD_SHADOW_TEXTURE_ID);
    Texture_Save("res/farm.png", FARM_TEXTURE_ID);
    Texture_Save("res/farm_outline.png", FARM_OUTLINE_TEXTURE_ID);
    Texture_Save("res/farm_shadow.png", FARM_SHADOW_TEXTURE_ID);
    Texture_Save("res/academy.png", ACADEMY_TEXTURE_ID);
    Texture_Save("res/academy_outline.png", ACADEMY_OUTLINE_TEXTURE_ID);
    Texture_Save("res/academy_shadow.png", ACADEMY_SHADOW_TEXTURE_ID);

    Texture_Save("res/infantry.png", INFANTRY_TEXTURE_ID);
    Texture_Save("res/cavalry.png", CAVALRY_TEXTURE_ID);
    Texture_Save("res/artillery.png", ARTILLERY_TEXTURE_ID);
    Texture_Save("res/engineer.png", ENGINEER_TEXTURE_ID);
    Texture_Save("res/ground_shadow.png", GROUND_SHADOW_TEXTURE_ID);

    Texture_Save("res/destroyer.png", DESTROYER_TEXTURE_ID);
    Texture_Save("res/cruiser.png", CRUISER_TEXTURE_ID);
    Texture_Save("res/battleship.png", BATTLESHIP_TEXTURE_ID);
    Texture_Save("res/ship_shadow.png", SHIP_SHADOW_TEXTURE_ID);
    Texture_Save("res/ship_outline.png", SHIP_OUTLINE_TEXTURE_ID);

    Texture_Save("res/fighter.png", FIGHTER_TEXTURE_ID);
    Texture_Save("res/fighter_outline.png", FIGHTER_OUTLINE_TEXTURE_ID);
    Texture_Save("res/fighter_shadow.png", FIGHTER_SHADOW_TEXTURE_ID);

    Texture_Save("res/attacker.png", ATTACKER_TEXTURE_ID);
    Texture_Save("res/attacker_outline.png", ATTACKER_OUTLINE_TEXTURE_ID);
    Texture_Save("res/attacker_shadow.png", ATTACKER_SHADOW_TEXTURE_ID);

    Texture_Save("res/bomber.png", BOMBER_TEXTURE_ID);
    Texture_Save("res/bomber_outline.png", BOMBER_OUTLINE_TEXTURE_ID);
    Texture_Save("res/bomber_shadow.png", BOMBER_SHADOW_TEXTURE_ID);

    Texture_Save("res/arrow.png", ARROW_TEXTURE_ID);
    Texture_Save("res/arrow_shadow.png", ARROW_SHADOW_TEXTURE_ID);

    Texture_Save("res/timber_indicator.png", TIMBER_INDICATOR_TEXTURE_ID);
    Texture_Save("res/ore_indicator.png", ORE_INDICATOR_TEXTURE_ID);
    Texture_Save("res/coal_indicator.png", COAL_INDICATOR_TEXTURE_ID);
}