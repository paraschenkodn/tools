#include "mapbuilder.h"

MapBuilder::MapBuilder()
{
currentmap=FLAT_MAP;
}

void MapBuilder::flatMap()
{
}

void MapBuilder::sphereMap()
{

}

void MapBuilder::newmap()
{
    filename=QFileDialog::getOpenFileName( 0, QString("Open File"),QString("/"),QString("Images (*.png *.xpm *.jpg)"));
}

