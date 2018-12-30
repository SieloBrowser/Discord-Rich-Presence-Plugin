/***********************************************************************************
** MIT License                                                                    **
**                                                                                **
** Copyright (c) 2018 Victor DENIS (victordenis01@gmail.com)                      **
**                                                                                **
** Permission is hereby granted, free of charge, to any person obtaining a copy   **
** of this software and associated documentation files (the "Software"), to deal  **
** in the Software without restriction, including without limitation the rights   **
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      **
** copies of the Software, and to permit persons to whom the Software is          **
** furnished to do so, subject to the following conditions:                       **
**                                                                                **
** The above copyright notice and this permission notice shall be included in all **
** copies or substantial portions of the Software.                                **
**                                                                                **
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     **
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       **
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    **
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         **
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  **
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  **
** SOFTWARE.                                                                      **
***********************************************************************************/

#pragma once
#ifndef DISCORDPLUGIN_HPP
#define DISCORDPLUGIN_HPP

#include "Plugins/PluginInterface.hpp"

#include <QObject>

#include <QPointer> 

#include "Discord.hpp"

class DiscordPluginSideBar;

namespace Sn
{
class WebView;

class DiscordPlugin: public QObject, public PluginInterface {
	Q_OBJECT
	Q_INTERFACES(Sn::PluginInterface) // Load the interface. It **has to** be "Sn::PluginInterface".
	Q_PLUGIN_METADATA(IID "com.feldrise.sielo.plugin.DiscordRichPresencePlugin") // Set a unique identifier for your plugin. 

public:
	DiscordPlugin();

	PluginProp pluginProp();
	
	void init(InitState state, const QString& settingsPath);
	void unload();
	bool testPlugin();

	void showSettings(QWidget* parent);

private slots:
	void updatePresence();

private:
	QPointer<QDialog> m_settings;
	QString m_settingsPath{};

	DiscordPluginSideBar* m_sideBar{nullptr};

	const QString m_appId{"527780485695275009"};
	QString m_details{"Browsing the web"};
	QString m_state{"Visit sielo.app to know more"};
	QString m_icon{"blue-icon"};

	Discord m_discord{};
	RichPresence m_richPresence{};
};
}

#endif // DISCORDPLUGIN_HPP