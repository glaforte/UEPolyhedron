// Copyright 2024 TabbyCoder

#pragma once

#include "Modules/ModuleInterface.h"

class FPolyhedronModule : public IModuleInterface {
public: // IModuleInterface interface
	void StartupModule() override;
	void ShutdownModule() override;
};
