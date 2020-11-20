///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file iinstancemanager.h
//	IInstanceManager is an empty baseclass for derived factories which can be
//  used for dependency injection into classes which should support mocked objects for
//  regression tests.
//  It declares methods where objects can get services/managers/singleton objects
//  which they depend on.
//  A test would mock/implement its own concrete IInstanceManager which returns
//  mock objects neede for the test. The real concrete IInstanceManager returns
//  the real object the according class needs for the product implementation.
//  Dependency Injection of thus Factory is completely removing hard dependencies
//  from objects through inversion of the dependency. The client is repsonsible
//  to tell on which services the object has to depend.
//  In a test it is a mock object, in product it is the real service.

// @project 	GLY_TCAM
// @subsystem	Application
// @author		linhaixia
// @Init date	24-Jul-2018
///////////////////////////////////////////////////////////////////

#ifndef INSTANCEMANAGER_H__
#define INSTANCEMANAGER_H__

#include "iinstancemanager.h"

class InstanceManager : public IInstanceManager
{
public:
	InstanceManager();
	virtual ~InstanceManager();
	
protected:
	
};

#endif //INSTANCEMANAGER_H__
