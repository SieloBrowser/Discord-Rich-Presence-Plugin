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

#include "DiscordPlugin.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialog>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>

#include <QMenu>
#include <QMessageBox>

#include <QSettings>

#include <chrono> 

#include "DiscordPluginSideBar.hpp"

#include "Web/WebView.hpp"
#include "Web/WebHitTestResult.hpp"

#include "Plugins/PluginProxy.hpp"
#include "Utils/SideBarManager.hpp"

#include "Application.hpp"

namespace Sn
{
DiscordPlugin::DiscordPlugin()
{
	// Empy
}

PluginProp DiscordPlugin::pluginProp()
{
	PluginProp props{};

	props.name = "Discord Rich Presence";
	props.info = "Show Rich Presence on Discord profile";
	props.desc = "A simple plugin to have a Sielo specific rich presence on your Discord profile";
	props.icon = QPixmap(":/images/ic_discord.svg");
	props.version = "1.0.0";
	props.author = "Victor DENIS <victordenis01@gmail.com";
	props.hasSettings = true;

	return props;
}

void DiscordPlugin::init(InitState state, const QString& settingsPath)
{
	m_settingsPath = settingsPath;

	// Load settings 
	QSettings settings{m_settingsPath + "/extensions.ini", QSettings::IniFormat};
	settings.beginGroup("DiscordRichPresence");

	m_details = settings.value("details", "Browsing the web").toString();
	m_state = settings.value("state", "Visit sielo.app to know more").toString();
	m_icon = settings.value("icon", "blue-icon").toString();

	settings.endGroup();

	// Adding new sidebar into application
	m_sideBar = new DiscordPluginSideBar(this);
	Application::instance()->addSidebar("DiscordPlugin-sidebar", m_sideBar);

	m_discord.initialise(m_appId.toStdString());
	m_richPresence = {};

	m_richPresence.timeStart = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	m_richPresence.timeEnd = 0;

	updatePresence();
}

void DiscordPlugin::unload()
{
	m_discord.shutdown();

	// Removing sidebar from application
	Application::instance()->removeSidebar(m_sideBar);
	delete m_sideBar;
	
	// Deleting settings dialog if opened
	delete m_settings.data();
}

bool DiscordPlugin::testPlugin()
{
	return Application::currentVersion.contains("1.17");
}

void DiscordPlugin::showSettings(QWidget* parent)
{
	if (!m_settings) {
		m_settings = new QDialog(parent);

		QHBoxLayout* layout{new QHBoxLayout(m_settings)};
		QVBoxLayout* imageLayout{new QVBoxLayout};
		QVBoxLayout* inputLayout{new QVBoxLayout};

		// Image part
		QPixmap bluePresencePixmap{QIcon(":/images/presence-blue.png").pixmap(300, 328)};
		QPixmap normalPresencePixmap{QIcon(":/images/presence-normal.png").pixmap(300, 328)};
		QLabel* presencePixmap{new QLabel(m_settings)};

		presencePixmap->setPixmap(m_icon == "normal-icon" ? normalPresencePixmap : bluePresencePixmap);

		QComboBox* iconToApply{new QComboBox(m_settings)};
		iconToApply->addItems({"Blue Icon", "Normal Icon"});

		iconToApply->setCurrentIndex(m_icon == "blue-icon" ? 0 : 1);

		// Input part
		QLabel* detailsDesc{new QLabel(tr("Details (eg: \"Browsing the web\"):", "Please, keep examples in english for screen"), m_settings)};
		QLineEdit* details{new QLineEdit(m_settings)};
		QLabel* stateDesc{new QLabel(tr("State (eg: \"Visit sielo.app to know more\")", "Please, keep examples in english for screen"), m_settings)};
		QLineEdit* state{new QLineEdit(m_settings)};

		details->setText(m_details);
		state->setText(m_state);

		QSpacerItem* spacer{new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Expanding)};
		QPushButton* updateButton{new QPushButton(tr("Update Rich Presence"), m_settings)};

		// Connection
		auto currentChanged = [=](const QString& newText) {
			if (newText == "Blue Icon")
				presencePixmap->setPixmap(bluePresencePixmap);
			else
				presencePixmap->setPixmap(normalPresencePixmap);
		};

		connect(iconToApply, &QComboBox::currentTextChanged, this, currentChanged);
		connect(updateButton, &QPushButton::clicked, this, [=]()
		{
			// Update presence
			m_details = details->text();
			m_state = state->text();

			QString newIcon{iconToApply->currentText()};
			newIcon.replace(" ", "-");

			m_icon = newIcon.toLower();

			updatePresence();

			// Save settings 
			QSettings settings{m_settingsPath + "/extensions.ini", QSettings::IniFormat};
			settings.beginGroup("DiscordRichPresence");

			settings.setValue("details", m_details);
			settings.setValue("state", m_state);
			settings.setValue("icon", m_icon);

			settings.endGroup();
		});

		// Layouts
		imageLayout->addWidget(presencePixmap);
		imageLayout->addWidget(iconToApply);

		inputLayout->addWidget(detailsDesc);
		inputLayout->addWidget(details);
		inputLayout->addWidget(stateDesc);
		inputLayout->addWidget(state);
		inputLayout->addItem(spacer);
		inputLayout->addWidget(updateButton);

		layout->addLayout(imageLayout);
		layout->addLayout(inputLayout);

		// Other
		m_settings.data()->setAttribute(Qt::WA_DeleteOnClose);
		m_settings.data()->setWindowTitle(tr("Discord Reach Presence Settings"));
		m_settings.data()->setWindowIcon(QIcon(":/images/ic_discord.svg"));
	}

	m_settings.data()->show();
	m_settings.data()->raise();
}

void DiscordPlugin::updatePresence()
{
	m_richPresence.details = m_details.toStdString();
	m_richPresence.state = m_state.toStdString();

	m_richPresence.largeText = "Sielo Browser";
	m_richPresence.largeKey = m_icon.toStdString();

	// Send Rich Presence update to Discord via Discord RPC
	// TO-DO: Error checking
	m_discord.updatePresence(m_richPresence);
}
}
