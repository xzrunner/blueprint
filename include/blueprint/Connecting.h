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

	std::shared_ptr<Pins> GetFrom() const;
	std::shared_ptr<Pins> GetTo() const;

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
	std::weak_ptr<Pins> m_from;
	std::weak_ptr<Pins> m_to;

	Curve m_curve;

}; // Connecting

std::shared_ptr<Connecting> make_connecting(
	const std::shared_ptr<Pins>& from, const std::shared_ptr<Pins>& to
);

void disconnect(const std::shared_ptr<Connecting>& conn);

}