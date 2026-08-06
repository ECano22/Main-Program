#pragma once
#include "zmq_classes.h"
#include <zmq.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
ZMQConnection::ZMQConnection(zmq::context_t& zmq_context, zmq::socket_t& zmq_socket, std::string tcp_ip_address) : context(zmq_context), socket(zmq_socket)
{
	socket.connect(tcp_ip_address);
}
std::string ZMQConnection::send_string(std::string message)
{
	auto send_message = socket.send(zmq::buffer(message), zmq::send_flags::none);
	zmq::message_t raw_message;
	auto recv_message = socket.recv(raw_message, zmq::recv_flags::none);
	return raw_message.to_string();
}
json ZMQConnection::send_json(json message)
{
	auto send_message = socket.send(zmq::buffer(message.dump()), zmq::send_flags::none);
	zmq::message_t raw_message;
	auto recv_message = socket.recv(raw_message, zmq::recv_flags::none);
	return json::parse(raw_message.to_string());
}