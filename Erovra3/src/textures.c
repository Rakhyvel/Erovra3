#pragma once
#include "./textures.h"
#include "./util/polygon.h"

/*
	Initializes TextureID's used in game, draws polygons onto some of them */
void Textures_Init()
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

    Polygon nacelle = Polygon_Create("res/nacelle.gon");

    CITY_TEXTURE_ID = Texture_RegisterTexture("res/city.png");
    FACTORY_TEXTURE_ID = Texture_RegisterTexture("res/factory.png");
    MINE_TEXTURE_ID = Texture_RegisterTexture("res/mine.png");
    PORT_TEXTURE_ID = Texture_RegisterTexture("res/port.png");
    BUILDING_OUTLINE_TEXTURE_ID = Texture_RegisterTexture("res/hit.png");
    CAPITAL_TEXTURE_ID = Texture_RegisterTexture("res/capital.png");
    WALL_TEXTURE_ID = Texture_RegisterTexture("res/wall.png");
    WALL_SHADOW_TEXTURE_ID = Texture_RegisterTexture("res/wall_shadow.png");
    CAPITAL_SHADOW_TEXTURE_ID = Texture_RegisterTexture("res/building_shadow.png");
    CITY_SHADOW_TEXTURE_ID = Texture_RegisterTexture("res/building_shadow.png");
    FACTORY_SHADOW_TEXTURE_ID = Texture_RegisterTexture("res/building_shadow.png");
    PORT_SHADOW_TEXTURE_ID = Texture_RegisterTexture("res/building_shadow.png");
    MINE_SHADOW_TEXTURE_ID = Texture_RegisterTexture("res/building_shadow.png");

    INFANTRY_TEXTURE_ID = Texture_RegisterTexture("res/infantry.png");
    CAVALRY_TEXTURE_ID = Texture_RegisterTexture("res/infantry.png");
    ARTILLERY_TEXTURE_ID = Texture_RegisterTexture("res/infantry.png");
    GROUND_OUTLINE_TEXTURE_ID = Texture_RegisterTexture("res/ground_outline.png");
    GROUND_SHADOW_TEXTURE_ID = Texture_RegisterTexture("res/shadow.png");

    DESTROYER_TEXTURE_ID = Texture_RegisterTexture("res/ship.png");
    CRUISER_TEXTURE_ID = Texture_RegisterTexture("res/ship.png");
    BATTLESHIP_TEXTURE_ID = Texture_RegisterTexture("res/ship.png");
    SHIP_SHADOW_TEXTURE_ID = Texture_RegisterTexture("res/ship.png");
    SHIP_OUTLINE_TEXTURE_ID = Texture_RegisterTexture("res/ship.png");

    FIGHTER_TEXTURE_ID = Texture_RegisterTexture("res/fighter.png");
    FIGHTER_OUTLINE_TEXTURE_ID = Texture_RegisterTexture("res/fighter.png");
    FIGHTER_SHADOW_TEXTURE_ID = Texture_RegisterTexture("res/fighter.png");

    ATTACKER_TEXTURE_ID = Texture_RegisterTexture("res/attacker.png");
    ATTACKER_OUTLINE_TEXTURE_ID = Texture_RegisterTexture("res/attacker.png");
    ATTACKER_SHADOW_TEXTURE_ID = Texture_RegisterTexture("res/attacker.png");

    BULLET_TEXTURE_ID = Texture_RegisterTexture("res/bullet.png");
    BULLET_SHADOW_TEXTURE_ID = Texture_RegisterTexture("res/bullet.png");
    SHELL_TEXTURE_ID = Texture_RegisterTexture("res/shell.png");
    COIN_TEXTURE_ID = Texture_RegisterTexture("res/coin.png");
    ORE_TEXTURE_ID = Texture_RegisterTexture("res/ore.png");

    Texture_CreateShadow(CAPITAL_SHADOW_TEXTURE_ID, CAPITAL_TEXTURE_ID);
    Texture_CreateShadow(CITY_SHADOW_TEXTURE_ID, CITY_TEXTURE_ID);
    Texture_CreateShadow(FACTORY_SHADOW_TEXTURE_ID, FACTORY_TEXTURE_ID);
    Texture_CreateShadow(PORT_SHADOW_TEXTURE_ID, PORT_TEXTURE_ID);
    Texture_CreateShadow(MINE_SHADOW_TEXTURE_ID, MINE_TEXTURE_ID);

    Texture_CreateShadow(BULLET_SHADOW_TEXTURE_ID, BULLET_TEXTURE_ID);

    Texture_FillPolygon(INFANTRY_TEXTURE_ID, groundBorderPoly, (SDL_Color) { 255, 255, 255, 255 });
    Texture_DrawPolygon(INFANTRY_TEXTURE_ID, groundBorderPoly, (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_FillPolygon(INFANTRY_TEXTURE_ID, Polygon_Create("res/infantry.gon"), (SDL_Color) { 0, 0, 0, 255 });
    Texture_FillPolygon(CAVALRY_TEXTURE_ID, groundBorderPoly, (SDL_Color) { 255, 255, 255, 255 });
    Texture_DrawPolygon(CAVALRY_TEXTURE_ID, groundBorderPoly, (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_FillPolygon(CAVALRY_TEXTURE_ID, Polygon_Create("res/cavalry.gon"), (SDL_Color) { 0, 0, 0, 255 });
    Texture_FillPolygon(ARTILLERY_TEXTURE_ID, groundBorderPoly, (SDL_Color) { 255, 255, 255, 255 });
    Texture_DrawPolygon(ARTILLERY_TEXTURE_ID, groundBorderPoly, (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_FillPolygon(ARTILLERY_TEXTURE_ID, Polygon_Create("res/artillery.gon"), (SDL_Color) { 0, 0, 0, 255 });
    printf("Um lol:\n");
    Texture_CreateShadow(GROUND_SHADOW_TEXTURE_ID, INFANTRY_TEXTURE_ID);

    Texture_FillBezier(BATTLESHIP_TEXTURE_ID, shipPoly, (SDL_Color) { 255, 255, 255, 255 });
    Texture_DrawBezier(BATTLESHIP_TEXTURE_ID, shipPoly, (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_FillPolygon(BATTLESHIP_TEXTURE_ID, Polygon_Create("res/battleship_stripe.gon"), (SDL_Color) { 0, 0, 0, 255 });

    Texture_FillBezier(CRUISER_TEXTURE_ID, shipPoly, (SDL_Color) { 255, 255, 255, 255 });
    Texture_DrawBezier(CRUISER_TEXTURE_ID, shipPoly, (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_FillPolygon(CRUISER_TEXTURE_ID, Polygon_Create("res/cruiser_stripe.gon"), (SDL_Color) { 0, 0, 0, 255 });

    Texture_FillBezier(DESTROYER_TEXTURE_ID, shipPoly, (SDL_Color) { 255, 255, 255, 255 });
    Texture_DrawBezier(DESTROYER_TEXTURE_ID, shipPoly, (SDL_Color) { 0, 0, 0, 255 }, 10);

    Texture_DrawBezier(SHIP_OUTLINE_TEXTURE_ID, shipPoly, (SDL_Color) { 255, 255, 255, 255 }, 30);
    Texture_CreateShadow(SHIP_SHADOW_TEXTURE_ID, DESTROYER_TEXTURE_ID);

    Texture_DrawBezier(FIGHTER_OUTLINE_TEXTURE_ID, fighterWing, (SDL_Color) { 255, 255, 255, 255 }, 30);
    Texture_DrawBezier(FIGHTER_OUTLINE_TEXTURE_ID, fighterTail, (SDL_Color) { 255, 255, 255, 255 }, 30);
    Texture_DrawBezier(FIGHTER_OUTLINE_TEXTURE_ID, fighterBody, (SDL_Color) { 255, 255, 255, 255 }, 30);

    Texture_FillBezier(FIGHTER_TEXTURE_ID, fighterWing, (SDL_Color) { 255, 255, 255, 255 }, 10);
    Texture_DrawBezier(FIGHTER_TEXTURE_ID, fighterWing, (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_FillBezier(FIGHTER_TEXTURE_ID, fighterTail, (SDL_Color) { 255, 255, 255, 255 }, 10);
    Texture_DrawBezier(FIGHTER_TEXTURE_ID, fighterTail, (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_FillBezier(FIGHTER_TEXTURE_ID, fighterBody, (SDL_Color) { 255, 255, 255, 255 }, 10);
    Texture_DrawBezier(FIGHTER_TEXTURE_ID, fighterBody, (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_CreateShadow(FIGHTER_SHADOW_TEXTURE_ID, FIGHTER_TEXTURE_ID);

	nacelle.x = 230;
    nacelle.y = 130;
    Texture_FillBezier(ATTACKER_TEXTURE_ID, nacelle, (SDL_Color) { 255, 255, 255, 255 }, 10);
    Texture_DrawBezier(ATTACKER_TEXTURE_ID, nacelle, (SDL_Color) { 0, 0, 0, 255 }, 10);
    nacelle.x = 390;
    Texture_FillBezier(ATTACKER_TEXTURE_ID, nacelle, (SDL_Color) { 255, 255, 255, 255 }, 10);
    Texture_DrawBezier(ATTACKER_TEXTURE_ID, nacelle, (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_FillBezier(ATTACKER_TEXTURE_ID, attackerWing, (SDL_Color) { 255, 255, 255, 255 }, 10);
    Texture_DrawBezier(ATTACKER_TEXTURE_ID, attackerWing, (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_FillPolygon(ATTACKER_TEXTURE_ID, attackerTail, (SDL_Color) { 255, 255, 255, 255 }, 10);
    Texture_DrawPolygon(ATTACKER_TEXTURE_ID, attackerTail, (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_FillBezier(ATTACKER_TEXTURE_ID, attackerBody, (SDL_Color) { 255, 255, 255, 255 }, 10);
    Texture_DrawBezier(ATTACKER_TEXTURE_ID, attackerBody, (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_CreateShadow(ATTACKER_SHADOW_TEXTURE_ID, ATTACKER_TEXTURE_ID);
}