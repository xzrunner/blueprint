#pragma once

#include <geoshape/Bezier.h>

#include <memory>

namespace bp
{

class Pin;

class Connecting
{
public:
	Connecting(const std::shared_ptr<Pin>& from,
		const std::shared_ptr<Pin>& to);

	void UpdateCurve();

	std::shared_ptr<Pin> GetFrom() const;
	std::shared_ptr<Pin> GetTo() const;

	auto& GetCurve() const { return m_curve; }

    bool IsActive() const { return m_active; }
    void SetActive(bool active) { m_active = active; }

private:
	struct Curve
	{
		gs::Bezier            shape;
		std::vector<uint32_t> color;
	};

private:
	void UpdateCurveShape();
	void UpdateCurveColor();

private:
	std::weak_ptr<Pin> m_from;
	std::weak_ptr<Pin> m_to;

	Curve m_curve;

    bool m_active = true;

}; // Connecting

std::shared_ptr<Connecting> make_connecting(
	const std::shared_ptr<Pin>& from, const std::shared_ptr<Pin>& to
);

void disconnect(const std::shared_ptr<Connecting>& conn);

}