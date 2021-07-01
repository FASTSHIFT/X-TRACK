#include "GPX_Parser.h"
#include <string.h>

GPX_Parser::GPX_Parser()
{
	memset(&priv, 0, sizeof(priv));
}

GPX_Parser::~GPX_Parser()
{

}

void GPX_Parser::SetCallback(Callback_t avaliableCallback, Callback_t readCallback)
{
	priv.avaliableCallback = avaliableCallback;
	priv.readCallback = readCallback;
}

bool GPX_Parser::ReadNext(Point_t* point)
{
	bool isFindLat = find((char*)"lat=\"");

	if(isFindLat)
	{
		String str = readStringUntil('"');
		point->latitude = str.toFloat();
	}

	bool isFindLng = find((char*)"lon=\"");

	if (isFindLng)
	{
		String str = readStringUntil('"');
		point->longitude = str.toFloat();
	}

	return isFindLat && isFindLng;
}

int GPX_Parser::available()
{
	if (priv.avaliableCallback)
	{
		return priv.avaliableCallback(this);
	}

	return 0;
}

int GPX_Parser::read()
{
	if (priv.readCallback)
	{
		return priv.readCallback(this);
	}

	return 0;
}
