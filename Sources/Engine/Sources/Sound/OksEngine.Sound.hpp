#pragma once 

#include <OksEngine.ECS.Component.hpp>
#include <OksEngine.ECS.System.hpp>

#include <SFML/Audio.hpp>

namespace OksEngine {

	struct Sound : public ECSComponent<Sound> {

		Sound() :
			ECSComponent{ nullptr } { }

		Sound(const sf::SoundBuffer& soundBuffer) :
			ECSComponent{ nullptr }, soundBuffer_{ soundBuffer }{ }
		sf::SoundBuffer soundBuffer_;
		sf::Sound sound_;
	};

	template<>
	inline void Edit<Sound>(Sound* sound) {
		
	}


}