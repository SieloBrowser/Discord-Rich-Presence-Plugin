#include "Discord.hpp"

Discord::Discord()
	: m_bInitialised(false)
{
	m_rich = { 0 };
}

Discord::~Discord()
{
	shutdown();
}

void Discord::initialise(std::string appId)
{
	if (m_bInitialised)
		return;

	// Initialise handlers
	m_handlers = {};

	Discord_Initialize(appId.c_str(), &m_handlers, 1, nullptr);

	m_bInitialised = true;

	return;
}

bool Discord::updatePresence(RichPresence &richPresence)
{
	if (!m_bInitialised)
		return false;

	// Copy Rich Presence details from parameter struct to member struct
	m_rich.details = richPresence.details.c_str();
	m_rich.state = richPresence.state.c_str();
	m_rich.largeImageKey = richPresence.largeKey.c_str();
	m_rich.largeImageText = richPresence.largeText.c_str();
	m_rich.partyMax = richPresence.partyMax;
	m_rich.partySize = richPresence.partySize;
	m_rich.smallImageKey = richPresence.smallKey.c_str();
	m_rich.smallImageText = richPresence.smallText.c_str();
	m_rich.startTimestamp = richPresence.timeStart;
	m_rich.endTimestamp = richPresence.timeEnd;

	Discord_UpdatePresence(&m_rich);

	return true;
}

void Discord::shutdown()
{
	if (m_bInitialised)
	{
		// Clean up Discord
		Discord_Shutdown();

		m_bInitialised = false;
	}
}