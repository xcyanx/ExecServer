/*
 * Packet.h
 *
 *  Created on: 11 Νοε 2012
 *      Author: Σταμάτης
 */

#ifndef PACKET_H_
#define PACKET_H_

#define MAX_BUFFER_LEN 2048

struct BasicPacket
{
	enum PacketIDs
	{
		ACC_VERIFICATION = 0x0,
		ACC_VERIFIED,
		ACC_NOTVERIFIED,
		ACC_CREATE,
		ACC_NOTAVAILABLE,
		ACC_CREATED,
		DATA_REQUESTROUTES,
		DATA_PACKETSIZE,
		DATA_XMLDATA,
		DATA_REQUESTROUTEDATA,
		DATA_ROUTEDATA,
		DATA_VIDEO_URLREQ,
		DATA_VIDEO_URLRES,
		DATA_IMG_URLRES,
		DATA_IMG_URLREQ,
		DATA_TEXT_REQ,
		DATA_TEXT_RES,
		DATA_UPLOAD,
		ERR
	};

	int PacketID;
};

struct RRouteData: public BasicPacket
{
	char route_name[200];
};

struct NextPacketSize: public BasicPacket
{
	int size;
};

struct XMLPacket: public BasicPacket
{
	char xmlData[0];
};

struct LogInPacket: public BasicPacket
{
	char username[20];
	char password[33];
};

struct VideoReqPacket: public BasicPacket
{
	int videoID;
};

struct VideoResPacket: public BasicPacket
{
	char url[100];
};

struct TextResPacket: public BasicPacket
{
	char text[1];
};

#endif /* PACKET_H_ */

