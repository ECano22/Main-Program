#pragma once
#include <zmq.hpp>
#include <nlohmann/json.hpp>

class ZMQConnection
{
	zmq::context_t& context;
	zmq::socket_t& socket;
	public:
		ZMQConnection(zmq::context_t& zmq_context, zmq::socket_t& zmq_socket, std::string tcp_ip_address);
		std::string send_string(std::string message);
		nlohmann::json send_json(nlohmann::json message);
};