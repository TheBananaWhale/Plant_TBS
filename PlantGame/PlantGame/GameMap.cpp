#pragma once
#include "GameMap.h"

GameMap::GameMap(int x, int y, int z)
{
	this->x = x;
	this->y = y;
	this->z = z;
	numPlayers = 2;	//Max of 2 players on this map
	curPlayer = 0;

	for(int i=0; i<x+1; i++)	//x
		for(int j=0; j<y+1; j++) //y
			for(int k=0; k<z+1; k++)
				blockMap[i][j][k] = NULL;

	for(int i=0; i<x; i++){	//x
		for(int j=0; j<y; j++) {//y
			//INITIALIZE BLOCKS
			for(int k=0; k<(z-UNEVEN_LAYERS); k++) {//z
				blockMap[i][j][k] = new Block(1 + rand() % 3);	//Bottom layers are solid/whole with random soil type
			}
			for(int k=z-UNEVEN_LAYERS; k<z; k++){ //z
				if(rand() % 2 && blockMap[i][j][k-1] != NULL)		//will randomly create some as long as the block below it is soil
					blockMap[i][j][k] = new Block(1 + rand() % 3);
			}
			//INITIALIZE UNITS
			unitsOnMap[i][j] = NULL;
		}
	}
	seedImages[0] = al_load_bitmap("Bitmaps/SeedTree.bmp");
	seedImages[1] = al_load_bitmap("Bitmaps/SeedFlower.bmp");

	for(int i=0;i<NUM_UNIT_TYPES;i++){
		al_convert_mask_to_alpha(seedImages[i], al_map_rgb(255,0,255));
		seedWidths[i] = al_get_bitmap_width(seedImages[i]);
		seedHeights[i] = al_get_bitmap_height(seedImages[i]);
	}

	blockImages[0] = al_load_bitmap("Bitmaps/Soil1.bmp");
	blockImages[1] = al_load_bitmap("Bitmaps/Soil2.bmp");
	blockImages[2] = al_load_bitmap("Bitmaps/Soil3.bmp");
	blockImages[3] = al_load_bitmap("Bitmaps/Soil4.bmp");
	for(int i=0;i<NUM_SOIL_TYPES;i++)
		al_convert_mask_to_alpha(blockImages[i], al_map_rgb(255,0,255)); //converts hideous magenta to transparent
	
	unitImages[0][0] = al_load_bitmap("Bitmaps/Tree1.bmp");
	unitImages[0][1] = al_load_bitmap("Bitmaps/Tree2.bmp");
	unitImages[0][2] = al_load_bitmap("Bitmaps/Tree3.bmp");
	unitImages[1][0] = al_load_bitmap("Bitmaps/Flower1.bmp");
	unitImages[1][1] = al_load_bitmap("Bitmaps/Flower2.bmp");
	unitImages[1][2] = al_load_bitmap("Bitmaps/Flower3.bmp");
	for(int i=0;i<NUM_UNIT_TYPES;i++)
		for(int j=0; j<NUM_IMAGES_PER_UNIT; j++)
			al_convert_mask_to_alpha(unitImages[i][j], al_map_rgb(255,0,255)); //converts hideous magenta to transparent

	blockWidth = al_get_bitmap_width(blockImages[0]);
	blockHeight = al_get_bitmap_height(blockImages[0])-1; //-1 because it looks better;
	blockPerceivedHeight = 32;

	for(int i=0;i<NUM_UNIT_TYPES;i++){
		for(int j=0;j<NUM_IMAGES_PER_UNIT;j++){
			unitWidths[i][j] = al_get_bitmap_width(unitImages[i][j]);
			unitHeights[i][j] = al_get_bitmap_height(unitImages[i][j]);
		}
	}
	//Initialize Block and Unit coordinates
	for(int i=0; i<x; i++){	//x
		for(int j=0; j<y; j++){ //y{	
			int top=0;
			for(int k=0; k<z; k++){//z
				//Initialize block coordinates
				if(blockMap[i][j][k] != NULL){
					blockMap[i][j][k]->setCoordinates(((x-1)*blockWidth/2)+(i*blockWidth/2)-(j*blockWidth/2),
						blockPerceivedHeight+((j+i)*(blockHeight-blockPerceivedHeight)/2)-((k)*(blockPerceivedHeight-4)),1);
					top++;
				}
			}
			//Initialize unit coordinates
			if(unitsOnMap[i][j] != NULL && (top-1 == z || blockMap[i][j][top] == NULL)){
				unitsOnMap[i][j]->setCoordinates(((x-1)*blockWidth/2)+(i*blockWidth/2)-((j-1)*blockWidth/2)-(unitWidths[unitsOnMap[i][j]->getClass()][unitsOnMap[i][j]->getSize()]/2),
					blockPerceivedHeight+((j+i+2)*(blockHeight-blockPerceivedHeight)/2)-((top)*(blockPerceivedHeight-4))-(unitHeights[unitsOnMap[i][j]->getClass()][unitsOnMap[i][j]->getSize()]),1);
			}
/*			//draw seeds on top of block on top of plant
			for(unsigned int k=0; k<seeds.size(); k++){
				if(seeds.at(k).xLoc == i && seeds.at(k).yLoc == j){
					int classID = players.at(seeds.at(k).playerNum)->getClass();
					al_draw_bitmap(seedImages[classID],
						camX+((x-1)*blockWidth/2)+(i*blockWidth/2)-((j-1)*blockWidth/2)-(seedWidths[classID]/2),
						camY+(camZ*blockPerceivedHeight)+((j+i+2)*(blockHeight-blockPerceivedHeight)/2)-((top)*(blockPerceivedHeight-4))-(seedHeights[classID]),0);
				}
			}
*/		}
	}
}

GameMap::~GameMap()
{

}

bool GameMap::addPlayer(Player* newPlayer)
{
	if(players.size() < numPlayers)
		players.push_back(newPlayer);
	else 
		return false;
	return true;
}

void GameMap::addUnit(int player, int xLoc, int yLoc)	//This should only add initial units (future units will be added by GameMap from seeds
{
	//Get player's settings for their unit and adds it to unitsOnMap[x][y]
	if(players.at(player)->getClass() == 0)
		unitsOnMap[xLoc][yLoc] = new Tree();
	else
		unitsOnMap[xLoc][yLoc] = new Flower();

	//SET coordinates for new block
	//First figure out how high top block is
	int top=0;
	for(int k=0; k<z; k++)
		if(blockMap[xLoc][yLoc][k] != NULL)
			top++;
		else
			break;

	unitsOnMap[xLoc][yLoc]->setOwner(player);

	//set coordinates based on block below it
	unitsOnMap[xLoc][yLoc]->setCoordinates(blockMap[xLoc][yLoc][top-1]->getX()+(blockWidth/2)-(unitWidths[players.at(player)->getClass()][unitsOnMap[xLoc][yLoc]->getSize()]/2),
		blockMap[xLoc][yLoc][top-1]->getY()+(blockHeight/2)-(unitHeights[players.at(player)->getClass()][unitsOnMap[xLoc][yLoc]->getSize()]), 1);
}

void GameMap::changeCamera(int dx, int dy, int dz, double dZoom){
	for(int i=0; i<x; i++){	//x
		for(int j=0; j<y; j++){ //y{	
			int top=0;
			for(int k=0; k<z; k++){//z
				if(blockMap[i][j][k] != NULL){
					blockMap[i][j][k]->incrementCoordinates(dx, dy, dZoom);
					top++;
				}else
					break;
			}
			if(unitsOnMap[i][j] != NULL)
				unitsOnMap[i][j]->incrementCoordinates(dx, dy, dZoom);
		}
	}
}

void GameMap::nextTurn()
{

	//Turn some seeds into units
	for(unsigned int i=0;i<seeds.size(); i++){
		if(!(unitsOnMap[seeds.at(i).xLoc][seeds.at(i).yLoc] != NULL) && seeds.at(i).playerNum == curPlayer && rand() % 4 == 0){	//should check germinate of block seed is on where 'rand()' is
			addUnit(seeds.at(i).playerNum, seeds.at(i).xLoc, seeds.at(i).yLoc);
			//delete the seed because it is now a plant!
		}
	}
	//move seeds
	for(unsigned int i=0; i<seeds.size(); i++){
		if(seeds.at(i).xLoc < 9)
			seeds.at(i).xLoc += rand() % 2;
		if(seeds.at(i).xLoc > 0)
			seeds.at(i).xLoc -= rand() % 2;
		
		if(seeds.at(i).yLoc < 9)
			seeds.at(i).yLoc += rand() % 2;
		if(seeds.at(i).yLoc > 0)
			seeds.at(i).yLoc -= rand() % 2;
	}

	curPlayer = (curPlayer+1) % numPlayers;
	//update units and produce their seeds
	for(int i=0; i<x; i++){
		for(int j=0; j<y; j++){
			if(unitsOnMap[i][j] != NULL && unitsOnMap[i][j]->getOwner() == curPlayer){
				int num = unitsOnMap[i][j]->addMinerals();
				if(num != -1){
					//Update x and y, because it will be different if the unit grew
					int top=0;
					for(int k=0; k<z; k++)
						if(blockMap[i][j][k] != NULL)
							top++;
						else
							break;

					unitsOnMap[i][j]->setCoordinates(blockMap[i][j][top-1]->getX()+(blockWidth/2)-(unitWidths[players.at(curPlayer)->getClass()][unitsOnMap[i][j]->getSize()]/2),
						blockMap[i][j][top-1]->getY()+(blockHeight/2)-(unitHeights[players.at(curPlayer)->getClass()][unitsOnMap[i][j]->getSize()]), 1);
					//Make a new seeds based on if the nuit made more or not
					for(int k=0; k<num;k++){
						struct seed newSeed;
						newSeed.playerNum = curPlayer;
						newSeed.xLoc = i;
						newSeed.yLoc = j;
						seeds.push_back(newSeed);
					}
				}
			}
		}
	}
}

void GameMap::draw(int camX, int camY, int camZ, double zoom)
{
	ALLEGRO_DISPLAY* tempDisplay = al_get_current_display();
	ALLEGRO_BITMAP* tempBitmap = al_create_bitmap(1920, 1080);
	al_set_target_bitmap(tempBitmap);
	//Draws all blocks that are missing a block above or in front of them
	for(int i=0; i<x; i++){	//x
		for(int j=0; j<y; j++){ //y{	
			int top=0;
			for(int k=0; k<camZ; k++){//z
				if(blockMap[i][j][k] != NULL){
					if(k==camZ-1 || !(blockMap[i+1][j][k] != NULL && blockMap[i][j+1][k] != NULL && blockMap[i][j][k+1] != NULL)){
						blockMap[i][j][k]->draw(blockImages[blockMap[i][j][k]->getBitmap()]);
						//al_flip_display();
						//al_rest(0.03);
					}
					top++;
				}
			}
			//draw plant on top of top block
			if(unitsOnMap[i][j] != NULL && (top-1 == camZ || blockMap[i][j][top] == NULL)){
				unitsOnMap[i][j]->draw(unitImages[unitsOnMap[i][j]->getClass()][unitsOnMap[i][j]->getSize()]);
			}
			//draw seeds on top of block on top of plant
			for(unsigned int k=0; k<seeds.size(); k++){
				if(seeds.at(k).xLoc == i && seeds.at(k).yLoc == j && (top-1 == camZ || blockMap[i][j][top] == NULL)){
					int classID = players.at(seeds.at(k).playerNum)->getClass();
					al_draw_bitmap(seedImages[classID],
						camX+((x-1)*blockWidth/2)+(i*blockWidth/2)-((j-1)*blockWidth/2)-(seedWidths[classID]/2),
						camY+blockPerceivedHeight+((j+i+2)*(blockHeight-blockPerceivedHeight)/2)-((top)*(blockPerceivedHeight-4))-(seedHeights[classID]),0);
				}
			}
			//let's you see how it draws
			//al_flip_display();
			//al_rest(0.03);
		}
	}
	al_set_target_bitmap(al_get_backbuffer(tempDisplay));
	al_draw_scaled_bitmap(tempBitmap, 0, 0, 1920, 1080, 0, 0, 1920*zoom, 1080*zoom, 0);
	al_destroy_bitmap(tempBitmap);
}