#pragma once

#include <functional>
#include <string>
#include <vector>
#include <unordered_map>

#include "Util.hpp"
#include "Types.hpp"

namespace SM {
	struct ChatCommand {
		struct Param {
			enum Type : uint32 {
				Bool,
				Int,
				Float,
				String
			} type;
			union {
				bool boolValue;
				int intValue;
				float floatValue;
				const char* stringValue = nullptr;
			};
		};
		struct ParamInfo {
			Param::Type type;
			std::string name;
			bool optional = true;
		};
		using VecParams = std::vector<Param>;
		using Callback = std::function<std::string(const VecParams&)>;

		std::string name;
		std::vector<ParamInfo> vecParams;
		std::string help;
		Callback cb;
		bool bHidden = false;
	};
	ASSERT_SIZE(ChatCommand::Param, 0x10);
	ASSERT_SIZE(ChatCommand::ParamInfo, 0x30);
	ASSERT_SIZE(ChatCommand, 0xA0);

	class ChatCommandManager {
		public:
			static ChatCommandManager** _selfPtr;
			inline static ChatCommandManager* Get() {return *_selfPtr;};

			void registerCommand(
				const std::string& name,
				const std::vector<ChatCommand::ParamInfo>& params,
				const ChatCommand::Callback& cb,
				const std::string& help = "",
				bool hidden = false
			);

		private:
			std::unordered_map<uint64, ChatCommand> m_mapChatCommands;
	};
	ASSERT_SIZE(ChatCommandManager, 0x40);
}
