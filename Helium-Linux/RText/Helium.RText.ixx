/*
* Helium is a customizable extension system for Minecraft server.
* You can get the lastest source code and releases of Helium at :
* https://github.com/Helium-DevTeam/Helium
* ----------------------------------------------------------------------------------------
* Helium Copyright (C) 2021-2022 HeliumDevTeam
*
* This file is a part of Helium
*
* Helium is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Helium is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Helium.  If not, see <https://www.gnu.org/licenses/>.
* ----------------------------------------------------------------------------------------
*/

module;

export module Helium.RText;

import <iostream>;
import <utility>;
import <memory>;
import <vector>;

using namespace std;

export {
	namespace Helium {
		constexpr const inline auto OPEN_URL = 1000000;
		constexpr const inline auto OPEN_FILE = 1000001;
		constexpr const inline auto RUN_COMMAND = 1000002;
		constexpr const inline auto SUGGEST_COMMAND = 1000003;
		constexpr const inline auto CHANGE_PAGE = 1000004;
		constexpr const inline auto COPY_TO_CLIPBOARD = 1000005;
		
		class RColor {
		private:
			class _inner_rcolor {
			public:
				_inner_rcolor(const int& value, const string& str) :
					Value(value),
					Str(str) {};
				int operator()() noexcept { return this->Value; };
				int GetValue() noexcept { return this->Value; };
				string GetStr() noexcept { 
					auto ret = this->Str;
					return ret; 
				};
				const int Value;
				const string Str;
			};
		public:
			const static inline RColor::_inner_rcolor black       {0          , "black"};
			const static inline RColor::_inner_rcolor dark_blue   {1          , "dark_blue"};
			const static inline RColor::_inner_rcolor dark_green  {2          , "dark_green"};
			const static inline RColor::_inner_rcolor dark_aqua   {3          , "dark_aqua"};
			const static inline RColor::_inner_rcolor dark_red    {4          , "dark_red"};
			const static inline RColor::_inner_rcolor dark_purple {5          , "dark_purple"};
			const static inline RColor::_inner_rcolor gold        {6          , "gold"};
			const static inline RColor::_inner_rcolor gray        {7          , "gray"};
			const static inline RColor::_inner_rcolor dark_gray   {8          , "dark_gray"};
			const static inline RColor::_inner_rcolor blue        {9          , "blue"};
			const static inline RColor::_inner_rcolor green       {10         , "green"};
			const static inline RColor::_inner_rcolor aqua        {11         , "aqua"};
			const static inline RColor::_inner_rcolor red         {12         , "red"};
			const static inline RColor::_inner_rcolor light_purple{13         , "light_purple"};
			const static inline RColor::_inner_rcolor yellow      {14         , "yellow"};
			const static inline RColor::_inner_rcolor white       {15         , "white"};
			const static inline RColor::_inner_rcolor reset       {16         , "reset"};
		};

		class RAction {
		public:
			explicit RAction(int actionId_, string value_);
			[[nodiscard]] string toJsonRawText() const;
			[[nodiscard]] string getAction() const;
			string getValue();
			[[nodiscard]] int getActionId() const;
			RAction setAction(int actionId_);
			RAction setValue(string value_);
		private:
			int actionId;
			string value;
		};

		class RText {
		public:
			explicit RText(string text);
			string toJSONRawText();
			RText setColor(string color_);
			RText setHoverText(RText spHoverText);

			[[nodiscard]] const string& getText() const;
			RText setText(const string& basicString);
			[[nodiscard]] bool isItalic1() const;
			RText setIsItalic(bool b);
			[[nodiscard]] bool isBold1() const;
			RText setIsBold(bool b);
			[[nodiscard]] bool isUnderlined1() const;
			RText setIsUnderlined(bool underlined);
			[[nodiscard]] bool isStrikethrough1() const;
			RText setIsStrikethrough(bool bStrikethrough);
			[[nodiscard]] bool isObfuscated1() const;
			RText setIsObfuscated(bool bObfuscated);
			[[nodiscard]] const RAction& getClickAction() const;
			RText setClickAction(const RAction& clickAction_);

		private:
			string color = (RColor::aqua.Str);
			shared_ptr<RText> hoverText;
			shared_ptr<RAction> clickAction;
			string text;
			bool isItalic = false;
			bool isBold = false;
			bool isUnderlined = false;
			bool isStrikethrough = false;
			bool isObfuscated = false;
			bool hasColor = false;
			bool hasHoverText = false;
			bool hasAction = false;
		};

		string RTextList2Json(vector<RText> rTextList);
	}
}

namespace Helium {
	RText::RText(string text) {
		this->text = std::move(text);
	}

	string RText::toJSONRawText() {
		string ret = "{";
		ret.append("\"").append("text").append("\":\"").append(text).append("\"");
		if (this->hasColor)         ret.append(R"(,"color":")").append(color).append("\"");
		if (this->isBold)           ret.append(",\"bold\":true");
		if (this->isItalic)         ret.append(",\"italic\":true");
		if (this->isObfuscated)     ret.append(",\"obfuscated\":true");
		if (this->isStrikethrough)  ret.append(",\"strikethrough\":true");
		if (this->isUnderlined)     ret.append(",\"underlined\":true");
		if (this->hasHoverText)     ret.append(R"(,"hoverEvent":{"action":"show_text","contents":)").append(hoverText->toJSONRawText()).append("}");
		if (this->hasAction)        ret.append(",\"clickEvent\":").append(this->clickAction->toJsonRawText());
		ret.append("}");
		return ret;
	}

	RText RText::setColor(string color_) {
		this->hasColor = true;
		this->color = std::move(color_);
		return *this;
	}

	const string& RText::getText() const {
		return text;
	}

	RText RText::setText(const string& basicString) {
		this->text = basicString;
		return *this;
	}

	bool RText::isItalic1() const {
		return isItalic;
	}

	RText RText::setIsItalic(bool b) {
		this->isItalic = b;
		return *this;
	}

	bool RText::isBold1() const {
		return isBold;
	}

	RText RText::setIsBold(bool b) {
		this->isBold = b;
		return *this;
	}

	bool RText::isUnderlined1() const {
		return isUnderlined;
	}

	RText RText::setIsUnderlined(bool underlined) {
		this->isUnderlined = underlined;
		return *this;
	}

	bool RText::isStrikethrough1() const {
		return isStrikethrough;
	}

	RText RText::setIsStrikethrough(bool bStrikethrough) {
		this->isStrikethrough = bStrikethrough;
		return *this;
	}

	bool RText::isObfuscated1() const {
		return isObfuscated;
	}

	RText RText::setIsObfuscated(bool bObfuscated) {
		this->isObfuscated = bObfuscated;
		return *this;
	}

	RText RText::setHoverText(RText spHoverText) {
		this->hasHoverText = true;
		this->hoverText = make_shared<RText>(spHoverText);
		return *this;
	}

	const RAction& RText::getClickAction() const {
		return *clickAction;
	}

	RText RText::setClickAction(const RAction& clickAction_) {
		this->hasAction = true;
		this->clickAction = make_shared<RAction>(clickAction_);
		return *this;
	}

	string RTextList2Json(vector<RText> rTextList) {
		string val = "[\"\"";
		for (const auto& item : rTextList) {
			val.append(",");
			RText text = item;
			val.append(text.toJSONRawText());
		}
		val += "]";
		return val;
	}

	RAction::RAction(int actionId_, string value_) {
		this->actionId = actionId_;
		this->value = value_;
	}

	string RAction::getAction() const {
		switch (this->actionId) {
		case OPEN_URL:
			return "open_url";
		case OPEN_FILE:
			return "open_file";
		case RUN_COMMAND:
			return "run_command";
		case SUGGEST_COMMAND:
			return "suggest_command";
		case CHANGE_PAGE:
			return "change_page";
		case COPY_TO_CLIPBOARD:
			return "copy_to_clipboard";
		default:
			return "";
		}
	}

	string RAction::getValue() {
		return this->value;
	}

	RAction RAction::setAction(int actionId_) {
		this->actionId = actionId_;
		return *this;
	}

	RAction RAction::setValue(string value_) {
		this->value = std::move(value_);
		return *this;
	}

	int RAction::getActionId() const {
		return actionId;
	}
	//{"text":"Hello World!","color":"blue","bold":true,"hoverEvent":{"action":"show_text","contents":{"text":"Fucking World!","color":"blue"}},"clickEvent":{"action":run_command","value":"/tp ~ ~ ~"}}
	string RAction::toJsonRawText() const {
		return string().append(R"({"action":")").append(this->getAction()).append(R"(","value":")").append(this->value).append("\"}");
	}
}