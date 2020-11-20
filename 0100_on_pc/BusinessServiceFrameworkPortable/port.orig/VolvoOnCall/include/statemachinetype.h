///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file eventsatemachine.h
//	This file handles statemachine types for application.

// @project 		GLY_TCAM
// @subsystem	Application
// @author		linhaixia
// @Init date		24-Jul-2018
///////////////////////////////////////////////////////////////////


#ifndef STATEMACHINE_TYPE_H__
#define STATEMACHINE_TYPE_H__


enum STATEMACHINE_TYPE
{
	STATEMACHINE_TYPE_ISTATEMACHINE,
	STATEMACHINE_TYPE_SVT,
	STATEMACHINE_TYPE_REMOTECTRL,
		
	MAXIMUM_NUMBER_OF_STATEMACHINE //MUST BE LAST
};


#endif // STATEMACHINE_TYPE_H__
