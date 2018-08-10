#pragma once

#include <painting2/BezierShape.h>

#include <memory>

namespace bp
{
namespace node
{

class Pins;

class Connecting
{
public:
	Connecting(const std::shared_ptr<Pins>& from,
		const std::shared_ptr<Pins>& to);

	void UpdateCurve();

	auto& GetFrom() const { return m_from; }
	auto& GetTo() const { return m_to; }

	auto& GetCurve() const { return m_curve; }

private:
	std::shared_ptr<Pins> m_from = nullptr;
	std::shared_ptr<Pins> m_to   = nullptr;

	pt2::BezierShape m_curve;

}; // Connecting

std::shared_ptr<Connecting> make_connecting(
	const std::shared_ptr<Pins>& from, const std::shared_ptr<Pins>& to
);

}
}