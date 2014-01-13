/*
http://stackoverflow.com/questions/86582/singleton-how-should-it-be-used
http://stackoverflow.com/questions/10892167/proper-implementation-of-global-configuration

 * C++ Singleton
 * Limitation: Single Threaded Design
 * See: http://www.aristeia.com/Papers/DDJ_Jul_Aug_2004_revised.pdf
 *      For problems associated with locking in multi threaded applications
 *
 * Limitation:
 * If you use this Singleton (A) within a destructor of another Singleton (B)
 * This Singleton (A) must be fully constructed before the constructor of (B)
 * is called.
 */

#pragma once
#include "SchladetschEvents.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <libconfig.h++>

using namespace Schladetsch::Events;
using namespace libconfig;

static const std::string filepath = "settings.cfg";

class SettingsManager
{
	private:
		// Private Constructor
		SettingsManager();

		//Stop the compiler from generating methods of copying the object
		SettingsManager(SettingsManager const& copy);            // Not Implemented
		SettingsManager& operator=(SettingsManager const& copy); // Not Implemented

	public:
		static SettingsManager& GetInstance()
		{
			// The only instance
			// Guaranteed to be lazy initialized
			// Guaranteed that it will be destroyed correctly
			static SettingsManager instance;
			return instance;
		}

		//As separate function because we also want to trigger the re-load event
		void ReloadSettings();
		
		Config& GetConfig() { return cfg; }

		/*
		Used like this:

		Foo foo;
		event.Add(foo, &Foo::Method);
		*/
		Event<Config*>* GetEvent() { return &loadEvent; }

	private:
		//Empty template, because we just want to call functions on rebuild
		Event<Config*> loadEvent;
		Config cfg;
};