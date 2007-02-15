///////////////////////////////////////////////////////////////////////////////
// A simpe load screen that works on all platforms
///////////////////////////////////////////////////////////////////////////////
#ifndef LOADSCREEN_H
#define LOADSCREEN_H

// You write your own load screen here.
class LoadScreenModule
{
public:
	LoadScreenModule() {}
	virtual ~LoadScreenModule() {}

	// You write these!
	virtual void Startup() {}
	virtual void Render() = 0;
	virtual void Cleanup() {}

	// Do you want the pretty loading screen text?
	virtual bool WantLoadingText()
		{ return true; }
};

// The load screen!
namespace LoadScreen
{
	// You give me this, I call it to make the object
	typedef LoadScreenModule *(*LoadScreenModuleCreate)();

	// Setup the loadscreen by registering a creator!
	bool Setup(LoadScreenModuleCreate creator);

	// Kickoff a loadscreen
	bool Kickoff();

	// Is the loadscreen running?
	bool IsRunning();

	// Kill the loadscreen
	bool Kill();
};

#endif
