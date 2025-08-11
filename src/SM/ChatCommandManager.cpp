
#include "ChatCommandManager.hpp"

using namespace SM;

ChatCommandManager** ChatCommandManager::_selfPtr = (ChatCommandManager**)0x06c5ef2;

void ChatCommandManager::registerCommand(
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

	m_mapChatCommands.emplace(hash, std::move(cmd));
}
