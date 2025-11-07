
#include "ChatCommandManager.hpp"

using namespace SM;

ChatCommandManager** ChatCommandManager::_selfPtr = (ChatCommandManager**)0x1267678;

void ChatCommandManager::registerOrUpdateCommand(
	const std::string& name,
	const std::vector<ChatCommand::ParamInfo>& params,
	const ChatCommand::Callback& cb,
	const std::string& help,
	bool hidden
) {
	uint64 hash = 0;
	for ( char ch : name )
		hash = (hash * 0x65) + uint64(ch);

	ChatCommand cmd;
	cmd.name = name;
	cmd.vecParams = params;
	cmd.help = help;
	cmd.cb = cb;
	cmd.bHidden = hidden;

	if ( !m_mapChatCommands.contains(hash) )
		m_mapChatCommands.emplace(hash, std::move(cmd));
	else
		m_mapChatCommands.find(hash)->second = std::move(cmd);
}
