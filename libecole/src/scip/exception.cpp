#include "ecole/scip/exception.hpp"

namespace ecole {
namespace scip {

Exception Exception::from_retcode(SCIP_RETCODE retcode) {
	auto message = [retcode]() -> std::string {
		switch (retcode) {
		case SCIP_OKAY:
			throw Exception("Normal termination must not raise exception");
		case SCIP_ERROR:
			return "Unspecified error";
		case SCIP_NOMEMORY:
			return "Insufficient memory error";
		case SCIP_READERROR:
			return "File read error";
		case SCIP_WRITEERROR:
			return "File write error";
		case SCIP_BRANCHERROR:
			return "Branch error";
		case SCIP_NOFILE:
			return "File not found error";
		case SCIP_FILECREATEERROR:
			return "Cannot create file";
		case SCIP_LPERROR:
			return "Error in LP solver";
		case SCIP_NOPROBLEM:
			return "No problem exists";
		case SCIP_INVALIDCALL:
			return "Method cannot be called at this time in solution process";
		case SCIP_INVALIDDATA:
			return "Method cannot be called with this type of data";
		case SCIP_INVALIDRESULT:
			return "Method returned an invalid result code";
		case SCIP_PLUGINNOTFOUND:
			return "A required plugin was not found";
		case SCIP_PARAMETERUNKNOWN:
			return "The parameter with the given name was not found";
		case SCIP_PARAMETERWRONGTYPE:
			return "The parameter is not of the expected type";
		case SCIP_PARAMETERWRONGVAL:
			return "The value is invalid for the given parameter";
		case SCIP_KEYALREADYEXISTING:
			return "The given key is already existing in table";
		case SCIP_MAXDEPTHLEVEL:
			return "Maximal branching depth level exceeded";
		default:
			return "Invalid return code";
		}
	}();
	return Exception(std::move(message) + " (Scip error " + std::to_string(retcode) + ").");
}

Exception::Exception(std::string const& message_) : message(message_) {}

Exception::Exception(std::string&& message_) : message(std::move(message_)) {}

const char* Exception::what() const noexcept {
	return message.c_str();
}

}  // namespace scip
}  // namespace ecole
