#pragma once
#include <Tomato.Core/Tomato.Core.h>
#include <cms/Connection.h>

namespace CES
{
	class Uploader
	{
	public:
		Uploader();

		void Initialize(std::wstring_view ip, uint16_t port, std::wstring_view destUri, bool useTopic);
		void Upload(IStream* imageStream, std::wstring&& fileName);
	private:
		std::unique_ptr<cms::Connection> _mqConnection;
		std::unique_ptr<cms::Session> _mqSession;
		std::unique_ptr<cms::Destination> _mqDestination;
		std::unique_ptr<cms::MessageProducer> _mqProducer;
	};
}
