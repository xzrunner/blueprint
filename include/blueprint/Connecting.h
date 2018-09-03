#pragma once

#include <painting2/BezierShape.h>

#include <memory>

namespace bp
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
	struct Curve
	{
		pt2::BezierShape      shape;
		std::vector<uint32_t> color;
	};

private:
	void UpdateCurveShape();
	void UpdateCurveColor();

private:
	std::shared_ptr<Pins> m_from = nullptr;
	std::shared_ptr<Pins> m_to   = nullptr;

	Curve m_curve;

}; // Connecting

std::shared_ptr<Connecting> make_connecting(
	const std::shared_ptr<Pins>& from, const std::shared_ptr<Pins>& to
);

}