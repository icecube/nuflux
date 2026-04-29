#include <string>

namespace nuflux{
	namespace detail{
		#define INSTALL_FLAG_PREFIX "@INSTALL_FLAG_PREFIX@"
		#define INSTALL_FLAG_SUFFIX "@INSTALL_FLAG_SUFFIX@"
		#define INSTALL_FLAG_OFF "@INSTALL_FLAG_OFF@"
		#define INSTALL_FLAG_ON "@INSTALL_FLAG_ON@"
		//Flag is off unless set during installation
		static const volatile char installBitSpace[]= INSTALL_FLAG_PREFIX INSTALL_FLAG_OFF INSTALL_FLAG_SUFFIX;
		
		static bool getInstalledBit(){
			static const std::size_t prefixLen = strlen(INSTALL_FLAG_PREFIX);
			return installBitSpace[prefixLen]==INSTALL_FLAG_ON[0];
		}
		
		//declaration without definition
		//build modes which use this must ensure that a definition is linked
		std::string getBasePathRuntime() __attribute__((weak));
		
		std::string getBasePath(){
			return getInstalledBit() ? @INSTALLED_DATA_PATH@ : "@BUILD_TIME_DATA_PATH@" ;
		}
		
		std::string getDataPath(std::string fname){
			const char* env_p = std::getenv("NUFLUX_DATA");
			if(env_p)
				return std::string(env_p) + "/" + fname;
			return getBasePath() + "/" + fname;
		}
	}
}
