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

export module Helium.TerminalUI.Shared;

import <ftxui/component/component.hpp>;
import <ftxui/component/component_base.hpp>;
import <ftxui/component/component_options.hpp>;
import <ftxui/component/event.hpp>;
import <ftxui/component/screen_interactive.hpp>;
import <ftxui/dom/elements.hpp>;
import <ftxui/dom/flexbox_config.hpp>;
import <ftxui/screen/color.hpp>;
import <ftxui/screen/color_info.hpp>;
import <ftxui/screen/terminal.hpp>;

import <vector>;

import Helium.Misc.Utils;

using namespace std;
using namespace ftxui;

export namespace ftxui {
	Component Window(std::string title, Component component) {
		return Renderer(component, [component, title] {  //
			return window(text(title), component->Render()) | flex;
			});
	}
	Component Empty() {
		return std::make_shared<ComponentBase>();
	}

	class ScrollerBase : public ComponentBase {
	public:
		ScrollerBase(Component child) { Add(child); }

	private:
		Element Render() final {
			auto focused = Focused() ? focus : ftxui::select;
			auto style = Focused() ? inverted : nothing;

			Element background = ComponentBase::Render();
			background->ComputeRequirement();
			size_ = background->requirement().min_y;
			return dbox({
					   std::move(background),
					   vbox({
						   text(L"") | size(HEIGHT, EQUAL, selected_),
						   text(L"") | style | focused,
					   }),
				}) |
				vscroll_indicator | yframe | yflex | reflect(box_);
		}

		bool OnEvent(Event event) final {
			if (event.is_mouse() && box_.Contain(event.mouse().x, event.mouse().y))
				TakeFocus();

			int selected_old = selected_;
			if (event == Event::ArrowUp || event == Event::Character('k') ||
				(event.is_mouse() && event.mouse().button == Mouse::WheelUp)) {
				selected_--;
			}
			if ((event == Event::ArrowDown || event == Event::Character('j') ||
				(event.is_mouse() && event.mouse().button == Mouse::WheelDown))) {
				selected_++;
			}
			if (event == Event::PageDown)
				selected_ += box_.y_max - box_.y_min;
			if (event == Event::PageUp)
				selected_ -= box_.y_max - box_.y_min;
			if (event == Event::Home)
				selected_ = 0;
			if (event == Event::End)
				selected_ = size_;

			selected_ = std::max(0, std::min(size_ - 1, selected_));
			return selected_old != selected_;
		}

		bool Focusable() const final { return true; }

		int selected_ = 0;
		int size_ = 0;
		Box box_;
	};

	Component Scroller(Component child) {
		return Make<ScrollerBase>(std::move(child));
	}

	Component Inner(std::vector<Component> children) {
		Component vlist = Container::Vertical(std::move(children));
		return Renderer(vlist, [vlist] {
			return hbox({
				text(" "),
				vlist->Render(),
				});
			});
	}
}