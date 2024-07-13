#include <chrono>

namespace RT::Clock {
	const std::chrono::high_resolution_clock::time_point TIME_START = std::chrono::high_resolution_clock::now();
	static float CurrentFrame = 0.0f, LastFrame = 0.0f, DeltaTime = 0.0f;
	static float GetTime() {
		typedef std::chrono::high_resolution_clock clock;
		typedef std::chrono::duration<float, std::ratio<1>> duration; // can use std::milli or std::ratio<1,1000> for ms instead

		duration elapsed = clock::now() - TIME_START;
	  
		return (float) elapsed.count(); // seconds
	}
	static float UpdateDeltaTime() {
		CurrentFrame = GetTime();
		DeltaTime = CurrentFrame - LastFrame;
		LastFrame = CurrentFrame;
		return DeltaTime;
	}
	static float GetDeltaTime() {
		return DeltaTime;
	}

}
