#ifndef UpdateServer_h
#define UpdateServer_h
#include <ESPAsyncWebServer.h>

class UpdateServerClass {
	public:
		void begin(AsyncWebServer *server);
};

extern UpdateServerClass UpdateServer;
#endif
