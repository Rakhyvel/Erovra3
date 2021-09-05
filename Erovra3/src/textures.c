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
    Polygon attackerNacelle = Polygon_Create("res/attacker_nacelle.gon");

    Polygon bomberWing = Polygon_Create("res/bomber_wings.gon");
    Polygon bomberBody = Polygon_Create("res/bomber_body.gon");
    Polygon bomberTail = Polygon_Create("res/bomber_tail.gon");
    Polygon bomberNacelle = Polygon_Create("res/bomber_nacelle.gon");

    CITY_TEXTURE_ID = Texture_RegisterTexture("res/city.png");
    FACTORY_TEXTURE_ID = Texture_RegisterTexture("res/factory.png");
    MINE_TEXTURE_ID = Texture_RegisterTexture("res/mine.png");
    PORT_TEXTURE_ID = Texture_RegisterTexture("res/port.png");
    AIRFIELD_TEXTURE_ID = Texture_RegisterTexture("res/airfield.png");
    FARM_TEXTURE_ID = Texture_RegisterTexture("res/farm.png");
    ACADEMY_TEXTURE_ID = Texture_RegisterTexture("res/academy.png");
    BUILDING_OUTLINE_TEXTURE_ID = Texture_RegisterTexture("res/hit.png");
    CAPITAL_TEXTURE_ID = Texture_RegisterTexture("res/capital.png");
    WALL_TEXTURE_ID = Texture_RegisterTexture("res/wall.png");
    WALL_SHADOW_TEXTURE_ID = Texture_RegisterTexture("res/wall_shadow.png");
    CAPITAL_SHADOW_TEXTURE_ID = Texture_RegisterTexture("res/city.png");
    CITY_SHADOW_TEXTURE_ID = Texture_RegisterTexture("res/city.png");
    FACTORY_SHADOW_TEXTURE_ID = Texture_RegisterTexture("res/factory.png");
    PORT_SHADOW_TEXTURE_ID = Texture_RegisterTexture("res/port.png");
    MINE_SHADOW_TEXTURE_ID = Texture_RegisterTexture("res/mine.png");
    ACADEMY_SHADOW_TEXTURE_ID = Texture_RegisterTexture("res/academy.png");

    INFANTRY_TEXTURE_ID = Texture_RegisterTexture("res/infantry.png");
    CAVALRY_TEXTURE_ID = Texture_RegisterTexture("res/infantry.png");
    ARTILLERY_TEXTURE_ID = Texture_RegisterTexture("res/infantry.png");
    ENGINEER_TEXTURE_ID = Texture_RegisterTexture("res/infantry.png");
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

    BOMBER_TEXTURE_ID = Texture_RegisterTexture("res/bomber.png");
    BOMBER_OUTLINE_TEXTURE_ID = Texture_RegisterTexture("res/bomber.png");
    BOMBER_SHADOW_TEXTURE_ID = Texture_RegisterTexture("res/bomber.png");

    BULLET_TEXTURE_ID = Texture_RegisterTexture("res/bullet.png");
    BULLET_SHADOW_TEXTURE_ID = Texture_RegisterTexture("res/bullet.png");
    SHELL_TEXTURE_ID = Texture_RegisterTexture("res/shell.png");
    SHELL_SHADOW_TEXTURE_ID = Texture_RegisterTexture("res/shell.png");
    BOMB_TEXTURE_ID = Texture_RegisterTexture("res/bomb.png");
    BOMB_SHADOW_TEXTURE_ID = Texture_RegisterTexture("res/bomb.png");
    COIN_TEXTURE_ID = Texture_RegisterTexture("res/coin.png");
    COIN_SHADOW_TEXTURE_ID = Texture_RegisterTexture("res/coin.png");
    ORE_TEXTURE_ID = Texture_RegisterTexture("res/ore.png");
    ORE_SHADOW_TEXTURE_ID = Texture_RegisterTexture("res/ore.png");
    POPULATION_TEXTURE_ID = Texture_RegisterTexture("res/population.png");

    ARROW_TEXTURE_ID = Texture_RegisterTexture("res/arrow.png");
    ARROW_SHADOW_TEXTURE_ID = Texture_RegisterTexture("res/arrow.png");

    Texture_CreateShadow(BULLET_SHADOW_TEXTURE_ID, BULLET_TEXTURE_ID);
    Texture_CreateShadow(BOMB_SHADOW_TEXTURE_ID, BOMB_TEXTURE_ID);
    Texture_CreateShadow(SHELL_SHADOW_TEXTURE_ID, SHELL_TEXTURE_ID);

    Texture_FillPolygon(CITY_TEXTURE_ID, Polygon_Create("res/city.gon"), (SDL_Color) { 255, 255, 255, 255 });
    Texture_DrawPolygon(CITY_TEXTURE_ID, Polygon_Create("res/city.gon"), (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_DrawPolygon(MINE_TEXTURE_ID, Polygon_Create("res/mine.gon"), (SDL_Color) { 0, 0, 0, 255 }, 20);
    Texture_DrawPolygon(PORT_TEXTURE_ID, Polygon_Create("res/port_bar.gon"), (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_DrawBezier(PORT_TEXTURE_ID, Polygon_Create("res/port_hook.gon"), (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_DrawBezier(PORT_TEXTURE_ID, Polygon_Create("res/port_hole.gon"), (SDL_Color) { 0, 0, 0, 255 }, 10);

    Texture_CreateShadow(CAPITAL_SHADOW_TEXTURE_ID, CAPITAL_TEXTURE_ID);
    Texture_CreateShadow(CITY_SHADOW_TEXTURE_ID, CITY_TEXTURE_ID);
    Texture_CreateShadow(FACTORY_SHADOW_TEXTURE_ID, FACTORY_TEXTURE_ID);
    Texture_CreateShadow(PORT_SHADOW_TEXTURE_ID, PORT_TEXTURE_ID);
    Texture_CreateShadow(MINE_SHADOW_TEXTURE_ID, MINE_TEXTURE_ID);
    Texture_CreateShadow(ACADEMY_SHADOW_TEXTURE_ID, ACADEMY_TEXTURE_ID);

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
    Texture_FillBezier(FIGHTER_TEXTURE_ID, fighterWing, (SDL_Color) { 255, 255, 255, 255 });
    Texture_DrawBezier(FIGHTER_TEXTURE_ID, fighterWing, (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_FillBezier(FIGHTER_TEXTURE_ID, fighterTail, (SDL_Color) { 255, 255, 255, 255 });
    Texture_DrawBezier(FIGHTER_TEXTURE_ID, fighterTail, (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_FillBezier(FIGHTER_TEXTURE_ID, fighterBody, (SDL_Color) { 255, 255, 255, 255 });
    Texture_DrawBezier(FIGHTER_TEXTURE_ID, fighterBody, (SDL_Color) { 0, 0, 0, 255 }, 10);
    Texture_CreateShadow(FIGHTER_SHADOW_TEXTURE_ID, FIGHTER_TEXTURE_ID);

    Texture_DrawBezier(ATTACKER_OUTLINE_TEXTURE_ID, attackerWing, (SDL_Color) { 255, 255, 255, 255 }, 30);
    Texture_DrawPolygon(ATTACKER_OUTLINE_TEXTURE_ID, attackerTail, (SDL_Color) { 255, 255, 255, 255 }, 30);
    Texture_DrawBezier(ATTACKER_OUTLINE_TEXTURE_ID, attackerBody, (SDL_Color) { 255, 255, 255, 255 }, 30);
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
    Texture_CreateShadow(ATTACKER_SHADOW_TEXTURE_ID, ATTACKER_TEXTURE_ID);

    Texture_DrawBezier(BOMBER_OUTLINE_TEXTURE_ID, bomberWing, (SDL_Color) { 255, 255, 255, 255 }, 30);
    Texture_DrawPolygon(BOMBER_OUTLINE_TEXTURE_ID, bomberTail, (SDL_Color) { 255, 255, 255, 255 }, 30);
    Texture_DrawBezier(BOMBER_OUTLINE_TEXTURE_ID, bomberBody, (SDL_Color) { 255, 255, 255, 255 }, 30);
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
    Texture_CreateShadow(BOMBER_SHADOW_TEXTURE_ID, BOMBER_TEXTURE_ID);

    Texture_CreateShadow(COIN_SHADOW_TEXTURE_ID, COIN_TEXTURE_ID);
    Texture_CreateShadow(ORE_SHADOW_TEXTURE_ID, ORE_TEXTURE_ID);

    Texture_FillPolygon(ARROW_TEXTURE_ID, Polygon_Create("res/arrow.gon"), (SDL_Color) { 60, 120, 250, 255 });
    Texture_CreateShadow(ARROW_SHADOW_TEXTURE_ID, ARROW_TEXTURE_ID);
}